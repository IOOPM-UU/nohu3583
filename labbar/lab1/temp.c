#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> 
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
    return true; // alla tecken är siffror
}



int main (int argc, char *argv[]) {
    if (argc > 1 && is_number(argv[1]))
{
  printf("%s is a number\n", argv[1]);
}
else
{
  if (argc > 1)
  {
    printf("%s is not a number\n", argv[1]);
  }
  else
  {
    printf("Please provide a command line argument!\n");
  }
}
    return 0;
}

