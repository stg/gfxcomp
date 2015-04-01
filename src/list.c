#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "list.h"

// Create and return a list
list_t list_new() {
  list_t list;
  list.data = NULL;
  list.space = 0;
  list.count = 0;
  return list;
}

// Frees all memory associated with a list
void list_free(list_t* list) {
  if(list->data) free(list->data);
  list->data = NULL;
  list->count = 0;
  list->space = 0;
}

// Fetches an item from the list
void* list_item(list_t* list, unsigned index) {
  return list->data[index];
}

// Removes an item from the list
void* list_remove(list_t* list, unsigned index) {
  void* temp;
  // Sanity: index in range?
  if(index >= list->count) return NULL;
  temp = list->data[index];
  // Collapse at z_index
  memmove(list->data + index, list->data + index + 1, (list->count - index) * sizeof(void*));
  list->count--;
  return temp;
}

// Adds an item to the end of the list
void list_insert(list_t* list, void* item) {
  // Need more space?
  if(list->space == list->count) {
    // Grow and allocate new space
    list->space = list->space == 0 ? 1 : list->space << 1;
    void *temp = list->data;
    list->data = malloc(list->space * sizeof(void*));
    // Copy over
    if(temp) memcpy(list->data, temp, list->count * sizeof(void*));
  }
  // Add child
  list->data[list->count++] = item;
}

// Adds an item at the specified index in the list
void list_insert_at(list_t* list, void* item, unsigned index) {
  // Sanity: limit index
  if(index > list->count) index = list->count;
  // Need more space?
  if(list->space == list->count) {
    // Grow and allocate new space
    list->space = list->space == 0 ? 1 : list->space << 1;
    void *temp = list->data;
    list->data = malloc(list->space * sizeof(void*));
    // Copy over
    if(temp) {
      memcpy(list->data, temp, index * sizeof(void*));
      memcpy(list->data + index + 1, temp + index, (list->count - index) * sizeof(void*));
    }
  } else {
    // Skoot over
    memmove(list->data + index + 1, list->data + index, (list->count - index) * sizeof(void*));
  }
  list->data[index] = item;
  list->count++;
}
