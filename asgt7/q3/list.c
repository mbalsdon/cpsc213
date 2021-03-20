#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "refcount.h"

struct list {
  struct list_node *head;
  struct list_node *tail;
};

struct list_node {
  struct element *elem;
  struct list_node *prev;
  struct list_node *next;
};

/**
 * Create new list
 */
struct list *list_new() {
  struct list *l = malloc(sizeof(*l));
  l->head = l->tail = NULL;
  return l;
}

/**
 * Delete the list (including its elements).
 */
void list_delete(struct list *l) {
  for(struct list_node *n = l->head, *next; n != NULL; n = next) {
    next = n->next;
    list_delete_element(l, n); // handles freeing node/element
  }
  free(l); // frees the struct list
}

/**
 * Add an element to the list.
 * This list takes ownership of the element pointer and will free it during deletion.
 * Returns pointer to newly added list node.
 */
struct list_node *list_add_element(struct list *l, struct element *e) {
  struct list_node *n = malloc(sizeof(*n)); // make node which n points to
  rc_keep_ref(e); // e is used somewhere (in list) so we keepref 
  n->elem = e; // put element in node
  n->prev = n->next = NULL; // prev and next dont point anywhere yet

  if(l->head == NULL) { // basic linked list addition stuff
    l->head = n;
  } else {
    l->tail->next = n;
    n->prev       = l->tail;
  }
  l->tail = n;

  return n; // return n (pointer to node)
}

/**
 * Remove element from list and free it
 */
void list_delete_element(struct list *l, struct list_node *n) {
  if (n == l->head)
    l->head = n->next;
  else
    n->prev->next = n->next;

  if (n == l->tail)
    l->tail = n->prev;
  else
    n->next->prev = n->prev;

  n->next = n->prev = NULL;
  rc_free_ref(n->elem); // remove a ref to the element
  free(n); // free the struct list_node
}

/**
 * Get first element of list.
 */
struct list_node *list_get_head(struct list *l) {
  return l->head;
}

/**
 * Get next element of list.
 */
struct list_node *list_node_get_next(struct list_node *n) {
  return n->next;
}

/**
 * Get list node element. Caller should keep_ref.
 */
struct element *list_node_get_element(struct list_node *n) {
  return n->elem;
}
