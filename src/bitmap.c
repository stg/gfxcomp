#include <stdint.h>
#include <string.h>
#include "stdio.h"
#include "layer.h"
#include "bitmap.h"
#include "rgba.h"
#include "upng.h"

void bitmap_init(bitmap_t *g, const char *filename) {
  layer_init(&g->layer);
  memset(((char*)g) + sizeof(layer_t), 0, sizeof(bitmap_t) - sizeof(layer_t));

  upng_t* upng = upng_new_from_file(filename);
  
  if(upng_get_error(upng) == UPNG_EOK) {
    upng_decode(upng);
    if(upng_get_error(upng) == UPNG_EOK) {
      //printf("upng load ok\n", upng_get_format(upng));
      if(upng_get_format(upng) == UPNG_RGBA8) {
        printf("[upng] loaded %ux%u ok\n", upng_get_width(upng), upng_get_height(upng));
        //printf("%i %i\n", upng_get_width(upng), upng_get_height(upng));
        g->layer.bitmap_w = upng_get_width(upng);
        g->layer.bitmap_h = upng_get_height(upng);
        g->layer.bitmap = rgba_from_buffer(upng_get_buffer(upng), g->layer.bitmap_w, g->layer.bitmap_h);
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
  g->layer.need_compose = true;
}

void bitmap_resize(bitmap_t *g, uint32_t w, uint32_t h) {
  if(!g->original) {
    g->original = g->layer.bitmap;
    g->original_w = g->layer.bitmap_w;
    g->original_h = g->layer.bitmap_h;
    g->layer.bitmap = NULL;
  }
  if(g->layer.bitmap) rgba_free(g->layer.bitmap);
  g->layer.bitmap = rgba_alloc(w, h);
}
