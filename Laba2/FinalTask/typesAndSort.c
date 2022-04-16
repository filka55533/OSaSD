#include "typesAndSort.h"

int strToInt(char* str)
{
    char* endPnt;
    int res = strtol(str, &endPnt, 10);

    if (errno)
        if (res == LONG_MAX || errno == ERANGE){
            fputs("Error! Overflow\n", stderr);
            res = ERROR_CODE;
        }else if (res == LONG_MIN || errno == ERANGE){
            fputs("Error! Underflow\n", stderr);
            res = ERROR_CODE; 
        }
        else {
            fputs("Error! Incorrect value\n", stderr);
            res = ERROR_CODE;
        }
    else{
        
        if (res < 0){
            fputs("Error! Number is negative\n", stderr);
            res = ERROR_CODE;
        }else        
        if (endPnt - str - strlen(str) || !strlen(str)){
            fputs("Error! Incorrect value\n", stderr);
            res = ERROR_CODE;
        }
    }
    return res;
}

int compareSize(TFILE_INFO_NODE* p1, TFILE_INFO_NODE* p2)
{
    return p1->info.size > p2->info.size ? 1 : -1;
}


int compareName(TFILE_INFO_NODE* p1, TFILE_INFO_NODE* p2)
{
    return strcmp(p1->info.name, p2->info.name) > 0;
}

TFILE_INFO_NODE* mergeSortedNodes(TFILE_INFO_NODE* p1, TFILE_INFO_NODE* p2, int (*isP1GreaterP2) (TFILE_INFO_NODE*, TFILE_INFO_NODE*) )
{
    TFILE_INFO_NODE* result;
    if (p1 == NULL)
        return p2;
    else if (p2 == NULL)
        return p1;

    if (isP1GreaterP2(p1, p2) > 0){
        result = p2;
        p2->ptOnNext = mergeSortedNodes(p1, p2->ptOnNext, isP1GreaterP2);
    }else{
        result = p1;
        p1->ptOnNext = mergeSortedNodes(p1->ptOnNext, p2, isP1GreaterP2);
    }
    return result;
}

TFILE_INFO_NODE* splitToParts(TFILE_INFO_NODE* head, TFILE_INFO_NODE** start, TFILE_INFO_NODE** end)
{
    
    TFILE_INFO_NODE* p1 = head, *p2 = head->ptOnNext;
    while (p2 != NULL)
    {
        p2 = p2->ptOnNext;
        if(p2 != NULL){
            p1 = p1->ptOnNext;
            p2 = p2->ptOnNext;
        }
    }
    
    *start = head;
    *end = p1->ptOnNext;
    p1->ptOnNext = NULL;
}


void sortList(TFILE_INFO_NODE** head, int (*isP1GreaterP2) (TFILE_INFO_NODE*, TFILE_INFO_NODE*) )
{
    TFILE_INFO_NODE* p1, *p2;
    if ((*head == NULL) || ( (*head)->ptOnNext == NULL)) return;
    
    splitToParts(*head, &p1, &p2);
    sortList(&p1, isP1GreaterP2);
    sortList(&p2, isP1GreaterP2);

    *head = mergeSortedNodes(p1, p2, isP1GreaterP2);
}

void freeNode(TFILE_INFO_NODE* node)
{
    free(node->info.name );
    free(node->info.path );
    free(node);
}