/*
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "stdio.h"
#include "layer.h"
#include "bitmap.h"
#include "rgba.h"
#include "upng.h"
#include "ft2build.h"
#include FT_FREETYPE_H

static FT_Library    library;
static FT_Face       face;

static void render(layer_t *l) {
  text_t *t = (text_t*)l;
}

static void text_free(layer_t *l) {
  text_t *t = (text_t*)l;
}

void text_init(layer_t *l, font_t *font, float size) {
  text_t *t = (text_t*)l;
  layer_init(l);
  memset(((char*)t) + sizeof(layer_t), 0, sizeof(text_t) - sizeof(layer_t));
  l->free = text_free;
}

layer_t *text_new(font_t *font, float size) {
  text_t *t = malloc(sizeof(text_t));
  text_init((layer_t*)t, font, size);
  return (layer_t*)t;
}

void text_set_text(layer_t *l, uint32_t w, uint32_t h) {
  text_t *t = (text_t*)l;
  t->render_w = w;
  t->render_h = h;
  l->need_render |= w != l->bitmap.w || h != l->bitmap.h;
}
*/