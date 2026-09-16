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
    char *commands[20];

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
            if (strlen(input + 5) == 0)
            {
                printf("PATH: missing path\n");
            }
            else
            {
                strcpy(myPath, input + 5);
                printf("PATH changed\n");
            }

            continue;
        }

        if (strcmp(input, "exit") == 0)
        {
            printf("Exiting shell...\n");
            break;
        }

        int commandCount = 0;

        char *token = strtok(input, "|");

        while (token != NULL && commandCount < 20)
        {
            commands[commandCount] = token;
            commandCount++;

            token = strtok(NULL, "|");
        }

        if (commandCount == 0)
        {
            continue;
        }

        if (commandCount == 1)
        {
            char *tempArgs[20];
            int tempCount = 0;

            char *tempToken = strtok(commands[0], " ");

            while (tempToken != NULL && tempCount < 19)
            {
                tempArgs[tempCount] = tempToken;
                tempCount++;

                tempToken = strtok(NULL, " ");
            }

            tempArgs[tempCount] = NULL;

            if (tempCount == 0)
            {
                continue;
            }

            if (strcmp(tempArgs[0], "cd") == 0)
            {
                if (tempArgs[1] == NULL)
                {
                    printf("cd: missing directory\n");
                }
                else if (chdir(tempArgs[1]) != 0)
                {
                    printf("Directory not found\n");
                }

                continue;
            }
        }

        int pipes[19][2];

        for (int i = 0; i < commandCount - 1; i++)
        {
            if (pipe(pipes[i]) == -1)
            {
                printf("Pipe failed\n");
                continue;
            }
        }

        for (int i = 0; i < commandCount; i++)
        {
            pid_t pid = fork();

            if (pid < 0)
            {
                printf("Fork failed\n");
                continue;
            }

            if (pid == 0)
            {
                if (i > 0)
                {
                    close(0);
                    dup(pipes[i - 1][0]);
                }

                if (i < commandCount - 1)
                {
                    close(1);
                    dup(pipes[i][1]);
                }

                for (int j = 0; j < commandCount - 1; j++)
                {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }

                char *args[50];
                int argCount = 0;

                char *arg = strtok(commands[i], " ");

                while (arg != NULL && argCount < 49)
                {
                    args[argCount] = arg;
                    argCount++;

                    arg = strtok(NULL, " ");
                }

                args[argCount] = NULL;

                if (argCount == 0)
                {
                    printf("Empty command\n");
                    exit(1);
                }

                int inputFile = -1;
                int outputFile = -1;

                for (int j = 0; j < argCount; j++)
                {
                    if (strcmp(args[j], "<") == 0)
                    {
                        if (args[j + 1] == NULL)
                        {
                            printf("Error: input file missing\n");
                            exit(1);
                        }

                        inputFile = open(args[j + 1], O_RDONLY);

                        if (inputFile == -1)
                        {
                            printf("Error: cannot open input file\n");
                            exit(1);
                        }

                        args[j] = NULL;
                    }

                    if (strcmp(args[j], ">") == 0)
                    {
                        if (args[j + 1] == NULL)
                        {
                            printf("Error: output file missing\n");
                            exit(1);
                        }

                        outputFile = open(
                            args[j + 1],
                            O_WRONLY | O_CREAT | O_TRUNC,
                            0644
                        );

                        if (outputFile == -1)
                        {
                            printf("Error: cannot open output file\n");
                            exit(1);
                        }

                        args[j] = NULL;
                    }
                }

                if (inputFile != -1)
                {
                    close(0);
                    dup(inputFile);
                    close(inputFile);
                }

                if (outputFile != -1)
                {
                    close(1);
                    dup(outputFile);
                    close(outputFile);
                }

                if (findCommand(args[0], args) == -1)
                {
                    printf("Command not found\n");
                    exit(1);
                }
            }
        }

        for (int i = 0; i < commandCount - 1; i++)
        {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        for (int i = 0; i < commandCount; i++)
        {
            wait(NULL);
        }
    }

    return 0;
}