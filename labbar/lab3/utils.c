#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

typedef union {
    int   int_value;
    float float_value;
    char *string_value;
} answer_t;


typedef bool (check_func)(char *);       // check function: char* -> bool
typedef answer_t (convert_func)(char *); // convert function: char* -> answer_t

char *read_string(void) {
    static char buffer[256];
    if (fgets(buffer, sizeof(buffer), stdin)) {
        buffer[strcspn(buffer, "\n")] = '\0';
        return buffer;
    }
    return NULL;
}

bool is_number(char *str) {
    if (*str == '\0') return false;
    if (*str == '-' || *str == '+') str++;
    if (*str == '\0') return false;

    while (*str) {
        if (!isdigit((unsigned char)*str)) return false;
        str++;
    }
    return true;
}

bool is_float(char *str) {
    if (*str == '\0') return false;
    char *endptr;
    strtod(str, &endptr);
    return (*endptr == '\0');
}

bool not_empty(char *str) {
    return strlen(str) > 0;
}

bool is_string(char *str) {
    if (!not_empty(str)){
        return false;
    }
    return !is_number(str);
}

bool is_in_list(char *str, char *valid_options[], int n) {
    if (!not_empty(str)) {
        return false; // empty input not valid
    }

    for (int i = 0; i < n; i++) {
        if (strcasecmp(str, valid_options[i]) == 0) {
            return true; // match found
        }
    }
    return false; // no match
}

answer_t make_int(char *str) {
    answer_t a;
    a.int_value = atoi(str);
    return a;
}

answer_t make_float(char *str) {
    answer_t a;
    a.float_value = atof(str);
    return a;
}

answer_t make_string(char *str) {
    answer_t a;
    a.string_value = strdup(str); // strdup creates a copy of the input string
    return a;
}

answer_t ask_question(char *question, check_func *check, convert_func *convert) {
    char *input;
    while (1) {
        printf("%s", question);
        input = read_string();
        if (input && check(input)) {
            return convert(input);
        } else {
            printf("Felaktigt format, försök igen!\n");
        }
    }
}

int ask_question_int(char *question) {
    return ask_question(question, is_number, make_int).int_value;
}

double ask_question_float(char *question) {
    return ask_question(question, is_float, make_float).float_value;
}


char *ask_question_string(char *question) {
    return ask_question(question, is_string, make_string).string_value;
}

