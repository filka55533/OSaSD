#include "stdio.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#define ERROR_CODE -1

//Should return non-negative number for success
int strToPositiveInt(char* str)
{
    int res = 0;
    for (int i = 0; str[i]; i++)
        if (str[i] >= '0' && str[i] <= '9')
            res = res * 10 + str[i] - '0';
        else return ERROR_CODE;
    return res;
}

//If error -- return -1
int outputLinesFromFile(FILE* f, int countLines)
{
    int lines = 0;    
    while (!feof(f) && (lines <= countLines || !countLines)){
        int smb = fgetc(f);
        if (smb == EOF){
            fputs("Error on reading file\n", stdin);
            return ERROR_CODE;
        }else{
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
    if (countLines == ERROR_CODE){
        fputs("Incorrect number of lines\n", stderr);
        return;
    }
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