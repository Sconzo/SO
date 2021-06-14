#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

# define MAX_T 2

struct targs 
{
    int id;
    int usecs;
};

typedef struct targs targs_t;


void *Execucao_thread(void *args)
{
    targs_t ta = *(targs_t*)args;
    int i=0;
    int *ret;
    ret = calloc(1,sizeof(int));
    *ret = 10;

    while(1)
    {
        printf("Thread %d em execucao %d\n", ta.id, i++);
        usleep(ta.usecs);
        if (i == 100) return ret;
    }
    pthread_exit(ret);

}

int main(int argc, char** argv) // Funcao main eh a thread principal do processo
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
    

    //pthread_exit // retorna o valor obtido na thread


    return 0;
}