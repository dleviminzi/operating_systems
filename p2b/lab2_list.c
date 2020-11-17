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

/* globale variables */
SortedListElement_t *elements;      /* elements for lists */
SortedList_t *lists;                /* list heads */
pthread_mutex_t *mutexLocks;        /* mutex locks for lists */
char **keys;
int *spinLocks;                     /* spin locks for lists */
int opt_yield = 0;                  /* yield option specifier */
int mutexFlg = 0;                   /* mutex lock flag */
int spinFlg = 0;                    /* spin lock flag */
int profileFlg = 0;                 /* profiling flag */


/* struct to pass arguments to thread calling function */
struct threadArgs {
    int threadNum;
    int numThreads;
    int numElements;
    long long *waitTime;
    int numLists;
    pthread_mutex_t **mutexLocks;
    int **spinLocks;
};

/* method to lock thread when needed */
long lock(pthread_mutex_t *mutexLock, int *spinLock) {
    struct timespec startTime, endTime;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &startTime);

    if (mutexFlg) pthread_mutex_lock(mutexLock);
    else if (spinFlg) {
        while (__sync_lock_test_and_set(spinLock, 1));
    }

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &endTime);

    return (endTime.tv_nsec - startTime.tv_nsec) + (endTime.tv_sec - startTime.tv_sec) * BILLION;
}

/* method to unlock thread when needed */
void unlock(pthread_mutex_t *mutexLock, int *spinLock) {
    if (mutexFlg) pthread_mutex_unlock(mutexLock);
    else if (spinFlg) __sync_lock_release(spinLock);
}

/* hash https://stackoverflow.com/questions/7666509/hash-function-for-string */
unsigned int hash(const char* key) {
 	int ret = 31;

 	while (*key) {
    		ret = 28 * ret ^ *key;
    		key++;
  	}

  	return ret;
}

