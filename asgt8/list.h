#ifndef __list_h__
#define __list_h__

/**
 * element_t * data;    // ptr. to data[0]
 * int         len;     // num. elements in list
 * int         size;    // num. slots in list
 */
struct list;
typedef void* element_t;


/* Create new empty list
   Mallocs the struct list and the data pointer */
struct list* list_create       ();
/* Destroy list
   Frees the data pointer and the struct list */
void         list_destroy      (struct list* list);
/* Append element to back of list
   Expands if nececssary */
void         list_append       (struct list* list, element_t element);
/* Appends n things (LR) from elements to back of list
   Basically just for-loops list_append */
void         list_append_array (struct list* list, element_t* elements, int n);
/* Inserts element at list->data[pos]
   Pushes elements right */
void         list_insert       (struct list* list, int pos, element_t element);
/* Removes element at list->data[pos]
   Elements move left to occupy */
void         list_remove       (struct list* list, int pos);
/* Returns element at list->data[pos] */
element_t    list_get          (struct list* list, int pos);
/* Returns index of element 
   Uses function ptr: int (*equal) (element_t, element_t) */
int          list_index        (struct list* list, element_t element, int (*equals) (element_t, element_t));
/* Returns len (size) */
int          list_len          (struct list* list);

/* Specs in .c file
   Directly changes out_list */
void         list_map1         (void (*f) (element_t*, element_t),            struct list* out_list,  struct list* in_list);
void         list_map2         (void (*f) (element_t*, element_t, element_t), struct list* out_list,  struct list* in_list0, struct list* in_list1);
void         list_foldl        (void (*f) (element_t*, element_t, element_t), element_t* out_value_p, struct list* in_list);
void         list_filter       (int  (*f) (element_t),                        struct list* out_list,  struct list* in_list);
void         list_foreach      (void (*f) (element_t),                        struct list* list);

#endif /* __list_h__ */
