// Trabalho 01 de SO

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>

#define INPUT_LEN 256
#define LIST_LEN 100
char *argumentos[LIST_LEN];
char *operadores[LIST_LEN];
int cont_op = 0;
int cont_arg = 0;

void Teste_Entrada(char** argv, int argc)
{
    for(int i=0;i<argc;i++)
    {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
}

int Execucao_Padrao(char** cmd)
{
    pid_t pid;
    pid = fork();
    if (pid == 0) // processo filho executa o comando
    {
        // pegando a matriz de argumentos sem o executável
        execvp(cmd[0],cmd); //
        printf ("AAKOJDIAIJDNFONLNFL\n"); // Não é executado pq execvp() substitui a imagem do processo
                                        // filho com o novo processo definido por cmd[0]
    }
    else if (pid > 0) // processo pai espera o término do filho
    {
        int status;
        waitpid(-1,&status,0); // espera a mudança no processo filho
        printf("Valor Status -> %d\n",status);
    }
    else // caso de erro
    {
        perror("fork()");
        return -1;
    }
}


void Tratar_Entrada(char** argv, int argc)
{
    
    char *argumentos_execucao[LIST_LEN];
    int cont=0;
    int migue = 1;
    memset(argumentos_execucao,0,sizeof(argumentos_execucao));

    for(int i=0;i<argc;i++)
    {
        
        if (strcmp(argv[i],"|") == 0)
        {
            operadores[cont_op] = argv[i];  
            cont_op++;
        }
        else if (strcmp(argv[i],";") == 0)
        {
            operadores[cont_op] = argv[i];  
            cont_op++;
            Execucao_Padrao(&argumentos_execucao[migue]);
            memset(argumentos_execucao,0,sizeof(argumentos_execucao));
            migue = 0;
            cont = 0;
        }
        else if (strcmp(argv[i],"||") == 0)
        {
            operadores[cont_op] = argv[i];  
            cont_op++;
        }
        else if (strcmp(argv[i],"&&") == 0)
        {
            operadores[cont_op] = argv[i];  
            cont_op++;
        }
        else if (strcmp(argv[i],"&") == 0)
        {
            operadores[cont_op] = argv[i];  
            cont_op++;
        }
        else{
            argumentos[cont_arg] = argv[i]; 
            argumentos_execucao[cont] =  argv[i];
            cont++;
            cont_arg++;
        }
    }
    if(migue<2) Execucao_Padrao(&argumentos_execucao[migue]);
    
}


int main(int argc, char **argv)
{    
    if (argc == 1)
    {
        printf("Uso: %s <comando> <p1> <p2> ... <pn>\n", argv[0]);
        return 0;
    }

    //printf("argc == %d\n", argc);
    Tratar_Entrada(argv,argc);

    //Teste_Entrada(argumentos,cont_arg);
    //Teste_Entrada(operadores,cont_op);

    return 0;
}