#include "stdio.h"
#include "string.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "limits.h"
#include "errno.h"
#define ERROR_CODE -1

//Should return non-negative number for success
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

//If error -- return -1
int outputLinesFromFile(FILE* f, int countLines)
{
    int lines = 1;    
    while (!feof(f) && (lines <= countLines || !countLines)){
        int smb = fgetc(f);
        if (smb == EOF){
            fputs("Error on reading file\n", stdin);
            return ERROR_CODE;
        }
        else{
            if (smb == '\0' || smb == '\n'){
                smb = '\n';
                lines++;
            }
            fputc(smb, stdout);
        }
    }
    return 0;
}

void main(int argc, char** argv)
{
    if (argc != 3){
        fputs("Incorrect count of arguments\n", stderr);
        return;
    }
    int countLines = strToPositiveInt(argv[2]);
    if (countLines == ERROR_CODE)   return;
    
    FILE* f = fopen(argv[1], "rt");
    if (!f){
        fputs("Error! Cannot open file\n", stderr);
        return;
    }
    while (!feof(f)){
        int code = outputLinesFromFile(f, countLines);
        if (code == ERROR_CODE) return;
        fgetc(stdin);
        fflush(stdin);
    }
    if (fclose(f) == EOF)
        fputs("Error! Cannot close file\n", stderr);
}