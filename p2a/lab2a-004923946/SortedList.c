#include "SortedList.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERROR2 2
#define ERROR1 1
#define SUCCESS 0

void SortedList_insert(SortedList_t *list, SortedListElement_t *element) {
    if ((list == NULL) || (element == NULL)) return;

    SortedListElement_t *curr = list;

    /* skip elements until we find insertion point */
    while (curr != list) {
        if (strcmp(element->key, curr->key) <= 0) break;
        curr = curr->next;
    }

    /* avoid meddling with list while it is being modified */
    if (INSERT_YIELD & opt_yield) sched_yield();

    curr->prev->next = element;
    element->prev = curr->prev;
    element->next = curr;
    curr->prev = element; 
}

int SortedList_delete(SortedListElement_t *element) {
    if (element == NULL) return 1;
    else if ((element->next == NULL) || (element->prev == NULL)) return 1;
    else if ((element->next->prev != element) || (element->prev->next != element))
        return 1;

    /* avoid meddling with list while it is being modified */
    if (DELETE_YIELD & opt_yield) sched_yield();

    element->prev->next = element->next;
    element->next->prev = element->prev;

    return 0;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key) {
    if (list == NULL) return NULL;

    SortedListElement_t *curr = list->next;

    while (curr != list) {
        /* avoid meddling while using parts of list actively */
        if (LOOKUP_YIELD & opt_yield) sched_yield();

        if (strcmp(curr->key, key) == 0) return curr;

        curr = curr->next;
    }
    
    return NULL;
}

int SortedList_length(SortedList_t *list) {
    if (list == NULL) return -1;

    int count = 0;

    SortedListElement_t *curr = list->next;

    while (curr != list) {
        if (curr->next == NULL || curr->prev == NULL) return -1;
        else if (curr->next->prev != curr || curr->prev->next != curr) return -1;

        /* don't want to mess with incrementation concurrently */
        if (LOOKUP_YIELD & opt_yield) sched_yield();
        curr = curr->next;
        count++;
    }

    return count;
}
