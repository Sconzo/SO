#include <stdio.h>
#include <pthread.h>

# define MAX_T 2
int N,P;
pthread_mutex_t mutex;

struct targs 
{
    int ini;
    int fim;
};
typedef struct targs targs_t;



void *Execucao_thread(void *args)
{
    targs_t ta = *(targs_t*)args;
    int perfeitos[100];

    for (int i=0;i<P;i++)
    {
        confere_numero_perfeito(ta.ini,ta.fim);
        pthread_mutex_lock(&mutex);
        printf("Regiao Critica\n");
        pthread_mutex_unlock(&mutex);
    }
    
}

void divisao_num_por_thread(int inicio[P], int final[P])
{
    int divisao = N/P;
    int aux = 0;
    int i=0;
    for (i;i<P;i++)
    {
        inicio[i] = aux+1;
        final[i]  = (divisao)*(i+1);
        aux = final[i];
        //printf("inicio[%d]=%d\n", i,inicio[i]);
        //printf("final[%d]=%d\n", i,final[i]);
    }
    final[i-1] = N;
    //printf("final[%d]=%d\n", i-1,final[i-1]);
}
int main(int argc, char** argv)
{
    scanf("%d", &N);
    scanf("%d", &P);
    int *ret_val[P];
    pthread_t tid[P];
    targs_t ta [P];
    int inicio[P],final[P];
    pthread_mutex_init(&mutex,NULL);
    divisao_num_por_thread(inicio,final);

    for (int i = 0; i < P; i++)
    {
        //cria a thread(thread, parametros da thread, funcao, argumentos da funcao)
        printf("%d\n",inicio[i]);
        printf("%d\n",final[i]);
        ta[i].ini = inicio[i];
        ta[i].fim = final[i];
        pthread_create(&tid[i],NULL,Execucao_thread,(void*)&ta[i]);
    }
    
    for (int i = 0; i < P; i++)
    {
        pthread_join(tid[i],NULL); // espera a execucao da thread

    }
    
    pthread_mutex_destroy(&mutex);
}