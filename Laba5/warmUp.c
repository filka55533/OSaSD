#include "time.h"
#include "sys/types.h"
#include "unistd.h"
#include "pthread.h"
#include "string.h"
#include "stdio.h"
#include "errno.h"
#define BUF_SIZE 25

struct threadInfo
{
    pthread_t tid;
    char isItCorrect;
};


void printInfo(){

    struct timespec tc;
    timespec_get(&tc, TIME_UTC);
    char buf[BUF_SIZE];
    strftime(buf, BUF_SIZE, "%D %T", gmtime(&tc.tv_sec));
    
    printf("PID: %d, PPID %d, %s.%03ld\n", getpid(), getppid(), buf, tc.tv_nsec / 1000000);
    fflush(NULL);

}

void* childrenAction(void * _){
    
    printInfo();
    printf("Nya poka\n");
    fflush(NULL);

    return NULL;
}


int main(){


    struct threadInfo tids[2];
    for (int i = 0; i < 2; i++){
        
        int errorCode = pthread_create(&tids[i].tid, NULL, childrenAction, NULL);
        if (errorCode){
            fprintf(stderr, "Error on creating %d thread\n", i);
            tids[i].isItCorrect = 0;
            char* message;
            switch (errorCode)
            {
                case EAGAIN:
                    message = " Insufficient resources to create another thread.\n";
                    break;

                case EINVAL:
                    message = "Invalid settings in attr\n";
                    break;

                case EPERM:
                    message = " No permission to set the scheduling policy and parameters specified in attr\n";
                    break;
                
                default:
                    message = "";
                    break;
            }
            fprintf(stderr, "%s", message);
        }else{
            printf("I create thread %d\n", i + 1);
        }

    }

    printInfo();

    //Wait threads
    for (int i = 0; i < 2; i++){
        if (tids[i].isItCorrect)
           pthread_join(tids[i].tid, NULL);
    }

    return 0;
}   