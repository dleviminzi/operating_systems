#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <pthread.h>

#define ERROR2 2
#define ERROR1 1
#define SUCCESS 0
#define BILLION 1E9

/* provided add function */
void add(long long *pointer, long long value) {
    long long sum = *pointer + value;
    *pointer = sum;
}

/* structure to hold arguments to pass to thread function */
struct threadArgs {
    long long numIt;
    long long *ptr;
};

/* function for thread to jump into */
void *threadAdd(void *threadArgs) {
    struct threadArgs *currArgs;
    currArgs = (struct threadArgs *) threadArgs;

    long long iterations = currArgs->numIt;
    long long *ptr = currArgs->ptr;

    int i; 
    for (i = 0; i < iterations; ++i) { 
        add(ptr, 1);
        add(ptr, -1);
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

    /* initializing getopt variables/struct */
    int opt = 0;           
    int option_index = 0; 

    static struct option long_options[] = {
        {"threads", optional_argument, NULL, 't'},
        {"iterations", optional_argument, NULL, 'i'},
        {0,0,0,0}
    };    

    /* default thread/iteration/counter values */
    int numThreads = 1;
    int numIterations = 1;
    long long counter = 0;

    /* initializing structs for start and end time */
    struct timespec startTime, endTime;

    /* Option processing */
    while ((opt = getopt_long(argc, argv, ":p:l", long_options, &option_index)) != -1) {
        switch(opt) {
            case 't':
                numThreads = atoi(optarg);
                break;
            case 'i':
                numIterations = atoi(optarg);
                break;
            case '?':
                fprintf(stderr, "Unknown option. Permitted option: shell");
                exit(ERROR1);
            case ':': 
                fprintf(stderr, "Invalid argument provided for option."); 
                exit(ERROR1);
        }
    }

    /* get starting time */
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &startTime);

    /* initializing array of threads and setting attributes */
    pthread_t threads[numThreads];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    /* argument to be passed with function jump point */
    struct threadArgs tArgs;
    tArgs.numIt = numIterations;
    tArgs.ptr = &counter;

    /* creating threads and sending em off to add stuff */  
    int rc, t;
    void *status;

    for (t = 0; t < numThreads; t++) {
        if ((rc = pthread_create(&threads[t], NULL, threadAdd, (void *) &tArgs))) {
            fprintf(stderr, "Thread creation failed with error code: %d\n", rc);
            exit(ERROR1);
        }
    }

    /* freeing attrribute and wiating for the other threads */
    pthread_attr_destroy(&attr);

    for (t = 0; t < numThreads; ++t) {
        if ((rc = pthread_join(threads[t], NULL))) {
            fprintf(stderr, "Thread joining failed with error code: %d\n", rc);
            exit(ERROR1); 
        }
    }

   /* get ending time */
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &endTime); 

    long long totalOp = numThreads * numIterations * 2;

    long long runTime = (endTime.tv_nsec - startTime.tv_nsec) + 
                      (endTime.tv_sec - startTime.tv_sec) * BILLION;

    fprintf(stdout, "add-none, %d, %d, %lld, %lld, %lld, %lld\n", numThreads, 
            numIterations, totalOp, runTime, runTime/totalOp, counter);

    exit(SUCCESS);
}