#include <stdlib.h>
#include <string.h>
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
 * For some reason the actual realloc is missing while compiling for the sky.
 */
void* shitty_realloc(void* oldptr, size_t oldsize, size_t newsize)
{
	void* newblock = malloc(newsize);

	printf("REALLOC: resizing from %d to %d, cpy %p --> %p ...\n", (int)oldsize, (int)newsize, oldptr, newblock);

	memcpy(newblock, oldptr, (newsize < oldsize) ? newsize : oldsize);
	free(oldptr);
	return newblock;
}

/**
 * Makes sure the buffer has enough free space to buffer data of size 'bytes'.
 */
void reserve_space(struct buffer* buf, size_t bytes)
{
    if((buf->offset + bytes) > buf->size)
    {
        buf->data = shitty_realloc(buf->data, buf->size, buf->size * 2);
        buf->size *= 2;
    }
}

/**
 *	Trims unused buffer space.
 */
void trim_buffer_size(struct buffer* buf)
{
	if(buf->offset < buf->size)
	{
		buf->data = shitty_realloc(buf->data, buf->size, buf->offset);
		buf->size = buf->offset;
	}
}
