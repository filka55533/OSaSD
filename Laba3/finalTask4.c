#include "stdio.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "sys/wait.h"
#include "dirent.h"
#include "unistd.h"
#include "errno.h"
#include "string.h"
#include "limits.h"
#include "stdlib.h"

#define ERROR_CODE -1 
#define BYTES_MAX_COUNT 255


#define EXIT_STATUS 0
#define STR(E) #E

// Success: [out] non-negative
// Error: [out] negative
int strToPositiveInt(char* str)
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

//Success: return non-zero
//Error: return 0
//Errors show in the functions
int isItHexFormat(char* str){
    int len = strlen(str);
    for (int i = 0; i < len; i++)
        if (!(str[i] >= '0' && str[i] <= '9') && !(str[i] >= 'A' && str[i] <= 'F') && !(str[i] >= 'a' && str[i] <= 'f')){
            fputs("Error! Incorrect symbols on the combination of bytes!\n Bytes should be represented in hex-format\n", stderr);
            return 0;
        }

    if (len > 2 * BYTES_MAX_COUNT){
        fputs("Error! Bytes count should be less or equal "STR(BYTES_MAX_COUNT), stderr);
        len = 0;
    }
    return len;    
}

char* formFullPath(char* dirName, char* fileName){
    char *c = malloc(strlen(dirName) + strlen(fileName) + 2);
    strcpy(c, dirName);
    
    if (c[strlen(c) - 1] != '/') 
        strcat(c, "/");

    strcat(c, fileName);
    return c;
}

int isRegFile(char* dirName, char* fileName){
    char* c = formFullPath(dirName, fileName); 
    int res;
    struct stat fileInfo;
    if (stat(c, &fileInfo)){
        switch (errno){   
            case EACCES:
                fputs("Error! Can\'t get permission for read ", stderr);
                break;
            case EFAULT:
                fputs("Error! Bad adress of ", stderr);
                break;
        }
        fputs(c, stderr);
        res = 0;
    }else
        res = ( (fileInfo.st_mode & S_IFMT) == S_IFREG) ? 1 : 0; 
    
    free(c);
    return res;
}

void hexStrToByteArr(char* hexStr){
    
    int arrLen = strlen(hexStr);
    arrLen = arrLen / 2 + arrLen % 2;
   
    char byte[3] = {'\0', '\0', '\0'};
    for (int i = 0; i < arrLen; i++){
        byte[0] = hexStr[2 * i];
        byte[1] = hexStr[2 * i + 1];

        hexStr[i] = strtol(byte, NULL, 16);
    }
}


//Succes: 0
int isSubsequenceEqual(char* dest, char* src, int len){
    char res;
    int i = 0;
    
    do
        res = dest[i] - src[i];
    while(!res && ++i < len);
    
    return res;
}

//Return: access -- count found subsequence
//        error -- ERROR_CODE
long long findByteSubsequence(char* byteArr, int arrLen, char* fileFullPath){
    FILE* file = fopen(fileFullPath, "rb");
    if (file == NULL){
        fprintf(stderr, "Error in opening file %s. ", fileFullPath);
        switch (errno){
            case EACCES: 
                fprintf(stderr, "Access to the file denied\n\n");
                break;
            case ENOMEM:
                fprintf(stderr, "Insufficient kernel memory\n\n");
                break;   
        }
        return ERROR_CODE;
    }
    char buffer[BYTES_MAX_COUNT];

    //Find file length
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);

    long long res = 0;

    for (long i = 0 ; i < fileSize; i++){
        fseek(file, i, SEEK_SET);
        fread(buffer, arrLen, sizeof(char), file);
        if(!isSubsequenceEqual(buffer, byteArr, arrLen))
            res++;
    }

    fclose(file);
    return res;
}

#define PROC_COUNT (argv[1])
#define DIRECTORY (argv[2])
#define SUBQ_BYTES (argv[3])
// Args: count procceses, directory, string (bytes)
int main(int argc, char** argv)
{
    if (argc < 4){
        fputs("Error! Incorrect argument count\nFormat: [processes count] [directory] [Combination of bytes in hex]\nExample: 1 ./ FF23D", stderr);
        return ERROR_CODE;
    }

    int maxProceses = strToPositiveInt(PROC_COUNT);
    if (maxProceses <= 0){
        fputs("Error! Count of proceses should be greater 0", stderr);
        return ERROR_CODE;
    }

    if (!isItHexFormat(SUBQ_BYTES)) return ERROR_CODE;

    DIR* dirDescr = opendir(DIRECTORY);
    
    //Process errors SUBQ to bytes array
    if (dirDescr == NULL){
        char* message = "";
        switch (errno){
            case EACCES: message = "Error! Permission for directory denied\n"; break;
            case ENFILE:
            case EMFILE: message = "Error! Limit on opening files\n"; break;
            case ENOENT: message = "Error! Directory does not exist\n"; break;
            case ENOTDIR: 
                fprintf(stderr, "Error! %s is not directory\n", DIRECTORY);
                break;
        }

        fputs(message, stderr);
        return ERROR_CODE;
    }
    
    struct dirent* fileInfo = NULL;
    int countProc = 0;
    
    int arrLen = strlen(SUBQ_BYTES);
    arrLen = arrLen / 2 + arrLen % 2;
    hexStrToByteArr(SUBQ_BYTES);

    //Perform 
    do{
        while (countProc < maxProceses && (fileInfo = readdir(dirDescr)) != NULL){
            if (isRegFile(DIRECTORY, fileInfo->d_name)){
                
                pid_t pid = fork();
                if (pid > 0)
                    countProc++;
                else if (pid == 0){
                    //Do main procedure
                    char* fullPath = formFullPath(DIRECTORY, fileInfo->d_name);
                    // printf("\nI start process %s file\n", fullPath);
                    long long bytesEqual = findByteSubsequence(SUBQ_BYTES, arrLen, fullPath);
                    pid_t pid = getpid();
                    
                    if (bytesEqual != ERROR_CODE)
                        printf("File: %s\nPID: %d\nFound equals subquences: %lld\n\n", fullPath, pid, bytesEqual);

                    // printf("\nI end process %s file\n", fullPath);
                    free(fullPath);

                    exit(EXIT_STATUS);
                }else{
                    fputs("Can not to start process", stderr);
                }
            }
        }

        int status;
        if (wait(&status) != ERROR_CODE ){
            if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_STATUS)
                countProc--;
        }
    }while (fileInfo != NULL);

    while (waitpid(-1, NULL, WNOHANG) != ERROR_CODE);
    
    closedir(dirDescr);
    return 0;   
}