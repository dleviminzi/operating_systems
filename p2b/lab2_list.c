#include "SortedList.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <gperftools/profiler.h>

#define ERROR2 2
#define ERROR1 1
#define SUCCESS 0
#define BILLION 1E9

/* list and elements */
SortedList_t *list;
SortedListElement_t *elements;

/* yield option specifier */
int opt_yield = 0;

/* mutex lock parameters */
pthread_mutex_t mutexLock;
int mutexFlg = 0;

/* spin lock */
int spinlock = 0;
int spinFlg = 0;

/* struct to pass arguments to thread calling function */
struct threadArgs {
    int threadNum;
    int *numThreads;
    int *numElements;
};

/* profiling flag */
int profileFlg = 0;

/* method to lock thread when needed */
long lock(struct timespec *startTime, struct timespec *endTime) {

    if (mutexFlg) pthread_mutex_lock(&mutexLock);
    else if (spinFlg) {
        while (__sync_lock_test_and_set(&spinlock, 1));
    }

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, endTime);

    return (endTime->tv_nsec - startTime->tv_nsec) + (endTime->tv_sec - startTime->tv_sec) * BILLION;
}

/* method to unlock thread when needed */
void unlock() {
    if (mutexFlg) pthread_mutex_unlock(&mutexLock);
    else if (spinFlg) __sync_lock_release(&spinlock);
}

/* function to be called when threading */
void *threadCall(void *threadArgs) {
    struct threadArgs *currArgs;
    currArgs = (struct threadArgs *) threadArgs;

    int threadNum = currArgs->threadNum;
    int numThreads = *(currArgs->numThreads);
    int numElements = *(currArgs->numElements);

    long waitTime = 0;

    struct timespec startTime, endTime;

    /* inserting elements into the list */
    int i;
    for (i = threadNum; i < numElements; i += numThreads) {
        waitTime += lock(&startTime, &endTime);
        SortedList_insert(list, &elements[i]);
        unlock();
    }

    /* check size of list */
    waitTime += lock(&startTime, &endTime);

    int length = 0;
    length = SortedList_length(list);
    if (length < 0) {
        fprintf(stderr, "Failed to get length of list: corruption.\n");
        exit(ERROR2);
    }

    unlock();

    /* lookup and delete elements that were added to list */
    for (i = threadNum; i < numElements; i += numThreads) {
        waitTime += lock(&startTime, &endTime);

        SortedListElement_t *element;
        if ((element = SortedList_lookup(list, elements[i].key)) == NULL) {
            fprintf(stderr, "Could not find element in list: corruption.\n");
            exit(ERROR2);
        }

        if (SortedList_delete(element) != 0) {
            fprintf(stderr, "Could not delete element from list: corruption.\n");
            exit(ERROR2);
        }

        unlock();
    } 

    pthread_exit((void *) waitTime);
}

/* handler for segmentation faults */
void handler(int sig) {
    if (sig == SIGSEGV) {
        fprintf(stderr, "Segmentation fault occured");
        exit(ERROR2);
    }
}

/* key gen */
void randomKey(char *key) {
    char options[] = "0123456789!\"#$%&'()*+-/.ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    int length = rand()%15;

    key[length] = '\0';

    /* character by character key creation */
    while (length) {
        int index = (double) rand() / RAND_MAX * (sizeof options - 1);
        key[--length] = options[index];
    }
}

