#include <stdint.h>
#include <stdbool.h>
#include "rect.h"
#include "utils.h"

rect_t rect_new(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
  return (rect_t){x, y, w, h};
}

bool rect_overlap(rect_t *a, rect_t *b) {
  return a->x <= b->x + b->w
      && b->x <= a->x + a->w
      && a->y <= b->y + b->h
      && b->y <= a->y + a->h;
}

rect_t rect_join(rect_t *a, rect_t *b) {
  return (rect_t){
    min(a->x, b->x),
    min(a->y, b->y),
    max(a->x + a->w, b->x + b->w) - min(a->x, b->x),
    max(a->y + a->h, b->y + b->h) - min(a->y, b->y),
  };
}
