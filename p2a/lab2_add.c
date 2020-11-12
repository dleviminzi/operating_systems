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

/* star of the show: counter variable to be updated by threads */
long long counter = 0;


/* mutex lock parameters */
pthread_mutex_t mutexSum;
int mutexFlg = 0;

/* spin lock */
int spinlock = 0;
int spinFlg = 0;

/* compare and swap parameters (just flg) */
int compswapFlg = 0;

/* provided add function */
int opt_yield = 0;
void add(long long *pointer, long long value) { 
    long long sum = *pointer + value;

    if (opt_yield)
        sched_yield();

    *pointer = sum;
}

/* mutex modified add */
void mAdd(long long *pointer, long long value) {
    pthread_mutex_lock(&mutexSum);
    
    long long sum = *pointer + value;

    if (opt_yield) sched_yield();

    *pointer = sum;

    pthread_mutex_unlock(&mutexSum);
}

/* spin lock modified add */
void sAdd(long long *pointer, long long value) {
    while (__sync_lock_test_and_set(&spinlock, 1));
    
    long long sum = *pointer + value;

    if (opt_yield) sched_yield();

    *pointer = sum; 

    __sync_lock_release(&spinlock);
}

/* compare and swap modified add */
void cAdd(long long *pointer, long long value) {
    long long old; 

    do {
        old = counter;

        if (opt_yield) sched_yield(); 
    } while (__sync_val_compare_and_swap(pointer, old, old + value) != old);
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
        if (mutexFlg) {
            mAdd(ptr, 1);
            mAdd(ptr, -1);
        } 
        else if (spinFlg) {
            sAdd(ptr, 1);
            sAdd(ptr, -1);
        }
        else if (compswapFlg) {
            cAdd(ptr, 1);
            cAdd(ptr, -1);
        } 
        else { 
            add(ptr, 1);
            add(ptr, -1);
        }
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
        {"yield", no_argument, NULL, 'y'},
        {"sync", optional_argument, NULL, 's'},
        {0,0,0,0}
    };    

    /* default thread/iteration values*/
    int numThreads = 1;
    int numIterations = 1;

    /* initializing structs for start and end time */
    struct timespec startTime, endTime;

    /* flag for syncing */
    int syncing = 0;

    /* Option processing */
    while ((opt = getopt_long(argc, argv, ":p:l", long_options, &option_index)) != -1) {
        switch(opt) {
            case 't':
                numThreads = atoi(optarg);
                break;
            case 'i':
                numIterations = atoi(optarg);
                break;
            case 'y':
                opt_yield = 1;
                break;
            case 's':
                syncing = 1;

                switch(optarg[0]) {
                    case 'm':
                        mutexFlg = 1;
                        break;
                    case 's':
                        spinFlg = 1;
                        break;
                    case 'c':
                        compswapFlg = 1;
                        break;
                    default:
                        fprintf(stderr, "Sync only allows s, m, c arguments");
                        exit(ERROR1);
                }
                break; 
            case '?':
                fprintf(stderr, "Unknown option. Permitted option: shell");
                exit(ERROR1);
            case ':': 
                fprintf(stderr, "Invalid argument provided for option."); 
                exit(ERROR1);
        }
    }

    if (mutexFlg) {
        pthread_mutex_init(&mutexSum, NULL);
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

    if (mutexFlg) {
        pthread_mutex_destroy(&mutexSum);
    } 

    long long totalOp = numThreads * numIterations * 2;

    long long runTime = (endTime.tv_nsec - startTime.tv_nsec) + 
                      (endTime.tv_sec - startTime.tv_sec) * BILLION;


    /* fucking disgusting output string stuff */
    char title[30] = "add";

    if (opt_yield) strcat(title, "-yield");
    if (mutexFlg) strcat(title, "-m");
    else if (spinFlg) strcat(title, "-s");
    else if (compswapFlg) strcat(title, "-c");
    else strcat(title, "-none");
    
    fprintf(stdout, "%s,%d,%d,%lld,%lld,%lld,%lld\n", title, numThreads, 
            numIterations, totalOp, runTime, runTime/totalOp, counter);

    pthread_exit(SUCCESS);
}