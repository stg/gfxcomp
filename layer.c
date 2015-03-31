#include <stdint.h>
#include <string.h>
#include "layer.h"
#include "rgba.h"

void layer_init(layer_t *g) {
  memset(g, 0, sizeof(layer_t));
  g->need_compose = true;
}

void layer_resize(layer_t *g, uint32_t w, uint32_t h) {
  if(g->bitmap) rgba_free(g->bitmap);
  g->bitmap = rgba_alloc(w, h);
  g->bitmap_w = w;
  g->bitmap_h = h;
  g->need_compose = true;
}

void layer_init_size(layer_t *g, uint32_t w, uint32_t h) {
  memset(g, 0, sizeof(layer_t));
  layer_resize(g, w, h);
}

void layer_fill(layer_t *g, uint32_t color) {
  uint32_t *dptr = g->bitmap;
  unsigned n;
  for(n = 0; n < g->bitmap_w * g->bitmap_h; n++) *dptr++ = color;
  g->need_compose = true;
}

void layer_move(layer_t* g, int32_t x, int32_t y, bool relative) {
  g->x = relative ? x + g->x : x;
  g->y = relative ? y + g->y : y;
  if(g->parent) g->parent->need_compose = true;
}

// Example: content change
void set_caption(layer_t* g) {
  // do whats needed
  g->need_render = true;
}