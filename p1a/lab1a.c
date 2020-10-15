#include <termios.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <poll.h>
#include <sys/wait.h>

#define SHL 1
#define KB 0
#define WR 1
#define RD 0
#define SHELL_RECEIVE 1
#define STDOUT_RECEIVE 0
#define ERROR 1
#define SUCCESS 0

int shellFlg = 0;           /* flag for shell option */
int termToShell[2];         /* pipe term ---> shell */
int shellToTerm[2];         /* pipe shell ---> term */
int cpid;                   /* child process ID     */
struct termios restoreAttr; /* hold initial terminal attributes */

void restoreTermAttributes() {
    if (tcsetattr(STDIN_FILENO, TCSANOW, &restoreAttr) != 0) {
        fprintf(stderr, "Failed to restore terminal attributes: %s\n", strerror(errno));
        exit(ERROR);
    }
}

/* exit while restoring terminal attributes and waiting on child process */
void shutdown(int exitStatus) {
    restoreTermAttributes();

    if (shellFlg) {
        /* waiting on child */
        int status;

        if (waitpid(cpid, &status, 0) == -1) {
            fprintf(stderr, "waiting for child failed: %s", strerror(errno));
        }

        fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d", WTERMSIG(status), WEXITSTATUS(status));
    }

    exit(exitStatus);
}

/* basic write error checking */
void extWrite(int fd, char *c, int numChars) {
    if (write(fd, c, numChars) < 0) {
        fprintf(stderr, "Failed to write to stdout: %s", strerror(errno));
        shutdown(ERROR);
    }
}

/* close with error handling */
void extClose(int fd) {
    if (close(fd) < 0) {
        fprintf(stderr, "Failed to close file descriptor: %s", strerror(errno));
        shutdown(ERROR);
    }
}


/* redirect file to target file descriptor */
void fdRedirect(int newFD, int targetFD) {
    extClose(targetFD);
    if (dup2(newFD, targetFD) < 0) {
        fprintf(stderr, "Dup2 failed: %s", strerror(errno));
        shutdown(ERROR);
    }
}

/* sig handler for SIGPIPE */
void sigHandler(int sig) {
    if (sig == SIGPIPE) {
        shutdown(SUCCESS);
    }
}

/* read buffer and return length of read */
int readBuff(int fd, char *buff) {
    int lenRead = read(fd, buff, sizeof buff);

    if (lenRead < 0) {
        fprintf(stderr, "Failed to read stdin: %s", strerror(errno));
        shutdown(ERROR);
    }

    return lenRead;
}

/* write buffer to channel specified in file descriptor */
void processBuff(int fd, char *buff, int lenBytes, int toShell) {
    int index = 0;

    while (index < lenBytes) {
        char *c = buff + index;

        switch (*c) {
            case 4:      /* ^D */
                /* terminal echos text form of EOF, but not to shell */
                if (!toShell) extWrite(fd, "^D", 2); 
                if (shellFlg) {
                    extClose(termToShell[WR]); /* both ends closed sends EOF to shell */
                } else shutdown(SUCCESS);
                break;
            case 3:      /* ^C */
                /* terminal echos text form of EOT, but not to shell */
                if (shellFlg) {
                    if (!toShell) extWrite(fd, "^C", 2);
                    if (kill(cpid, SIGINT) < 0) {
                        fprintf(stderr, "Kill failed: %s", strerror(errno));
                        restoreTermAttributes();
                        shutdown(ERROR);
                    }
                } else {
                    extWrite(fd, c, 1);
                }
                break;
            case '\r':
            case '\n':
                if (toShell) {
                    extWrite(fd, "\n", 1);
                } else {
                    extWrite(fd, "\r\n", 2);
                }
                break;
            default:
                extWrite(fd, c, 1);
                break;
        }
        index++;
    }
}


