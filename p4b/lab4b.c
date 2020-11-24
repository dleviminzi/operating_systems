#include <mraa.h>
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

mraa_aio_context tempSensor;
mraa_gpio_context button;
const int B = 4275;               // B value of the thermistor
const int R0 = 100000;

char *scale;      /* temp in f or c */
int logFD;            /* file descriptor of logfile */
int shutdown;


struct pollfd pollArr[1]; /* specifying events to watch for */


/* redirects newFD to targetFD */
void fdRedirect(int newFD, int targetFD) {
    close(targetFD);
    dup2(newFD, targetFD);
}

/* calculate the temperature as is done on the doc page */
float calcTemp(float temp, char *scale) {
    float R = 1023.0/temp -1.0;

    R = R0*R;

    float outputTemp = 1.0/(log(R/R0)/B+1/298.15)-273.15;

    /* convert if needed */
    if (scale == 'F') {
        outputTemp = outputTemp * 9/5 + 32;
    }

    return outputTemp;
}

void reportTemp(float convTemp) {
    time_t raw;
    struct tm* Time;
    time(&raw);
    Time = localtime(&raw);

    fprintf(stdout, "%.2d:%.2d:%.2d %.1f\n", Time->tm_hour, Time->tm_min, Time->tm_sec, convTemp);
    if (logFD != -1) {
        dprintf(logFD, "%.2d:%.2d:%.2d %.1f\n", Time->tm_hour, Time->tm_min, Time->tm_sec, convTemp);
    }
}

/* thread function to gather temp and then wait */
void getTemp(void *period) {
    int period = *((int *) period);

    while (!shutdown) {
        float temp = mraa_aio_read_float(tempSensor);
        float convTemp = calcTemp(temp, scale);
        reportTemp(convTemp);
        sleep(period);
    }

    pthread_exit(NULL);
}

char *timeNow() {

}

int main(int argc, char* argv[]) {
    scale = 'F';
    int period = 1;         /* seconds to wait between reading temp */         
    logFD = -1;
    shutdown = FALSE;

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
                if (strlen(optarg) > 1 || optarg[0] != 'F' && optarg[0] != 'S') {
                    fprintf(stderr, "Scale must be either S or F. Try again.");
                    exit(ERROR);
                }

                scale = optarg[0];
                break;
            case 'l':
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

    pthread_t tempSensor;

    if ((tempSensor = mraa_aio_init(1)) == NULL) {
        mraa_deinit();
        fprintf(stderr, "Failed to initialize temperature sensor.");
        exit(ERROR);
    }

    /* init attributes of threads */
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); 

    pthread_create(&tempSensor, NULL, getTemp, &period);


    pollArr[0].fd = STDIN_FILENO;
    pollArr[0].events = POLLIN | POLLERR | POLLHUP;

    while(1) {
        /* check for shutdown button */
        if (mraa_gpio_read(button)) {
            shutdown = TRUE;
            int rc;
            if ((rc = pthread_join(tempSensor, NULL))) {  /* join temp thread */
                fprintf(stderr, "Thread joining failed with error code: %d\n", rc);
                exit(ERROR); 
            }
            shutdown();
        }

        /* perform polling */
        int ret;
        if ((ret = poll(pollArr, 1, 0) < 0) {
            fprintf(stderr, "Polling failed");
            exit(ERROR);
        }

        

    }






}