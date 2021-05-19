//Código exemplo mostrado pelo professor 
//modelando um shell simples

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

void Teste_Entrada(char** argv, int argc)
{
    for(int i=0;i<argc;i++)
    {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
}

int main(int argc, char **argv)
{
    pid_t pid;
    pid = fork();

    if (argc == 1)
    {
        printf("Uso: %s <comando> <p1> <p2> ... <pn>\n", argv[0]);
        return 0;
    }

    if (pid == 0) // processo filho executa o comando
    {
        char ** cmd;
        cmd = &argv[1]; // pegando a matriz de argumentos sem o executável

        //Teste_Entrada(cmd,20);
        execvp(cmd[0],cmd); //

        printf ("AAKOJDIAIJDNFONLNFL"); // Não é executado pq execvp() substitui a imagem do processo
                                        // filho com o novo processo definido por cmd[0]
    }
    else if (pid > 0) // processo pai espera o término do filho
    {
        int status;
        printf("antes do wait pid\n");
        printf("antes do wait pid\n");
        printf("antes do wait pid\n");
        printf("antes do wait pid\n");

        waitpid(-1,&status,0); // espera a mudança no processo filho

        printf("depois do wait pid\n");
        printf("Valor Status -> %d\n",status);

    }
    else // caso de erro
    {
        perror("fork()");
        return -1;
    }
    return 0;
}