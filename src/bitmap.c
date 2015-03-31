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
  if(b->original.data == l->bitmap.data) l->bitmap.data = NULL;

  // Sanity: size check
  if(b->render_w < b->original.w) b->render_w = b->original.w;
  if(b->render_h < b->original.h) b->render_h = b->original.h;

  // Allocate new size bitmap
  if(l->bitmap.data) rgba_free(&l->bitmap);
  l->bitmap = rgba_new(b->render_w, b->render_h, NULL);

  // Easy access
  uint32_t src_w = b->original.w;
  uint32_t src_h = b->original.h;
  uint32_t src_s = b->original.s;
  uint32_t dst_w = l->bitmap.w;
  uint32_t dst_h = l->bitmap.h;
  uint32_t dst_s = l->bitmap.s;
  pixel_t *src = b->original.data;
  pixel_t *dst = l->bitmap.data;
  
  // Stretch at midpoint
  // TODO: Not very efficient and should support scaling DOWN by one pixel to remove the midpoints
  unsigned n;
  rgba_copy_low(dst, dst_s, src, src_s, src_w >> 1, src_h);
  for(n = 0; n <= dst_w - src_w; n++)
    rgba_copy_low(dst + (src_w >> 1) + n, dst_s, src + (src_w >> 1), src_s, 1, src_h);
  rgba_copy_low(dst + dst_w - (src_w >> 1), dst_s, src + ((src_w >> 1) + 1), src_s, src_w >> 1, src_h);
  rgba_copy_low(dst + (dst_h - (src_h >> 1)) * dst_s, dst_s, dst + dst_w * (src_h >> 1), dst_s, dst_w, src_h >> 1);
  for(n = 1; n <= dst_h - src_h; n++)
    rgba_copy_low(dst + ((src_h >> 1) + n) * dst_s, dst_s, dst + dst_w * ((src_h >> 1)), dst_s, dst_w, 1);
}

static void bitmap_free(layer_t *l) {
  bitmap_t *b = (bitmap_t*)l;
  if(b->original.data == l->bitmap.data) l->bitmap.data = NULL;
  rgba_free(&b->original);
}

void bitmap_init(layer_t *l, const char *filename) {
  bitmap_t *b = (bitmap_t*)l;
  layer_init(l);
  memset(((char*)b) + sizeof(layer_t), 0, sizeof(bitmap_t) - sizeof(layer_t));
  l->free = bitmap_free;
  // Read file
  upng_t* upng = upng_new_from_file(filename);
  if(upng_get_error(upng) == UPNG_EOK) {
    // Decode
    upng_decode(upng);
    if(upng_get_error(upng) == UPNG_EOK) {
      if(upng_get_format(upng) == UPNG_RGBA8) {
        printf("[upng] loaded %ux%u ok\n", upng_get_width(upng), upng_get_height(upng));
        // Convert to RGBA
        l->bitmap = b->original = rgba_new(upng_get_width(upng), upng_get_height(upng), upng_get_buffer(upng));
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
  b->render_w = w;
  b->render_h = h;
  l->need_render |= w != l->bitmap.w || h != l->bitmap.h;
}
