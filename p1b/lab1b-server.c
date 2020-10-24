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
#include "zlib.h"

#define ERROR 1
#define SUCCESS 0
#define WR 1
#define RD 0
#define SCT 0
#define SHL 1
#define SHELL_RECEIVE 1
#define SOCK_RECEIVE 0
#define BUFF_SIZE 256

/* GLOBAL VARIABLES */
int shellFlg = 0;               /* shell flag               */
int servToShell[2];             /* pipe server -----> shell */
int shellToServ[2];             /* pipe shell -----> server */
int cpid;                       /* child process id         */
int port;                       /* port number              */
int portFlg = 0;                /* port flag                */
int sockfd, nsockfd;            /* socket file descriptors  */
int compFlg = 0;
z_stream out_stream;
z_stream in_stream;

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

/* function to shut socket on exit */
void closeSCT() {
    extClose(sockfd);   /* ? */
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

    if (toShell) {
        /* must parse character at a time to send to shell */
        int index = 0;

        while (index < lenBytes) {
            char *c = buff + index;

            switch (*c) {
                case 4:      
                    /* ^D close writing end of pipe to shell */
                    extClose(servToShell[WR]); 
                    break;
                case 3:      
                    /* ^C kill the child process */
                    if (kill(cpid, SIGINT) < 0) {
                        fprintf(stderr, "Kill failed: %s", strerror(errno));
                        exit(ERROR);
                    }                
                    break;
                case '\r':
                case '\n':
                    extWrite(fd, "\n", 1);
                    break;
                default:
                    extWrite(fd, c, 1);
                    break;
            }
            index++;
        }
    }
    else {
        /* sending to socket */
        extWrite(fd, buff, lenBytes);
    }
}

/* call deflate end */
void dfE() {
    deflateEnd(&out_stream);
}

/* call inflate end */
void ifE() {
    inflateEnd(&in_stream);
}

/* MAIN ROUTINE */
int main(int argc, char* argv[]) {
    int opt = 0;
    int option_index = 0;
    char *program;          /* variable for storing shell option input */

    /* server variables */
    socklen_t lenCli;
    struct sockaddr_in serv_addr, cli_addr;

    /* input buffer */
    char buff[BUFF_SIZE];
    char cBuffIn[BUFF_SIZE];
    char cBuffOut[BUFF_SIZE];

    static struct option long_options[] = {
        {"shell", required_argument, NULL, 's'},
        {"port", required_argument, NULL, 'p'},         /* mandatory */
        {"compress", no_argument, NULL, 'c'},
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
            case 'c':
                compFlg = 1;
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

    /* init compression streams */
    z_stream out_stream;
    z_stream in_stream;

    if (compFlg) {
        /* out stream */
        out_stream.zalloc = Z_NULL;
        out_stream.zfree = Z_NULL;
        out_stream.opaque = Z_NULL;

        if (deflateInit(&out_stream, Z_DEFAULT_COMPRESSION) < 0) {
            fprintf(stderr, "Error creating compression stream: %s", strerror(errno));
            exit(ERROR);
        }

        atexit(dfE);

        /* in stream */
        in_stream.zalloc = Z_NULL;
        in_stream.zfree = Z_NULL;
        in_stream.opaque = Z_NULL;

        if (inflateInit(&in_stream) < 0) {
            fprintf(stderr, "Failed to create decompression stream: %s", strerror(errno));
            exit(ERROR);
        }

        atexit(ifE);
    }

    /* init socket */
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

            /* socket input is being received and must be sent to shell */
            if (pollArr[SCT].revents & POLLIN) {     
                /* decompress data if compression is enabled */
                if (compFlg) { 
                    int lenBuff = readBuff(nsockfd, buff);
                    in_stream.avail_in = lenBuff;
                    in_stream.next_in = (Bytef *) buff;
                    in_stream.avail_out = BUFF_SIZE;
                    in_stream.next_out = (Bytef *) cBuffIn;
                    inflate(&in_stream, Z_SYNC_FLUSH);

                    processBuff(servToShell[WR], cBuffIn, BUFF_SIZE - in_stream.avail_out, SHELL_RECEIVE);
                }
                else {
                    int lenBuff = readBuff(nsockfd, buff);
                    processBuff(servToShell[WR], buff, lenBuff, SHELL_RECEIVE);
                }
            }
            
            /* shell data is being received and must be sent to socket */
            if (pollArr[SHL].revents & POLLIN) {     
                /* compress data if compression is enabled */
                if (compFlg) {
                    int lenBuff = readBuff(shellToServ[RD], buff);
                
                    out_stream.avail_in = lenBuff;
                    out_stream.next_in = (Bytef *) buff;
                    out_stream.avail_out = BUFF_SIZE;
                    out_stream.next_out = (Bytef *)cBuffOut;
                    deflate(&out_stream, Z_SYNC_FLUSH);

                    processBuff(nsockfd, cBuffOut, BUFF_SIZE - out_stream.avail_out, SOCK_RECEIVE);
                }
                else {
                    int lenBuff = readBuff(shellToServ[RD], buff);
                    processBuff(nsockfd, buff, lenBuff, SOCK_RECEIVE);
                }
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

    return 0;
}