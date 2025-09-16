#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Funktion som beräknar n:te Fibonacci talet rekursivt

int fib(int num) {
    if (num <= 0) {
        return 0;
    } else if (num == 1) {
        return 1;
    } else {
        return fib(num - 1) + fib(num - 2);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s number\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    printf("fib(%s) = %d\n", argv[1], fib(n));

    return 0;
}
