#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define BUFSIZE 1024
#define FUNCTIONNUM 6
#define STRLENGTH 1024
char pwd[1024];
extern char ** environ;

char * functionNames[] = {"cd", "exit", "help", "version", "type", "echo"};

void commandExecute(char * argv[]);
void cd(char * argv[]);
char** readCmd();
int isContain(char *string, char a);
void exitCmd(char **argv);
void executeProcess(char ** argv);
int executeProgram(char ** argv);
void exeProcessWithVars(char ** argv);

int main()
{    
    char *username = malloc(sizeof(*username)* STRLENGTH);
    char *hostname =  malloc(sizeof(*hostname)* STRLENGTH);
    char * line = malloc(sizeof(*line)*BUFSIZE);

    getlogin_r(username, sizeof(username));
    gethostname(hostname, sizeof(hostname));
    chdir(getenv("HOME"));
    while(1)
    {
    char **buffer = malloc(sizeof(char) * BUFSIZE);
    getcwd(pwd, sizeof(pwd));

    printf("[{%s}@{%s} {%s}] $ ", username, hostname, pwd);
    fflush(stdout);

    buffer = readCmd();

    commandExecute(buffer);

    free(buffer);



    }

    free(username);
    free(hostname);
    return 0;
}


int isContain(char *string, char a) {
    char *c = string;
    while (*c) {
        if (*c == a)
            return 1;
        else            
        ++c;
    }
    return 0;
}

char** readCmd()
{
    int i = 0;
    int j = 0;
    int end = 0;
    int l =0;
    char * buffer = malloc(sizeof(char) * BUFSIZE);
    char ** bufferSep = malloc(sizeof(char*) * BUFSIZE);
    *(bufferSep+j) = buffer;
    j++;
    while(1)
    {
        l = getchar();
        if (l == '\n')
        {
            end = 1;
        }
        if(l == 32 || l == '\n')
        {
            buffer[i] = '\0';
            i++;
            if (end != 1)
            *(bufferSep+j) = buffer + i;
            j++;
        }
        else {
        buffer[i] = l;
        i++;
        }
        if (end == 1)
        {

            *(bufferSep+j) = NULL;
            return bufferSep;
        }
    }
}


void executeProcess(char ** argv)
{

      pid_t pid;
        if(pid = fork())
        {

            int wstatus;            
            wait(&wstatus);

        }
        else
        {
            if ((execvp(argv[0], argv) == -1))
            {
                printf("mysh: ");
                printf("%s ", argv[0]);
                printf("command not found.\n");

                    kill(getpid(), SIGKILL);
        }

        }

    argv[1] = NULL;
}

int executeProgram(char ** argv)
{

     char * path = malloc(sizeof(*path) * 1000);
     strcat(path, pwd);
     strcat(path, "/");
     strcat(path, argv[0]);
     pid_t pid;
     int rvalue = -1;
    if ((access(path, X_OK)) == 0)
    {
     rvalue = 1;
        if(pid = fork())
        {
            int wstatus;
            wait(&wstatus);
        }
        else
        {
            rvalue = execvp(path, argv);
            if (rvalue == -1)
            {

            kill(getpid(), SIGKILL);
            }

        }
        argv[1] = NULL;
}
        free(path);

        return rvalue;

}

void exeProcessWithVars(char ** argv)
{
     pid_t pid;
     int returnValue = 0;
        if(pid = fork())
        {

            int wstatus;
            wait(&wstatus);
        }
        else
        {
            putenv(argv[0]);
            returnValue = execvpe(argv[1], argv+1, environ);
            perror("mysh");
            if(returnValue == -1)
            {
                argv[1] = NULL;
                kill(getpid(), SIGKILL);
            }
        }

argv[1] = NULL;


}

void cd(char ** argv)
{
    if (argv[1] == NULL)
    {
        printf("no args.");
    } else
    {
        if (chdir(argv[1]) != 0)
            perror("cd");
    }
    argv[1] = NULL;
}

