#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include "errno.h"
#define ERROR_CODE -1
#define EXIT_CODE 6


void writeInFile(char* str)
{ 
    int f = open(str, O_WRONLY | O_CREAT, 0b110100100);
    if (f == ERROR_CODE){
        fputs("Error! Couldn't open file", stderr);
        if (errno == EACCES)
            fputs("\nNo roots", stderr);
        return;
    } 

    FILE* ff = fdopen(f, "wb");


    char smb = 1;
    while (smb){
        fflush(stdin);
        smb = getc(stdin);
        if (smb == '\n' || smb == EXIT_CODE || smb == EOF) smb = '\0';
        fputc(smb, ff);
    }
    if (fclose(ff) == ERROR_CODE)
        fputs("Error! Couldn't close file", stderr);
}

void main(int argc, char** argv, char** envp)
{
    if (argc == 2) writeInFile(argv[1]);
    else fputs("Error! Incorrect count arguments", stderr);
}