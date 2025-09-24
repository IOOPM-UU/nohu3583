#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>



bool is_number(char *str) {
    int len = strlen(str);

    if (len == 0) {
        return false; // tom sträng är inte ett nummer
    }

    if (str[0] == '-') { //Första tecknet kan vara ett minustecken
        if (len == 1) {     // inte om bara det är ett minustecken
            return false; // endast ett tecken är inte ett nummer
        }
        str++; // Gå till nästa tecken
        len--; // minska längden med 1
    }

    for (int i = 0; i < len; i++) {
        if (!isdigit((char)str[i])) {
            return false; // hittade ett icke-siffrigt tecken
        }
    }

    // for(int i = 0; i < len; i++){
    //   if(str[i] < '1' || str[i] > '9') {
    //     return false;
    //   }
    // }
    return true; // alla tecken är siffror
}

int main(int argc, char *argv[]) {

    if (argc <2 || argc > 3)   // fel antal argument
    {
        printf("Fel antal argument");
    }
    else {
        int num_1 = atoi(argv[1]);     // tal som ska kontrolleras
        int num_2 = atoi(argv[2]);     // tal som ska kontrolleras
        if (is_number(argv[1]) && num_1 > 0 && is_number(argv[2]) && num_2 > 0) {
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
        else {
        printf("Båda inputsen måste vara icke negativa heltal\n");
    }
    }
    return 0;
}