#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "rgba.h"

typedef struct __attribute__ ((__packed__)) {
  uint8_t r, g, b, a;
} pixel_t;

// Single pixel RGBA overlay core operation
// TODO: not mathematically accurate nor very effecient
static inline void rgba_pixel(pixel_t *dptr, pixel_t *sptr) {
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
  pixel_t  *dptr, *sptr;
  //printf("src se is %08X\n", sizeof(((pixel_t*)src)[0]) );
  // Keep a copy of source width
  uint32_t org_w = src_w;
  
  while(src_h--) {
    // Set up destination pointer for right=>left
    src_w = org_w;
    dptr = &((pixel_t*)dst)[(y + src_h) * dst_stride + (src_w + x)];
    sptr = &((pixel_t*)src)[src_h * src_stride + src_w];
    // Loop over each line and compose pixels as we go
    while(src_w--) rgba_pixel(--dptr, --sptr);
  }
}

// Software fallback RGBA drawing
void rgba_copy(void *dst, uint32_t dst_stride, void *src, uint32_t x, uint32_t y, uint32_t src_w, uint32_t src_h, uint32_t src_stride) {
  // Sanity: nothing to do?
  if(src_w == 0 || src_h == 0) return;
  // Set up source pointer for right=>left, bottom=>up
  pixel_t  *dptr, *sptr;
  //printf("src se is %08X\n", sizeof(((pixel_t*)src)[0]) );
  // Keep a copy of source width
  uint32_t org_w = src_w;
  
  while(src_h--) {
    // Set up destination pointer for right=>left
    src_w = org_w;
    dptr = &((pixel_t*)dst)[(y + src_h) * dst_stride + x];
    sptr = &((pixel_t*)src)[src_h * src_stride];
    // Copy line
    memcpy(dptr, sptr, src_w * 4);
  }
}

// Allocate RGBA surface
rgba_t rgba_alloc(uint32_t w, uint32_t h, const void *data) {
  rgba_t rgba;
  rgba.data = malloc(w * h * 4);
  rgba.w = w;
  rgba.h = h;
  if(data) memcpy(rgba.data, data, w * h * 4);
  else     memset(rgba.data, 0,    w * h * 4);
  return rgba;
}

// Free RGBA surface
void rgba_free(rgba_t *rgba) {
  if(rgba->data) {
    free(rgba->data);
    rgba->data = NULL;
  }
}