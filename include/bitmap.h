#ifndef _BITMAP_H_
#define _BITMAP_H_
#include <stdint.h>
#include "layer.h"

struct bitmap_t {
  layer_t layer;
  void *original;
  uint32_t original_w;
  uint32_t original_h;
};

typedef struct bitmap_t bitmap_t;

void bitmap_init(bitmap_t *g, const char *filename);
void bitmap_resize(bitmap_t *g, uint32_t w, uint32_t h);

#endif//_BITMAP_H_