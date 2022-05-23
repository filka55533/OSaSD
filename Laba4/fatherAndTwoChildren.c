#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define BUF_SIZE 25

int childNumber = 0;
int messageNumber = 0;

void printMessage(int isGet, int isItSig1, int senderPid)
{   

    struct timespec tc;
    timespec_get(&tc, TIME_UTC);
    char buf[BUF_SIZE];
    strftime(buf, BUF_SIZE, "%D %T", gmtime(&tc.tv_sec));
    
    pid_t pid = getpid();
    pid_t ppid = getppid();

    printf("%d %d %d %s.%09ld UTC ", ++messageNumber, pid, ppid, buf, tc.tv_nsec);
    printf(isGet ? "get %s" : "put %s", isItSig1 ? "SIGUSR1" : "SIGUSR2");
    
    if (senderPid != -1 && isGet)
        printf(" from %d\n", senderPid);
    else    
        printf("\n");

    fflush(NULL);
}

void childrenHandler(int sg)
{
    printMessage(1, 1, -1);
   
    kill(getppid(), SIGUSR2);
    printMessage(0, 0, -1);    
}


void terminateChild(int sg)
{
    exit(0);
}


void parentHandler(int sig, siginfo_t * info, void * ptr)
{
    
    printMessage(1, 0, info->si_pid);

    struct timespec tm;
    
    tm.tv_sec = 0;
    tm.tv_nsec = 100000000;
    
    nanosleep(&tm, NULL);

    printMessage(0, 1, 0);
    kill(0, SIGUSR1);
}

void initParent()
{
    struct sigaction new;
    memset(&new, 0, sizeof(new));
    sigemptyset(&new.sa_mask);
    sigaddset(&new.sa_mask, SIGUSR1);
    new.sa_flags = SA_SIGINFO;
    // sigaddset(&new.sa_mask, SIGUSR2);
    new.sa_sigaction = parentHandler;

    sigaction(SIGUSR2, &new, NULL);

    childNumber = 0;
}


int main(){

    signal(SIGUSR2, SIG_IGN);
    signal(SIGUSR1, SIG_IGN);
    
    pid_t pid = 1;
    for (int i = 1; i <= 2 && pid; i++){
        childNumber++;
        pid = fork();
        switch (pid)
        {
            case 0:
                signal(SIGUSR1, childrenHandler);
                signal(SIGTERM, terminateChild);
                break;
            
            case -1: 
                perror("fork");
                kill(0, SIGTERM);
                while (wait(NULL) != -1) ;
                break;

            default:
                if (i != 2)
                    break;
                
                initParent();
                
                

        }
    }

    if (pid){
        sleep(1);
        kill(0, SIGUSR1);
    }

    while (1) ;
    return 0;
}
