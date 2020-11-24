#include <mraa.h>
#include <mraa/gpio.h>
#include <mraa/aio.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <poll.h>

#define TRUE 1
#define FALSE 0
#define ERROR 1
#define SUCCESS 0

#define BUFF_SIZE 256

mraa_aio_context tempSensor;
mraa_gpio_context button;
const int B = 4275;               // B value of the thermistor
const int R0 = 100000;

char scale;      /* temp in f or c */
int logFD;            /* file descriptor of logfile */
int off;
int reporting;
int period;

void printTime() {
    time_t raw;
    struct tm* Time;
    time(&raw);
    Time = localtime(&raw);

    fprintf(stdout, "%.2d:%.2d:%.2d ", Time->tm_hour, Time->tm_min, Time->tm_sec);
    if (logFD != -1 && reporting) {
        dprintf(logFD, "%.2d:%.2d:%.2d ", Time->tm_hour, Time->tm_min, Time->tm_sec);
    } 
}


/* redirects newFD to targetFD */
void fdRedirect(int newFD, int targetFD) {
    close(targetFD);
    dup2(newFD, targetFD);
}

/* calculate the temperature as is done on the doc page */
double calcTemp(int temp) {
    double t = 1023.0/(double)temp - 1.0;

    t = R0*t;

    float outputTemp = 1.0/(log(t/R0)/B+1/298.15)-273.15;

    /* convert if needed */
    if (scale == 'F') {
        outputTemp = outputTemp * 9/5 + 32;
    }

    return outputTemp;
}

void reportTemp(float convTemp) {
    printTime();

    fprintf(stdout, "%.1f\n", convTemp);
    if (logFD != -1 && reporting) {
        dprintf(logFD, "%.1f\n", convTemp);
    }
}

/* thread function to gather temp and then wait */
void *getTemp(void *prd) {
    int *period = (int *) prd;

    while (!off) {
        float temp = mraa_aio_read(tempSensor);
        float convTemp = calcTemp(temp);
        reportTemp(convTemp);
        sleep(*period);
    }

    float temp = mraa_aio_read(tempSensor);
    float convTemp = calcTemp(temp);
    reportTemp(convTemp);

    pthread_exit(NULL);
}

void shutdown() {
    printTime();

    fprintf(stdout, "SHUTDOWN\n");
    if (logFD != -1) {
        dprintf(logFD, "SHUTDOWN\n");
    } 

    mraa_aio_close(tempSensor);
    mraa_gpio_close(button);
    close(logFD);

    exit(SUCCESS);
}

void command(char *cmd) {
    if (strcmp(cmd, "SCALE=F") == 0) {
        scale = 'F';
    }
    else if (strcmp(cmd, "SCALE=C") == 0) {
        scale = 'C';
    }
    else if (strncmp(cmd, "PERIOD=", 7) == 0) {
        period = (int) atoi(cmd+7);
    }
    else if (strcmp(cmd, "STOP") == 0) {
        reporting = FALSE;
    }
    else if (strcmp(cmd, "START") == 0) {
        reporting = TRUE;
    }
    else if (strcmp(cmd, "OFF") == 0) {
        shutdown();
    }
    else if (!(strncmp(cmd, "LOG", 3) == 0)) {
        fprintf(stderr, "Command not valid");
        exit(ERROR);
    }
}


int main(int argc, char* argv[]) {
    scale = 'F';
    period = 1;         /* seconds to wait between reading temp */         
    logFD = -1;
    off = FALSE;
    reporting = FALSE;

    struct pollfd inputPoll; /* specifying events to watch for */

    char buff[BUFF_SIZE];

    /* getopt variables and options */
    int opt = 0;           
    int option_index = 0; 

    static struct option long_options[] = {
        {"period", required_argument, NULL, 'p'},
        {"scale", required_argument, NULL, 's'},
        {"log", required_argument, NULL, 'l'},
        {0,0,0,0}
    };

    while ((opt = getopt_long(argc, argv, ":p:l", long_options, &option_index)) != -1) {
        switch(opt) {
            case 'p':
                period = atoi(optarg);
                break;
            case 's':
                switch(*optarg) {
                    case 'c':
                    case 'C':
                        scale = 'C';
                        break;
                    case 'f':
                    case 'F':
                        scale = 'F';
                        break;
                    default:
                        fprintf(stderr, "Scale must be either S or F. Try again.");
                        exit(ERROR);
                }
            case 'l':
                reporting = TRUE;

                if ((logFD = open(optarg, O_CREAT | O_RDWR | O_TRUNC, 0666)) <= 0) {
                    fprintf(stderr, "Error on output: %s \nProper use: --logfile=filename", strerror(errno));
                    exit(ERROR);
                }
                break;
            case '?':
                fprintf(stderr, "Unknown option. Permitted option: shell");
                exit(ERROR);
            case ':': 
                fprintf(stderr, "Invalid argument provided for option."); 
                exit(ERROR);
        }
    }

    if ((tempSensor = mraa_aio_init(1)) == NULL) {
        mraa_deinit();
        fprintf(stderr, "Failed to initialize temperature sensor.");
        exit(ERROR);
    }

    if ((button = mraa_gpio_init(60)) == NULL) {
        mraa_deinit();
        fprintf(stderr, "Failed to initialize temperature sensor.");
        exit(ERROR);
    }

    mraa_gpio_dir(button, MRAA_GPIO_IN);

    pthread_t tSensor;

    /* init attributes of threads */
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); 

    pthread_create(&tSensor, NULL, getTemp, &period);


    inputPoll.fd = STDIN_FILENO;
    inputPoll.events = POLLIN | POLLERR | POLLHUP;

    while(1) {
        /* check for shutdown button */
        if (mraa_gpio_read(button)) {
            off = TRUE;

            int rc;
            if ((rc = pthread_join(tSensor, NULL))) {  /* join temp thread */
                fprintf(stderr, "Thread joining failed with error code: %d\n", rc);
                exit(ERROR); 
            }

            shutdown();
        }

        /* perform polling */
        int events;
        if ((events = poll(&inputPoll, 1, 0)) < 0) {
            fprintf(stderr, "Polling failed");
            exit(ERROR);
        }
        else if (events == 0) {
            continue;
        }

        int lenBuff;
        if (inputPoll.revents && POLLIN) {
            if ((lenBuff = read(STDIN_FILENO, buff, BUFF_SIZE)) < 0) {
                fprintf(stderr, "Failed to read from stdin: %s", strerror(errno));
                exit(ERROR);
            }
        }
        else if (lenBuff == 0) {
            continue;
        }

        int start = 0;
        int i;

        for (i = 0; i < lenBuff; ++i) {
            if (buff[i] == '\n') {
                buff[i] = '\0';
                dprintf(logFD, "%s\n", buff + start);
                command(buff + start);
                start = i + 1;
            }
        }
    }

    mraa_aio_close(tempSensor);
    mraa_gpio_close(button);
    close(logFD);

    exit(SUCCESS);
}