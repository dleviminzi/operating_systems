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


#define BUFFSIZE 256
#define SHL 1
#define KB 0
#define CONFUSED 1

int readBuff(int fd, char *buff) {
    int lenRead = read(fd, buff, BUFFSIZE);

    if (lenRead < 0) {
        fprintf(stderr, "Failed to read stdin: %s", strerror(errno));
        exit(1);
    }

    return lenRead;
}

void writeBuff(int fd, char *buff, int lenBytes, int shell) {
    int index = 0;

    while (index < lenBytes) {
        char *c = buff + index;

        switch (*c) {
            case 4:      /* ^D */
                exit(0);
                break;
            case 3:      /* ^C */
                exit(0);
                break;
            case '\r':
            case '\n':
                if (shell) {
                    if (write(fd, "\n", 1) < 0) {
                        fprintf(stderr, "Failed to write to stdout: %s", strerror(errno));
                        exit(1);
                    }
                } else {
                    if (write(fd, "\r\n", 2) < 0) {
                        fprintf(stderr, "Failed to write to stdout: %s", strerror(errno));
                        exit(1);
                    }
                }
                break;
            default:
                if (write(fd, c, sizeof c) < 0) {
                    fprintf(stderr, "Failed to write to stdout: %s", strerror(errno));
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

int main(int argc, char *argv[]) {
    char buff[256]; /* how big should this be? */

    /*
    *                   OPT/ARG HANDLING
    */

    int shellFlg = 0;
    char *program;

    /* define option for shell argument */
    static struct option long_options[] = {
        {"shell", required_argument, NULL, 's'},
        {0, 0, 0, 0}
    };

    int opt = 0;
    int option_index = 0;

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
                fprintf(stderr, "Invalid argument provided for option.");
                exit(1);
        }
    }
    
    /* check for additional non-option arguments */
    if (argv[optind]) {
        fprintf(stderr, "Non-option arguments are not permitted: %s\n", argv[optind]);
        exit(1);
    }


    /*
    *               CHANGING TERMINAL ATTRIBUTES
    */

    struct termios restoreAttr; /* hold initial terminal attributes */

    /* populate restore point w/ initial terminal attributes */
    if (tcgetattr(STDIN_FILENO, &restoreAttr) != 0) {
        fprintf(stderr, "Failure to retrieve current terminal attributes"); 
    } 

    /* make copy of terminal attributes w/ changes defined in spec */
    struct termios newAttr = restoreAttr;
    newAttr.c_iflag = ISTRIP;
    newAttr.c_oflag = 0;
    newAttr.c_lflag = 0;

    /* set terminal to use new attributes */
    if (tcsetattr(STDIN_FILENO, TCSANOW, &newAttr) != 0) {
        fprintf(stderr, "Failure to set new terminal attributes.");
    }

    /*
    *           FORKING
    */ 

    if (shellFlg) {
        int pFromTerm[2];
        int pToTerm[2];

        if (pipe(pFromTerm) == -1 || pipe(pToTerm) == -1) {
            fprintf(stderr, "Pipe setup failed: %s", strerror(errno));
        }

        int frk = fork();

        if (frk > 0) {          /* parent */    
            close(pFromTerm[1]); 
            close(pToTerm[0]);

            /* polling structure */
            struct pollfd pollArr[2];

            pollArr[KB].fd = STDIN_FILENO; /* kb */
            pollArr[KB].events = POLLIN;
            pollArr[SHL].fd = pFromTerm[0];       /* shell */
            pollArr[SHL].events = POLLIN;

            /* polling for input from keyboard or shell */
            while (1) {
                int ret = poll(pollArr, 2, 0);

                /* -1 => error in polling, 0 => no events */
                if (ret == -1) {
                    fprintf(stderr, "Polling failed: %s", strerror(errno));
                    exit(1);
                } else if (ret == 0) continue;

                /* checking keyboard for events */
                if (pollArr[KB].revents & POLLIN) {
                    int lenBuff = readBuff(STDIN_FILENO, buff);
                    writeBuff(STDOUT_FILENO, buff, lenBuff, 0);
                    writeBuff(pFromTerm[0], buff, lenBuff, 1);
                }
                
                /* checking shell for events */
                if (pollArr[SHL].revents & POLLIN) {
                    int lenBuff = readBuff(pFromTerm[0], buff);
                    writeBuff(STDOUT_FILENO, buff, lenBuff, 0);
                }

                if (pollArr[SHL].revents & POLLHUP || pollArr[1].revents & POLLERR) {
                    exit(1);
                }
            }


        } else if (frk == 0) {  /* kid */
            /* setting up stdin */
            close(pFromTerm[0]); 
            fdRedirect(pFromTerm[0], STDIN_FILENO);

            /* setting up stdout */
            close(pToTerm[1]);
            fdRedirect(pToTerm[1], STDOUT_FILENO);
            fdRedirect(pToTerm[1], STDERR_FILENO);

            if (execl(program, program, NULL) == -1) {
                fprintf(stderr, "Exec failed: %s", strerror(errno));
            }

        } else if (frk == -1){  /* failed */
            fprintf(stderr, "Fork failed: %s", strerror(errno));
            exit(1);
        }

    } else {
        /*
        *             NO SHELL OPT/ARG WERE PROVIDED
        */

        /* reading into buffer and then writing into stdout */
        while (1) {
            int lenBuff = readBuff(STDIN_FILENO, buff);
            writeBuff(STDOUT_FILENO, buff, lenBuff, 0);
        }
    }

    /* restore terminal attributes to the original state */
    tcsetattr(STDIN_FILENO, TCSANOW, &restoreAttr);

    return 0;   
}