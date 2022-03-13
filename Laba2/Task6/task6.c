#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <malloc.h>

//I'm so sorry for global variable
static int chunk = 0;

int enumFiles(DIR* d)
{
    struct dirent* dirFile;
    do{
        errno = ENOENT;
        dirFile = readdir(d);
        if (dirFile) printf("%s\n", dirFile->d_name);
        else if (errno == EBADF){
            fputs("Error on getting content of the path", stderr);
            return 1;
        }
    }while(dirFile);
    return 0;
}

int main(int argc, char** argv)
{   
    char* path = NULL;
    do{
        chunk += 20; 
        path = (char*)realloc(path, chunk*sizeof(char));
        if (!path) {
           fputs("Error to allocate memory\n", stderr);
           return 1;
        }
        path = getcwd(path, chunk);

        //Check problems
        if (!path && errno != ERANGE){
            fputs("Error on getting path\n", stderr);
            free(path);
            return 1;
        }
    }while(!path);

    DIR* dirInf = opendir(path);
    if (!dirInf){
        fputs("Error on opening directory information\n", stderr);
        free(path);
        return 1;
    }
    printf("\nFile of curent directory (%s):\n", path);
    if (enumFiles(dirInf)) {free(path); return 1;};
    if (closedir(dirInf)){
        fputs("Error on clothing directory\n", stderr);
        free(path);
        return 1;
    }
   
    printf("\nRoot directory files:\n");
   
    path[1] = '\0';
    if(!(dirInf = opendir(path))){
        fputs("Error on opening directory\n", stderr);
        free(path);
        return 1;
    }
    
    if (enumFiles(dirInf)){free(path); return 1;}
    if (closedir(dirInf)){
        fputs("Error on clothing directory\n", stderr);
        free(path);
        return 1;
    }    
    
    free(path);
    return 0;
}