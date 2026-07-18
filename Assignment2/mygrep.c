#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int hasWord(char line[], int len, char word[])
{
    int wordLen = 0;

    while (word[wordLen] != '\0')
    {
        wordLen++;
    }

    for (int i = 0; i < len; i++)
    {
        int j = 0;
        int k = i;

        while (j < wordLen && k < len && line[k] == word[j])
        {
            j++;
            k++;
        }

        if (j == wordLen)
        {
            return 1;
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        write(1, "Usage: ./mygrep <file> <word>\n", 30);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);

    if (fd == -1)
    {
        write(1, "FaileD to Open\n", 20);
        return 1;
    }

    char *searchWord = argv[2];

    char line[1024];
    char ch;
    int index = 0;

    while (read(fd, &ch, 1) > 0)
    {
        if (ch != '\n' && index < 1023)
        {
            line[index++] = ch;
        }
        else
        {
            line[index] = '\0';

            if (hasWord(line, index, searchWord))
            {
                write(1, line, index);
                write(1, "\n", 1);
            }

            index = 0;
        }
    }

    if (index > 0)
    {
        line[index] = '\0';

        if (hasWord(line, index, searchWord))
        {
            write(1, line, index);
            write(1, "\n", 1);
        }
    }

    close(fd);

    return 0;
}