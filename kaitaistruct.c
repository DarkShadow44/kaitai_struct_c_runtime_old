#include "kaitaistruct.h"

typedef struct ks_handle_
{
} ks_handle;

typedef struct ks_stream_
{
} ks_stream;

ks_stream* ks_stream_create_from_file(FILE* file)
{
	ks_stream* stream = calloc(1, sizeof(ks_stream));

	return stream;
}

int ks_stream_read_u1(ks_stream* stream, uint8_t* value, void* ignored1, void* ignored2)
{
}

int ks_stream_read_u2le(ks_stream* stream, uint16_t* value, void* ignored1, void* ignored2)
{
}

int ks_stream_read_u4le(ks_stream* stream, uint32_t* value, void* ignored1, void* ignored2)
{
}
