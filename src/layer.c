#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "layer.h"
#include "rgba.h"
#include "list.h"

void layer_init(layer_t *l) {
  memset(l, 0, sizeof(layer_t));
  l->need_compose = true;
  l->children = list_new(sizeof(layer_t*));
}

layer_t *layer_new() {
  layer_t *l = malloc(sizeof(layer_t));
  layer_init(l);
  return l;  
}

void layer_resize(layer_t *l, uint32_t w, uint32_t h) {
  if(l->bitmap.data) rgba_free(&l->bitmap);
  l->bitmap = rgba_new(w, h, NULL);
  l->need_compose = true;
}

void layer_init_size(layer_t *l, uint32_t w, uint32_t h) {
  memset(l, 0, sizeof(layer_t));
  layer_resize(l, w, h);
}

void layer_fill(layer_t *l, uint32_t color) {
  uint32_t *dptr = (uint32_t*)l->bitmap.data;
  unsigned n;
  for(n = 0; n < l->bitmap.w * l->bitmap.h; n++) *dptr++ = color;
  l->need_compose = true;
}

void layer_move(layer_t* l, int32_t x, int32_t y, bool relative) {
  l->x = relative ? x + l->x : x;
  l->y = relative ? y + l->y : y;
  if(l->parent) l->parent->need_compose = true;
}


// Removes a child by z index from the specified parent
// Child will not be deleted and may be re-used
layer_t* remove_child_at(layer_t* parent, uint32_t z_index) {
  layer_t* l = (layer_t*)list_remove(&parent->children, z_index);
  if(l) {
    l->parent = NULL;
    parent->need_compose = true;
  }
  return l;

/*  
  // Sanity: z_index in range?
  if(z_index >= parent->children_count) return NULL;
  // Set child to orphan
  layer_t* l = parent->children[z_index];
  l->parent = NULL;
  // Collapse at z_index
  memmove(parent->children + z_index, parent->children + z_index + 1, (parent->children_count - z_index) * sizeof(layer_t*));
  parent->children_count--;
  // Signal composition required
  parent->need_compose = true;
  // Return removed child
  return l;
*/
}

// Removes the specified child from its associated parent
// Child will not be deleted and may be re-used
layer_t* remove_child(layer_t* l) {
  unsigned n;
  // Sanity: child has parent?
  if(!l->parent) return NULL;
  // Find child
  for(n = 0; n < l->parent->children.count; n++) {
    if(list_item(&l->children, n) == l) break;
  }
  // Remove child from parent (will sanity check n)
  return remove_child_at(l->parent, n);
}

void layer_free(layer_t *l) {
  // Remove child if not orphan
  if(l->parent) remove_child(l);
  // Call custom delete function
  if(l->free) l->free(l);
  // Free up memory
  rgba_free(&l->bitmap);
  rgba_free(&l->composite);
  list_free(&l->children);
  free(l);
}

// Adds a child to the end of the display list
void add_child(layer_t* parent, layer_t* l) {
  // Remove child from current parent
  remove_child(l);
  // Insert child and assign to parent
  list_insert(&parent->children, l);
  l->parent = parent;
  // Signal composition required
  parent->need_compose = true;
}

// Adds a child at a specific z index in the display list
// Also used to move a child from one z index to another
void add_child_at(layer_t* parent, layer_t* l, uint32_t z_index) {
  // Remove child from current parent
  remove_child(l);
  // Insert child and assign to parent
  list_insert_at(&parent->children, l, z_index);
  l->parent = parent;
  // Signal composition required
  parent->need_compose = true;
}

// Example: content change
void set_caption(layer_t* l) {
  // do whats needed
  l->need_render = true;
}