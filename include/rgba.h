#ifndef _RGBA_H_
#define _RGBA_H_
#include <stdint.h>

struct rgba_t {
  void *data;
  uint32_t w;
  uint32_t h;
};

typedef struct rgba_t rgba_t;

void  rgba_draw(void *dst, uint32_t dst_stride, void *src, uint32_t x, uint32_t y, uint32_t src_w, uint32_t src_h, uint32_t src_stride);
void  rgba_copy(void *dst, uint32_t dst_stride, void *src, uint32_t x, uint32_t y, uint32_t src_w, uint32_t src_h, uint32_t src_stride);
rgba_t rgba_alloc(uint32_t w, uint32_t h, const void* data);
void  rgba_free(rgba_t *rgba);
#endif//_RGBA_H_