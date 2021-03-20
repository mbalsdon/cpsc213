#include "int_element.h"
#include <stdio.h>
#include <refcount.h>
#include "str_element.h"

/* TODO: Implement all public int_element functions, including element interface functions.

You may add your own private functions here too. */

struct int_element_class {
    void (*print)(struct element *);
    int (*compare)(struct element *, struct element *);
};

struct int_element {
    struct int_element_class * class;
    int item;
};

void int_print(void * thisv) {
    struct int_element * this = thisv;
    printf("%d", this->item);
}

int int_compare(struct element * first, struct element * second) {
    /* If both elements are str_elements */
    if (is_str_element(first) && is_str_element(second)) return 0;
    /* If first element is a str_element */
    if (is_str_element(first)) return 1;
    /* If second element is a str_element */
    if (is_str_element(second)) return -1;
    /* Two int_elements case */
    struct int_element * e1 = first;
    struct int_element * e2 = second;
    if (e1->item < e2->item) return -1;
    if (e1->item == e2->item) return 0;
    /* e1->item > e2->item */
    return 1;
}

struct int_element_class int_element_class = { int_print, int_compare };

struct int_element * int_element_new(int i) {
    struct int_element * obj = rc_malloc(sizeof(struct int_element), NULL);
    obj->class = &int_element_class;
    obj->item = i;
    return obj;
}

int int_element_get_value(struct int_element * elmt) {
    return elmt->item;
}

int is_int_element(struct element * elmt) {
    int a = elmt->class->print == int_element_class.print;
    int b = elmt->class->compare == int_element_class.compare;
    return (a && b);
}