/* function to be called when threading */
void *threadCall(void *threadArgs) {
    struct threadArgs *currArgs;
    currArgs = (struct threadArgs *) threadArgs;

    int threadNum = currArgs->threadNum;
    int numThreads = currArgs->numThreads;
    int numElements = currArgs->numElements;
    long long *waitTime = currArgs->waitTime;
    int numLists = currArgs->numLists;

    /* inserting elements into the list */
    int i;
    for (i = threadNum; i < numElements; i += numThreads) {
        /* determine which list the element will be inserted into */
        unsigned int hsh = hash(elements[i].key);

        int listToIns;
        listToIns = hsh % numLists;

        /* debug method to check distribution of elements 
        if (i < 0) {
            fprintf(stderr, "%d\n", i);
            fprintf(stderr, "%s\n", elements[i].key);
            fprintf(stderr, "%d\n", hash(elements[i].key));
            fprintf(stderr, "%d\n\n", listToIns);
        } */

        *waitTime += lock(&mutexLocks[listToIns], &spinLocks[listToIns]);
        SortedList_insert(&lists[listToIns], &elements[i]);
        unlock(&mutexLocks[listToIns], &spinLocks[listToIns]);
    }


    int length = 0;
    for (i = 0; i < numLists; ++i) {
        /* lock all lists so we can get the length */
        *waitTime += lock(&mutexLocks[i], &spinLocks[i]);

        int thisLength = SortedList_length(&lists[i]);

        /* also here to check distribution of elements 
        fprintf(stderr, "List #%d :", i);
        fprintf(stderr, "%d\n", thisLength); */

        length += thisLength;

        /* unlock all the lists now that we've counted it */
        unlock(&mutexLocks[i], &spinLocks[i]);
    }

    if (length < 0) {
        fprintf(stderr, "Failed to get length of list: corruption.\n");
        exit(ERROR2);
    }

    /* lookup and delete elements that were added to list */
    for (i = threadNum; i < numElements; i += numThreads) {
        unsigned int hsh = hash(elements[i].key);

        int listToIns;
        listToIns = hsh % numLists;

        *waitTime += lock(&mutexLocks[listToIns], &spinLocks[listToIns]);

        SortedListElement_t *element;
        if ((element = SortedList_lookup(&lists[listToIns], elements[i].key)) == NULL) {
            fprintf(stderr, "Could not find element in list: corruption.\n");
            exit(ERROR2);
        }

        if (SortedList_delete(element) != 0) {
            fprintf(stderr, "Could not delete element from list: corruption.\n");
            exit(ERROR2);
        }

        unlock(&mutexLocks[listToIns], &spinLocks[listToIns]);
    } 

    pthread_exit(NULL);
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
        {"lists", required_argument, NULL, 'l'},
        {0,0,0,0}
    };    

    /* initializing structs for start and end time */
    struct timespec startTime, endTime;

    /* default thread/iteration/element values*/
    int numThreads = 1;
    int numIterations = 1;
    int numElements = 1;
    int numLists = 1;

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
            case 'l':
                numLists = atoi(optarg);
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

    /* check for/start profiling */
    if (profileFlg) {
        ProfilerStart("profile.prof");
    }

    /* initialize array of pointers to list heads */
    lists = (SortedListElement_t *) malloc(sizeof(SortedListElement_t) * numLists);

    int i;
    for (i = 0; i < numLists; ++i) {        
        lists[i].next = &lists[i];
        lists[i].prev = &lists[i];
        lists[i].key = NULL;
    }

    /* create arrays of locks, each corresponding to a list head */
    mutexLocks = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t) * numLists);
    spinLocks = (int *) malloc(sizeof(int)* numLists);

    for (i = 0; i < numLists; ++i) {
        if (mutexFlg) {
            pthread_mutex_init(&mutexLocks[i], NULL);
        }
        else if (spinFlg) {
            spinLocks[i] = 0;
        }
    }

    /* init elements */
    numElements = numIterations * numThreads;
    elements = (SortedListElement_t*) malloc(sizeof(SortedListElement_t) * numElements); 
    keys = (char **) malloc(sizeof(char*) * numElements);

    for (i = 0; i < numElements; ++i) {
        keys[i] = (char *) malloc(sizeof(char) * 15);
        randomKey(keys[i]);
        elements[i].key = keys[i];
    }

    /* get starting time */
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &startTime);

    /* initializing array of threads and setting attributes */
    pthread_t threads[numThreads];
    struct threadArgs tArgs[numThreads];
    long long waitTimes[numThreads];

    /* init attributes of threads */
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); 

    /* creating threads and sending em off to add stuff */  
    int rc, t;

    for (t = 0; t < numThreads; t++) {
        waitTimes[t] = 0;

        /* argument to be passed with function jump point */
        tArgs[t].threadNum = t;
        tArgs[t].numElements = numElements;
        tArgs[t].numThreads = numThreads;
        tArgs[t].waitTime = &waitTimes[t];
        tArgs[t].numLists = numLists;

        if ((rc = pthread_create(&threads[t], NULL, threadCall, (void *) &tArgs[t])) < 0) {
            fprintf(stderr, "Thread creation failed with error code: %d\n", rc);
            exit(ERROR1);
        }
    }

    /* freeing attrribute and waiting for the other threads */
    pthread_attr_destroy(&attr); 

    for (t = 0; t < numThreads; ++t) {
        if ((rc = pthread_join(threads[t], NULL))) {
            fprintf(stderr, "Thread joining failed with error code: %d\n", rc);
            exit(ERROR1); 
        }
    }

    /* get ending time */
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &endTime);

    int finalLength = 0;

    for (i = 0; i < numLists; ++i) {
        if (mutexFlg) {
            pthread_mutex_destroy(&mutexLocks[i]);
        }

        finalLength += SortedList_length(&lists[i]);
    }

    /* checking length of list */
    if (finalLength != 0) {
        fprintf(stderr, "List was corrupted.");
        exit(ERROR2);
    }

    if (profileFlg) {
        ProfilerStop();
    }

    /* calculating total lock wait time */
    long long totalWaitTime = 0;
    for (t = 0; t < numThreads; ++t) {
        totalWaitTime += waitTimes[t];
    }

    free(elements);
    free(keys);
    free(lists);
    free(mutexLocks);
    free(spinLocks);

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

    totalWaitTime = totalWaitTime/totalOp;

    fprintf(stdout, "%s,%d,%d,%d,%d,%lld,%lld,%lld\n", title, numThreads, 
            numIterations, numLists, totalOp, runTime, timePerOperation, 
            totalWaitTime);

    exit(SUCCESS);
}