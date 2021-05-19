//Código exemplo mostrado pelo professor 
//modelando um shell com pipes

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define STR_LEN 100

int main(int argc, char **argv)
{
    int fd[2]; // descritor de arquivo
    int rev_fd[2]; // descritor reverso
    pid_t pid;
    
    if ((pipe(fd) < 0) || (pipe(rev_fd) < 0))
    {
        perror("pipe()");
        return 0;
    }

    pid = fork();

    if (pid ==0) // filho usa o descritor em 0 para leitura
    {
        char msg[STR_LEN];
        close(fd[1]); // Ele nao utilizará o descritor de escrita
        close(rev_fd[0]);
        bzero(msg,STR_LEN);

        if (read(fd[0],msg,STR_LEN) > 0)
        {
            printf("Filho de PID %ld leu a seguinte mensagem:\n\t%s\n", (long)getpid(),msg);
        }
        bzero(msg,STR_LEN);
        sprintf(msg,"Ok papai, vou fazer isso!");
        write(rev_fd[1],msg,strlen(msg));
        printf("Filho terminou, dormindo por 2s\n");
        sleep(2);
        return 0;
    }

    else if(pid > 0) //pai usa o descritor em 1 para fazer a escrita
    {
        char msg[STR_LEN];
        int status;
        close(fd[0]);
        close(rev_fd[1]);
        bzero(msg, STR_LEN);
        printf("Pai de PID %ld está escrevendo a mensagem\n",(long)getpid());
        sprintf(msg,"'Preste atenção no que seu pai fala'");
        write(fd[1],msg,strlen(msg));
        bzero(msg,STR_LEN);
        if (read(rev_fd[0],msg,STR_LEN) > 0)
        {
            printf("Pai de PID %ld leu a seguinte mensagem:\n\t%s\n", (long)getpid(),msg);
        }

        waitpid(-1,&status,0);
        printf("Pai terminou\n");

    }

    else // caso de erro
    {
        perror("fork()");
        return -1;
    }
    return 0;
}