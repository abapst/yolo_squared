#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct item
{
    int id;
    void *data;
    struct item *next;
} item_t;

/* Function declarations */
//typedef struct item item_t;
item_t *list_init();
void print_list(item_t *head);
void push_end(item_t *head, int id, void *data);
void push_start(item_t **head, int id, void *data);
item_t *pop_start(item_t **head);
item_t *pop_end(item_t **head);
void delete_list(item_t **head);
void delete_node(item_t *node);
item_t *reverse_list(item_t *head);
int list_length(item_t *head);
/************************/

#endif
