#include "stdio.h"
#include "sys/types.h"
#include "unistd.h"
#include "stdlib.h"
#include "time.h"
#define BUF_SIZE 25

int main()
{
    pid_t procPID = fork();
    if (procPID == -1){
        fputs("Error on creating process\n", stderr);
        return -1;

    }else if (procPID > 0){
        procPID = fork();
        if (procPID == -1){
            fputs("Error on creating second process\n", stderr);
            return -1;
        }
    }
    struct timespec tc;
    timespec_get(&tc, TIME_UTC);
    char buf[BUF_SIZE];
    strftime(buf, BUF_SIZE, "%D %T", gmtime(&tc.tv_sec));
    
    printf("Process PID %d, parent PID %d\nTime: %s.%09ld UTC\n\n", getpid(), getppid(), buf, tc.tv_nsec);
    if (procPID != 0)
        system("ps -x");
    return 0;
}