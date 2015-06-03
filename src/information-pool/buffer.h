#include "contiki.h"
#include <stdlib.h>
#include <stdio.h>

#ifndef __BUFFER_H_
#define __BUFFER_H_

#define INITIAL_BUFFER_SIZE 32

struct buffer {
	void* data;
	uintptr_t offset;
	size_t size;
};

struct buffer* new_buffer();
void delete_buffer(struct buffer *buf);
void reserve_space(struct buffer *buf, size_t bytes);
void trim_buffer_size(struct buffer *buf);

#endif
