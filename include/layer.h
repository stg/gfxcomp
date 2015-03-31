#ifndef _LAYER_H_
#define _LAYER_H_
#include <stdint.h>
#include <stdbool.h>

struct layer_t {
  bool             need_render;                 // rendering needs to be done
  bool             need_compose;                // compositing needs to be done
  uint32_t         x;                           // position-x
  uint32_t         y;                           // position-y
  void            *bitmap;                      // bitmap
  uint32_t         bitmap_w;                    // -width
  uint32_t         bitmap_h;                    // -height
  void            *composite;                   // composite
  uint32_t         composite_w;                 // -width
  uint32_t         composite_h;                 // -height
  struct layer_t **children;                    // children
  unsigned         children_count;              // -count
  unsigned         children_space;              // -space(allocated)
  struct layer_t  *parent;                      // parent
  void           (*render)(struct layer_t *me); // render function
  void           (*delete)(struct layer_t *me); // delete function
};

typedef struct layer_t layer_t;

void layer_init(layer_t *g);
void layer_init_size(layer_t *g, uint32_t w, uint32_t h);
void layer_move(layer_t* g, int32_t x, int32_t y, bool relative);
void layer_fill(layer_t *g, uint32_t color);

#endif//_LAYER_H_