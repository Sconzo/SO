#include <stdio.h>
#include <pthread.h>

# define MAX_T 2

struct targs 
{
    int id;
    int usecs;
};
typedef struct targs targs_t;

int turn = 1;

void critical_region (int pid)
{
    printf("RC\tpid=%d turn=%d\n", pid, turn);
    fflush(stdout);
}

void noncritical_region(int pid)
{
    printf("OUT\tpid=%d turn=%d\n", pid, turn);
    fflush(stdout);
}

void troca()
{
    if (turn == 0) turn = 1;
    else turn = 0;
}

void *Execucao_thread(void *args)
{
    int pid = *(int*)args;

    while(1)
    {
        while(turn!= pid){}

        critical_region(pid);
        troca();
        noncritical_region(pid);
    }
}

int main(int argc, char** argv)
{
    int *ret_val[MAX_T];
    pthread_t tid[MAX_T];
    targs_t ta [MAX_T];

    for (int i = 0; i < MAX_T; i++)
    {
        //cria a thread(thread, parametros da thread, funcao, argumentos da funcao)
        ta[i].id = i;
        ta[i].usecs = 100000;
        pthread_create(&tid[i],NULL,Execucao_thread,(void*)&ta[i]);
    }
    
    for (int i = 0; i < MAX_T; i++)
    {
        pthread_join(tid[i],(void**) &ret_val[i]); // espera a execucao da thread
        printf("Valor de retorno da thread[%d] = %d\n", i,*ret_val[i]);

    }
}