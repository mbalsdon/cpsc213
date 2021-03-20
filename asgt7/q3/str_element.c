#include "str_element.h"
#include <stdio.h>
#include <refcount.h>
#include <string.h> 
#include <stdlib.h>
#include "int_element.h"

/* TODO: Implement all public str_element functions, including element interface functions.

You may add your own private functions here too. */

struct str_element_class {
    void (*print)(struct element *);
    int (*compare)(struct element *, struct element *);
};

struct str_element {
    struct str_element_class * class;
    char * item;
};

void str_print(void * thisv) {
    struct str_element * this = thisv;
    printf("%s", this->item);

}

int str_compare(struct element * first, struct element * second) {
    /* If both elements are int_elements */
    if (is_int_element(first) && is_int_element(second)) return 0;
    /* If first element is an int_element */
    if (is_int_element(first)) return -1;
    /* If second element is an int_element */
    if (is_int_element(first)) return 1;
    /* Two str_elements case */
    /* Compares first char byte value */
    struct str_element * e1 = first;
    struct str_element * e2 = second;
    if (e1->item[0] < e2->item[0]) return -1;
    if (e1->item[0] == e2->item[0]) return 0;
    /* e1->item[0] > e2->item[0] */
    return 1;
}

struct str_element_class str_element_class = { str_print, str_compare };

void callback(struct str_element * ptr) {
    free(ptr->item);
}

void (*cb_ptr)(void*) = &callback;

struct str_element * str_element_new(char* s) {
    struct str_element * obj = rc_malloc(sizeof(struct str_element), cb_ptr);
    obj->class = &str_element_class;
    char * new_s = strdup(s);
    obj->item = new_s;
}

char * str_element_get_value(struct str_element * elmt) {
    return elmt->item;
}

int is_str_element(struct element * elmt) {
    int a = elmt->class->print == str_element_class.print;
    int b = elmt->class->compare == str_element_class.compare;
    return (a && b);
}