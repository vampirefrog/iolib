#pragma once

#include <stdint.h>
#include <stdio.h>

#include "buffer.h"

struct stream {
	size_t position;
	int _errno;

	size_t (*read)(struct stream *, void *ptr, size_t size);
	size_t (*write)(struct stream *, void *ptr, size_t size);
	size_t (*seek)(struct stream *, long offset, int whence);
	int (*eof)(struct stream *);
	long (*tell)(struct stream *);
	int (*close)(struct stream *);
	int (*printf)(struct stream *, const char *fmt, ...);
	void (*get_memory_access)(struct stream *, size_t *length);
	void (*revoke_memory_access)(struct stream *);
};

size_t stream_seek(struct stream *stream, long offset, int whence);
uint8_t stream_read_uint8(struct stream *stream);
uint16_t stream_read_big_uint16(struct stream *stream);
uint32_t stream_read_big_uint32(struct stream *stream);
ssize_t stream_write(struct stream *stream, void *ptr, size_t size);
ssize_t stream_write_buffer(struct stream *stream, struct buffer *buffer);
ssize_t stream_write_uint8(struct stream *stream, uint8_t i);
ssize_t stream_write_big_uint16(struct stream *stream, uint16_t i);
ssize_t stream_write_big_uint32(struct stream *stream, uint32_t i);
int stream_read_compare(struct stream *stream, const void *data, int len);
int stream_eof(struct stream *stream);
long stream_tell(struct stream *stream);
int stream_printf(struct stream *stream, const char *fmt, ...);
void *stream_get_memory_access(struct stream *stream, size_t *length); /* For plain files, try mmap(), for memory streams, return the pointer to the data, and for zipped and gzipped files, allocate a buffer and read+uncompress the whole file in the buffer */
int stream_revoke_memory_access(struct stream *stream);
int stream_close(struct stream *stream);

struct mem_stream {
	struct stream stream;
	struct buffer *buffer;
};
int mem_stream_init(struct mem_stream *stream, struct buffer *buffer);
int mem_stream_destroy(struct mem_stream *stream);

struct file_stream {
	struct stream stream;
	FILE *f;
};
int file_stream_init(struct file_stream *stream, char *filename, const char *mode);
int file_stream_destroy(struct file_stream *stream);

struct file_type_filter {
	const char *ext;
	int (*file_cb)(const char *full_path, struct stream *stream, void *data_ptr); /* The stream * pointer is non null if the EF_OPEN_STREAM flag is set */
	void *data_ptr;
};

#define EF_RECURSE_DIRS 0x01
#define EF_RECURSE_ARCHIVES 0x02 /* Currently zip only */
#define EF_OPEN_STREAM 0x04 /* Open the file and provide the instance */
int each_file(const char *path, struct file_type_filter *filters, int flags);
