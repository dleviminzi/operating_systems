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
#define KB 0
#define SCT 1

/* GLOBAL VARIABLES */
int port;
int portFlg = 0;
struct termios restoreAttr;
int logfd;
int logFlg = 0;
int sockfd;

/* METHODS */

/* reset the terminal attributes */
void restoreTermAttributes() {
    if (tcsetattr(STDIN_FILENO, TCSANOW, &restoreAttr) != 0) {
        fprintf(stderr, "Failed to restore terminal attributes: %s\n", strerror(errno));
        exit(ERROR);
    }
}

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
void processBuff(int fd, char *buff, int lenBytes, int logType) {
    int index = 0;

/*
    if (logType == 1) {
        char *len;
        itoa(lenBytes, len);        // DAFUQ
        char *msg1 = "SENT ";
        char *msg2 = " bytes: ";
        extWrite(fd, msg1, strlen(msg1));       
        extWrite(fd, len, strlen(len));
        extWrite(fd, msg2, strlen(msg2));
    } 
    else if (logType == 2) {
        char *len; 
        itoa(lenBytes, len);
        char *msg1 = "RECEIVED ";
        char *msg2 = " bytes: ";
        extWrite(fd, msg1, strlen(msg1));
        extWrite(fd, len, strlen(len));
        extWrite(fd, msg2, strlen(msg2));
    } */

    while (index < lenBytes) {
        char *c = buff + index;

        switch (*c) {
        case '\r':
        case '\n':
            if (fd == sockfd) {
                extWrite(fd, "\n", 1);
            } 
            else {
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

/* MAIN ROUTINE */
int main(int argc, char *argv[]) {
    int opt = 0;            /* getopt vals */
    int option_index = 0;

    /* client variables */
    struct sockaddr_in serv_addr;       /* address of server to connect to */
    struct hostent *server;

    /* buffer */
    char buff[256];

    static struct option long_options[] = {
        {"port", required_argument, NULL, 'p'},     /* mandatory */
        {"log", optional_argument, NULL, 'l'},
        {0,0,0,0}
    };

    /* Option processing */
    while ((opt = getopt_long(argc, argv, ":p:l", long_options, &option_index)) != -1) {
        switch(opt) {
            case 'p':
                port = atoi(optarg);
                portFlg = 1;
                break;
            case 'l':
                if ((logfd = open(optarg, O_CREAT | O_RDWR | O_TRUNC, 0666)) < 0) {
                    fprintf(stderr, "Failed to open file for logging: %s", strerror(errno));
                    exit(ERROR);
                }
                /* at exit close logging file ??? */
                logFlg = 1;
                break;
            case '?':
                fprintf(stderr, "Unknown option. Permitted option: shell");
                exit(ERROR);
            case ':': fprintf(stderr, "Invalid argument provided for option."); exit(ERROR);
        }
    }

    /* check that mandatory port argument was passed */
    if (!portFlg) {
        fprintf(stderr, "Failed to provide mandatory port argument");
        exit(ERROR);
    }

    /* Terminal attributes modifications (keeping a restore point) */
    if (tcgetattr(STDIN_FILENO, &restoreAttr) != 0) {
        fprintf(stderr, "Failed to retrieve terminal attributes: %s", strerror(errno));
        exit(ERROR); 
    } 

    struct termios newAttr = restoreAttr;
    newAttr.c_iflag = ISTRIP;
    newAttr.c_oflag = 0;
    newAttr.c_lflag = 0;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &newAttr) != 0) {
        fprintf(stderr, "Failed to set new terminal attributes: %s", strerror(errno));
        restoreTermAttributes();
        exit(ERROR);
    }

    /* return to regular terminal attributes when exiting */
    atexit(restoreTermAttributes);

    /* init socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "Failed to open socket: %s", strerror(errno));
        exit(ERROR);
    }

    /* init server */
    server = gethostbyname("localhost");
    if (server == NULL) {
        fprintf(stderr, "Error finding host: %s", strerror(errno));
        exit(ERROR);
    }

    /* init serv_addr components */
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port);

    /* connecting... */
    if (connect(sockfd, &serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(stderr, "Error connecting socket: %s", strerror(errno));
        exit(ERROR);
    }

    /* polling for input from keyboard or socket */
    struct pollfd pollArr[2];

    pollArr[KB].fd = STDIN_FILENO;
    pollArr[KB].events = POLLIN;

    pollArr[SCT].fd = sockfd;
    pollArr[SCT].events = POLLIN;

    while (1) {
        int events = poll(pollArr, 2, 0);

        if (events == -1) {
            fprintf(stderr, "Polling failed: %s", strerror(errno));
            exit(ERROR);
        }
        else if (events == 0) continue;

        /* keyboard input */
        if (pollArr[KB].revents & POLLIN) {
            int lenBuff = readBuff(STDIN_FILENO, buff);
            processBuff(STDOUT_FILENO, buff, lenBuff, 0);

            if (logFlg) processBuff(logfd, buff, lenBuff, 1);

            processBuff(sockfd, buff, lenBuff, 0);
        }
        /* socket input */
        if (pollArr[SCT].revents & POLLIN) {
            int lenBuff = readBuff(sockfd, buff);

            if (lenBuff == 0) {     /* server as shut down */
                exit(SUCCESS);
            }

            if (logFlg) processBuff(logfd, buff, lenBuff, 2);

            processBuff(STDOUT_FILENO, buff, lenBuff, 0);
        }

        if (pollArr[SCT].revents & POLLHUP || pollArr[SCT].revents & POLLERR) {
            exit(ERROR);
        }
   }

   return 0;
}
