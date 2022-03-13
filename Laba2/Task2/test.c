#include <stdio.h>

int main(int argc, char** argv, char* envp)
{
    fputs("Output message: ", stdout);
    for (int i = 1; i < argc; i++){
        fputs(argv[i], stdout);
        fputc(' ', stdout);
    }    
    fputc('\n', stdout);
    return 0;
}