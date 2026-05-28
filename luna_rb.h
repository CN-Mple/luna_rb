/* luna_rb.h */
#ifndef LUNA_RB_H
#define LUNA_RB_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#ifndef LUNA_ASSERT
#include <assert.h>
#define LUNA_ASSERT		assert
#endif

struct luna_rb {
        uint8_t *buffer;
        size_t w;
        size_t r;
        size_t size;
};

void luna_rb_init(struct luna_rb *rb, uint8_t *buffer, size_t size);

bool luna_rb_is_empty(struct luna_rb *rb);
bool luna_rb_is_full(struct luna_rb *rb);

size_t luna_rb_get_used(struct luna_rb *rb);
size_t luna_rb_get_free(struct luna_rb *rb);

size_t luna_rb_get_byte(struct luna_rb *rb, uint8_t *byte);
size_t luna_rb_put_byte(struct luna_rb *rb, uint8_t byte);
size_t luna_rb_peek(struct luna_rb *rb, uint8_t *byte);

size_t luna_rb_read(struct luna_rb *rb, uint8_t *bytes, size_t len);
size_t luna_rb_write(struct luna_rb *rb, const uint8_t *bytes, size_t len);

size_t luna_rb_drop(struct luna_rb *rb, size_t len);
size_t luna_rb_wipe(struct luna_rb *rb);
size_t luna_rb_size(struct luna_rb *rb);

#endif

#ifdef LUNA_RB_IMPLEMENTATION

void luna_rb_init(struct luna_rb *rb, uint8_t *buffer, size_t size)
{
        LUNA_ASSERT(rb);
        LUNA_ASSERT(buffer);
        LUNA_ASSERT(size >= 2);

        rb->buffer = buffer;
        rb->w   = 0;
        rb->r   = 0;
        rb->size   = size;
}

bool luna_rb_is_empty(struct luna_rb *rb)
{
        LUNA_ASSERT(rb);
        return (rb->w == rb->r);
}

bool luna_rb_is_full(struct luna_rb *rb)
{
        LUNA_ASSERT(rb);
        return (((rb->w + 1) % rb->size) == rb->r);
}

size_t luna_rb_get_used(struct luna_rb *rb)
{
        LUNA_ASSERT(rb);
        return ((rb->w - rb->r + rb->size) % rb->size);
}

size_t luna_rb_get_free(struct luna_rb *rb)
{
        LUNA_ASSERT(rb);
        return ((rb->r - rb->w - 1 + rb->size) % rb->size);
}

size_t luna_rb_get_byte(struct luna_rb *rb, uint8_t *byte)
{
        LUNA_ASSERT(rb);
        LUNA_ASSERT(byte);
	if (luna_rb_is_empty(rb)) {
		return 0;
	}
        *byte = rb->buffer[rb->r];
        rb->r = (rb->r + 1) % rb->size;
	return 1;
}

size_t luna_rb_put_byte(struct luna_rb *rb, uint8_t byte)
{
        LUNA_ASSERT(rb);
	if (luna_rb_is_full(rb)) {
		return 0;
	}
	rb->buffer[rb->w] = byte;
	rb->w = (rb->w + 1) % rb->size;
	return 1;
}

size_t luna_rb_peek(struct luna_rb *rb, uint8_t *byte)
{
        LUNA_ASSERT(rb);
	if (luna_rb_is_empty(rb)) {
		return 0;
	}
        *byte = rb->buffer[rb->r];
	return 1;
}

size_t luna_rb_read(struct luna_rb *rb, uint8_t *bytes, size_t len)
{
        LUNA_ASSERT(rb);
	LUNA_ASSERT(bytes);
        if (len == 0) {
                return 0;
        }
	size_t used = luna_rb_get_used(rb);
	size_t read_len = (len < used) ? len : used;
	size_t remain = rb->size - rb->r;

	if (read_len <= remain) {
		memcpy(bytes, rb->buffer + rb->r, read_len);
	} else {
		memcpy(bytes, rb->buffer + rb->r, remain);
		memcpy(bytes + remain, rb->buffer, read_len - remain);
	}
	rb->r = (rb->r + read_len) % rb->size;
	return read_len;
}

size_t luna_rb_write(struct luna_rb *rb, const uint8_t *bytes, size_t len)
{
        LUNA_ASSERT(rb);
	LUNA_ASSERT(bytes);
        if (len == 0) {
                return 0;
        }
	size_t free = luna_rb_get_free(rb);
	size_t write_len = (len < free) ? len : free;
	size_t remain = rb->size - rb->w;

	if (write_len <= remain) {
		memcpy(rb->buffer + rb->w, bytes, write_len);
	} else {
		memcpy(rb->buffer + rb->w, bytes, remain);
		memcpy(rb->buffer, bytes + remain, write_len - remain);
	}
	rb->w = (rb->w + write_len) % rb->size;
	return write_len;
}

size_t luna_rb_drop(struct luna_rb *rb, size_t len)
{
	LUNA_ASSERT(rb);
	size_t used = luna_rb_get_used(rb);
	if (len > used) {
		len = used;
	}
	rb->r = (rb->r + len) % rb->size;
	return len;
}

size_t luna_rb_wipe(struct luna_rb *rb)
{
	LUNA_ASSERT(rb);
	return luna_rb_drop(rb, luna_rb_get_used(rb));
}

size_t luna_rb_size(struct luna_rb *rb)
{
	LUNA_ASSERT(rb);
	return rb->size;
}


#endif
