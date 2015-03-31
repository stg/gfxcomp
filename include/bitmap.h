#ifndef _BITMAP_H_
#define _BITMAP_H_
#include <stdint.h>
#include "layer.h"

struct bitmap_t {
  layer_t layer;
  void *original;
  uint32_t original_w;
  uint32_t original_h;
  uint32_t requested_w;
  uint32_t requested_h;
};

typedef struct bitmap_t bitmap_t;

layer_t* bitmap_new(const char *filename);
void bitmap_init(layer_t *l, const char *filename);
void bitmap_resize(layer_t *l, uint32_t w, uint32_t h);

#endif//_BITMAP_H_