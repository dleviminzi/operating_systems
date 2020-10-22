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
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#define ERROR 1
#define SUCCESS 0
#define WR 1
#define RD 0
#define SCT 0
#define SHL 1
#define SHELL_RECEIVE 1
#define SOCK_RECEIVE 0

/* GLOBAL VARIABLES */
int shellFlg = 0;               /* shell flag               */
int servToShell[2];             /* pipe server -----> shell */
int shellToServ[2];             /* pipe shell -----> server */
int cpid;                       /* child process id         */
int port;                       /* port number              */
int portFlg = 0;                /* port flag                */
int sockfd, nsockfd;            /* socket file descriptors  */

/* METHODS */

/* basic write error checking */
void extWrite(int fd, char *c, int numChars) {
    if (write(fd, c, numChars) < 0) {
        fprintf(stderr, "Failed to write to stdout: %s", strerror(errno));
        exit(ERROR);
    }
}

/* close with error handling */
void extClose(int fd) {
    if (close(fd) < 0) {
        fprintf(stderr, "Failed to close file descriptor: %s", strerror(errno));
        exit(ERROR);
    }
}

/* redirect file to target file descriptor */
void fdRedirect(int newFD, int targetFD) {
    extClose(targetFD);
    if (dup2(newFD, targetFD) < 0) {
        fprintf(stderr, "Dup2 failed: %s", strerror(errno));
        exit(ERROR);
    }
}

void shellLog() {
    if (shellFlg) {
        /* waiting on child */
        int status;

        if (waitpid(cpid, &status, 0) == -1) {
            fprintf(stderr, "waiting for child failed: %s", strerror(errno));
        }

        fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d", WTERMSIG(status), WEXITSTATUS(status));
    }
}

void closeSCT() {
    extClose(nsockfd);
}

/* sig handler for SIGPIPE */
void sigHandler(int sig) {
    if (sig == SIGPIPE) {
        exit(SUCCESS);
    }
}

/* read buffer and return length of read */
int readBuff(int fd, char *buff) {
    int lenRead = read(fd, buff, sizeof buff);

    if (lenRead < 0) {
        fprintf(stderr, "Failed to read stdin: %s", strerror(errno));
        exit(ERROR);
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
                extClose(servToShell[WR]); /* both ends closed sends EOF to shell */
                break;
            case 3:      /* ^C */
                if (kill(cpid, SIGINT) < 0) {
                    fprintf(stderr, "Kill failed: %s", strerror(errno));
                    exit(ERROR);
                }                
                break;
            /*
            case '\r':
            case '\n':
                if (toShell) {
                    extWrite(fd, "\n", 1);
                } 
                else {
                    extWrite(fd, "\r\n", 2);
                }
                
                break;
                */
            default:
                extWrite(fd, c, 1);
                break;
        }
        index++;
    }
}

/* MAIN ROUTINE */
int main(int argc, char* argv[]) {
    int opt = 0;
    int option_index = 0;
    char *program;          /* variable for storing shell option input */

    /* server variables */
    int lenCli;
    struct sockaddr_in serv_addr, cli_addr;

    /* input buffer */
    char buff[256];

    static struct option long_options[] = {
        {"shell", required_argument, NULL, 's'},
        {"port", required_argument, NULL, 'p'},         /* mandatory */
        {0,0,0,0}
    };

    /* Option processing */
    while ((opt = getopt_long(argc, argv, ":s:p:", long_options, &option_index)) != -1) {
        switch(opt) {
            case 's':
                shellFlg = 1;
                program = optarg;
                break;
            case 'p':
                portFlg = 1;
                port = atoi(optarg);
                break;
            case '?':
                fprintf(stderr, "Unknown option. Permitted option: shell");
                exit(ERROR);
            case ':':
                fprintf(stderr, "Invalid argument provided for option.");
                exit(ERROR);
        }
    }

    if (!portFlg || !shellFlg) {
        fprintf(stderr, "Usage requires both port and shell flags");
        exit(ERROR);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "Failed to open socket: %s", strerror(errno));
        exit(ERROR);
    }

    /* zero out server address */
    bzero((char *) &serv_addr, sizeof(serv_addr));

    /* init server address fields */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    
    /* binding socket to address */
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(stderr, "Failed to bind: %s", strerror(errno));
        exit(ERROR);
    }

    /* listen for connections on socket */
    listen(sockfd, 5);

    /* accepting connection and storing client address information */
    lenCli = sizeof(cli_addr);
    nsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &lenCli);
    if (nsockfd < 0) {
        fprintf(stderr, "Error accepting connection: %s", strerror(errno));
        exit(ERROR);
    }
    
    atexit(closeSCT);   /* at exit close socket */

    signal(SIGPIPE, sigHandler);

    if (pipe(servToShell) == -1 || pipe(shellToServ) == -1) {
            fprintf(stderr, "Pipe setup failed: %s", strerror(errno));
            exit(ERROR);
    }

    cpid = fork();
    atexit(shellLog); /* in the case of EOF or SIGPIPE we will log */

    if (cpid > 0) {  /* parent */    
        extClose(shellToServ[WR]);     /* parent only reads from this pipe */
        extClose(servToShell[RD]);     /* parent only writes in this pipe */

        /* Polling for input from shell and keyboard */
        struct pollfd pollArr[2]; /* specifying events to watch for */

        pollArr[SCT].fd = nsockfd;  
        pollArr[SCT].events = POLLIN;           

        pollArr[SHL].fd = shellToServ[RD];      
        pollArr[SHL].events = POLLIN;

        while (1) {
            int events = poll(pollArr, 2, 0);

            if (events == -1) {        /* -1 err in poll, 0 no events */
                fprintf(stderr, "Polling failed: %s", strerror(errno));
                exit(ERROR);
            } 
            else if (events == 0) continue;

            if (pollArr[SCT].revents & POLLIN) {         /* socket event check */
                int lenBuff = readBuff(nsockfd, buff);
                processBuff(servToShell[WR], buff, lenBuff, SHELL_RECEIVE);
            }
            
            if (pollArr[SHL].revents & POLLIN) {     /* shell event check */
                int lenBuff = readBuff(shellToServ[RD], buff);
                processBuff(nsockfd, buff, lenBuff, SOCK_RECEIVE);
            }

            if (pollArr[SHL].revents & POLLHUP || pollArr[SHL].revents & POLLERR) {
                exit(ERROR);
            }
        }

    } 
    else if (cpid == 0) {  /* kid */
        /* setup piping to/from terminal */
        extClose(servToShell[WR]);  /* child only reads from this pipe */
        extClose(shellToServ[RD]);  /* child only writes to this pipe */

        fdRedirect(servToShell[RD], STDIN_FILENO);  /* read to stdin */
        extClose(servToShell[RD]);                     

        fdRedirect(shellToServ[WR], STDOUT_FILENO);  /* write to stdout/err */
        fdRedirect(shellToServ[WR], STDERR_FILENO);
        extClose(shellToServ[WR]);

        /* shell process takes over */
        if (execl(program, program, NULL) == -1) {
            fprintf(stderr, "Exec failed: %s", strerror(errno));
            exit(ERROR);
        }

    } 
    else if (cpid == -1){  /* failed */
        fprintf(stderr, "Fork failed: %s", strerror(errno));
        exit(ERROR); /* normal exit bc waitpid in shutdown will spiral */
    }


}