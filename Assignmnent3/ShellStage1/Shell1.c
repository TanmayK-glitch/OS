#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    char input[100];
    char *args[20];

    while (1)
    {
        printf("myshell> ");

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("\nExiting shell...\n");
            break;
        }

        // this calculates the index of \n and it is replaced with \0 (null terminator)
        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) == 0)
        {
            continue;
        }

        int i = 0;
        char *token = strtok(input, " ");

        while (token != NULL && i < 19)
        {
            args[i] = token;
            i++;
            token = strtok(NULL, " ");
        }

        args[i] = NULL;

        if (strcmp(args[0], "exit") == 0)
        {
            printf("Exiting shell...\n");
            break;
        }

        if (strcmp(args[0], "cd") == 0)
        {
            if (args[1] == NULL)
            {
                printf("cd: missing directory\n");
            }
            else
            {
                if (chdir(args[1]) != 0)
                {
                    printf("Directory not found\n");
                }
            }
            continue;
        }

        pid_t pid = fork();

        if (pid < 0)
        {
            printf("Fork failed\n");
        }
        else if (pid == 0)
        {
            execvp(args[0], args);
            printf("Command not found\n");
            exit(1);
        }
        else
        {
            wait(NULL);
        }
    }

    return 0;
}