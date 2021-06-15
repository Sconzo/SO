// Implementacao do problema Banheiros busywait para fazer a sincronização do processos

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int ocupado = 0;
pthread_mutex_t mutex;


void* woman_leaves()
{
    pthread_mutex_lock(&mutex);
    ocupado--;
    pthread_mutex_unlock(&mutex);
}


void* woman_wants_to_enter()
{
    pthread_mutex_lock(&mutex);
    if (ocupado>=0)
    {
        printf("1 mulher entrou\n");
        ocupado++;
        printf("%d mulheres no banheiro\n", ocupado);//usar_banheiro();
        printf("1 mulher saiu\n");
    }
    pthread_mutex_unlock(&mutex);
}
void* man_leaves()
{
    pthread_mutex_lock(&mutex);
    ocupado++;
    pthread_mutex_unlock(&mutex);
}

void* man_wants_to_enter()
{
    pthread_mutex_lock(&mutex);
    if (ocupado<=0)
    {
        printf("1 homem entrou\n");
        ocupado--;
        printf("%d homens no banheiro\n", ocupado); //usar_banheiro();
        printf("1 homem saiu\n");
    }
    pthread_mutex_unlock(&mutex);    
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        printf("São necessários 2 argumentos, o primeiro indicando o número de produtores e segundo indicando o numero de consumidores");
        return 0;
    }

    long mulheres = strtol(argv[1],NULL,10), homens = strtol(argv[2],NULL,10);
    //long pessoas = strtol(argv[1],NULL,10);
    pthread_mutex_init(&mutex,NULL);
    pthread_t wo_tid[mulheres];
    pthread_t man_tid[homens];

    for (int i = 0; i < mulheres; i++)
    {
        pthread_create(&wo_tid[i],NULL,woman_wants_to_enter,NULL);
        pthread_create(&wo_tid[i],NULL,woman_leaves,NULL);
    }
    for (int i = 0; i < homens; i++)
    {
        pthread_create(&man_tid[i],NULL,man_wants_to_enter,NULL);
        pthread_create(&man_tid[i],NULL,man_leaves,NULL);
    }
    
    for (int i = 0; i < mulheres; i++)
    {
        pthread_join(wo_tid[i],NULL); // espera a execucao da thread
    }

    for (int i = 0; i < homens; i++)
    {
        pthread_join(man_tid[i],NULL); // espera a execucao da thread
    }
    
    pthread_mutex_destroy(&mutex);

    return 0;
}