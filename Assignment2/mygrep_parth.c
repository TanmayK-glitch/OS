#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

void checkAndPrint(char temp[], int len, char str[]) {
	int strLength = strlen(str);
	for(int i = 0; i < len; i++) {
		int j = 0;
		int k = i;
		if(str[j] == temp[k]) {
			while(j < strLength && k < len && str[j] == temp[k]) {
				j++;
				k++;
			}
			if(j == strLength) {
				write(1, temp, len);
				write(1, "\n", 1);
				return;
			}
		}
	}
}

int main(int argc, char* argv[]) {
	int fd;
	fd = open(argv[1], O_RDONLY);
	if(fd == -1) {
		printf("Couldn't open %s", argv[1]);
		return 1;
	}
	char* word = argv[2];
	char temp[1024];
	char ch;
	int i = 0;
	while(read(fd, &ch, 1) > 0) {
		if(ch != '\n' && i < 1023)  {
			temp[i++] = ch;
		}
		else {
			temp[i] = '\0';
			checkAndPrint(temp, i, word);
			i = 0;
		}	
	} 
	if(i > 0) {
		temp[i] = '\0';
		checkAndPrint(temp, i, word);
	}
	close(fd);
	return 0;
}
