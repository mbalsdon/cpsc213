#include <stdio.h>
#include <stdlib.h>

#include "int_element.h"
#include "str_element.h"
#include "element.h"
#include "refcount.h"

/* If the string is numeric, return an int_element. Otherwise return a str_element. */
struct element *parse_string(char *str) {
  char *endp;
  /* strtol returns a pointer to the first non-numeric character in endp.
     If it gets to the end of the string, that character will be the null terminator. */
  int value = strtol(str, &endp, 10);
  if(str[0] != '\0' && endp[0] == '\0') {
    /* String was non-empty and strtol conversion succeeded - integer */
    return (struct element *)int_element_new(value);
  } else {
    return (struct element *)str_element_new(str);
  }
}

int compair(const void * a, const void * b) {
  struct element ** first = (struct element **) a;
  struct element ** second = (struct element **) b;
  return (*first)->class->compare(*first, *second);
}

void callback0(struct element ** ptr) {
  int x = 0;
  while (ptr[x]) {
    rc_free_ref(ptr[x]);
    x++;
  }
}

void (*cb_ptr0)(void*) = &callback0;

int main(int argc, char **argv) {
  /* TODO: Read elements into a new array using parse_string */
  struct element ** elmt_list = rc_malloc(argc*sizeof(void *), cb_ptr0); // probably need finalizer
  for (int i=0; i<argc-1; i++) {
    struct element * e = parse_string(argv[i+1]);
    elmt_list[i] = e;
  }

  int i = 0;
  while (elmt_list[i]) i++;

  /* TODO: Sort elements with qsort */
  qsort(elmt_list, i, sizeof(struct element *), compair);
  /* TODO: Print elements, separated by a space */
  int j = 0;
  printf("Sorted: ");
  while (elmt_list[j]) {
    elmt_list[j]->class->print(elmt_list[j]);
    printf(" ");
    j++;
  }
  printf("\n");

  rc_free_ref(elmt_list);

}
