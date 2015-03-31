#ifndef _LIST_H_
#define _LIST_H_

struct list_t {
  void **data;
  unsigned count;
  unsigned space;
};

typedef struct list_t list_t;

list_t list_new();
void list_free(list_t* list);
void* list_item(list_t* list, unsigned index);
void* list_remove(list_t* list, unsigned index);
void list_insert(list_t* list, void* item);
void list_insert_at(list_t* list, void* item, unsigned index);

#endif//_LIST_H_