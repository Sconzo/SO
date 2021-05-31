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

void Teste_Entrada(char **argv, int argc)
{
    for (int i = 0; i < argc; i++)
    {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
}

int Execucao_Background(char **cmd)
{
    pid_t pid;
    pid = fork();
    if (pid == 0)
    {
        execvp(cmd[0], cmd);
        printf("AAKOJDIAIJDNFONLNFL\n");
    }
    else if (pid > 0)
    {
        int status;
        return status;
    }
    else
    {
        perror("fork()");
        return -1;
    }
}

int Execucao_Padrao(char **cmd)
{
    pid_t pid;
    pid = fork();
    if (pid == 0) // filho
    {
        execvp(cmd[0], cmd);
        printf("AAKOJDIAIJDNFONLNFL\n"); // nunca deve ocorrer esse print
    }
    else if (pid > 0) //pai
    {
        int status;
        waitpid(-1, &status, 0);
        return status;
    }
    else
    {
        perror("fork()");
        return -1;
    }
}

void Tratar_Descritores_Padrao(int fd_velho, int fd_novo)
{
    if (fd_velho != fd_novo)
    {
        if (dup2(fd_velho, fd_novo) != -1)
        {
            close(fd_velho);
        }
        else
            perror("dup2()");
    }
}

void Tratar_Descritores(int entrada, int saida)
{
    Tratar_Descritores_Padrao(entrada, STDIN_FILENO);
    Tratar_Descritores_Padrao(saida, STDOUT_FILENO);
}

int Execucao_Pipe(char **argv, int argc, int *i, int posicao_operador)
{
    int k = 0;
    int numpipes = 1; // numero de pipes
    int quantarg[20]; //vetor que armazena a quantidade de argumentos em cada pipe, sendo que o indice do vetor eh o pipe respectivo
    int cont = 0;
    int passados = *i - 1; // quantos argumentos já foram lidos anteriormente de argv
    
    quantarg[0] = *i;

    for (k = *i; k < argc; k++) //laco for para encontrar todas as pipes
    {
        // Break  se aparecer um novo operador
        if (strcmp(argv[k], ";") == 0 || strcmp(argv[k], "||") == 0 || strcmp(argv[k], "&&") == 0 || strcmp(argv[k], "&") == 0)
            break;
        
        if (strcmp(argv[k], "|") == 0) // Encontrei uma nova PIPE
        {
            if (k == *i) // Se essa PIPE for a primeira
            {
                quantarg[cont] = *i - posicao_operador - 1;
            }
            else // Se essa PIPE for a  segunda, terceira...... n
            {
                quantarg[cont] = k - passados - 1 - cont;
                passados = passados + quantarg[cont];
            }

            numpipes++;
            cont++;
        }
    }
    // Nessa logica o ultimo pipe nao eh considerado dentro do for, entao alocamos aqui fora
    quantarg[cont] = k - passados - 1 - cont;

    // Defiimos uma struct para armazenar os argumentos
    // Seria mais simples se tivessemos feito isso para os outros operadores
    struct comando
    {
        char *argumentos[LIST_LEN];
    };

    struct comando comandos[20];
    int j = posicao_operador + 1;

    for (int m = 0; m < numpipes; m++)// laco externo que percorre os pipes
    {   
        for (int n = 0; n < quantarg[m]; n++)// laco interno percorre os argumentos de cada pipe
        {
            comandos[m].argumentos[n] = argv[j];
            j++;
        }
        j = j + 1;
    }
    //atualiza i em tratarEntrada
    *i = k - 1;

    //comecando a logica do pipe em si
    int entrada = STDIN_FILENO; // entrada recebe a entrada padrao
    int p;

    for (int p = 0; p < numpipes; p++)
    {
        int fd[2]; // descritores de arquivo
        pid_t pid;
        int status;
        if (pipe(fd) == -1) //pipe
            perror("pipe()");
        if ((pid = fork()) == -1) //fork
            perror("fork()");

        if (pid == 0) // processo filho
        {
            close(fd[0]); // nao usa o descritor de leitura
            Tratar_Descritores(entrada, fd[1]); // coloca no descritor de escrita oq tiver em entrada
            execvp(comandos[p].argumentos[0], comandos[p].argumentos);
        }
        else // processo pai
        {
            waitpid(-1, &status, 0); // espera o filho 
            close(fd[1]); // fecha o descritor de escrita
            close(entrada); // fecha a entrada antes de sobrescrever
            entrada = fd[0]; //entrada recebe o descritor de leitura
        }
    }
    // o ultimo pipe tem que ser tratado fora do for
    Tratar_Descritores(entrada, STDOUT_FILENO); // coloca na saida padrao o conteudo de entrada
    // para executar esse ultimo pipe tivemos que criar um novo fork para que o processo como um todo
    // nao acabe com a execucao de execvp
    int status;
    pid_t pid2;
    if ((pid2 = fork()) == -1)
        perror("fork()");
    if (pid2 == 0)
        execvp(comandos[p + 1].argumentos[0], comandos[p + 1].argumentos);
    else
        waitpid(-1, &status, 0);

    return status;
}

void Tratar_Entrada(char **argv, int argc)
{
    // A ideia inicial eh que em um laço for a gente pegue os argumentos que serao executados de cada vez
    // nao funcionou para o comando pipe, que tivemos que usar uma outra lógica
    char *argumentos_execucao[LIST_LEN]; // guarda os argumentos que serao executados em uma passagem laco
    int cont = 0;     // quantos argumentos estão na lista de argumentos
    int migue = 1;    //garante que nao usamos o primeiro elemento da string de entrada (o ./trab-1-apresentacao)
    int status = -1;  // status == 0 processo executou corretamente // status > 0 erro na execucao. Importante para as funcoes logica
    int ultimo_comando = 0;  // guarda o ultimo comando executado
    // 0 -> nenhum comando // 1-> ; // 2-> || // 3-> && // 4-> & // 5-> |
    int posicao_operacao = 0; //importante para o comando pipe saber onde comecar seu laço for

    memset(argumentos_execucao, 0, sizeof(argumentos_execucao)); // zerando a matriz argumentos_execucao

    //laco for percorre toda a matriz de argumentos argv
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "|") == 0) // comandos encadeados
        {
            
            if (ultimo_comando == 3) // Caso ultimo comando AND
            {
                if (status == 0) // Caso o ultimo processo tenha sido bem sucedido
                    status = Execucao_Pipe(argv, argc, &i, posicao_operacao);
                else // Caso o ultimo processo nao tenha sido bem sucedido
                    status = 99;
                memset(argumentos_execucao, 0, sizeof(argumentos_execucao));
                migue = 0;
                cont = 0;
                ultimo_comando = 5;
            }
            else if (ultimo_comando == 2) // Caso ultimo comando OR
            {
                if (status > 0) //Caso tenha um erro no ultimo processo
                    status = Execucao_Pipe(argv, argc, &i, posicao_operacao);
                memset(argumentos_execucao, 0, sizeof(argumentos_execucao));
                migue = 0;
                cont = 0;
                ultimo_comando = 5;
            }
            else
            {
                status = Execucao_Pipe(argv, argc, &i, posicao_operacao);
                memset(argumentos_execucao, 0, sizeof(argumentos_execucao));
                migue = 0;
                cont = 0;
                ultimo_comando = 5;
            }
        
        }
        else if (strcmp(argv[i], ";") == 0) // comandos independentes
        {
            posicao_operacao = i;
            if (ultimo_comando == 3) // Caso ultimo comando AND
            {
                if (status == 0) // Caso o ultimo processo tenha sido bem sucedido
                    status = Execucao_Padrao(&argumentos_execucao[migue]);
                else // Caso o ultimo processo nao tenha sido bem sucedido
                    status = 99;
                memset(argumentos_execucao, 0, sizeof(argumentos_execucao));
                migue = 0;
                cont = 0;
                ultimo_comando = 1;
            }
            else if (ultimo_comando == 2) // Caso ultimo comando OR
            {
                if (status > 0) //Caso tenha um erro no ultimo processo
                    status = Execucao_Padrao(&argumentos_execucao[migue]);
                memset(argumentos_execucao, 0, sizeof(argumentos_execucao));
                migue = 0;
                cont = 0;
                ultimo_comando = 1;
            }
            else if (ultimo_comando == 5) //Caso ultimo comando PIPE
            {
                memset(argumentos_execucao, 0, sizeof(argumentos_execucao));
                migue = 0;
                cont = 0;
                ultimo_comando = 1;
            }
            else
            {
                status = Execucao_Padrao(&argumentos_execucao[migue]);
                memset(argumentos_execucao, 0, sizeof(argumentos_execucao));
                migue = 0;
                cont = 0;
                ultimo_comando = 1;
            }
        
        }

        else if (strcmp(argv[i], "||") == 0) //comando condicional OR
        {
            posicao_operacao = i;
            if (ultimo_comando == 3) // Caso ultimo comando AND
            {
                if (status == 0) // Caso o ultimo processo tenha sido bem sucedido
                    status = Execucao_Padrao(&argumentos_execucao[migue]);
                else // Caso o ultimo processo nao tenha sido bem sucedido
                    status = 99;
                memset(argumentos_execucao, 0, sizeof(argumentos_execucao));
                migue = 0;
                cont = 0;
                ultimo_comando = 2;
            }
            else if (ultimo_comando == 2) // Caso ultimo comando OR
            {
                if (status > 0) //Caso tenha um erro no ultimo processo
                    status = Execucao_Padrao(&argumentos_execucao[migue]);
                memset(argumentos_execucao, 0, sizeof(argumentos_execucao));
                migue = 0;
                cont = 0;
                ultimo_comando = 2;
            }
            else if (ultimo_comando == 5) //Caso ultimo comando PIPE
            {
                memset(argumentos_execucao, 0, sizeof(argumentos_execucao));
                migue = 0;
                cont = 0;
                ultimo_comando = 2;
            }
            else
            {
                status = Execucao_Padrao(&argumentos_execucao[migue]);
                memset(argumentos_execucao, 0, sizeof(argumentos_execucao));
                migue = 0;
                cont = 0;
                ultimo_comando = 2;
            }
        }
        else if (strcmp(argv[i], "&&") == 0) // comando condicional AND
        {
            posicao_operacao = i;
            if (ultimo_comando == 3) //Caso ultimo comando AND
            {
                if (status == 0) // bem sucedido
                    status = Execucao_Padrao(&argumentos_execucao[migue]);
                else // erro
                    status = 99;
                memset(argumentos_execucao, 0, sizeof(argumentos_execucao));
                migue = 0;
                cont = 0;
                ultimo_comando = 3;
            }
            else if (ultimo_comando == 2) // Caso ultimo comando OR
            {
                if (status > 0) // erro
                    status = Execucao_Padrao(&argumentos_execucao[migue]);
                memset(argumentos_execucao, 0, sizeof(argumentos_execucao));
                migue = 0;
                cont = 0;
                ultimo_comando = 3;
            }
            else if (ultimo_comando == 5) // CAso ultimo comando PIPE
            {
                memset(argumentos_execucao, 0, sizeof(argumentos_execucao));
                migue = 0;
                cont = 0;
                ultimo_comando = 3;
            }
            else
            {
                status = Execucao_Padrao(&argumentos_execucao[migue]);
                memset(argumentos_execucao, 0, sizeof(argumentos_execucao));
                migue = 0;
                cont = 0;
                ultimo_comando = 3;
            }
        }

        else if (strcmp(argv[i], "&") == 0) // comando em background
        {
            if (ultimo_comando == 3) // Caso ultimo comando AND
            {
                if (status == 0) // Caso o ultimo processo tenha sido bem sucedido
                    status = Execucao_Background(&argumentos_execucao[migue]);
                else // Caso o ultimo processo nao tenha sido bem sucedido
                    status = 99;
                memset(argumentos_execucao, 0, sizeof(argumentos_execucao));
                migue = 0;
                cont = 0;
                ultimo_comando = 4;
            }
            else if (ultimo_comando == 2) // Caso ultimo comando OR
            {
                if (status > 0) //Caso tenha um erro no ultimo processo
                    status = Execucao_Background(&argumentos_execucao[migue]);
                memset(argumentos_execucao, 0, sizeof(argumentos_execucao));
                migue = 0;
                cont = 0;
                ultimo_comando = 4;
            }
            else if (ultimo_comando == 5) //Caso ultimo comando PIPE
            {
                memset(argumentos_execucao, 0, sizeof(argumentos_execucao));
                migue = 0;
                cont = 0;
                ultimo_comando = 4;
            }
            else
            {
                status = Execucao_Background(&argumentos_execucao[migue]);
                memset(argumentos_execucao, 0, sizeof(argumentos_execucao));
                migue = 0;
                cont = 0;
                ultimo_comando = 4;
            }
        }
        else // nao encontrou nenhum operador
        {
            argumentos_execucao[cont] = argv[i];
            cont++;
        }
    }


    if (ultimo_comando == 0 || ultimo_comando == 1)
        Execucao_Padrao(&argumentos_execucao[migue]);
    if (ultimo_comando == 2)
    {
        if (status > 0)
            Execucao_Padrao(&argumentos_execucao[migue]);
    }
    if (ultimo_comando == 3)
    {
        if (status == 0)
            Execucao_Padrao(&argumentos_execucao[migue]);
    }
}

int main(int argc, char **argv)
{
    if (argc == 1) // Conferindo o uso correto do programa
    {
        printf("Uso: %s <comando> <p1> <p2> ... <pn>\n", argv[0]);
        return 0;
    }
    // Funcao que executa o shell
    Tratar_Entrada(argv, argc);

    return 0;
}