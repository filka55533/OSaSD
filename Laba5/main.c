#include "crypto.h"
#define DIRECTORY argv[1]
#define NUMBER argv[2]

typedef struct _dir_stack_element{
    DIR* dirInfo;
    char* dirName;
    struct _dir_stack_element * ptOnNext;
} dir_stack_element;


//Init global variables
static volatile int threadWorkes = 0;
static int threadMax;
pthread_mutex_t lock;
dir_stack_element * dir_stack;


void * doThreadCiphering(void * _)
{
    pthread_mutex_lock(&lock);

    pthread_t tid;
    while ((threadWorkes < threadMax) && dir_stack && !pthread_create(&tid, NULL, doThreadCiphering, NULL)) ;
        threadWorkes++;        

    int isItStarted = 0;
    struct dirent* d;
    char * fileName;
    if (dir_stack)
    do {
        do {
            do{
                d = readdir(dir_stack->dirInfo);
            }while (d && (!strcmp(d->d_name, ".") || !strcmp(d->d_name, "..")));
            
            if (!d){
                free(dir_stack->dirName);
                closedir(dir_stack->dirInfo);

                dir_stack_element * prev = dir_stack;
                dir_stack = prev->ptOnNext;
                free(prev);
            }
        
        }while (!d && dir_stack);

        if (d){
            fileName = formFullPath(dir_stack->dirName, d->d_name);
            if (isItDirectory(fileName)){

                dir_stack_element * temp = malloc(sizeof(dir_stack_element));
                temp->ptOnNext = dir_stack;
                temp->dirName = fileName;
                temp->dirInfo = opendir(fileName);

                dir_stack = temp;

            }

            if (!dir_stack->dirInfo){
                fprintf(stderr, "Error! Can not open %s\n", dir_stack->dirName);
                
                free(dir_stack->dirName);
                dir_stack_element * prev = dir_stack;
                dir_stack = prev->ptOnNext;

                free(prev);
                free(fileName);
            }else
                isItStarted = 1;        
        }

    }while (!isItStarted && dir_stack);
    pthread_mutex_unlock(&lock);
    
    
    if (dir_stack){ 
        long long bytesCiphered = cryptFile(fileName);
        if (bytesCiphered >= 0){
            printf("Thread ID: %lu  File name: %s  Bytes ciphered: %lld\n", pthread_self(), fileName, bytesCiphered);
        }
    
    }

    return NULL;
}


int main(int argc, char** argv)
{
    if (argc < 3){
        fprintf(stderr, "Error! Incorrect count arguments\nCommand: progname [directory] [processes number]\nExample: ./progname . 5\n");
        return 1;
    } 

    struct stat info;
    if (!isItDirectory(DIRECTORY)){
        fprintf(stderr, "Error! %s is not directory\n", DIRECTORY);
        return 1;
    }

    threadMax = strToInt(NUMBER);
    if (threadMax == INCORRECT_NUMBER){
        return 1;
    }   

    if (pthread_mutex_init(&lock, NULL)){
        fprintf(stderr, "Error! Can not initialize mutex\n");
        return 1;
    }

    dir_stack = malloc(sizeof(dir_stack_element));
    
    dir_stack->ptOnNext = NULL;
    dir_stack->dirInfo = opendir(DIRECTORY);

    dir_stack->dirName = malloc(strlen(DIRECTORY) + 1);
    strcpy(dir_stack->dirName, DIRECTORY);

    while (dir_stack){
        threadWorkes++;
        doThreadCiphering(NULL);
    }

    return 0;
}
