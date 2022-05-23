#include <signal.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

struct buildTree
{
    int father;
    int son;
};

struct buildTree processes [] = {  
#define START 14
                                    {0, 1},
                                    {1, 2},
                                    {2, 3},
                                    {2, 4},
                                    {2, 5},
                                    {4, 6},
                                    {3, 7},
                                    {5, 8},
#define PROC_LEN (23 - START - 1)
                                };

int currentNode = 0;
int currentOffset = -1;

void sigHandler(int sg)
{
    currentOffset++;
    if (currentOffset >= PROC_LEN)
        return; 


    if (processes[currentOffset].father == currentNode){
        
        printf("I\'m proccess %d create proccess %d\n", currentNode, processes[currentOffset].son);
        
        switch(fork()){
            case 0:
                currentNode = processes[currentOffset].son;
                printf("I\'m process %d was created by father %d\n", currentNode, processes[currentOffset].father);

                kill(0, SIGUSR1);                
                break;
            case -1:
                perror("Oops! I can\'t to create\n");

                kill(0, SIGUSR1);
                break;
            default: 

        }
    }
}

int main()
{
    signal(SIGUSR1, sigHandler);
    kill(0, SIGUSR1);
    
    while (1) ;

    return 0;
}