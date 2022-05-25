#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ERROR_CODE -1
#define BUF_SIZE 30

struct buildTree
{
    int father;
    int son;
};

struct buildTree processes [] = {  
#define START 18
                                    {0, 1},
                                    {1, 2},
                                    {2, 3},
                                    {2, 4},
                                    {2, 5},
                                    {4, 6},
                                    {3, 7},
                                    {5, 8},
#define PROC_LEN (27 - START - 1)
                                };

struct signalCount
{
    int usrOne, usrTwo;
};

struct signalCount sigCnt = {0, 0};


int procNumber = 0;
int procOffset = -1;

pid_t pids[PROC_LEN + 1] = { 0 };

void sigTermHandler(int);

void printInfo(int isItSigOne, int isItPut)
{
    struct timespec tc;
    timespec_get(&tc, TIME_UTC);
    char buf[BUF_SIZE];
    strftime(buf, BUF_SIZE, "%D %T", gmtime(&tc.tv_sec));
    

    printf("%d  %d  %d  %d  %d  ", sigCnt.usrOne, sigCnt.usrTwo ,procNumber, pids[procNumber], getppid());
    printf(" %s SIGUSR%s  %s.%03ld \n", isItPut ? "put" : "get", isItSigOne ? "1" : "2", buf, tc.tv_nsec / 1000000);
    fflush(NULL);

}

void sigOneHandler(int sig, siginfo_t * info, void * ptr) {
    
    if (procOffset < PROC_LEN){       
        
        procOffset++;
        
        if (info != NULL)
            pids[processes[procOffset - 1].son] = info->si_pid;

        if (processes[procOffset].father == procNumber && procOffset < PROC_LEN){
            
            pids[procNumber] = getpid();
            pid_t pid = fork();

            switch (pid)
            {
                case 0:
                    
                    procNumber = processes[procOffset].son;
                                    
                    pids[procNumber] = getpid();           
                    kill(0, SIGUSR1);
                    break;
                
                case -1:

                    pids[processes[procOffset].son] = ERROR_CODE;
                    kill(0, SIGUSR1);
                    break;

                default:
                    break;

            }

        }

    }else{
       
        sigCnt.usrOne++;
        printInfo(1, 0);
        
        switch (procNumber){
            
            case 8:
                printInfo(1, 1);
                kill(-pids[5], SIGUSR1);
                break;

            case 5:
                printInfo(0, 1);
                kill(-pids[2], SIGUSR2);
                break;

        } 

    }


}

void sigTwoHandler(int sig, siginfo_t * info, void * ptr) {
    sigCnt.usrTwo++;
    printInfo(0, 0);
    
    switch (procNumber){
        case 2:

            printInfo(0, 1);
            kill(pids[1], SIGUSR2);
            break;

        case 1:

            if (sigCnt.usrTwo == 101){
                sigTermHandler(0);
            }else{
                printInfo(1, 1);
                kill(-pids[7], SIGUSR1);
            }

            break;

    }
}

void sigTermHandler(int sig)
{
    for (int i = 0; i <= PROC_LEN; i++){
        if (processes[i].father == procNumber){
            kill(pids[processes[i].son], SIGTERM);
            waitpid(pids[processes[i].son], NULL, 0);
        }
    }
    
    printf("\nPID:%d  PPID:%d ends work after %d SIGUSR1 and %d SIGUSR2\n", pids[procNumber], getppid(), sigCnt.usrOne, sigCnt.usrTwo);    
    fflush(NULL);

    exit(0);
}

void initParent()
{
    //SA_SIGUSR 1 set handler
    struct sigaction new;
    memset(&new, 0, sizeof(new));
    sigemptyset(&new.sa_mask);

    sigaddset(&new.sa_mask, SIGUSR1);
    sigaddset(&new.sa_mask, SIGUSR2);
    sigaddset(&new.sa_mask, SIGTERM);

    new.sa_flags = SA_SIGINFO;
    new.sa_sigaction = sigOneHandler;
    sigaction(SIGUSR1, &new, NULL);

    //SA_SIGUSR 2 set handler
    memset(&new, 0, sizeof(new));
    sigemptyset(&new.sa_mask);

    sigaddset(&new.sa_mask, SIGUSR1);
    sigaddset(&new.sa_mask, SIGUSR2);
    sigaddset(&new.sa_mask, SIGTERM);

    new.sa_flags = SA_SIGINFO;
    new.sa_sigaction = sigTwoHandler;
    sigaction(SIGUSR2, &new, NULL);

    //SIG_TERM set handler
    signal(SIGTERM, sigTermHandler);
    signal(SIGINT, sigTermHandler);
    
}


void initGroups()
{
    pid_t pid;
    setpgid(pids[procNumber], pids[procNumber]);
    switch (procNumber)
    {
        case 7:
        case 8:
            pid = 7;    
            break;

        case 6:
        case 5:
            pid = 5;
            break;
        
        case 4:
        case 3:
        case 2:
            pid = 2;
            break;

        default:
            pid = procNumber;
            break;
    }

     
    if (procNumber == 3 || procNumber == 4){
        printf("I am proc %d\n", procNumber);
        for (int i = 0; i <= PROC_LEN; i++)
            printf("P%d : %d\n", i, pids[i]);
        fflush(NULL);
    }

    while (getpgid(pids[pid]) != pids[pid]) ;
    while (setpgid(pids[procNumber], pids[pid]) == -1){
        printf("%d aaaa\n", pids[procNumber]);
        fflush(NULL);
    }
}

void synchronizeProcesses()
{
    int flag = 0;
    do{ 

        flag = getpgid(pids[7]) != pids[7] || getpgid(pids[8]) != pids[7];
        flag = flag || getpgid(pids[5]) != pids[5] || getpgid(pids[6]) != pids[5];

        // printf("p2 %d, pgid2 %d, pgid3 %d, pgid4 %d\n" ,  pids[2], getpgid(pids[2]), getpgid(pids[3]), getpgid(pids[4]));
        flag = flag || getpgid(pids[2]) != pids[2] || getpgid(pids[3]) != pids[2] || getpgid(pids[4]) != pids[2];

        fflush(NULL);
    }while (flag);
}

int main()
{
    initParent();
    sigOneHandler(0, NULL, NULL);
    
    if (procNumber == 0){
        printf("N pid ppid action signal time\n");
        fflush(NULL);
    }

    int isInited = 0;
    
    while (1)
    if ((procOffset == PROC_LEN) && !isInited ){
            
        setpgid(pids[procNumber], pids[procNumber]);
        sleep(1);
        initGroups();

        printf("%d\n", getpgid(0));
        fflush(NULL);

        if (procNumber == 0){
            
            waitpid(pids[1], NULL, 0);
        
        }else if (procNumber == 1){
            
            //Wait synchronize
            synchronizeProcesses();
            
            kill(-pids[7], SIGUSR1);
        
        }

        isInited = 1;

    }else pause();

    return 0;
}