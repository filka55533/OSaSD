#ifndef CRYPTO_H
    #define CRYPTO_H

    //Error codes
    
    #define ERROR_OPEN_FILE -1
    #define INCORRECT_PATH -2
    #define INCORRECT_NUMBER -3
    #define ERROR_CREATE_FILE -4
    

    //include libraries
    #include "stdlib.h"
    #include "stdio.h"
    #include "string.h"
    #include "pthread.h"

    #include "sys/types.h"
    #include "sys/stat.h"
    #include "dirent.h"
    #include "fcntl.h"
   
    #include "errno.h"
    #include "malloc.h"
    #include "unistd.h"
    #include "limits.h"
   

    /*Crypt -- for MONOLIT!1!!
      All checks inside function
    */
    long long cryptFile(char* fullPath);

    long long decryptFile(char* fullPath);

    //In error -- return negative
    int strToInt(char* str);

    //Errors can be only with malloc
    char* formFullPath(char* path, char* file);

    //Use like bool
    int isItDirectory(char* path);


    char* formDirectoryName(char* path, char* file);


    #include "crypto.c"
#endif
