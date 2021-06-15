// Implementacao do problema Produtor Consumidor utilizando mutex para fazer a sincronização do processos

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define tam 100
int cont =0;
int vetor[tam];
pthread_mutex_t mutex;

struct pilha
{
    int valor;
    int *prox;
}pilha;

void *Consome ()
{
    while(1)
    {
        sleep(1);
        pthread_mutex_lock(&mutex);
        if(cont>0){
            printf("Consumido item Vetor[%d]=%d\n", cont,vetor[cont-1]);
            vetor[cont-1] = 0;
            cont--;
        }
        pthread_mutex_unlock(&mutex);
        
    }
}

void* Produz()
{
    int item=0;

    while(1)
    {
        sleep(1);
        item = item+1;
        pthread_mutex_lock(&mutex);
        if(cont<tam)
        {
            vetor[cont] = item;
            printf("Produzido item Vetor[%d]=%d\n", cont,item);
            cont++;
        }
        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        printf("São necessários 2 argumentos, o primeiro indicando o número de produtores e segundo indicando o numero de consumidores");
        return 0;
    }

    long prod = strtol(argv[1],NULL,10), cons = strtol(argv[2],NULL,10);
    pthread_mutex_init(&mutex,NULL);

    pthread_t prod_tid[prod];
    pthread_t cons_tid[cons];

    for (int i = 0; i < prod; i++)
    {
        pthread_create(&prod_tid[i],NULL,Produz,NULL);
    }
    for (int i = 0; i < cons; i++)
    {
        pthread_create(&cons_tid[i],NULL,Consome,NULL);
    }
    for (int i = 0; i < prod; i++)
    {
        pthread_join(prod_tid[i],NULL); // espera a execucao da thread
    }
    for (int i = 0; i < cons; i++)
    {
        pthread_join(cons_tid[i],NULL); // espera a execucao da thread
    }
    
    pthread_mutex_destroy(&mutex);

    return 0;
}