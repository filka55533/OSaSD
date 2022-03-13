#ifndef TYPES_AND_SORT
    #define TYPES_AND_SORT
    #include <stdio.h>
    #include <malloc.h>
    #include <dirent.h>
    #include <sys/types.h>
    #include <unistd.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <errno.h>
    #include <string.h>
    #include <math.h>
  
    #define ALL_OK 0
    #define DIR_NOT_EXIST 1
    #define OTHER (14-12) 
    #define INCORRECT_DIRECTORY 25

    struct fileInfo{
        char* name;
        char* path;
        int size;
        int ind;
    };

    typedef struct fileInfoNode{
        struct fileInfo info;
        struct fileInfoNode *ptOnNext;
    }TFILE_INFO_NODE;


    //Functions

    int strToInt(char* str);

    int compareSize(TFILE_INFO_NODE* p1, TFILE_INFO_NODE* p2);

    int compareName(TFILE_INFO_NODE* p1, TFILE_INFO_NODE* p2);

    void sortList(TFILE_INFO_NODE** head, int (*isP1GreaterP2) (TFILE_INFO_NODE*, TFILE_INFO_NODE*) );

    void freeNode(TFILE_INFO_NODE* node);

    #include "typesAndSort.c"

#endif    