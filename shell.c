#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<dirent.h>
#include<fcntl.h>

char* remove_whitespace(char*);
void pipe0(char**, char*);
void pipe1(char**, char*);
void pipe2(char**, char*);
void cmd_handler(char*, char*);
void execute_cat(char**, char*);
void sig_handler()
{
    printf("\n");
    return;
}

int main()
{
    char *dir = getcwd(dir, BUFSIZ);
    char input[BUFSIZ];
    char c;
    
    signal(SIGINT, sig_handler);
    while(1)
    {
        printf("%s", dir);
        printf("$ ");
        int i=0, piped=0, temp;
        
        while((c = getchar()) != '\n')
        {
            if(c == EOF)
                exit(0);
            input[i++] = c;
        }
        input[i] = '\0';
        if(i == 0)
            continue;

        char *cmd_pip[BUFSIZ], *rest = input;

        /*Separate the piped commands*/
        while((cmd_pip[piped++]=strtok_r(rest, "|", &rest)) != NULL);
            // printf("%d - %s\n", piped, cmd_pip[piped-1]);
        --piped;
        if(piped>3)
        {
            fprintf(stderr, "You entered %d pipes. Only 2 pipes are allowed\n", piped-1);
            continue;
        }
        for(int i=0; i<piped; ++i)
        {
            cmd_pip[i] = remove_whitespace(cmd_pip[i]);
            // printf("%s\n", cmd_pip[i]);
        }
        // printf("%d\n", piped);


        if(piped == 3)
            pipe2(cmd_pip, dir);    //abhi banana hai
        else if(piped == 2)
            pipe1(cmd_pip, dir);
        else
            pipe0(cmd_pip, dir);
    }
    
}

void cmd_handler(char *str, char* dir)
{
    char *cmd[BUFSIZ], *rest = str;
    int args=0;

    while((cmd[args++]=strtok_r(rest, " ", &rest)) != NULL);
        // printf("%d - %s\n", args, cmd[args-1]);
    --args;
    cmd[args] = NULL;
    // printf("args = %d\n", args);    

    // printf("%s\n", cmd[0]);
    if(strcmp(cmd[0], "exit") == 0)
        exit(0);
    else if(strcmp(cmd[0], "cd") == 0)
    {
        if(args >2)
        {
            fprintf(stderr, "You entered %d arguments for cd: 1 is needed", args-1);
            return;
        }
        if(chdir(cmd[1]) < 0)
            perror("Error in cd");
        else
            getcwd(dir, BUFSIZ);
    }
    else if(strcmp(cmd[0], "pwd") == 0)
    {
        if(args > 1)
        {
            fprintf(stderr, "You entered %d arguments for pwd: 0 are needed", args-1);
            return;
        }
        printf("%s\n", dir);
    }
    else if(strcmp(cmd[0], "echo") == 0)
    {
        for(int i=1; i<args; ++i)
        {
            printf("%s ", cmd[i]);
            if(i == args-1)
                printf("\n");
        }      
    }
    else if(strcmp(cmd[0], "cat") == 0)
    {
        // printf("HEre");
        int flag=0;
        if(args <=1)
        {
            char *str[]={"cat", NULL};
            execute_cat(str, "NULL");
            return;
        }
        if(strcmp(cmd[1], "<") == 0)
        {
            int i;
            char *str[BUFSIZ];
            str[0] = cmd[0];
            for(i=2; i<=args; ++i)
            {
                if(strcmp(cmd[i] , ">") == 0)
                {
                    flag = 1;
                    break;
                }
                str[i-1] = cmd[i];
            }
            str[i] = NULL;

            if(flag)
            {
                // int fd = open(cmd[i+1], O_RDONLY | O_CREAT);
                // if(fd < 0)
                // {
                //     perror("Error opening file");
                //     return;
                // }
                // if(dup2(fd, 1) < 0)
                // {
                //     perror("Error in dup in cat");
                //     return;
                // }
                // close(fd);

                execute_cat(str, cmd[i+1]);
                return;
            }
            execute_cat(str, "NULL");
            return;
        }
        int i =0;
        char *str[BUFSIZ];
        for(i=0; i<args; ++i)
        {
            str[i] = cmd[i];
            if(strcmp(cmd[i], ">") == 0)
            {
                str[i] = NULL;
                flag=1;
                break;
            }
        }
        // printf("flag = %d\n", flag);
        if(flag)
        {
            // printf("HERE\n");
            execute_cat(str, cmd[i+1]);
        }
        else
        {
            execute_cat(str, "NULL");
        }
        
    }
    else if((strcmp("source", cmd[0]) == 0))
    {
        char *src[] = {"./a.out", NULL};
        int status;
        // for(int i=0; i<args; ++i)
        //     printf("%s\n", cmd[i]);
        pid_t p = fork();
        if(p<0)
            perror("Error in fork");
            
        if(p != 0)
        {
            wait(&status);
        }
        else
        {
            int fd = open(cmd[1], O_RDONLY);
            dup2(fd, 0);
            close(fd);
            if(execvp(src[0], src) == -1)
            {
                perror("Error in command");
                exit(0);
            }
        }
    }
    else
    {
        // printf("I am here\n");
        int status;
        // for(int i=0; i<args; ++i)
        //     printf("%s\n", cmd[i]);
        pid_t p = fork();
        if(p<0)
            perror("Error in fork");
            
        if(p != 0)
        {
            wait(&status);
        }
        else
        {
            if(execvp(cmd[0], cmd) == -1)
            {
                perror("Error in command");
                exit(0);
            }
        }
        
        
    }
}
// execv("ls", argv);

