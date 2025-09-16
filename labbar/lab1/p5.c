#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc < 2 || argc > 3)   // fel antal argument
    {
        printf("Fel antal argument");
    }
    else
    {
        int tal = atoi(argv[1]);     // tal som ska kontrolleras
        int is_prime = 1;            // anta att tal är ett primtal

        if (tal < 2) {
            is_prime = 0; }   // 0 och 1 är inte primtal

        for (int j = 2; j <= tal /2; j++) // kontrollera delbarhet
        {
            if (tal % j == 0)
            {
                is_prime = 0;
                break;
            }
        }

        if (is_prime)
            printf("%d is a prime number\n", tal);
        else
            printf("%d is not a prime number\n", tal);
    }
    return 0;
}
