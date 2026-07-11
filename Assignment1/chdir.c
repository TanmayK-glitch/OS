#include <stdio.h>
#include <unistd.h>
int main(){
    // chdir(char* path)
    char path[500];
    char customPath[500];

    getcwd(path, 500);
    printf("Before: %s\n", path);

    printf("Enter valid dir (press 0 to stop): ");
    scanf("%s", customPath);
    
    if(customPath[0] == '0' && customPath[1] == '\0'){
        return 0;
    }
    
    chdir(customPath);

    getcwd(path, 500);
    printf("After: %s\n", path);

    return 0;
}