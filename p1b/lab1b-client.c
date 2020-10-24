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
#define KB 0
#define SCT 1
#define BUFF_SIZE 256
#define SENDING_BYTES 1
#define RECEIVING_BYTES 2
#define NO_LOG 0

/* GLOBAL VARIABLES */
int port;
int portFlg = 0;
struct termios restoreAttr;
int logfd;
int logFlg = 0;
int sockfd;
int compFlg = 0;
z_stream out_stream;
z_stream in_stream;

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

/* logging method */
void logBuff(int direction, char *buff, int lenBytes) {
    char msg[BUFF_SIZE];

    if (direction == SENDING_BYTES) {
        sprintf(msg, "SENT %d bytes: ", lenBytes);
        extWrite(logfd, msg, strlen(msg));
        extWrite(logfd, buff, lenBytes);
        extWrite(logfd, "\n", 1);
    }
    else {
        sprintf(msg, "RECEIVED %d bytes: ", lenBytes);
        extWrite(logfd, msg, strlen(msg));
        extWrite(logfd, buff, lenBytes);
        extWrite(logfd, "\n", 1);
    }
}

/* write buffer to channel specified in file descriptor */
void processBuff(int fd, char *buff, int lenBytes) {
    int index = 0;

    while (index < lenBytes) {
        char *c = buff + index;

        switch (*c) {
        case '\r':
        case '\n':
            if (fd == sockfd) {
                /* send raw input to the socket */
                extWrite(fd, c, 1);
            } 
            else {
                /* echoing to the terminal */
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

/* close socket on exit */
void closeSCT() {
    extClose(sockfd);
}

/* close log on exit */
void closeLog() {
    extClose(logfd);
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
int main(int argc, char *argv[]) {
    int opt = 0;            /* getopt vals */
    int option_index = 0;

    /* client variables */
    struct sockaddr_in serv_addr;       /* address of server to connect to */
    struct hostent *server;

    /* buffer */
    char buff[BUFF_SIZE];
    char cBuffOut[BUFF_SIZE];
    char cBuffIn[BUFF_SIZE];

    static struct option long_options[] = {
        {"port", required_argument, NULL, 'p'},     /* mandatory */
        {"log", optional_argument, NULL, 'l'},
        {"compress", no_argument, NULL, 'c'},
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
                atexit(closeLog);
                logFlg = 1;
                break;
            case 'c':
                compFlg = 1;
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

    /* shut socket on shutdown note: atexit is executed LIFO */
    atexit(closeSCT);

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
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
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

        /* keyboard input is being received */
        if (pollArr[KB].revents & POLLIN) {

            /* compress data if enabled */
            if (compFlg) {
                int lenBuff = readBuff(STDIN_FILENO, buff);
               
                out_stream.avail_in = lenBuff;
                out_stream.next_in = (Bytef *) buff;
                out_stream.avail_out = BUFF_SIZE;
                out_stream.next_out = (Bytef *) cBuffOut;
                deflate(&out_stream, Z_SYNC_FLUSH);

                /* number of bytes occupied by compressed data */
                int lenComp = BUFF_SIZE - out_stream.avail_out;

                if (logFlg) logBuff(SENDING_BYTES, cBuffOut, lenComp);
                
                processBuff(sockfd, cBuffOut, lenComp);
                processBuff(STDOUT_FILENO, buff, lenBuff);
            }
            else {
                int lenBuff = readBuff(STDIN_FILENO, buff);

                if (logFlg) logBuff(SENDING_BYTES, buff, lenBuff);

                processBuff(sockfd, buff, lenBuff);
                processBuff(STDOUT_FILENO, buff, lenBuff);
            }

            /* output to terminal remains normal */
        }
        /* socket input is being received and must be processed */
        if (pollArr[SCT].revents & POLLIN) {

            /* decompress data if compression is enabled */
            if (compFlg) { 
                int lenBuff = readBuff(sockfd, buff);

                if (logFlg) logBuff(RECEIVING_BYTES, buff, lenBuff);

                if (lenBuff == 0) {     /* server has shut down */
                    exit(SUCCESS);
                }
 
                in_stream.avail_in = lenBuff;
                in_stream.next_in = (Bytef *) buff;
                in_stream.avail_out = BUFF_SIZE;
                in_stream.next_out = (Bytef *) cBuffIn;
                inflate(&in_stream, Z_SYNC_FLUSH);

                /* number of bytes occupied by decompressed data */
                int lenComp = BUFF_SIZE - in_stream.avail_out;

                processBuff(STDOUT_FILENO, cBuffIn, lenComp); 
            }
            else {
                int lenBuff = readBuff(sockfd, buff);

                if (lenBuff == 0) {     /* server has shut down */
                    exit(SUCCESS);
                }
                if (logFlg) logBuff(RECEIVING_BYTES, buff, lenBuff);
                processBuff(STDOUT_FILENO, buff, lenBuff);
            }
        }

        if (pollArr[SCT].revents & POLLHUP || pollArr[SCT].revents & POLLERR) {
            exit(ERROR);
        }
   }

   return 0;
}
