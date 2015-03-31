#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "rgba.h"

typedef struct __attribute__ ((__packed__)) {
  uint8_t r, g, b, a;
} rgba_t;

// 0xAABBGGRR AA=0xFF=OPAQUE, 0x00=TRANSPARENT


// Single pixel RGBA overlay core operation
// Note: not mathematically accurate
static inline void rgba_pixel(rgba_t *dptr, rgba_t *sptr) {
  dptr->a = 255 - ((uint8_t)((((uint16_t)(255 - dptr->a)) * ((uint16_t)(255 - sptr->a))) >> 8));
  
  dptr->r = (((uint16_t)sptr->r * (uint16_t)sptr->a) + ((uint16_t)dptr->r * (uint16_t)(255 - sptr->a))) >> 8;
  dptr->g = (((uint16_t)sptr->g * (uint16_t)sptr->a) + ((uint16_t)dptr->g * (uint16_t)(255 - sptr->a))) >> 8;
  dptr->b = (((uint16_t)sptr->b * (uint16_t)sptr->a) + ((uint16_t)dptr->b * (uint16_t)(255 - sptr->a))) >> 8;
}

// Software fallback RGBA drawing
void rgba_draw(void *dst, uint32_t dst_stride, void *src, uint32_t x, uint32_t y, uint32_t src_w, uint32_t src_h, uint32_t src_stride) {
  // Sanity: nothing to do?
  if(src_w == 0 || src_h == 0) return;
  // Set up source pointer for right=>left, bottom=>up
  rgba_t  *dptr, *sptr, spx, dpx;
  //printf("src se is %08X\n", sizeof(((rgba_t*)src)[0]) );
  // Keep a copy of source width
  uint32_t org_w = src_w;
  
  while(src_h--) {
    // Set up destination pointer for right=>left
    src_w = org_w;
    dptr = &((rgba_t*)dst)[(y + src_h) * dst_stride + (src_w + x)];
    sptr = &((rgba_t*)src)[src_h * src_stride + src_w];
    // Loop over each line and compose pixels as we go
    while(src_w--) rgba_pixel(--dptr, --sptr);
  }
}

// Software fallback RGBA drawing
void rgba_copy(void *dst, uint32_t dst_stride, void *src, uint32_t x, uint32_t y, uint32_t src_w, uint32_t src_h, uint32_t src_stride) {
  // Sanity: nothing to do?
  if(src_w == 0 || src_h == 0) return;
  // Set up source pointer for right=>left, bottom=>up
  rgba_t  *dptr, *sptr, spx, dpx;
  //printf("src se is %08X\n", sizeof(((rgba_t*)src)[0]) );
  // Keep a copy of source width
  uint32_t org_w = src_w;
  
  while(src_h--) {
    // Set up destination pointer for right=>left
    src_w = org_w;
    dptr = &((rgba_t*)dst)[(y + src_h) * dst_stride + x];
    sptr = &((rgba_t*)src)[src_h * src_stride];
    // Copy line
    memcpy(dptr, sptr, src_w * 4);
  }
}



// Allocate RGBA surface
void *rgba_alloc(uint32_t w, uint32_t h) {
  void *rgba = malloc(w * h * 4);
  memset(rgba, 0, w * h * 4);
  return rgba;
}

// Allocate RGBA surface
void *rgba_from_buffer(const void *buf, uint32_t w, uint32_t h) {
  void *rgba = malloc(w * h * 4);
  uint32_t *dptr = rgba, *sptr = (uint32_t*)buf;
  unsigned n;
  for(n = 0; n < w * h; n++) {
    *dptr++ = *sptr++;
  }
  return rgba;
}

// Free RGBA surface
void rgba_free(void *rgba) {
  free(rgba);
}