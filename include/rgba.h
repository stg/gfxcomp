#ifndef _RGBA_H_
#define _RGBA_H_
#include <stdint.h>
#include <rect.h>

struct __attribute__ ((__packed__)) pixel_t {
  uint8_t r, g, b, a;
};

typedef struct pixel_t pixel_t;

enum blit_e {
  BLIT_ALPHA  = 0,
  BLIT_COPY   = 1,
};

typedef enum blit_e blit_e;

struct rgba_t {
  pixel_t *data;
  uint32_t w; // width
  uint32_t h; // height
  uint32_t s; // stride
};

typedef struct rgba_t rgba_t;

rgba_t rgba_new(uint32_t w, uint32_t h, const void* data);
void   rgba_free(rgba_t *rgba);

// Low level pixel alpha-blending
void rgba_alpha_low(void *dst, size_t dst_stride, void *src, size_t src_stride, uint32_t w, uint32_t h);
// Low level pixel copy
void rgba_copy_low(void *dst, size_t dst_stride, void *src, size_t src_stride, uint32_t w, uint32_t h);
// High level blitter
void rgba_blit(rgba_t* dst, rect_t *rdst, rgba_t* src, rect_t *rsrc, blit_e mode);

#endif//_RGBA_H_