// Trabalho 01 de SO

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

#define STR_LEN 100
#define LIST_LEN 20
#define clear() printf("\033[h\003")


// Funcao que inicializa nosso Shell, apenas estética
void Inicializando_Shell()
{
    clear();
    printf("\n*************************************");
    printf("\n\n ****Trabaho 01 de SO - Unifesp 2021****");
    printf("\n ****Leonardo Enne e Rodrigo Sconzo****");
    printf("\n\n*************************************\n\n\n");
    sleep(1);
    clear();
}

// Funcao que lê a entrada digitada pelo usuário
int Ler_Entrada(char* str)
{
    char* buffer;
    // buffer deve ser um ponteiro, elerecebe a saída de readline (funcao da biblioteca <readline>)
    // readline consegue ler a linha que o usuario digitou por completo
    buffer = readline("\n>>>");

    // Se o usuario digitou alguma coisa adicionamos ao histórico e copiamos para str e retornamos 1
    if(strlen(buffer)!= 0)
    {
        strcpy(str,buffer);
        return 1;
    }
    // Se o usuário nao digitar nada retornamos 0;
    else return 0;
}

void Processar_Entrada(char* str, char** argumentos,int* quant_arg)
{
    for (int i=0; i<LIST_LEN; i++)
    {
        argumentos[i] = strsep(&str," ");        
        if(argumentos[i] == NULL){
             *quant_arg = i;
             break;
        }
        //printf("%s\n", argumentos[i]);
        //printf("%d\n", i);
    }

}

void Comando_Sequencial(char** cmd)
{

    pid_t pid;
    pid = fork();

    if (pid == 0) // processo filho executa o comando
    {
        execvp(cmd[0],cmd); 
    }
}


void Execucao(char** argumentos, int quant_arg)
{
    int flag = 0;
    char* aux[STR_LEN];
    char ** cmd;
    int contador=0;
    aux[0]= '\0';

    aux[contador] = argumentos[0];
    //printf("\n%s ", aux[contador]);
    contador++;
    for (int i=1; i<quant_arg; i++)
    {
        
        if (strcmp(argumentos[i],"|") == 0)
        {
            printf("\n%s ", argumentos[i]);  
            flag = 1;
        }
        else if (strcmp(argumentos[i],";") == 0)
        {
            Comando_Sequencial(&aux[0]);
            printf("\n%s ", argumentos[i]);
            flag = 1;
        }
        else if (strcmp(argumentos[i],"||") == 0)
        {
            printf("\n%s ", argumentos[i]);
            flag = 1;
        }
        else if (strcmp(argumentos[i],"&&") == 0)
        {
            printf("\n%s ", argumentos[i]);
            flag = 1;
        }
        else if (strcmp(argumentos[i],"&") == 0)
        {
            printf("\n%s ", argumentos[i]);
            flag = 1;
        }
        else{
            aux[contador] = argumentos[i];
            //printf("\n%s ", aux[contador]);
            contador++;
        }
        //printf("\n%s ", aux[0]);
        if (flag == 1)
        {
            aux[0] = '\0';
            contador = 0;
            flag = 0;
        }
        
    }
}


int main()
{
    char entrada[STR_LEN];
    char *argumentos[LIST_LEN];
    int quant_arg;

    Inicializando_Shell();

    

    while(1)
    {
        //Passamos o vetor entrada para a funcao, como o vetor eh um ponteiro disfaçado nao precisa de &
        Ler_Entrada(entrada);
        printf("\nO Comando digitado foi: %s\n", entrada);
        
        Processar_Entrada(entrada,argumentos,&quant_arg);
        //printf("%d",quant_arg);
        Execucao(argumentos,quant_arg);
    }
    return 0;
}