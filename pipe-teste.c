#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define STR_LEN 100

int main(int argc, char **argv)
{
    //argv = ./pipe-teste ls | grep shell | grep shell-2 | grep .c 
    //argc = 11
    
    int fd[2]; // descritor de arquivo
    pid_t pid;
    if ((pipe(fd) < 0))
    {
        perror("pipe()");
        return 0;
    }
    pid = fork(); // Como estamos abrindo o fork depois de declarar o pipe então tanto pai quanto filho
                //têm os mesmos descritores. Sendo assim, é necessário fechar a extremidade nao aproveitada
    
    
}