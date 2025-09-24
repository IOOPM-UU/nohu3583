#include "utils.h"
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

struct item
{
    char* Name;
    char* Desc;
    int Price; // in ören
    char* Shelf;
};

typedef struct item item_t;

item_t make_item(char* name, char* desc, int price, char* shelf) {
    item_t t;
    t.Name = name;
    t.Desc = desc;
    t.Price = price;
    t.Shelf = shelf;
    return t;
}

void print_item(const item_t *t)
{
    printf("Name:  %s\n",  t->Name);
    printf("Desc:  %s\n",  t->Desc);
    printf("Price: %d.%02d SEK\n", t->Price / 100, t->Price % 100);
    printf("Shelf: %s\n",  t->Shelf);
    printf("\n");
}

void print_menu(){
    printf("[L]ägga till en vara\n"
           "[T]a bort en vara\n"
            "[R]edigera en vara\n"
            "Ån[g]ra senaste ändringen\n"
            "Lista [h]ela varukatalogen\n"
            "[A]vsluta\n");
}

char ask_question_menu() {
    print_menu();
    char *val = ask_question_string("Ange val: ");
    char *valid[] = {"L", "T", "R", "G", "H", "A"};
    int n = sizeof(valid) / sizeof(valid[0]);

    while (strlen(val) != 1 || !is_in_list(val, valid, n)) {
        printf("Valet är fel, du får endast välja mellan det som är i menyn med [] runt\n");
        free(val);
        val = ask_question_string("Ange val:");
    }
    char result = val[0];  // only take the first character
    free(val);
    return result;
}

char* ask_question_shelf(void) {
    char *input;
    bool valid = false;

    while (!valid) {
        input = ask_question_string("Enter shelf place (Format: Letter followed by number(s))");

        int len = strlen(input);
        if (len >= 2 && isalpha((unsigned char)input[0])) {
            valid = true;
            for (int i = 1; i < len; i++) {
                if (!isdigit((unsigned char)input[i])) {
                    valid = false;
                    break;
                }
            }
        }

        if (!valid) {
            printf("Shelf must start with a letter followed by numbers (e.g., A25). Try again!\n");
            free(input);
        }
    }

    return input;
}

item_t input_item(void) {
    char* name_prod = ask_question_string("Enter name of product:");
    char* desc_prod = ask_question_string("Enter description:");
    int price = ask_question_int("Enter price of item (in ören):");
    char* shelf_prod = ask_question_shelf();

    return make_item(name_prod, desc_prod, price, shelf_prod);

}

void change_item(item_t *t) {
    char* name_prod = ask_question_string("Enter name of product:");
    char* desc_prod = ask_question_string("Enter description:");
    int price = ask_question_int("Enter price of item (in ören):");
    char* shelf_prod = ask_question_shelf();

    *t = make_item(name_prod, desc_prod, price, shelf_prod);
}

void add_item_to_db(item_t db[], int *db_size){
    item_t item = input_item();
    db[*db_size] = item;
    (*db_size)++;
}
void list_db(item_t *items, int no_items) {
    for (int i = 0; i < no_items; i++) {  
        printf("%d. %s\n", i + 1, items[i].Name);
    }
}

void remove_item_from_db(item_t db[], int *db_size) {
    list_db(db, *db_size);

    int val = ask_question_int("Välj vilken vara du vill ta bort:");

    while (val < 1 || val > *db_size) {
        printf("Valet är utanför intervall av artiklar, försök igen\n");
        list_db(db, *db_size);
        val = ask_question_int("Välj vilken vara du vill ta bort:");
    }

    for (int i = val - 1; i < *db_size - 1; i++) {
        db[i] = db[i + 1];
    }

    // Decrease database size
    (*db_size)--;

    printf("Vara borttagen.\n");
}


int string_length(const char *str)
{
    int len = 0;
    while (str[len] != '\0')
        len++;
    return len;
}

char* magick(char *array1[], char *array2[], char *array3[], int len) {
    char *word1 = array1[rand() % len];
    char *word2 = array2[rand() % len];
    char *word3 = array3[rand() % len];

    char buf[255];
    snprintf(buf, sizeof(buf), "%s-%s %s", word1, word2, word3);
    return strdup(buf);
}

void edit_db(item_t *items, int size) {
    int val = ask_question_int("Choose which item to edit:");

    while(val < 1 || val > size) {
        printf("Invalid choice, try again. Valid items:\n");
        list_db(items, size);
        val = ask_question_int("Choose which item to edit:");
    }

    print_item(&items[val-1]);
    change_item(&items[val-1]);
}
void event_loop(item_t *db, int *db_size) {
    while (true) {
        char event = ask_question_menu();
        event = toupper(event);

        if(event == 'L') {
            add_item_to_db(db, db_size);
        } 
        else if(event == 'T') {
            remove_item_from_db(db, db_size);
        } 
        else if(event == 'R') {
            edit_db(db, *db_size);
        }
        else if(event == 'G') {
            printf("Not yet implemented!\n");
        }
        else if(event == 'H') { 
            list_db(db, *db_size);
        }
        else {
            printf("Exiting program.\n");
            break; // terminate the loop
        }
    }
}


int main(void){
    item_t db[16]; // Array med plats för 16 varor
    int db_siz = 0; // Antalet varor i arrayen just nu
    event_loop(db, &db_siz);
    return 0;
}
