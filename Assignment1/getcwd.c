#include <stdio.h>
#include <unistd.h>
int main(){
    // getcwd(char *buffer, int size_t)
    char path[500];

    getcwd(path, 500);

    printf("%s\n", path);
    return 0;

}