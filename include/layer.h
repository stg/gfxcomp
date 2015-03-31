#ifndef _LAYER_H_
#define _LAYER_H_
#include <stdint.h>
#include <stdbool.h>
#include "rgba.h"
#include "list.h"

struct layer_t {
  bool             need_render;                 // rendering needs to be done
  bool             need_compose;                // compositing needs to be done
  uint32_t         x;                           // position-x
  uint32_t         y;                           // position-y
  rgba_t           bitmap;                      // local bitmap
  rgba_t           composite;                   // local+children composition bitmap
  list_t           children;                    // list of children
  struct layer_t  *parent;                      // parent
  void           (*render)(struct layer_t *me); // render function
  void           (*free)(struct layer_t *me);   // delete function
};

typedef struct layer_t layer_t;

layer_t *layer_new();
void layer_init(layer_t *l);
void layer_init_size(layer_t *l, uint32_t w, uint32_t h);
void layer_move(layer_t* l, int32_t x, int32_t y, bool relative);
void layer_fill(layer_t *l, uint32_t color);
void layer_free(layer_t* g);

// Removes a child by z index from the specified parent
// Child will not be deleted and may be re-used
layer_t* remove_child_at(layer_t* parent, uint32_t z_index);

// Removes the specified child from its associated parent
// Child will not be deleted and may be re-used
layer_t* remove_child(layer_t* g);

// Adds a child to the end of the display list
void add_child(layer_t* parent, layer_t* g);

// Adds a child at a specific z index in the display list
// Also used to move a child from one z index to another
void add_child_at(layer_t* parent, layer_t* g, uint32_t z_index);

#endif//_LAYER_H_