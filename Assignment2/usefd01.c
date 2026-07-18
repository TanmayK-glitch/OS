#include <stdio.h>
#include <unistd.h>

int main() {

    int buffer[16];
    int num = 0;
    char ch;
    int i = 0;

    while (i < 16 && read(0, &ch, 1) > 0) {
        if (ch == '\n') break;

        if (ch >= '0' && ch <= '9') {
            buffer[i++] = ch - '0';
        }
    }

    // Convert digits to integer
    for (int j = 0; j < i; j++) {
        num = (num * 10) + buffer[j];
    }

    // Print using printf
    printf("\nYou entered: %d\n", num);

    // ------------------------------------------

    int x = 999666333;
    char result_buffer[16];
    int len = 0;

    while (x > 0) {
        result_buffer[len++] = (x % 10) + '0';
        x /= 10;
    }

    printf("Length :%d\n", len);

    // Print the number
    for (int k = len - 1; k >= 0; k--) {
        write(1, &result_buffer[k], 1);
    }

    write(1, "\n", 1);

    return 0;
}