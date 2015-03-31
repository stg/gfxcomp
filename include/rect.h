#ifndef _RECT_H_
#define _RECT_H_
#include <stdint.h>
#include <stdbool.h>

struct rect_t {
  uint32_t x;
  uint32_t y;
  uint32_t w;
  uint32_t h;
};

typedef struct rect_t rect_t;

rect_t rect_new(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
bool rect_overlap(rect_t *a, rect_t *b);
rect_t rect_join(rect_t *a, rect_t *b);

#endif// _RECT_H_