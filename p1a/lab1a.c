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
#define CONFUSED 1

int dFlg = 0; /* flag that EOF was passed in */
int cFlg = 0; /* flag that ^C was passed in */
int spFlg = 0;
struct termios restoreAttr; /* hold initial terminal attributes */

/* restore terminal attributes to the original state */
void restoreTermAttributes() {
    if (tcsetattr(STDIN_FILENO, TCSANOW, &restoreAttr) != 0) {
        fprintf(stderr, "Failed to restore terminal attributes: %s\n", strerror(errno));
        exit(1);
    }
}

/* read buffer and return length of read */
int readBuff(int fd, char *buff) {
    int lenRead = read(fd, buff, sizeof buff);

    if (lenRead < 0) {
        fprintf(stderr, "Failed to read stdin: %s", strerror(errno));
        restoreTermAttributes();
        exit(1);
    }

    return lenRead;
}

/* write buffer to channel specified in file descriptor */
void writeBuff(int fd, char *buff, int lenBytes, int shell) {
    int index = 0;

    while (index < lenBytes) {
        char *c = buff + index;

        switch (*c) {
            case 4:      /* ^D */
                if (write(fd, "^D", 2) < 0) {
                    fprintf(stderr, "Failed to write to stdout: %s", strerror(errno));
                    restoreTermAttributes();
                    exit(1);
                }
                dFlg = 1;
                break;
            case 3:      /* ^C */
                if (write(fd, "^C", 2) < 0) {
                    fprintf(stderr, "Failed to write to stdout: %s", strerror(errno));
                    restoreTermAttributes();
                    exit(1);
                }
                cFlg = 1;
                break;
            case '\r':
            case '\n':
                if (shell) {
                    if (write(fd, "\n", 1) < 0) {
                        fprintf(stderr, "Failed to write to stdout: %s", strerror(errno));
                        restoreTermAttributes();
                        exit(1);
                    }
                } else {
                    if (write(fd, "\r\n", 2) < 0) {
                        fprintf(stderr, "Failed to write to stdout: %s", strerror(errno));
                        restoreTermAttributes();
                        exit(1);
                    }
                }
                break;
            default:
                if (write(fd, c, 1) < 0) {
                    fprintf(stderr, "Failed to write to stdout: %s", strerror(errno));
                    restoreTermAttributes();
                    exit(1);
                }
                break;
        }
        index++;
    }

}

void fdRedirect(int newFD, int targetFD) {
    close(targetFD);
    dup2(newFD, targetFD);
}

void sigHandler(int sig) {
    if (sig == SIGPIPE) {
        spFlg = 1;
    }
}

int main(int argc, char *argv[]) {
    int shellFlg = 0;           /* flag for shell option */
    int opt = 0;                /* getopt options */
    int option_index = 0;       
    char *program;              /* shell name */
    char buff[256];             /* buffer initialization */

    signal(SIGPIPE, sigHandler);

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
                exit(1);
            case ':':
                fprintf(stderr, "Invalid argument provided for option.\n");
                exit(1);
        }
    }

    /* Terminal attributes modifications (keeping a restore point) */
    if (tcgetattr(STDIN_FILENO, &restoreAttr) != 0) {
        fprintf(stderr, "Failure to retrieve current terminal attributes: %s\n", strerror(errno));
        exit(1); 
    } 

    struct termios newAttr = restoreAttr;
    newAttr.c_iflag = ISTRIP;
    newAttr.c_oflag = 0;
    newAttr.c_lflag = 0;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &newAttr) != 0) {
        fprintf(stderr, "Failure to set new terminal attributes: %s", strerror(errno));
        restoreTermAttributes();
        exit(1);
    }

    /* Multiprocess mode */
    if (shellFlg) {
        int termToShell[2];
        int shellToTerm[2];

        if (pipe(termToShell) == -1 || pipe(shellToTerm) == -1) {
            fprintf(stderr, "Pipe setup failed: %s", strerror(errno));
        }

        int cpid = fork();

        if (cpid > 0) {  /* parent */    
            close(shellToTerm[WR]);     /* parent only reads from this pipe */
            close(termToShell[RD]);     /* parent only writes in this pipe */

            /* Polling for input from shell and keyboard */
            struct pollfd pollArr[2]; /* specifying events to watch for */
            pollArr[KB].fd = STDIN_FILENO;  
            pollArr[KB].events = POLLIN;           
            pollArr[SHL].fd = shellToTerm[0];      
            pollArr[SHL].events = POLLIN;

            while (1) {
                int ret = poll(pollArr, 2, 0);

                if (ret == -1) {        /* -1 err in poll, 0 no events */
                    fprintf(stderr, "Polling failed: %s", strerror(errno));
                    restoreTermAttributes();
                    exit(1);
                } else if (ret == 0) continue;

                if (pollArr[KB].revents & POLLIN) {         /* kb event check */
                    int lenBuff = readBuff(STDIN_FILENO, buff);
                    writeBuff(STDOUT_FILENO, buff, lenBuff, 0);
                    if (dFlg) {
                        close(termToShell[WR]);
                    } 
                    if (cFlg) {
                        kill(cpid, SIGINT);
                    }
                    if (!cFlg && !dFlg) writeBuff(termToShell[WR], buff, lenBuff, 1);
                }
                
                if (pollArr[SHL].revents & POLLIN) {     /* shell event check */
                    int lenBuff = readBuff(shellToTerm[RD], buff);
                    writeBuff(STDOUT_FILENO, buff, lenBuff, 0);
                }

                if (pollArr[SHL].revents & POLLHUP || pollArr[SHL].revents & POLLERR) {
                    fprintf(stderr, "Error occured during polling: %s\n", strerror(errno));
                    restoreTermAttributes();
                    exit(1);
                }

                if (dFlg || spFlg) {
                    int status;
                    waitpid(cpid, &status, 0);
                    fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d", WTERMSIG(status), WEXITSTATUS(status));
                    exit(0);
                }
            }

        } else if (cpid == 0) {  /* kid */
            /* setup piping to/from terminal */
            close(termToShell[WR]);  /* child only reads from this pipe */
            close(shellToTerm[RD]);  /* child only writes to this pipe */

            fdRedirect(termToShell[0], STDIN_FILENO);  /* read to stdin */
            close(termToShell[0]);                     

            fdRedirect(shellToTerm[1], STDOUT_FILENO);  /* write to stdout/err */
            fdRedirect(shellToTerm[1], STDERR_FILENO);
            close(shellToTerm[1]);

            /* Shell process takes over */
            if (execl(program, program, NULL) == -1) {
                fprintf(stderr, "Exec failed: %s", strerror(errno));
                restoreTermAttributes();
                exit(1);
            }

        } else if (cpid == -1){  /* failed */
            fprintf(stderr, "Fork failed: %s", strerror(errno));
            restoreTermAttributes();
            exit(1);
        }

    } else {
        /* Default mode */
        while (1) {
            int lenBuff = readBuff(STDIN_FILENO, buff);                 /* read stdin */
            writeBuff(STDOUT_FILENO, buff, lenBuff, 0);   /* write to stdout */
        }
    }

    return 0;   
}