int main(int argc, char *argv[]) {
    /* register seg fault handler */
    signal(SIGSEGV, handler);

    /* initializing getopt variables/struct */
    int opt = 0;           
    int option_index = 0; 

    static struct option long_options[] = {
        {"threads", required_argument, NULL, 't'},
        {"iterations", required_argument, NULL, 'i'},
        {"sync", required_argument, NULL, 's'},
        {"yield", required_argument, NULL, 'y'},
        {"profile", no_argument, &profileFlg, 1},
        {0,0,0,0}
    };    

    /* initializing structs for start and end time */
    struct timespec startTime, endTime;

    /* default thread/iteration/element values*/
    int numThreads = 1;
    int numIterations = 1;
    int numElements = 1;

    char *yldOpts;

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
                yldOpts = strdup(optarg);

                unsigned long i;
                for (i = 0; i < strlen(optarg); ++i) {
                    if (optarg[i] == 'i') {
                        opt_yield |= INSERT_YIELD;
                    }
                    else if (optarg[i] == 'd') {
                        opt_yield |= DELETE_YIELD;
                    }
                    else if (optarg[i] == 'l') {
                        opt_yield |= LOOKUP_YIELD;
                    }
                    else {
                        fprintf(stderr, "Invalid option provided for yield. Only i, d, l are valid");
                        exit(ERROR1);
                    }
                } 
                break;
            case 's':
                switch(optarg[0]) {
                    case 'm':
                        mutexFlg = 1;
                        break;
                    case 's':
                        spinFlg = 1;
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

    /* check for/initialize mutex lock */
    if (mutexFlg) {
        pthread_mutex_init(&mutexLock, NULL);
    }

    /* check for/start profiling */
    if (profileFlg) {
        ProfilerStart("profile.prof");
    }

    /* init list of elements */
    list = (SortedList_t *) malloc(sizeof(SortedList_t)); 
    list->next = list;
    list->prev = list;
    list->key = NULL;

    /* init elements */
    numElements = numIterations * numThreads;
    elements = (SortedListElement_t*) malloc(sizeof(SortedListElement_t) * numElements); 

    int i;
    for (i = 0; i < numElements; ++i) {
        char key[15];
        randomKey(key);
        elements[i].key = key;
    }

    /* get starting time */
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &startTime);

    /* initializing array of threads and setting attributes */
    pthread_t threads[numThreads];
    struct threadArgs tArgs[numThreads];

    /* init attributes of threads */
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); 

    /* creating threads and sending em off to add stuff */  
    int rc, t;

    for (t = 0; t < numThreads; t++) {
        /* argument to be passed with function jump point */
        tArgs[t].threadNum = t;
        tArgs[t].numElements = &numElements;
        tArgs[t].numThreads = &numThreads;

        if ((rc = pthread_create(&threads[t], NULL, threadCall, (void *) &tArgs[t])) < 0) {
            fprintf(stderr, "Thread creation failed with error code: %d\n", rc);
            exit(ERROR1);
        }
    }

    /* freeing attrribute and waiting for the other threads */
    pthread_attr_destroy(&attr); 

    /* calculating total lock wait time */
    long totalWaitTime = 0;
    void **waitTime = (void *) malloc(sizeof(void **));

    for (t = 0; t < numThreads; ++t) {
        if ((rc = pthread_join(threads[t], waitTime))) {
            fprintf(stderr, "Thread joining failed with error code: %d\n", rc);
            exit(ERROR1); 
        }

        totalWaitTime += (long) *waitTime;
    }

    /* get ending time */
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &endTime);

    if (mutexFlg) {
        pthread_mutex_destroy(&mutexLock);
    } 

    /* checking length of list */
    if (SortedList_length(list) != 0) {
        fprintf(stderr, "List was corrupted.");
        exit(ERROR2);
    }

    if (profileFlg) {
        ProfilerStop();
    }

    free(elements);
    free(list);

    char title[65] = "list-";

    /* formatting for yield options */
    if (opt_yield == 0) {
        strcat(title, "none");
    }
    else {
        strcat(title, yldOpts);
    }
    
    /* formatting for locking mechanism */
    if (mutexFlg) {
        strcat(title, "-m");
    }
    else if (spinFlg) {
        strcat(title, "-s");
    }
    else {
        strcat(title, "-none");
    }

    /* creating output for the csv file */
    int totalOp = numThreads * numIterations * 3;

    long long runTime = (endTime.tv_nsec - startTime.tv_nsec) + 
                        (endTime.tv_sec - startTime.tv_sec) * BILLION;
    
    long long timePerOperation = runTime/totalOp;

    long long operationsPerSec = BILLION/timePerOperation;

    fprintf(stdout, "%s,%d,%d,%d,%d,%lld,%lld,%lld,%ld\n", title, numThreads, 
            numIterations, 1, totalOp, runTime, timePerOperation, 
            operationsPerSec, totalWaitTime);

    exit(SUCCESS);
}