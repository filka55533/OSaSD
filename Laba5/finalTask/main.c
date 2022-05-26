#include "crypto.h"
#define DIRECTORY argv[1]
#define NUMBER argv[2]

typedef struct _dir_stack_element{
    DIR* dirInfo;
    char* dirName;
    struct _dir_stack_element * ptOnNext;
} dir_stack_element;


//Init global variables
static int threadWorkes = 0;
static int threadMax;
pthread_mutex_t lock;
dir_stack_element * dir_stack;
pthread_t* tids;


int findFreeThreadIndex(){

    int res = 0;
    int isFind = 0;

    for (; res < threadMax && !isFind; )
        if (tids[res] == 0)
            isFind = 1;
        else 
            res++;

    return isFind ? res : -1;
}


void * doThreadCiphering(void * _)
{
    pthread_mutex_lock(&lock);
    
    int threadPrev;
    char isCreated = 0;
    while ((threadPrev = findFreeThreadIndex()) >= 0 && dir_stack && threadWorkes < threadMax && !isCreated){
        
        tids[threadPrev] = pthread_self();

        isCreated = pthread_create(tids + threadPrev, NULL, doThreadCiphering, NULL) ;
        threadWorkes++;        

    }

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
        pthread_mutex_lock(&lock);
        if (bytesCiphered >= 0){
            printf("Thread ID: %lu  File name: %s  Bytes ciphered: %lld\n", pthread_self(), fileName, bytesCiphered);
            fflush(NULL);
        }
        pthread_mutex_unlock(&lock);
    }


    pthread_mutex_lock(&lock);
    
    pthread_t tid = pthread_self();
    for (int i = 0; i < threadMax; i++)
        if (tids[i] == tid){
            tids[i] = 0;
            tid = -1;
            break;
        }

    if (tid != -1){
        fprintf(stderr, "Error in free thread\n");
        fflush(NULL);
    }

    pthread_mutex_unlock(&lock);
    
    return NULL;
}


int main(int argc, char** argv)
{
    if (argc < 3){
        fprintf(stderr, "Error! Incorrect count arguments\nCommand: progname [directory] [processes number]\nExample: ./progname . 5\n");
        return 1;
    } 

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

    tids = calloc(threadMax, sizeof(pthread_t));
    printf("threadMax %d\n", threadMax);
    fflush(NULL);


    while (dir_stack){
        threadWorkes++;
        
        pthread_mutex_lock(&lock);
        int index = findFreeThreadIndex();
        if (index >= 0)
            tids[index] = pthread_self();
        pthread_mutex_unlock(&lock);
        doThreadCiphering(NULL);
    }

    for (int i = 1; i < threadMax; i++)
        pthread_join(tids[i], NULL);

    free(tids);

    return 0;
}
