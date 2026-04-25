/* luna_rb.h */
#ifndef __LUNA_RB_H
#define __LUNA_RB_H

#include <stdint.h>
#include <stdbool.h>

struct luna_rb {
        uint8_t *stream;
        uint32_t w;
        uint32_t r;
        uint32_t size;
};

void luna_rb_init(struct luna_rb *rb, uint8_t *stream, uint32_t size);

bool luna_rb_is_empty(struct luna_rb *rb);
bool luna_rb_is_full(struct luna_rb *rb);

uint32_t luna_rb_get_used(struct luna_rb *rb);
uint32_t luna_rb_get_free(struct luna_rb *rb);

void *luna_rb_get_wptr(struct luna_rb *rb);
void *luna_rb_get_rptr(struct luna_rb *rb);

void luna_rb_push_wptr(struct luna_rb *rb, uint32_t size);
void luna_rb_push_rptr(struct luna_rb *rb, uint32_t size);

void luna_rb_get_byte(struct luna_rb *rb, uint8_t *byte);
void luna_rb_put_byte(struct luna_rb *rb, uint8_t byte);

int luna_rb_read(struct luna_rb *rb, uint8_t *bytes, uint32_t len);
int luna_rb_write(struct luna_rb *rb, const uint8_t *bytes, uint32_t len);

#endif

#ifdef LUNA_RB_IMPLEMENTATION


void luna_rb_init(struct luna_rb *rb, uint8_t *stream, uint32_t size)
{
        ASSERT(rb);
        ASSERT(stream);
        REQUIRE(size > 2);
        rb->stream = stream;
        rb->w   = 0;
        rb->r   = 0;
        rb->size   = size;
}

bool luna_rb_is_empty(struct luna_rb *rb)
{
        ASSERT(rb);
        return (rb->w == rb->r);
}

bool luna_rb_is_full(struct luna_rb *rb)
{
        ASSERT(rb);
        return (((rb->w + 1) % rb->size) == rb->r);
}

uint32_t luna_rb_get_used(struct luna_rb *rb)
{
        ASSERT(rb);
        return ((rb->w - rb->r + rb->size) % rb->size);
}

uint32_t luna_rb_get_free(struct luna_rb *rb)
{
        ASSERT(rb);
        return ((rb->r - rb->w - 1 + rb->size) % rb->size);
}

void *luna_rb_get_wptr(struct luna_rb *rb)
{
        ASSERT(rb);
        return (&rb->stream[rb->w]);
}

void *luna_rb_get_rptr(struct luna_rb *rb)
{
        ASSERT(rb);
        return (&rb->stream[rb->r]);
}

void luna_rb_push_wptr(struct luna_rb *rb, uint32_t size)
{
        ASSERT(rb);
        rb->w = (rb->w + size) % rb->size;
}

void luna_rb_push_rptr(struct luna_rb *rb, uint32_t size)
{
        ASSERT(rb);
        rb->r = (rb->r + size) % rb->size;
}

void luna_rb_get_byte(struct luna_rb *rb, uint8_t *byte)
{
        ASSERT(rb);
        ASSERT(byte);
        *byte = rb->stream[rb->r];
        rb->r = (rb->r + 1) % rb->size;
}

void luna_rb_put_byte(struct luna_rb *rb, uint8_t byte)
{
        ASSERT(rb);
        rb->stream[rb->w] = byte;
        rb->w = (rb->w + 1) % rb->size;
}

int luna_rb_read(struct luna_rb *rb, uint8_t *bytes, uint32_t len)
{
        int i = 0;
        if (len == 0) {
                return 0;
        }
        while(!luna_rb_is_empty(rb) && len--) {
                luna_rb_get_byte(rb, &bytes[i]);
                i++;
        }
        return i;
}

int luna_rb_write(struct luna_rb *rb, const uint8_t *bytes, uint32_t len)
{
        int i = 0;
        if (len == 0) {
                return 0;
        }
        while(!luna_rb_is_full(rb) && len--) {
                luna_rb_put_byte(rb, bytes[i]);
                i++;
        }
        return i;
}

#endif