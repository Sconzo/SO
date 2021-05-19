//Código exemplo mostrado pelo professor 
//utilizando a primitiva fork() para entender o 
//conceito de processos pai e filho


#include <stdio.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char **argv)
{
    pid_t pid;

    pid = fork();

    if (pid == 0) // processo filho
    {
        while(1) //laço infinito
        {
            
            printf("PID do filho -> %ld, pid-> %ld\n", (long)getpid(),(long)pid);
            sleep(1);
        }
    }
    else if (pid > 0) // processo pai
    {
        while(1) //laço infinito
        {
            
            printf("PID do pai -> %ld, pid -> %ld\n", (long)getpid(),(long)pid);
            sleep(1);
        }
    }
    else // caso de erro
    {
        perror("fork()");
        return -1;
    }
    return 0;
}