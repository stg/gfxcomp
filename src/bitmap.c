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
}

void bitmap_init(layer_t *l, rgba_t rgba) {
  bitmap_t *b = (bitmap_t*)l;
  layer_init(l);
  memset(((char*)b) + sizeof(layer_t), 0, sizeof(bitmap_t) - sizeof(layer_t));
  l->free = bitmap_free;
  l->bitmap = b->original = rgba;
  l->w = rgba.w; l->h = rgba.h; // Report size
  l->need_compose = true;
  l->render = render;
}

layer_t *bitmap_new(rgba_t rgba) {
  bitmap_t *b = malloc(sizeof(bitmap_t));
  bitmap_init((layer_t*)b, rgba);
  return (layer_t*)b;
}

void bitmap_resize(layer_t *l, uint32_t w, uint32_t h) {
  bitmap_t *b = (bitmap_t*)l;
  b->render_w = w;
  b->render_h = h;
  l->need_render |= w != l->bitmap.w || h != l->bitmap.h;
  l->w = w; l->h = h; // Report size
}
