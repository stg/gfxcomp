#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "rgba.h"
#include "rect.h"
#include "utils.h"
#include "upng.h"

// Single pixel RGBA overlay core operation
// TODO: not mathematically accurate nor very efficient
static inline void rgba_pixel(pixel_t *dptr, pixel_t *sptr) {
  dptr->a = 255 - ((uint8_t)((((uint16_t)(255 - dptr->a)) * ((uint16_t)(255 - sptr->a))) >> 8));
  dptr->r = (((uint16_t)sptr->r * (uint16_t)sptr->a) + ((uint16_t)dptr->r * (uint16_t)(255 - sptr->a))) >> 8;
  dptr->g = (((uint16_t)sptr->g * (uint16_t)sptr->a) + ((uint16_t)dptr->g * (uint16_t)(255 - sptr->a))) >> 8;
  dptr->b = (((uint16_t)sptr->b * (uint16_t)sptr->a) + ((uint16_t)dptr->b * (uint16_t)(255 - sptr->a))) >> 8;
}

// Software fallback RGBA drawing
void rgba_alpha_low(void *dst, size_t dst_stride, void *src, size_t src_stride, uint32_t w, uint32_t h) {
  if(w) {
    while(h--) {
      pixel_t *pdst = &((pixel_t*)dst)[h * dst_stride + w];
      pixel_t *psrc = &((pixel_t*)src)[h * src_stride + w];
      for(uint32_t n = w; n--; ) rgba_pixel(--pdst, --psrc);
    }
  }
}

// Software fallback RGBA drawing
void rgba_copy_low(void *dst, size_t dst_stride, void *src, size_t src_stride, uint32_t w, uint32_t h) {
  if(w) {
    while(h--) {
      pixel_t *pdst = &((pixel_t*)dst)[h * dst_stride];
      pixel_t *psrc = &((pixel_t*)src)[h * src_stride];
      memcpy(pdst, psrc, w * sizeof(pixel_t));
    }
  }
}

// If rdst/rsrc is NULL, all of dst/src will be used.
// If .w and/or .h will be copied from rsrc if they are zero in rdst.
void rgba_blit(rgba_t* dst, rect_t *rdst, rgba_t* src, rect_t *rsrc, blit_e mode) {
  // Sanity: impossible xy
  if(rsrc->x >= src->w) return;
  if(rsrc->y >= src->h) return;
  if(rdst->x >= dst->w) return;
  if(rdst->y >= dst->h) return;
  // Condition src rectangle
  rect_t sr = rsrc ? (rect_t){
    rsrc->x, rsrc->y,
    min(rsrc->x + rsrc->w, src->w) - rsrc->x,
    min(rsrc->y + rsrc->h, src->h) - rsrc->y,
  } : (rect_t){0, 0, src->w, src->h};
  // Condition dst rectangle
  rect_t dr = rdst ? (rect_t){
    rdst->x, rdst->y,
    min(rdst->x + rdst->w, dst->w) - rdst->x,
    min(rdst->y + rdst->h, dst->h) - rdst->y,
  } : (rect_t){0, 0, dst->w, dst->h};
  // Condition size
  if(dr.w == 0) dr.w = sr.w;
  if(dr.h == 0) dr.h = sr.h;
  uint32_t w = min(sr.w, dr.w);
  uint32_t h = min(sr.h, dr.h);
  // Draw
  if(mode == BLIT_ALPHA)
    rgba_alpha_low(&dst->data[dr.y * dst->s + dr.x], dst->s, &src[sr.y * src->s + sr.x], src->s, w, h);
  else if(mode == BLIT_COPY)
    rgba_copy_low(&dst->data[dr.y * dst->s + dr.x], dst->s, &src[sr.y * src->s + sr.x], src->s, w, h);
}

// Allocate RGBA surface
rgba_t rgba_new(uint32_t w, uint32_t h, const void *data) {
  rgba_t rgba;
  rgba.data = malloc(w * h * 4);
  rgba.s = w;
  rgba.w = w;
  rgba.h = h;
  if(data) memcpy(rgba.data, data, w * h * 4);
  else     memset(rgba.data, 0,    w * h * 4);
  return rgba;
}

rgba_t rgba_from_file(const char *filename) {
  rgba_t rgba;
  upng_t* upng = upng_new_from_file(filename);
  if(upng_get_error(upng) == UPNG_EOK) {
    // Decode
    upng_decode(upng);
    if(upng_get_error(upng) == UPNG_EOK) {
      if(upng_get_format(upng) == UPNG_RGBA8) {
        printf("[upng] loaded %ux%u ok\n", upng_get_width(upng), upng_get_height(upng));
        // Convert to RGBA
        rgba = rgba_new(upng_get_width(upng), upng_get_height(upng), upng_get_buffer(upng));
      } else {
        printf("[upng] format error (%i)\n", upng_get_format(upng));
      }
    } else {
      printf("[upng] decode failed (%i)\n", upng_get_error(upng));
    }
    upng_free(upng);
  } else {
    printf("[upng] load failed (%i)\n", upng_get_error(upng));
  }
  return rgba;
}

// Free RGBA surface
void rgba_free(rgba_t *rgba) {
  if(rgba->data) {
    free(rgba->data);
    rgba->data = NULL;
  }
}
