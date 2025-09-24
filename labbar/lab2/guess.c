#include "utils.h"
#include <stdio.h>
#include <stdlib.h>  // för rand()

int main(void)
{
    int nmr = rand() % 1024;  // slumpa ett tal mellan 0 och 1023
    int amount_of_guesses = 0;
    int sant = 0;

    char name[100];  // buffert för namn
    ask_question_string("What is your name?\n", name, sizeof(name));

    printf("Du %s, jag tänker på ett nummer ... kan du gissa vilket?\n", name);

    while(amount_of_guesses < 15) {
        int guess = ask_question_int("Gissa ett tal:\n");
        if (guess < nmr) {
            printf("För litet!\n");
        } else if (guess > nmr) {
            printf("För stort!\n");
        } else {
            printf("Bingo!\n");
            sant = 1;
            break;
        }
        amount_of_guesses++;
    }

    if (sant) {
        printf("Det tog %s %d gissningar att komma fram till rätt nummer!\n", name, amount_of_guesses);
    } else {
        printf("Nu har du slut på gissningar! Jag tänkte på %d!\n", nmr);
    }

    return 0;
}
