#include "typesAndSort.h"

int strToInt(char* str)
{
    int res = 0;
    for (int i = 0; i < strlen(str); i++)
        if (str[i] >= '0' && str[i] <= '9') res = 10 * res + str[i] - '0';
    
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