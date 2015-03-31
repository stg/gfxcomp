#ifndef _COMPOSER_H_
#define _COMPOSER_H_

#include <stdint.h>
#include <stdbool.h>
#include "layer.h"

// Composes all underlying children into the specified layer
// Resulting RGBA data is available in g->composite upon completion
// Return value indicated wether g->composite was updated by the operation
bool compose(layer_t *g);

#endif//_COMPOSER_H_