int toInt(char * s)
{

    int i, n;
    n = 0;
    for (i = 0; s[i]!='\0'; ++i)
    {
        if (s[i] >='0' && s[i] <='9')
        n = 10 * n +(s[i] - '0');
    }

    return n;
}

void exitCmd(char **argv)
{

  int code = 0;
  if (argv[1] != NULL)
      code = toInt(argv[1]);

  exit(code);
  exit(code);

}

void help()
{
    printf("mysh\nhave built-in functions\n-cd [arg] changes working directory\n");	
    printf("-exit [n] closes shell with optional return code\n");
    printf("-help displays description of functions\n-version displays version\n");
    printf("-type [arg] displays information about command\n");
    printf("-echo [arg] displays entered text, if argument begins with '$' displays this environment variable\n");
	printf("mysh lets to execute programs. Argument should be relative or absolute path to executable file or name in $PATH\n");	
    printf("typing variable_name=value changes or adds environment variable\n");


}
void version()
{

printf("mysh v1.0\n");

}

void type(char ** argv)
{
    if (isContain(argv[1], '/') == 1)
    {
        if(access(argv[1], F_OK) == 0)
        {
            printf("%s is ", argv[1]);
            printf("%s\n", argv[1]);
            return;
        }

    }


    for (int i = 0; i < FUNCTIONNUM; i++)
    {
        if (strcmp(argv[1], functionNames[i]) == 0)
        {
            printf("%s is built-in shell function.\n", argv[1]);
            return;
        }
    }
    char* pathget = getenv("PATH");
    char buffer[100];
    char * path = malloc(sizeof(*path) * 100);
    *path = NULL;
    int i = 0;
    int j = 0;
    int k = 0;
    while(*(pathget+i) != NULL)
    {
        k = 0;
        while(*(pathget+j) != ':' && *(pathget+j) != '\0')
        {
            buffer[k] = *(pathget+j);
            j++;
            k++;
        }
        buffer[k] = '\0';
        strcat(path, buffer);
        strcat(path, "/");
        strcat(path, argv[1]);

        if(access(path, F_OK) == 0)
        {
            printf("%s is ", argv[1]);
            printf("%s\n", path);
            return;
        }
        i = j;
        j++;

        *path = NULL;

    }
    printf("%s not found.\n", argv[1]);
    free(path);
    argv[1] = NULL;


}
void echo(char ** argv)
{
    char * env = malloc(sizeof(*env) * 200);
    char * string = malloc(sizeof(*string) * 200);

    for(int i = 1; *(argv+i) != NULL; i++)
    {
        if((*(*(argv+i))) == '$')
        {
            string = *(argv+i)+1;
            env = getenv(string);
            if(env != NULL)
            printf("%s ", env);
            else
            {
                printf("variable doesnt exist");
                argv[1] = NULL;
            }
        } else {
            printf("%s ", *(argv+i));
        }

        if (*(argv+i+1) == NULL)
        {
            printf("\n");
        }
    }

    argv[1] = NULL;

}

void commandExecute(char ** argv)
{

    int lastStatus = 0;

    if (argv[0] == NULL)
    {
        return;
    }

    for(int i = 0; i < FUNCTIONNUM; i++)
    {
        if (strcmp(argv[0], functionNames[i]) == 0)
        {
            switch(i)
            {
            case 0:
                cd(argv);                
                return;
            case 1:
                exitCmd(argv);
                return;
            case 2:
                help();
                return;
            case 3:
                version();
                return;
            case 4:
                type(argv);
                return;
            case 5:
                echo(argv);
                return;
             default :                
            return;
            }
        }
    }

    if(isContain(argv[0], '=') && argv[1] == NULL){
       putenv(argv[0]);
     
        return;
    } else if (isContain(argv[0], '=') && argv[1] != NULL){
        exeProcessWithVars(argv);
        return;
    }

    lastStatus = executeProgram(argv);
    if (lastStatus == -1 )
    executeProcess(argv);

}