void execute_cat(char **str, char *file)
{
    int status;
    int i=0;
    // while(str[i] != NULL)
    //     printf("%s\t", str[i++]);
    // printf("%s\n", file);
    // exit(0);
    // printf("%s\n", cmd[0]);
    pid_t p = fork();
    if(p<0)
        perror("Error in fork");
            
    if(p != 0)
    {
        wait(&status);
    }
    else
    {
        if(strcmp(file, "NULL") != 0)
        {
            int fd = creat(file, 0644);
            if(fd < 0)
            {
                perror("Error opening file");
                exit(0);
            }
            if(dup2(fd, 1) < 0)
            {
                perror("Error in dup in cat");
                exit(0);
            }
            close(fd);
        }
        if(execvp("cat", str) == -1)
        {
            perror("Error in command");
            exit(0);
        }
    }
}
void pipe0(char **str, char* dir)
{
    // printf("I am in pipe1\n");
    char *cmd_sem[BUFSIZ], *rest = str[0];
    int sems=0;

    while((cmd_sem[sems++]=strtok_r(rest, ";", &rest)) != NULL);
        // printf("%d - %s\n", sems, cmd_sem[sems-1]);
    --sems;
    // printf("sems = %d\n", sems);

    for(int i=0; i<sems; ++i)
    {
        cmd_sem[i] = remove_whitespace(cmd_sem[i]);
        cmd_handler(cmd_sem[i], dir);
    }
}

