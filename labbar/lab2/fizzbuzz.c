#include <stdio.h>
#include <stdlib.h>

void print_fizzbuzz(int n) {
    if(n % 3 == 0 && n % 5 == 0)
        printf("FizzBuzz");
    else if(n % 3 == 0)
        printf("Fizz");
    else if(n % 5 == 0)
        printf("Buzz");
    else
        printf("%d", n);
}

int main(int argc, char *argv[])
{
    int tal = atoi(argv[1]);
    for(int i = 1; i <= tal; i++)
    {
        print_fizzbuzz(i);
        if (i != tal)
            printf(", ");
        else
            printf("\n");
    }
}