int main(int argc, char *argv[]) {
    int opt = 0;                /* getopt options */
    int option_index = 0;       
    char *program;              /* shell name */
    char buff[256];             /* buffer initialization */

    static struct option long_options[] = {         /* shell option */ 
        {"shell", required_argument, NULL, 's'},
        {0, 0, 0, 0}
    };

    /* Option processing */
    while ((opt = getopt_long(argc, argv, ":s:", long_options, &option_index)) != -1) {
        switch(opt) {
            case 's':
                shellFlg = 1;
                program = optarg;
                break;
            case '?':
                fprintf(stderr, "Unknown option. Permitted option: shell\n");
                exit(ERROR);
            case ':':
                fprintf(stderr, "Invalid argument provided for option.\n");
                exit(ERROR);
        }
    }

    /* Terminal attributes modifications (keeping a restore point) */
    if (tcgetattr(STDIN_FILENO, &restoreAttr) != 0) {
        fprintf(stderr, "Failure to retrieve current terminal attributes: %s\n", strerror(errno));
        exit(ERROR); 
    } 

    struct termios newAttr = restoreAttr;
    newAttr.c_iflag = ISTRIP;
    newAttr.c_oflag = 0;
    newAttr.c_lflag = 0;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &newAttr) != 0) {
        fprintf(stderr, "Failure to set new terminal attributes: %s", strerror(errno));
        restoreTermAttributes();
        exit(ERROR);
    }

    /* Multiprocess mode */
    if (shellFlg) {
        signal(SIGPIPE, sigHandler);

        if (pipe(termToShell) == -1 || pipe(shellToTerm) == -1) {
            fprintf(stderr, "Pipe setup failed: %s", strerror(errno));
            shutdown(ERROR);
        }

        cpid = fork();

        if (cpid > 0) {  /* parent */    
            extClose(shellToTerm[WR]);     /* parent only reads from this pipe */
            extClose(termToShell[RD]);     /* parent only writes in this pipe */

            /* Polling for input from shell and keyboard */
            struct pollfd pollArr[2]; /* specifying events to watch for */
            pollArr[KB].fd = STDIN_FILENO;  
            pollArr[KB].events = POLLIN;           
            pollArr[SHL].fd = shellToTerm[RD];      
            pollArr[SHL].events = POLLIN;

            while (1) {
                int events = poll(pollArr, 2, 0);

                if (events == -1) {        /* -1 err in poll, 0 no events */
                    fprintf(stderr, "Polling failed: %s", strerror(errno));
                    restoreTermAttributes();
                    shutdown(ERROR);
                } else if (events == 0) continue;

                if (pollArr[KB].revents & POLLIN) {         /* kb event check */
                    int lenBuff = readBuff(STDIN_FILENO, buff);
                    processBuff(STDOUT_FILENO, buff, lenBuff, STDOUT_RECEIVE);
                    processBuff(termToShell[WR], buff, lenBuff, SHELL_RECEIVE);
                }
                
                if (pollArr[SHL].revents & POLLIN) {     /* shell event check */
                    int lenBuff = readBuff(shellToTerm[RD], buff);
                    processBuff(STDOUT_FILENO, buff, lenBuff, STDOUT_RECEIVE);
                }

                if (pollArr[SHL].revents & POLLHUP || pollArr[SHL].revents & POLLERR) {
                    shutdown(ERROR);
                }
            }

        } else if (cpid == 0) {  /* kid */
            /* setup piping to/from terminal */
            extClose(termToShell[WR]);  /* child only reads from this pipe */
            extClose(shellToTerm[RD]);  /* child only writes to this pipe */

            fdRedirect(termToShell[RD], STDIN_FILENO);  /* read to stdin */
            extClose(termToShell[RD]);                     

            fdRedirect(shellToTerm[WR], STDOUT_FILENO);  /* write to stdout/err */
            fdRedirect(shellToTerm[WR], STDERR_FILENO);
            extClose(shellToTerm[WR]);

            /* Shell process takes over */
            if (execl(program, program, NULL) == -1) {
                fprintf(stderr, "Exec failed: %s", strerror(errno));
                restoreTermAttributes();
                shutdown(ERROR);
            }

        } else if (cpid == -1){  /* failed */
            fprintf(stderr, "Fork failed: %s", strerror(errno));
            restoreTermAttributes();
            exit(ERROR); /* normal exit bc waitpid in shutdown will spiral */
        }
    } else {
        /* Default mode */
        while (1) {
            int lenBuff = readBuff(STDIN_FILENO, buff);   /* read stdin */
            processBuff(STDOUT_FILENO, buff, lenBuff, STDOUT_RECEIVE);   /* write to stdout */
        }
    }
    return SUCCESS;   
}