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
  uint32_t composite_w = l->bitmap.w;
  uint32_t composite_h = l->bitmap.h;

  // Render children where necessary
  n = l->children.count;
  if(n) {
    children = (layer_t**)l->children.data;
    while(n--) {
      child = *children++;
      if(compose(child)) {
        // Rendering resulted in an alteration of the childs composite bitmap.
        // This means we need to re-composite as well.
        l->need_compose = true;
      }
      // Track the required size of our composite bitmap.
      if(child->x + child->composite.w > composite_w) composite_w = child->x + child->composite.w;
      if(child->y + child->composite.h > composite_h) composite_h = child->y + child->composite.h;
    }
  }

  // Do we need to re-compose?
  if(l->need_compose) {
    if(l->composite.data && l->composite.data != l->bitmap.data) rgba_free(&l->composite);
    n = l->children.count;
    if(n) {
      // Have children - compose them
      l->composite = rgba_alloc(composite_w, composite_h, NULL);
      // Render our local bitmap first
      if(l->bitmap.data) rgba_draw(l->composite.data, composite_w, l->bitmap.data, 0, 0, l->bitmap.w, l->bitmap.h, l->bitmap.w);
      children = (layer_t**)l->children.data;
      while(n--) {
        child = *children++;
        rgba_draw(l->composite.data, composite_w, child->composite.data, child->x, child->y, child->composite.w, child->composite.h, child->composite.w);
        child++;
      }
    } else {
      // No children - composite and bitmap are identical
      l->composite = l->bitmap;
    }
    // Composition done
    l->need_compose = false;
    // Signal composition performed
    return true;
  }

  // Signal composition unaltered
  return false;
}
