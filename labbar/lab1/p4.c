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
        int rows = atoi(argv[1]);     // antal rader
        int growth = atoi(argv[2]);   // tillväxt
        int total = 0;                // total antal stjärnor

        for (int i = 1; i <= rows; i++)   // loop för varje rad
        {
            int stars = i * growth;     // antal stjärnor på aktuell rad
            total += stars;           // lägg till antal stjärnor till totalen

            for (int j = 0; j < stars; j++) // loop för att skriva ut stjärnor
            {
                printf("*");    // skriv ut en stjärna
            }
            printf("\n");   // ny rad efter varje rad av stjärnor
        }

        printf("Totalt: %d\n", total); // skriv ut total antal stjärnor
    }

    return 0;
}
