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
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define TRUE 1
#define FALSE 0
#define ERROR 1
#define SUCCESS 0
#define SERV 1

#define BUFF_SIZE 256

mraa_aio_context tempSensor;
mraa_gpio_context button;
const int B = 4275;               // B value of the thermistor
const int R0 = 100000;
//int tempSensor;
//int button;

char scale;      /* temp in f or c */
int logFD;            /* file descriptor of logfile */
int off;
int reporting;
int period;

/* new stuff for 4c */
char *id = "";
int port = -1;
char* host = "";
int sock;

struct hostnet *server;
struct sockaddr_in serv_addr;
struct hostent* serv_host;

void prnt(char *toPrint, int serv) {
    if (serv) {
        dprintf(sock, "%s\n", toPrint);
    }
    fprintf(stderr, "%s\n", toPrint);
    dprintf(logFD, "%s\n", toPrint);
}

void printTime() {
    time_t raw;
    struct tm* Time;
    time(&raw);
    Time = localtime(&raw);

    char toPrint[160];
    sprintf(toPrint, "%.2d:%.2d:%.2d ", Time->tm_hour, Time->tm_min, Time->tm_sec);
    prnt(toPrint, SERV);
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
        prnt(cmd, 0);
        scale = 'F';
    }
    else if (strcmp(cmd, "SCALE=C") == 0) {
        prnt(cmd, 0);
        scale = 'C';
    }
    else if (strncmp(cmd, "PERIOD=", 7) == 0) {
        prnt(cmd, 0);
        period = (int) atoi(cmd+7);
    }
    else if (strcmp(cmd, "STOP") == 0) {
        prnt(cmd, 0);
        reporting = FALSE;
    }
    else if (strcmp(cmd, "START") == 0) {
        prnt(cmd, 0);
        reporting = TRUE;
    }
    else if (strcmp(cmd, "OFF") == 0) {
        prnt(cmd, 0);
        shutdown();
    }
    else if (strncmp(cmd, "LOG", 3) == 0) {
        prnt(cmd, 0);
    }
    else {
        fprintf(stderr, "Command not valid");
        exit(ERROR);
    }
}

void handle_input(char *input) {
    int ret = read(sock, input, 256);
    if (ret > 0) {
        input[ret] = 0;
    }
    
    char *start = input;

    while (start < &input[ret]) {
        char *end = start;

        while(start < &input[ret] && *end != '\n') {
            end++;
        }

        *end = 0;

        command(start);
        start = &end[1];
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
        {"id", required_argument, NULL, 'i'},
        {"host", required_argument, NULL, 'h'},
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
            case 'i':
                id = optarg;
                break;
            case 'h':
                host = optarg;
                break;
            case '?':
                fprintf(stderr, "Unknown option. Permitted option: shell");
                exit(ERROR);
            case ':': 
                fprintf(stderr, "Invalid argument provided for option."); 
                exit(ERROR);
        }
    }
    
    /* collect port number */
    if (optind < argc) {
        port = atoi(argv[optind]);
        if (port <= 0) {
            fprintf(stderr, "Port number must be greater than 0");
            exit(ERROR);
        }
    }

    /* check that mandatory options were given/valid */
    if (strlen(host) == 0 || strlen(id) != 9 || logFD == 0) {
        fprintf(stderr, "Invalid or missing mandatory arguments: host, id, log");
        exit(ERROR);
    }

    /* connect to server via TCP */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        fprintf(stderr, "Could not initiate socket.");
        exit(ERROR);
    }
    else {
        server = gethostbyname(host);
        
        if (server == NULL) {
            fprintf(stderr, "Host was not found.");
        }
    }

    bzer((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)(serv_host->h_addr), (char *)&serv_addr.sin_addr.s_addr, serv_host->h_length);
    serv_addr.sin_port = htons(port);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(stderr, "Client failed to connect.");
        exit(ERROR);
    }

    char out[80];
    sprintf(out, "ID=%s", id);
    prnt(out, SERV);

    if ((tempSensor = mraa_aio_init(1)) == NULL) {
        mraa_deinit();
        fprintf(stderr, "Failed to initialize temperature sensor.");
        exit(ERROR);
    }

    pthread_t tSensor;

    /* init attributes of threads */
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); 

    pthread_create(&tSensor, NULL, getTemp, &period);

    /* poll for input from server */
    struct pollfd pIn;
    pIn.fd = sock;
    pIn.events = POLLIN;

    char *input;
    input = (char *)malloc(1024 * sizeof(char));
    
    while(1) {
        int ret = poll(&pIn, 1, 0);
        if (ret) {
            handle_input(input);
        }
    }

    int rc;
    if ((rc = pthread_join(tSensor, NULL))) {  /* join temp thread */
        fprintf(stderr, "Thread joining failed with error code: %d\n", rc);
        exit(ERROR); 
    }

    mraa_aio_close(tempSensor);
    close(logFD);

    exit(SUCCESS);
}