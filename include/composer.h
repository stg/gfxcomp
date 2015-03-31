#ifndef _COMPOSER_H_
#define _COMPOSER_H_

#include <stdint.h>
#include <stdbool.h>
#include "layer.h"

// Composes all underlying children into the specified layer
// Resulting RGBA data is available in g->composite upon completion
// Return value indicated wether g->composite was updated by the operation
bool compose(layer_t *g);

// Removes a child by z index from the specified parent
// Child will not be deleted and may be re-used
layer_t* remove_child_at(layer_t* parent, uint32_t z_index);

// Removes the specified child from its associated parent
// Child will not be deleted and may be re-used
layer_t* remove_child(layer_t* g);

// Deletes a child and frees up its associated memory
// Child can not be re-used after this
void delete_child(layer_t* g);

// Adds a child to the end of the display list
void add_child(layer_t* parent, layer_t* g);

// Adds a child at a specific z index in the display list
// Also used to move a child from one z index to another
void add_child_at(layer_t* parent, layer_t* g, uint32_t z_index);

#endif//_COMPOSER_H_