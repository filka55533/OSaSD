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
    int fdop = open(fullPath, O_RDONLY);
    long long res = 0;
    if (fdop == -1){
        perror(fullPath);
        return ERROR_OPEN_FILE;
    }

    char* cpyPath = createCopyPath(fullPath);

    struct stat info;
    fstat(fdop, &info);
    int fdcp = open(cpyPath, O_WRONLY | O_CREAT, info.st_mode);
    if (fdcp == -1){
        perror(fullPath);
        close(fdop);
        if (cpyPath) free(cpyPath);
        return ERROR_CREATE_FILE;
    }

    FILE* fop = fdopen(fdop, "rb");
    FILE* fcp = fdopen(fdcp, "wb");    
    while (!feof(fop))
    {
        unsigned char buffer[BUFFER_SIZE];
        int byteRead = fread(buffer, sizeof(unsigned char), BUFFER_SIZE, fop);
        for (int i = 0; i < byteRead; i++)
            buffer[i] = (buffer[i] + 3) % 0xff;

        fwrite(buffer, sizeof(unsigned char), byteRead, fcp);
        res += byteRead;
    }
    
    free(cpyPath);
    fclose(fcp);
    fclose(fop);

    close(fdop);
    close(fdcp);
    
    return res;
}


long long decryptFile(char* fullPath)
{
    int fd = open(fullPath, O_RDWR);
    if (fd == -1){
        perror(fullPath);
        return ERROR_OPEN_FILE;
    }

    FILE* f = fdopen(fd, "rw");
    while (!feof(f)){
        unsigned char buffer[BUFFER_SIZE];
        long prevPos = ftell(f);

        int readedBytes = fread(buffer, sizeof(unsigned char), BUFFER_SIZE, f);
        for (int i = 0; i < readedBytes; i++)
            buffer[i] = (buffer[i] + 0xff - 3) % 0xff;
        
        fseek(f, prevPos, SEEK_SET);
        fwrite(buffer, sizeof(unsigned char), readedBytes, f);
        
    }

    return 0;
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

