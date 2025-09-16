#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {

    if (argc <2 || argc > 3)   // fel antal argument
    {
        printf("Fel antal argument");
    }
    else {
        int num_1 = atoi(argv[1]);     // tal som ska kontrolleras
        int num_2 = atoi(argv[2]);     // tal som ska kontrolleras
        int gcd = 1;                    // anta starta gcd som 1

        while (num_1 != num_2)
        {
            if (num_1 > num_2)
                num_1 -= num_2;
            else
                num_2 -= num_1;
        }
        printf("gcd(%s, %s) = %d\n", argv[1], argv[2], num_1); // skriver ut resultatet
    }
    return 0;
}