#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "layer.h"
#include "composer.h"
#include "rgba.h"

bool compose(layer_t *l) {
  layer_t **children, *child;
  unsigned n;
  
  // Do we need to render our local bitmap?
  if(l->need_render) {
    l->render(l);
    l->need_render = false;
    l->need_compose = true;
  }

  // Reset composite size
  l->composite_w = l->bitmap_w;
  l->composite_h = l->bitmap_h;

  // Render children where necessary
  n = l->children_count;
  if(n) {
    children = l->children;
    while(n--) {
      child = *children++;
      if(compose(child)) {
        // Rendering resulted in an alteration of the childs composite bitmap.
        // This means we need to re-composite as well.
        l->need_compose = true;
      }
      // Track the required size of our composite bitmap.
      if(child->x + child->composite_w > l->composite_w) l->composite_w = child->x + child->composite_w;
      if(child->y + child->composite_h > l->composite_h) l->composite_h = child->y + child->composite_h;
    }
  }

  // Do we need to re-compose?
  if(l->need_compose) {
    if(l->composite && l->composite != l->bitmap) rgba_free(l->composite);
    if(l->children) {
      // Have children - compose them
      l->composite = rgba_alloc(l->composite_w, l->composite_h);
      // Render our local bitmap first
      if(l->bitmap) rgba_draw(l->composite, l->composite_w, l->bitmap, 0, 0, l->bitmap_w, l->bitmap_h, l->bitmap_w);
      n = l->children_count;
      if(n) {
        children = l->children;
        while(n--) {
          child = *children++;
          rgba_draw(l->composite, l->composite_w, child->composite, child->x, child->y, child->composite_w, child->composite_h, child->composite_w);
          child++;
        }
      }
    } else {
      // No children - composite and bitmap are identical
      l->composite = l->bitmap;
      l->composite_w = l->bitmap_w;
      l->composite_h = l->bitmap_h;
    }
    // Composition done
    l->need_compose = false;
    // Signal composition performed
    return true;
  }

  // Signal composition unaltered
  return false;
}

// Removes a child by z index from the specified parent
// Child will not be deleted and may be re-used
layer_t* remove_child_at(layer_t* parent, uint32_t z_index) {
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
}

// Removes the specified child from its associated parent
// Child will not be deleted and may be re-used
layer_t* remove_child(layer_t* l) {
  unsigned n;
  // Sanity: child has parent?
  if(!l->parent) return;
  // Find child
  for(n = 0; n < l->parent->children_count; n++) {
    if(l->parent->children[n] == l) break;
  }
  // Remove child from parent (will sanity check n)
  return remove_child_at(l->parent, n);
}

// Deletes a child and frees up its associated memory
// Child can not be re-used after this
void delete_child(layer_t* l) {
  // Remove child if not orphan
  if(l->parent) remove_child(l);
  // Call custom delete function
  if(l->delete) l->delete(l);
  // Free up memory
  free(l);
}

// Adds a child to the end of the display list
void add_child(layer_t* parent, layer_t* l) {
  // Remove child from current parent
  if(l->parent) remove_child(l);
  // Need more space?
  if(parent->children_space == parent->children_count) {
    // Grow space
    size_t temp_space = parent->children_space;
    parent->children_space = temp_space == 0 ? 4 : temp_space << 1;
    // Allocate new space
    layer_t **temp = parent->children;
    parent->children = malloc(parent->children_space * sizeof(layer_t*));
    // Copy over
    memcpy(parent->children, temp, parent->children_count * sizeof(layer_t*));
  }
  // Add child
  parent->children[parent->children_count] = l;
  parent->children_count++;
  l->parent = parent;
  // Signal composition required
  parent->need_compose = true;
}

// Adds a child at a specific z index in the display list
// Also used to move a child from one z index to another
void add_child_at(layer_t* parent, layer_t* l, uint32_t z_index) {
  // Remove child from current parent
  if(l->parent) remove_child(l);
  // Limit z_index
  if(z_index > parent->children_count) z_index = parent->children_count;
  // Need more space?
  if(parent->children_space == parent->children_count) {
    // Grow space
    size_t temp_space = parent->children_space;
    parent->children_space = temp_space == 0 ? 4 : temp_space << 1;
    layer_t **temp = parent->children;
    // Allocate new space
    parent->children = malloc(parent->children_space * sizeof(layer_t*));
    // Copy over
    memcpy(parent->children, temp, z_index * sizeof(layer_t*));
    memcpy(parent->children + z_index + 1, temp + z_index, (parent->children_count - z_index) * sizeof(layer_t*));
  } else {
    // Skoot over
    memmove(parent->children + z_index + 1, parent->children + z_index, (parent->children_count - z_index) * sizeof(layer_t*));
  }
  // Add child
  parent->children[z_index] = l;
  parent->children_count++;
  l->parent = parent;
  // Signal composition required
  parent->need_compose = true;
}