void pipe1(char **str, char* dir)
{
    char *cmd_sem1[BUFSIZ], *cmd_sem2[BUFSIZ], *rest;
    int sems=0, status;
    pid_t p1, p2;

    int fd[2];
    if(pipe(fd) < 0)
    {
        perror("Error in pipe");
        return;
    }

    if((p1 = fork()) != 0)
    {
        if(p1 < 0)
        {
            perror("Fork failed in first child");
            return;
        }
        if((p2 = fork()) != 0)
        {
            if(p2 < 0)
            {
                perror("Fork failed in second child");
                return;
            }
            close(fd[0]);
            close(fd[1]);
            wait(&status);
            wait(&status);
        }
        else
        {
            // fprintf(stderr, "Inside second forked child\n");
            close(fd[1]);
            if(dup2(fd[0], 0) < 0)
            {
                perror("Dup failed");
                return;
            }
            close(fd[0]);

            sems = 0;
            rest = str[1];
            while((cmd_sem2[sems++]=strtok_r(rest, ";", &rest)) != NULL);
            --sems;

            for(int i=0; i<sems; ++i)
            {
                cmd_sem2[i] = remove_whitespace(cmd_sem2[i]);
                cmd_handler(cmd_sem2[i], dir);
            }
            exit(0);
        }
        
    }
    else
    {
        // fprintf(stderr, "Inside first forked child\n");
        close(fd[0]);
        if(dup2(fd[1], 1) < 0)
        {
            perror("Dup failed");
            return;
        }
        close(fd[1]);

        sems = 0;
        rest = str[0];
        while((cmd_sem1[sems++]=strtok_r(rest, ";", &rest)) != NULL);
            --sems;

        for(int i=0; i<sems; ++i)
        {
            cmd_sem1[i] = remove_whitespace(cmd_sem1[i]);
            cmd_handler(cmd_sem1[i], dir);
        }
        exit(0);
    }
    
}
void pipe2(char **str, char *dir)
{
    char *cmd_sem1[BUFSIZ], *cmd_sem2[BUFSIZ], *cmd_sem3[BUFSIZ], *rest;
    int sems=0, status;
    pid_t p1, p2, p3;

    int fd[4];
    if(pipe(fd) < 0)
    {
        perror("Error in first pipe");
        return;
    }
    if(pipe(fd+2) < 0)
    {
        perror("Error in second pipe");
        return;
    }

    if((p1 = fork()) != 0)
    {
        if(p1 < 0)
        {
            perror("Fork failed for first child");
            return;
        }
        if((p2 = fork()) != 0)
        {
            if(p2 < 0)
            {
                perror("Fork failed for second child");
                return;
            }
            if((p3 = fork()) != 0)
            {
                if(p3 < 0)
                {
                    perror("Fork failed for third child");
                    return;
                }
                close(fd[0]);
                close(fd[1]);
                close(fd[2]);
                close(fd[3]);
                wait(&status);
                wait(&status);
                wait(&status);
            }
            else    //third child
            {
                // fprintf(stderr, "Inside third forked child\n");
                close(fd[1]);
                close(fd[3]);
                close(fd[0]);
                if(dup2(fd[2], 0) < 0)
                {
                    perror("Dup failed");
                    return;
                }
                close(fd[2]);

                sems = 0;
                rest = str[2];
                while((cmd_sem3[sems++]=strtok_r(rest, ";", &rest)) != NULL);
                --sems;

                for(int i=0; i<sems; ++i)
                {
                    cmd_sem3[i] = remove_whitespace(cmd_sem3[i]);
                    cmd_handler(cmd_sem3[i], dir);
                }
                exit(0);
            }
            
        }
        else    //second child
        {
            // fprintf(stderr, "Inside second forked child\n");
            close(fd[1]);
            close(fd[2]);
            if(dup2(fd[0], 0) < 0)
            {
                perror("Dup failed");
                return;
            }
            if(dup2(fd[3], 1) < 0)
            {
                perror("Dup failed");
                return;
            }
            close(fd[0]);
            close(fd[3]);

            sems = 0;
            rest = str[1];
            while((cmd_sem2[sems++]=strtok_r(rest, ";", &rest)) != NULL);
            --sems;

            for(int i=0; i<sems; ++i)
            {
                cmd_sem2[i] = remove_whitespace(cmd_sem2[i]);
                cmd_handler(cmd_sem2[i], dir);
            }
            exit(0);
        }
            
    }

    else    //first child
    {
        close(fd[0]);
        close(fd[2]);
        close(fd[3]);
        if(dup2(fd[1], 1) < 0)
        {
            perror("Dup failed");
            return;
        }
        close(fd[1]);

        sems = 0;
        rest = str[0];
        while((cmd_sem1[sems++]=strtok_r(rest, ";", &rest)) != NULL);
            --sems;

        for(int i=0; i<sems; ++i)
        {
            cmd_sem1[i] = remove_whitespace(cmd_sem1[i]);
            cmd_handler(cmd_sem1[i], dir);
        }
        exit(0);
    }
}
char *remove_whitespace(char *str)
{
    int k=0, j=-1, n=strlen(str), flag = 0;
    while(str[k] == ' ')
        ++k;
    for(int i=0; i<n-k; ++i)
    {
        str[i] = str[i+k];
    }
    str[n-k] = '\0';
    for(int i = 0; i<strlen(str); ++i)
    {
        if(j>0 && str[i] == ' ')
        {
            j=i;
        }
        else
            j = -1;
    }
    if(j>0)
        str[j] = '\0';
    // printf("length = %lu :", strlen(str));
    return str;
}
