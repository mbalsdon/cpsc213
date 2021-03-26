#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h"

void print_string(element_t ev) {
    printf("%s\n", (char*) ev);
}

void print_int(element_t ev) {
    int *e = ev;
    printf("%d\n", *e);
}

void parse_string(element_t* rv, element_t av) {
    char* a = av;
    int** r = (int**) rv;
    *r = malloc(sizeof(int));

    char *endp;
    int val = strtol(a, &endp, 10);
    if(val) {
        **r = val;
    } else {
        **r = -1;
    }
}

void stringy_thingy(element_t* rv, element_t av, element_t bv) {
    char* a = (char*) av;
    int* b = (int*) bv;
    char** r = (char**) rv;

    if (*b == -1) {
        *r = a;
    } else {
        *r = NULL;
    }
}

int is_positive(element_t av) {
    int* a = (int*) av;
    return (*a != -1);
}

int not_null(element_t av) {
    char* a = (char*) av;
    return (a != NULL);
}

void der_truncinator(element_t* rv, element_t av, element_t bv) {
    int* a = (int*) av;
    char* b = (char*) bv;
    char** r = (char**) rv;
    *r = malloc(sizeof(char)*(*a)+1);

    memset(*r, '\0', *a + 1);
    strncpy(*r, b, *a);
}

void le_concatenator(element_t ev) {
    printf("%s ", (char*) ev);
}

void dr_doofs_maximizinator_4000(element_t* rv, element_t av, element_t bv) {
    int *a = (int*) av;
    int *b = (int*) bv;
    int **r = (int**) rv;
    
    if (*b > *a) {
        **r = *b;
    }

}

int main(int argc, char** argv[]) {

    struct list* uflist = list_create();
    list_append_array(uflist, (element_t*) &argv[1], argc-1);

    struct list* intlist = list_create();
    list_map1(parse_string, intlist, uflist);

    struct list* strlist = list_create();
    list_map2(stringy_thingy, strlist, uflist, intlist);

    struct list* poslist = list_create();
    list_filter(is_positive, poslist, intlist);

    struct list* nonullslist = list_create();
    list_filter(not_null, nonullslist, strlist);

    struct list* trunclist = list_create();
    list_map2(der_truncinator, trunclist, poslist, nonullslist);

    list_foreach(print_string, trunclist);

    list_foreach(le_concatenator, trunclist);
    printf("\n");
    
    element_t max_val = list_get(poslist, 0);
    list_foldl(dr_doofs_maximizinator_4000, &max_val, poslist);
    if (max_val != NULL) {
        print_int(max_val);
    }

    list_foreach(free, trunclist);
    list_foreach(free, intlist);
    list_destroy(uflist);
    list_destroy(intlist);
    list_destroy(strlist);
    list_destroy(poslist);
    list_destroy(nonullslist);
    list_destroy(trunclist);
}