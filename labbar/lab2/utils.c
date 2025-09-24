#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"

void clear_input_buffer(void)
{
    int c;
    do
    {
        c = getchar();
    }
    while (c != '\n' && c != EOF);
}

int ask_question_int(char *question)
{
    int result = 0;
    int conversions = 0;

    do
    {
        printf("%s", question);
        conversions = scanf("%d", &result);
        clear_input_buffer();  // töm bufferten efter varje inläsning
    }
    while (conversions < 1);

    return result;
}

char *ask_question_string(char *question, char *buf, int buf_siz)
{
    int i = 0;

    printf("%s", question);
    i = read_string(buf, buf_siz);

    while(i == 0) // tom sträng, fråga igen
    {
        printf("Du måste ange något!\n");
        printf("%s\n", question);
        i = read_string(buf, buf_siz);
    }

    return buf;
}

int read_string(char *buf, int buf_siz) {
    int count = 0;
    int c;

    if (buf_siz <= 0) {
        return 0; // ogiltig buffertstorlek
    }

    while(count < buf_siz - 1) { // Sista platsen för null-terminering
        c = getchar();
        if(c == EOF || c == '\n') {
            break; // Avsluta vid EOF eller ny rad
        }
        buf[count++] = (char)c; // Lägg till tecken i bufferten
    }

    buf[count] = '\0'; // Null-terminera strängen

    if (c != '\n' && c != EOF) {
        clear_input_buffer(); // Rensa resterande tecken i inmatningsbufferten
    }

    return count; // Returnera antal lästa tecken
}

void print(const char *str) {
    int i = 0;
    while (str[i] != '\0') {
        putchar(str[i]);
        i++;
    }
}

void println(const char *str) {
    print(str);
    putchar('\n');
}
