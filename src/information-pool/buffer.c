#include <stdlib.h>
#include "buffer.h"

/**
 * Creates and initializes a new buffer.
 */
struct buffer* new_buffer()
{
	struct buffer *buf = malloc(sizeof *buf);
	buf->data = malloc(INITIAL_BUFFER_SIZE);
	buf->size = INITIAL_BUFFER_SIZE;
	buf->offset = 0;

	return buf;
}

/**
 * Deletes a buffer. DOES NOT DELETE BUFFERED DATA.
 */
void delete_buffer(struct buffer* buf)
{
	free(buf);
}

/**
 * Makes sure the buffer has enough free space to buffer data of size 'bytes'.
 */
void reserve_space(struct buffer* buf, size_t bytes)
{
    if((buf->offset + bytes) > buf->size)
    {
        buf->data = realloc(buf->data, buf->size * 2);
        buf->size *= 2;
    }
}

/**
 *	Trims unused buffer space.
 */
void trim_buffer_size(struct buffer* buf)
{
	buf->size = buf->offset;
	buf->data = realloc(buf->data, buf->offset);
}
