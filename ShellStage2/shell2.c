#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

char myPath[500] = "/usr/bin:/bin:/sbin";

int findCommand(char *command, char *args[])
{
    char path[500];
    char temp[500];

    int i = 0;
    int j;

    if (strchr(command, '/') != NULL)
    {
        execv(command, args);
        return -1;
    }

    while (myPath[i] != '\0')
    {
        j = 0;

        while (myPath[i] != ':' && myPath[i] != '\0')
        {
            temp[j] = myPath[i];
            i++;
            j++;
        }

        temp[j] = '\0';

        strcpy(path, temp);
        strcat(path, "/");
        strcat(path, command);

        execv(path, args);

        if (myPath[i] == ':')
        {
            i++;
        }
    }

    return -1;
}

int main()
{
    char input[500];
    char *args[50];

    while (1)
    {
        printf("myshell> ");

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("\nExiting shell...\n");
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) == 0)
        {
            continue;
        }

        if (strncmp(input, "PATH=", 5) == 0)
        {
            strcpy(myPath, input + 5);

            printf("PATH changed\n");
            continue;
        }

        int i = 0;
        char *token = strtok(input, " ");

        while (token != NULL && i < 49)
        {
            args[i] = token;
            i++;
            token = strtok(NULL, " ");
        }

        args[i] = NULL;

        if (i == 0)
        {
            continue;
        }

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
            else if (chdir(args[1]) != 0)
            {
                printf("Directory not found\n");
            }

            continue;
        }

        int inputFile = -1;
        int outputFile = -1;
        int commandCount = 0;

        for (int j = 0; j < i; j++)
        {
            if (strcmp(args[j], "<") == 0)
            {
                if (args[j + 1] == NULL)
                {
                    printf("Error: input file missing\n");
                    commandCount = -1;
                    break;
                }

                inputFile = open(args[j + 1], O_RDONLY);

                if (inputFile == -1)
                {
                    printf("Error: cannot open input file\n");
                    commandCount = -1;
                    break;
                }

                args[j] = NULL;
                break;
            }

            if (strcmp(args[j], ">") == 0)
            {
                if (args[j + 1] == NULL)
                {
                    printf("Error: output file missing\n");
                    commandCount = -1;
                    break;
                }

                outputFile = open(args[j + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);

                if (outputFile == -1)
                {
                    printf("Error: cannot open output file\n");
                    commandCount = -1;
                    break;
                }

                args[j] = NULL;
                break;
            }

            commandCount++;
        }

        if (commandCount == -1)
        {
            if (inputFile != -1)
                close(inputFile);

            if (outputFile != -1)
                close(outputFile);

            continue;
        }

        pid_t pid = fork();

        if (pid < 0)
        {
            printf("Fork failed\n");

            if (inputFile != -1)
                close(inputFile);

            if (outputFile != -1)
                close(outputFile);

            continue;
        }

        if (pid == 0)
        {
            if (inputFile != -1)
            {
                close(0);

                if (dup(inputFile) == -1)
                {
                    printf("Input redirection failed\n");
                    exit(1);
                }

                close(inputFile);
            }

            if (outputFile != -1)
            {
                close(1);

                if (dup(outputFile) == -1)
                {
                    printf("Output redirection failed\n");
                    exit(1);
                }

                close(outputFile);
            }

            if (findCommand(args[0], args) == -1)
            {
                printf("Command not found\n");
                exit(1);
            }
        }
        else
        {
            if (inputFile != -1)
                close(inputFile);

            if (outputFile != -1)
                close(outputFile);

            wait(NULL);
        }
    }

    return 0;
}