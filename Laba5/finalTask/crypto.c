#include "crypto.h"

#define BUFFER_SIZE 1024

int strToInt(char *str)
{
    char* endPnt;
    int res = strtol(str, &endPnt, 10);

    if (errno)
        if (res == LONG_MAX || errno == ERANGE){
            fputs("Error! Overflow\n", stderr);
            res = INCORRECT_NUMBER;
        }else if (res == LONG_MIN || errno == ERANGE){
            fputs("Error! Underflow\n", stderr);
            res = INCORRECT_NUMBER; 
        }
        else {
            fputs("Error! Incorrect value\n", stderr);
            res = INCORRECT_NUMBER;
        }
    else{
        
        if (res < 2){
            fputs("Error! Number should be greater than 1\n", stderr);
            res = INCORRECT_NUMBER;
        }else        
        if (endPnt - str - strlen(str) || !strlen(str)){
            fputs("Error! Incorrect value\n", stderr);
            res = INCORRECT_NUMBER;
        }
        else if (res > 1000){
            fputs("Error! Number of threads should be less than 1000\n", stderr);
            res = INCORRECT_NUMBER;
        }
    }
    return res;
}


char* formFullPath(char* path, char* file){
    int len = strlen(path) + strlen(file);
    char* res = (char*)malloc(len + 2);
    for (int i = 0; i <= strlen(path); i++)
        res[i] = path[i];
    len = strlen(path);
    res[len] = '/';
    for(int i = 0; i <= strlen(file); i++)
        res[i + len + 1] = file[i];

    return res;
}


int isItDirectory(char* path){
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}


char* createCopyPath(char* path){
    
    char* postfix = ".ciphered.copy";
    char* res = malloc(strlen(postfix) + strlen(path) + 1);
    if (res){   
        strcpy(res, path);
        strcat(res, postfix);
    }
    return res; 

}


long long cryptFile(char* fullPath)
{
   
    long long res = 0;
    struct stat info;    
    
    if (stat(fullPath, &info)){
        perror(fullPath);
        return ERROR_OPEN_FILE;
    }

    
    int fdop = open(fullPath, O_RDWR, info.st_mode);
    if (fdop == -1){
        perror(fullPath);
        return ERROR_OPEN_FILE;
    }

    FILE* fop = fdopen(fdop, "rb+");    

    while (!feof(fop))
    {
        long prevOffset = ftell(fop);        

        unsigned char buffer[BUFFER_SIZE];
        int byteRead = fread(buffer, sizeof(unsigned char), BUFFER_SIZE, fop);
        for (int i = 0; i < byteRead; i++)
            buffer[i] = (buffer[i] + 3) % 0xff;

        fseek(fop, prevOffset, SEEK_SET);

        fwrite(buffer, sizeof(unsigned char), byteRead, fop);
        if (byteRead == 0)
            break;
        res += byteRead;
    }
    
    fclose(fop);
    close(fdop);
    
    fflush(NULL);
    return res;
}


long long decryptFile(char* fullPath)
{
      
    long long res = 0;
    struct stat info;    
    
    if (stat(fullPath, &info)){
        perror(fullPath);
        return ERROR_OPEN_FILE;
    }

    
    int fdop = open(fullPath, O_RDWR, info.st_mode);
    if (fdop == -1){
        perror(fullPath);
        return ERROR_OPEN_FILE;
    }

    FILE* fop = fdopen(fdop, "rb+");    

    while (!feof(fop))
    {
        long prevOffset = ftell(fop);        

        unsigned char buffer[BUFFER_SIZE];
        int byteRead = fread(buffer, sizeof(unsigned char), BUFFER_SIZE, fop);
        for (int i = 0; i < byteRead; i++)
            buffer[i] = (buffer[i] + 0xff - 3) % 0xff;

        fseek(fop, prevOffset, SEEK_SET);

        fwrite(buffer, sizeof(unsigned char), byteRead, fop);
        if (byteRead == 0)
            break;
        res += byteRead;
    }
    
    fclose(fop);
    close(fdop);
    
    fflush(NULL);
    return res;
}


char* formDirectoryName(char* path, char* file){
    int len = strlen(path) + strlen(file);
    char* res = (char*)malloc(len + 2);
    for (int i = 0; i <= strlen(path); i++)
        res[i] = path[i];
    len = strlen(path);
    res[len] = '/';
    for(int i = 0; i <= strlen(file); i++)
        res[i + len + 1] = file[i];

    return res;
}

