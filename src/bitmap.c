#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "stdio.h"
#include "layer.h"
#include "bitmap.h"
#include "rgba.h"
#include "upng.h"

static void render(layer_t *l) {
  bitmap_t *b = (bitmap_t*)l;
  
  // Back up the original if not already done
  if(!b->original) {
    b->original = l->bitmap;
    b->original_w = l->bitmap_w;
    b->original_h = l->bitmap_h;
    l->bitmap = NULL;
  }

  // Easy access
  uint32_t src_w = b->original_w;
  uint32_t src_h = b->original_h;
  uint32_t dst_w = b->requested_w;
  uint32_t dst_h = b->requested_h;

  // Allocate new size bitmap
  if(l->bitmap) rgba_free(l->bitmap);
  l->bitmap = rgba_alloc(dst_w, dst_h);
  l->bitmap_w = dst_w;
  l->bitmap_h = dst_h;

  // Easy access
  uint32_t *src = b->original;
  uint32_t *dst = l->bitmap;
  
  // Stretch at midpoint
  unsigned n;
  rgba_copy(dst, dst_w, src, 0, 0, src_w >> 1, src_h, src_w);
  for(n = 0; n <= dst_w - src_w; n++)
    rgba_copy(dst, dst_w, src + (src_w >> 1), (src_w >> 1) + n, 0, 1, src_h, src_w);
  rgba_copy(dst, dst_w, src + ((src_w >> 1) + 1), dst_w - (src_w >> 1), 0, src_w >> 1, src_h, src_w);
  rgba_copy(dst, dst_w, dst + dst_w * (src_h >> 1), 0, dst_h - (src_h >> 1), dst_w, src_h >> 1, dst_w);
  for(n = 1; n <= dst_h - src_h; n++)
    rgba_copy(dst, dst_w, dst + dst_w * ((src_h >> 1)), 0, (src_h >> 1) + n, dst_w, 1, src_w);
}

void bitmap_init(layer_t *l, const char *filename) {
  bitmap_t *b = (bitmap_t*)l;
  layer_init(l);
  memset(((char*)b) + sizeof(layer_t), 0, sizeof(bitmap_t) - sizeof(layer_t));
  // Read file
  upng_t* upng = upng_new_from_file(filename);
  if(upng_get_error(upng) == UPNG_EOK) {
    // Decode
    upng_decode(upng);
    if(upng_get_error(upng) == UPNG_EOK) {
      if(upng_get_format(upng) == UPNG_RGBA8) {
        printf("[upng] loaded %ux%u ok\n", upng_get_width(upng), upng_get_height(upng));
        // Convert to RGBA
        l->bitmap_w = upng_get_width(upng);
        l->bitmap_h = upng_get_height(upng);
        l->bitmap = rgba_from_buffer(upng_get_buffer(upng), l->bitmap_w, l->bitmap_h);
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
  l->need_compose = true;
  l->render = render;
}

layer_t *bitmap_new(const char *filename) {
  bitmap_t *b = malloc(sizeof(bitmap_t));
  bitmap_init((layer_t*)b, filename);
  return (layer_t*)b;
}

void bitmap_resize(layer_t *l, uint32_t w, uint32_t h) {
  bitmap_t *b = (bitmap_t*)l;
  b->requested_w = w;
  b->requested_h = h;
  l->need_render |= w != l->bitmap_w || h != l->bitmap_h;
}
