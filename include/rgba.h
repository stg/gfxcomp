#ifndef _RGBA_H_
#define _RGBA_H_
#include <stdint.h>
void  rgba_draw(void *dst, uint32_t dst_stride, void *src, uint32_t x, uint32_t y, uint32_t src_w, uint32_t src_h, uint32_t src_stride);
void  rgba_copy(void *dst, uint32_t dst_stride, void *src, uint32_t x, uint32_t y, uint32_t src_w, uint32_t src_h, uint32_t src_stride);
void *rgba_alloc(uint32_t w, uint32_t h);
void  rgba_free(void *rgba);
void *rgba_from_buffer(const void *buf, uint32_t w, uint32_t h);
#endif//_RGBA_H_