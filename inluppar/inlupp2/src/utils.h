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

char *read_string(void);

bool is_number(char *str);

bool is_float(char *str); 

bool not_empty(char *str); 

bool is_string(char *str);

bool is_in_list(char *str, char *valid_options[], int n);

answer_t make_int(char *str);

answer_t make_float(char *str); 

answer_t make_string(char *str);

answer_t ask_question(char *question, check_func *check, convert_func *convert);

int ask_question_int(char *question);

double ask_question_float(char *question);

char *ask_question_string(char *question);

char *trim(char *str);


char* ask_question_shelf(void);