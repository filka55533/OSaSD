#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#define ERROR_CODE -1

void writeInFile(char* str)
{ 
    int f = open(str, O_WRONLY | O_CREAT, 0b110100100);
    if (f == ERROR_CODE){
        fputs("Error! Couldn't open file", stderr);
        return;
    } 

    char smb = 1;
    while (smb){
        fflush(stdin);
        smb = getc(stdin);
        if (smb == '\n' || smb == '\6' || smb == EOF) smb = '\0'; else fputc(smb, f);
    }
    if (close(f) == ERROR_CODE)
        fputs("Error! Couldn't close file", stderr);
}

void main(int argc, char** argv, char** envp)
{
    if (argc == 2) writeInFile(argv[1]);
    else fputs("Error! Incorrect count arguments", stderr);
}