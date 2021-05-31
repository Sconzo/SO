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

int Execucao_Background(char** cmd)
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
        //printf("Valor Status -> %d\n",status);
        return status;
    }
    else // caso de erro
    {
        perror("fork()");
        return -1;
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
        //printf("Valor Status -> %d\n",status);
        return status;
    }
    else // caso de erro
    {
        perror("fork()");
        return -1;
    }
}

void Tratar_Descritores_Padrao(int fd_velho, int fd_novo)
{
    if (fd_velho != fd_novo)
    {
        if (dup2(fd_velho,fd_novo) != -1)
        {
            close(fd_velho);
        }
        else perror("dup2()");
    }
}

void Tratar_Descritores(int entrada, int saida)
{
    Tratar_Descritores_Padrao(entrada, STDIN_FILENO);
    Tratar_Descritores_Padrao(saida, STDOUT_FILENO);
}

int Execucao_Pipe(char** argv, int argc, int* i,int posicao_operador)
{
    int k=0; int numpipes=1;
    int posicao_pipe[20]; int cont=0;
    int passados = *i-1;
    //printf("Ponteiro de i -> %d\n", *i);
    //printf("Posicao operador -> %d\n", posicao_operador);
    //pegando o numero de pipes
    //guardando os valores em que começam os argumentos
    posicao_pipe[0] = *i;
    //printf("argv[i] = %s\n", argv[*i]);
    for (k=*i; k<argc;k++)
    {
        if (strcmp(argv[k],";") == 0 ||strcmp(argv[k],"||") == 0 ||strcmp(argv[k],"&&") == 0 ||strcmp(argv[k],"&") == 0){break;}
        if (strcmp(argv[k],"|") == 0) {
            if (k==*i) {
                posicao_pipe[cont] = *i-posicao_operador -1;
                //printf("quantarg[%d] = %d\n", cont,posicao_pipe[cont]);
                }
            else 
            {
                posicao_pipe[cont] = k - passados- 1 - cont;
                //printf("quantarg[%d] = %d\n", cont,posicao_pipe[cont]);
                passados = passados + posicao_pipe[cont];    
            }
            
            numpipes++;
            cont++;
            }
        
    }
    posicao_pipe[cont] = k - passados- 1 - cont;
    //printf("quantarg[%d] = %d\n", cont,posicao_pipe[cont]);
    //estrutura para armazenar os argumentos pipes
    struct comando{
        char *argumentos[LIST_LEN];
    };
    
    struct comando comandos[20];
    int j=posicao_operador+1;
    //printf("j -> %d\n", j);
    //alocando os srgumentos
    
    for (int m=0;m<numpipes;m++)
    {
        for (int n = 0; n<posicao_pipe[m]; n++)
        {
            comandos[m].argumentos[n] = argv[j];
            //printf("comandos[%d].argumentos[%d] == %s\n", m,n,comandos[m].argumentos[n]);
            j++;
        }
        j = j+1;
    }
    *i = k-1;
    //printf("TESTE");
    //printf("Ponteiro de i -> %d", *i);
    int entrada = STDIN_FILENO;
    int p;

    for(int p=0;p<numpipes;p++)
    {
        
        int fd[2];
        pid_t pid;
        int status;
        if(pipe(fd) == -1) perror("pipe()");
        if((pid = fork()) == -1) perror("fork()");

        if(pid == 0)
        {
            
            close(fd[0]);
            Tratar_Descritores(entrada,fd[1]);
            execvp(comandos[p].argumentos[0],comandos[p].argumentos);
        }
        else
        {
            waitpid(-1,&status,0);
            //printf("TESTE\n");
            close(fd[1]);
            close(entrada);
            entrada = fd[0];
        }
        
    }
    //printf("%d\n", p);
    Tratar_Descritores(entrada,STDOUT_FILENO);
    int status;
    pid_t pid2;
    if((pid2 = fork()) == -1) perror("fork()");
    if(pid2 == 0)
    execvp(comandos[p+1].argumentos[0],comandos[p+1].argumentos);
    else waitpid(-1,&status,0);

    return status;

}

void Tratar_Entrada(char** argv, int argc)
{

    char *argumentos_execucao[LIST_LEN];
    int cont=0;
    int migue = 1;
    int status = -1;
    int ultimo_comando = 0;
    int posicao_operacao = 0;
    // 0->nada // 1->; // 2->|| // 3->&& // 4->& // 5->|
    memset(argumentos_execucao,0,sizeof(argumentos_execucao));

    for(int i=0;i<argc;i++)
    {
        //printf("argv[%d] = %s\n", i, argv[i]);
        if (strcmp(argv[i],"|") == 0) // PIPE
        {
            //printf("Antes do fork ->%d\n", i);
            //pid_t pid;
            //if((pid = fork()) == -1) perror("fork()");
//
            //if(pid == 0)
            //{
            //    Execucao_Pipe(argv,argc,&i);
            //}
            //else{
            status = Execucao_Pipe(argv,argc,&i,posicao_operacao);
            //printf("Valor status %d\n",status);
            memset(argumentos_execucao,0,sizeof(argumentos_execucao));
            //printf("Depois do fork ->%d\n", i);
            migue = 0;
            cont = 0;
            ultimo_comando = 5;
            //}
        }
        else if (strcmp(argv[i],";") == 0)
        {
            posicao_operacao = i; ////////////
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
            else if (ultimo_comando == 5)
            {
                memset(argumentos_execucao,0,sizeof(argumentos_execucao));
                migue = 0;
                cont = 0;
                ultimo_comando = 2;
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
            else if (ultimo_comando == 5)
            {
                memset(argumentos_execucao,0,sizeof(argumentos_execucao));
                migue = 0;
                cont = 0;
                ultimo_comando = 3;
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
            Execucao_Background(&argumentos_execucao[migue]);
            memset(argumentos_execucao,0,sizeof(argumentos_execucao));
            migue = 0;
            cont = 0;
            ultimo_comando = 4;
        }
        else{
            argumentos_execucao[cont] =  argv[i];
            cont++;
        }
    }
    
    if(ultimo_comando == 0 || ultimo_comando == 1) Execucao_Padrao(&argumentos_execucao[migue]);
    //printf("Valor status %d\n", status);
    if(ultimo_comando == 2)
    {
        if(status > 0)Execucao_Padrao(&argumentos_execucao[migue]);
        //printf("AQUI\n");}
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