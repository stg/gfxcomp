#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "layer.h"
#include "rgba.h"

void layer_init(layer_t *l) {
  memset(l, 0, sizeof(layer_t));
  l->need_compose = true;
}

layer_t *layer_new() {
  layer_t *l = malloc(sizeof(layer_t));
  layer_init(l);
  return l;  
}

void layer_resize(layer_t *l, uint32_t w, uint32_t h) {
  if(l->bitmap) rgba_free(l->bitmap);
  l->bitmap = rgba_alloc(w, h);
  l->bitmap_w = w;
  l->bitmap_h = h;
  l->need_compose = true;
}

void layer_init_size(layer_t *l, uint32_t w, uint32_t h) {
  memset(l, 0, sizeof(layer_t));
  layer_resize(l, w, h);
}

void layer_fill(layer_t *l, uint32_t color) {
  uint32_t *dptr = l->bitmap;
  unsigned n;
  for(n = 0; n < l->bitmap_w * l->bitmap_h; n++) *dptr++ = color;
  l->need_compose = true;
}

void layer_move(layer_t* l, int32_t x, int32_t y, bool relative) {
  l->x = relative ? x + l->x : x;
  l->y = relative ? y + l->y : y;
  if(l->parent) l->parent->need_compose = true;
}

// Example: content change
void set_caption(layer_t* l) {
  // do whats needed
  l->need_render = true;
}