#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define ERROR_CODE -1

void main(int argc, char** argv)
{
    if (argc != 3){
        fputs("Error! Icorrect count of arguments\n", stderr);
        return;
    }

    int f1 = open(argv[1], O_RDONLY);
    FILE* ff1;
    if (f1 == ERROR_CODE || !(ff1 = fdopen(f1, "rb"))){
        fputs("Error! Can not open first file\n", stderr);
        return;
    }

    struct stat info;
    if (fstat(f1, &info) == ERROR_CODE){
        fputs("Error! Can not get info of the first file\n",stderr);
        return;
    }

    FILE* ff2;
    int f2 = open(argv[2], O_WRONLY | O_CREAT, info.st_mode);
    if (f2 == ERROR_CODE || !(ff2 = fdopen(f2, "wb"))){
        fputs("Error! Can not open second file", stderr);
        return; 
    }
    int smb;
    while ((smb = fgetc(ff1)) != EOF){
        if (smb != fputc(smb, ff2)){
            fputs("Error! Can not write byte\n", stderr);
            break;
        }
    }
    if (fclose(ff1) != 0) fputs("Error! Can not close the first file\n", stderr);
    if (fclose(ff2) != 0) fputs("Error! Can not close the second file\n", stderr);
}