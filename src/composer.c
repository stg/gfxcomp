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
      l->composite = rgba_new(composite_w, composite_h, NULL);
      // Render our local bitmap first
      if(l->bitmap.data) rgba_alpha_low(l->composite.data, l->composite.s, l->bitmap.data, l->bitmap.s, l->bitmap.w, l->bitmap.h);
      children = (layer_t**)l->children.data;
      while(n--) {
        child = *children++;
        rgba_alpha_low(&l->composite.data[child->y * l->composite.s + child->x], l->composite.s, child->composite.data, child->composite.s, child->composite.w, child->composite.h);
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
