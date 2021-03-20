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
   if (is_int_element(first) && is_str_element(second)) return -1;
   if (is_str_element(first) && is_int_element(second)) return 1;

   struct str_element * e1 = first;
   struct str_element * e2 = second;
   return strcmp(e1->item, e2->item);
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