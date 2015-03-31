#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "layer.h"
#include "composer.h"
#include "rgba.h"

bool compose(layer_t *g) {
  layer_t **children, *child;
  unsigned n;
  
  //printf("%08X has %u\n", g, g->children_count);
  
  // Do we need to render our local bitmap?
  if(g->need_render) {
    g->render(g);
    g->need_render = false;
    g->need_compose = true;
  }

  // Reset composite size
  g->composite_w = g->bitmap_w;
  g->composite_h = g->bitmap_h;

  // Render children where necessary
  n = g->children_count;
  if(n) {
    children = g->children;
    while(n--) {
      child = *children++;
      //printf("[c] 0x%08X<=0x%08X\n", g, child);
      printf("%u %u\n", child->bitmap_w, child->bitmap_h);
      if(compose(child)) {
        // Rendering resulted in an alteration of the childs composite bitmap.
        // This means we need to re-composite as well.
        g->need_compose = true;
      }
      // Track the required size of our composite bitmap.
      if(child->x + child->composite_w > g->composite_w) g->composite_w = child->x + child->composite_w;
      if(child->y + child->composite_h > g->composite_h) g->composite_h = child->y + child->composite_h;
    }
  }

  // Do we need to re-compose?
  if(g->need_compose) {
    if(g->composite && g->composite != g->bitmap) rgba_free(g->composite);
    if(g->children) {
      // Have children - compose them
      g->composite = rgba_alloc(g->composite_w, g->composite_h);
      // Render our local bitmap first
      if(g->bitmap) rgba_draw(g->composite, g->composite_w, g->bitmap, 0, 0, g->bitmap_w, g->bitmap_h);
      n = g->children_count;
      if(n) {
        children = g->children;
        while(n--) {
          child = *children++;
          //printf("%u %u\n", child->composite_w, child->composite_h);
          //printf("dst: %08X to %08X is %ux%u\n", g->composite, g->composite + g->composite_w * g->composite_h * 4, g->composite_w, g->composite_h);
          //printf("src: %08X to %08X is %ux%u\n", child->composite, child->composite + child->composite_w * child->composite_h * 4, child->composite_w, child->composite_h);
          printf("%08X => ", ((uint32_t*)g->composite)[16 * 641]);
          rgba_draw(g->composite, g->composite_w, child->composite, child->x, child->y, child->composite_w, child->composite_h);
          printf("%08X\n",   ((uint32_t*)g->composite)[16 * 641]);
          child++;
        }
      }
    } else {
      // No children - composite and bitmap are identical
      g->composite = g->bitmap;
      g->composite_w = g->bitmap_w;
      g->composite_h = g->bitmap_h;
    }
    // Composition done
    g->need_compose = false;
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
  layer_t* g = parent->children[z_index];
  g->parent = NULL;
  // Collapse at z_index
  memmove(parent->children + z_index, parent->children + z_index + 1, (parent->children_count - z_index) * sizeof(layer_t*));
  parent->children_count--;
  // Signal composition required
  parent->need_compose = true;
  // Return removed child
  return g;
}

// Removes the specified child from its associated parent
// Child will not be deleted and may be re-used
layer_t* remove_child(layer_t* g) {
  unsigned n;
  // Sanity: child has parent?
  if(!g->parent) return;
  // Find child
  for(n = 0; n < g->parent->children_count; n++) {
    if(g->parent->children[n] == g) break;
  }
  // Remove child from parent (will sanity check n)
  return remove_child_at(g->parent, n);
}

// Deletes a child and frees up its associated memory
// Child can not be re-used after this
void delete_child(layer_t* g) {
  // Remove child if not orphan
  if(g->parent) remove_child(g);
  // Call custom delete function
  if(g->delete) g->delete(g);
  // Free up memory
  free(g);
}

// Adds a child to the end of the display list
void add_child(layer_t* parent, layer_t* g) {
  // Remove child from current parent
  if(g->parent) remove_child(g);
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
  parent->children[parent->children_count] = g;
  parent->children_count++;
  g->parent = parent;
  // Signal composition required
  parent->need_compose = true;
}

// Adds a child at a specific z index in the display list
// Also used to move a child from one z index to another
void add_child_at(layer_t* parent, layer_t* g, uint32_t z_index) {
  // Remove child from current parent
  if(g->parent) remove_child(g);
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
  parent->children[z_index] = g;
  parent->children_count++;
  g->parent = parent;
  // Signal composition required
  parent->need_compose = true;
}