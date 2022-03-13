#include "typesAndSort.h"



char* formFileName(char* fileName, int ind)
{
    
    if (ind != 0){

        int chunk = (int)log10((double)ind) + 1;
        int prevLen = strlen(fileName);
        fileName = realloc(fileName, prevLen + chunk + 2 + 2 + 1);
        if (fileName){
            fileName[prevLen] = '_';
            fileName[++prevLen] = '_';
            fileName[prevLen] = '(';
            prevLen += chunk;
            while (ind){
                fileName[prevLen--] = ind % 10 + '0';
                ind /= 10;
            }
            prevLen += chunk;
            fileName[++prevLen] = ')';
            fileName[++prevLen] = '\0';
        }

    }
    return fileName;
}

void checkFileNames(TFILE_INFO_NODE* head)
{
   
    while (head && head->ptOnNext)
    {
        TFILE_INFO_NODE* enumNode = head->ptOnNext;
        int ind = 0;
        while(!head->info.ind && enumNode){
            if (!strcmp(enumNode->info.name, head->info.name)) 
                enumNode->info.ind = ++ind;
            enumNode = enumNode->ptOnNext;
        }
        head = head->ptOnNext;
    }
    
}

char* formDirectoryName(char* path, char* file){
    int len = strlen(path) + strlen(file);
    char* res = (char*)malloc(len + 2);
    for (int i = 0; i <= strlen(path); i++)
        res[i] = path[i];
    len = strlen(path);
    res[len] = '/';
    for(int i = 0; i <= strlen(file); i++)
        res[i + len + 1] = file[i];

    return res;
}

TFILE_INFO_NODE* createNode(char* path, char* name, char* fullName, struct stat* statInfo)
{
    TFILE_INFO_NODE* res = (TFILE_INFO_NODE*)malloc(sizeof(TFILE_INFO_NODE));   
    if (res){
        res->info.name = (char*)malloc(strlen(name) + 1);
        res->info.name = strcpy(res->info.name, name);
        res->info.path = (char*)malloc(strlen(path) + 1);
        res->info.path = strcpy(res->info.path, path);
        res->info.size = statInfo->st_size;
        res->info.ind = 0;
    }
    return res;
}

int isItDirectory(char* path, struct stat* path_stat){
    stat(path, path_stat);
    return S_ISDIR(path_stat->st_mode);
}

int enumFiles(char* pathName, struct fileInfoNode** head, char* incorrPath)
{
    DIR* dir = opendir(pathName);
    int resCode = 0;
    if (!dir){ 
        int errorCode = errno;
        resCode = (errorCode == ENOTDIR || errorCode == ENOENT) ? DIR_NOT_EXIST : OTHER;
    }else{
        struct dirent* d;
        while (d = readdir(dir)){             
            if(strcmp(d->d_name, "..") && strcmp(d->d_name, ".")){
                char* c = formDirectoryName(pathName, d->d_name);
                struct stat fileStat;
                if (isItDirectory(c, &fileStat))
                    resCode = !strcmp(c, incorrPath) ? INCORRECT_DIRECTORY : enumFiles(c, head, incorrPath);
                else{
                    TFILE_INFO_NODE* node = createNode(pathName, d->d_name, c, &fileStat);
                    if (node){
                        node->ptOnNext = *head;
                        *head = node;
                    }
                }
                free(c);
            }
        }
    }

    closedir(dir); 
    return resCode;
}


int main(int argc, char** argv)
{
    if (argc != 4){
        fputs("Error! Incorrect count of arguments\n", stderr);
        return 1;
    }

    int code = strToInt(argv[2]);
    int (*compFunc) (TFILE_INFO_NODE*, TFILE_INFO_NODE*);
    if (code == 1)
        compFunc = compareSize;
    else if (code == 2) 
        compFunc = compareName;
    else{
        fputs("Error! Incorrect value of the 2nd parameter\n", stderr);
        return 1;
    }

    TFILE_INFO_NODE* head = NULL;
     if ((code = enumFiles(argv[1], &head, argv[3])) != ALL_OK){
        switch (code)
        {
        case DIR_NOT_EXIST:
            fputs("Error! Directory doesn\'t exis\n", stderr);
            break;
        case INCORRECT_DIRECTORY:
            fputs("Error! Incorrect directory\n", stderr);
            break;

        default:
            fprintf(stderr, "Error code: %d", code);
            break;
        }

        while (head != NULL){
            TFILE_INFO_NODE* prev = head;
            head = head->ptOnNext;
            freeNode(prev);
        }
        return 1;
     }

    checkFileNames(head);   
    sortList(&head, compFunc);
    
    while (head != NULL){
        char* prevPath = formDirectoryName(head->info.path, head->info.name); 

        head->info.name = formFileName(head->info.name, head->info.ind);
        printf("\n%s", head->info.name);             
        char* futPath = formDirectoryName(argv[3], head->info.name);
        symlink(prevPath, futPath);
        
        TFILE_INFO_NODE* prev = head;
        head = head->ptOnNext;
        freeNode(prev);
        free(prevPath);
        free(futPath);
    }
    return 0;
}