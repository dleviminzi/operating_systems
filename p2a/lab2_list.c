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

#define ERROR2 2
#define ERROR1 1
#define SUCCESS 0
#define BILLION 1E9


SortedList_t *list;
SortedListElement_t *elements;

int opt_yield = 0;

/* mutex lock parameters */
pthread_mutex_t mutexLock;
int mutexFlg = 0;

/* spin lock */
int spinlock = 0;
int spinFlg = 0;

/* compare and swap parameters (just flg) */
int compswapFlg = 0;

/* struct to pass arguments to thread calling function */
struct threadArgs {
    SortedListElement_t *startElement;
    //SortedList_t *list;
    int numToInsert;
};

void lock() {
    if (mutexFlg) pthread_mutex_lock(&mutexLock);
    else if (spinFlg) {
        while (__sync_lock_test_and_set(&spinlock, 1));
    }
}

void unlock() {
    if (mutexFlg) pthread_mutex_unlock(&mutexLock);
    else if (spinFlg) __sync_lock_release(&spinlock);
}

/* function to be called when threading */
void *threadCall(void *threadArgs) {
    /* gathering arguments passed to thread  */
    struct threadArgs *currArgs;
    currArgs = (struct threadArgs *) threadArgs;

    //SortedList_t *list = currArgs->list;
    SortedListElement_t *startElement = currArgs->startElement;
    int numToInsert = currArgs->numToInsert;

    /* inserting elements into the list */
    int i;
    for (i = 0; i < numToInsert; ++i) {
        lock();
        SortedList_insert(list, (SortedListElement_t *) startElement+i);
        unlock();
    }
    /* check size of list */
    lock();
    int length = SortedList_length(list);
    unlock();
    if (length < 0) {
        fprintf(stderr, "Failed to get length of list: corruption.\n");
        exit(ERROR2);
    }
        
    lock();
    /* delete elements that were added to list */
    for (i = 0; i < numToInsert; ++i) {
        SortedListElement_t *element;
        if ((element = SortedList_lookup(list, (startElement+i)->key)) == NULL) {
            fprintf(stderr, "Could not find element in list: corruption.\n");
            exit(ERROR2);
        }

        if (SortedList_delete(element) != 0) {
            fprintf(stderr, "Could not delete element from list: corruption.\n");
            exit(ERROR2);
        }
    } 
    unlock();

    pthread_exit(NULL);
}

void handler(int sig) {
    if (sig == SIGSEGV) {
        fprintf(stderr, "Segmentation fault occured");
        exit(ERROR2);
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
        {0,0,0,0}
    };    

    /* default thread/iteration values*/
    int numThreads = 1;
    int numIterations = 1;
    int elementsPerThread = 1;

    /* initializing structs for start and end time */
    struct timespec startTime, endTime;

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
                    if (optarg[i] == 'i') opt_yield |= INSERT_YIELD;
                    else if (optarg[i] == 'd') opt_yield |= DELETE_YIELD;
                    else if (optarg[i] == 'l') opt_yield |= LOOKUP_YIELD;
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

    if (mutexFlg) {
        pthread_mutex_init(&mutexLock, NULL);
    }

    int numElements = numIterations * numThreads;

    list = (SortedList_t *) malloc(sizeof(SortedList_t)); 
    list->next = list;
    list->prev = list;
    list->key = NULL;

    elements = (SortedListElement_t*) malloc(sizeof(SortedListElement_t) * numElements); 

    int i;
    for (i = 0; i < numElements; ++i) {

        /* generating random key */
        char key[2];
        key[0] = (char) rand()%26 + 65;
        key[1] = '\0';

        elements[i].key = key;
    }

    /* get starting time */
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &startTime);

    /* initializing array of threads and setting attributes */
    pthread_t threads[numThreads];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    /* creating threads and sending em off to add stuff */  
    int rc, t;

    for (t = 0; t < numThreads; t++) {
        /* argument to be passed with function jump point */
        struct threadArgs tArgs;
        tArgs.numToInsert = elementsPerThread;
        tArgs.startElement = elements + t*elementsPerThread;
        //tArgs.list = list;

        if ((rc = pthread_create(&threads[t], NULL, threadCall, (void *) &tArgs))) {
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
        pthread_mutex_destroy(&mutexLock);
    } 

    if (SortedList_length(list) != 0) {
        fprintf(stderr, "List was corrupted.");
        exit(ERROR2);
    }

    free(elements);
    free(list);

    char title[65] = "list-";

   /* if ((opt_yield & INSERT_YIELD) && (opt_yield & LOOKUP_YIELD)) strcat(title, "il");
    else if ((opt_yield & DELETE_YIELD) && (opt_yield & LOOKUP_YIELD)) strcat(title, "dl");
    else if (opt_yield & INSERT_YIELD) strcat(title, "i");
    else if (opt_yield & DELETE_YIELD) strcat(title, "i");*/
    if (opt_yield == 0) strcat(title, "none");
    else strcat(title, yldOpts);
    if (mutexFlg) strcat(title, "-m");
    else if (spinFlg) strcat(title, "-s");
    else if (compswapFlg) strcat(title, "-c");
    else strcat(title, "-none");

    int totalOp = numThreads * numIterations * 3;

    long long runTime = (endTime.tv_nsec - startTime.tv_nsec) + 
                      (endTime.tv_sec - startTime.tv_sec) * BILLION;

    fprintf(stdout, "%s,%d,%d,%d,%d,%lld,%lld\n", title, numThreads, 
            numIterations, 1, totalOp, runTime, runTime/totalOp);

    pthread_exit(SUCCESS);
}