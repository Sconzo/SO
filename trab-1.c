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
        return status;
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
    int status = -1;
    int ultimo_comando = 0;
    // 0->nada // 1->; // 2->|| // 3->&&
    memset(argumentos_execucao,0,sizeof(argumentos_execucao));

    for(int i=0;i<argc;i++)
    {
        
        if (strcmp(argv[i],"|") == 0) // PIPE
        {
            operadores[cont_op] = argv[i];  
            cont_op++;
        }
        else if (strcmp(argv[i],";") == 0)
        {
            Execucao_Padrao(&argumentos_execucao[migue]);
            memset(argumentos_execucao,0,sizeof(argumentos_execucao));
            migue = 0;
            cont = 0;
            ultimo_comando = 1;
        }
        else if (strcmp(argv[i],"||") == 0)
        {
            if (ultimo_comando == 3)
            {
                if (status == 0)
                {
                    status = Execucao_Padrao(&argumentos_execucao[migue]);
                    memset(argumentos_execucao,0,sizeof(argumentos_execucao));
                    migue = 0;
                    cont = 0;
                    ultimo_comando = 2;
                }
                else
                {
                    status = 4;
                    memset(argumentos_execucao,0,sizeof(argumentos_execucao));
                    migue = 0;
                    cont = 0;
                    ultimo_comando = 2;
                }
            }
            else if (ultimo_comando == 2)
            {
                if (status > 0)
                {
                    status = Execucao_Padrao(&argumentos_execucao[migue]);
                    memset(argumentos_execucao,0,sizeof(argumentos_execucao));
                    migue = 0;
                    cont = 0;
                    ultimo_comando = 2;
                }
            }
            else{
                status = Execucao_Padrao(&argumentos_execucao[migue]);
                memset(argumentos_execucao,0,sizeof(argumentos_execucao));
                migue = 0;
                cont = 0;
                ultimo_comando = 2;
            }
        }
        else if (strcmp(argv[i],"&&") == 0)
        {

            if (ultimo_comando == 3)
            {
                if (status == 0)
                {
                    status = Execucao_Padrao(&argumentos_execucao[migue]);
                    memset(argumentos_execucao,0,sizeof(argumentos_execucao));
                    migue = 0;
                    cont = 0;
                    ultimo_comando = 3;
                }
                else
                {
                    status = 4;
                    memset(argumentos_execucao,0,sizeof(argumentos_execucao));
                    migue = 0;
                    cont = 0;
                    ultimo_comando = 3;
                }
            }
            else if (ultimo_comando == 2)
            {
                if (status > 0)
                {
                    status = Execucao_Padrao(&argumentos_execucao[migue]);
                    memset(argumentos_execucao,0,sizeof(argumentos_execucao));
                    migue = 0;
                    cont = 0;
                    ultimo_comando = 3;
                }
            }
            else{
                status = Execucao_Padrao(&argumentos_execucao[migue]);
                memset(argumentos_execucao,0,sizeof(argumentos_execucao));
                migue = 0;
                cont = 0;
                ultimo_comando = 3;
            }
            
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
    if(ultimo_comando == 0 || ultimo_comando == 1) Execucao_Padrao(&argumentos_execucao[migue]);
    if(ultimo_comando == 2)
    {
        if(status > 0)Execucao_Padrao(&argumentos_execucao[migue]);
    }
    if(ultimo_comando == 3)
    {
        if(status == 0)Execucao_Padrao(&argumentos_execucao[migue]);
    }
    
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