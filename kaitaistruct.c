#define KS_DEPEND_ON_INTERNALS
#include "kaitaistruct.h"

int ks_stream_create_from_file(ks_stream* stream, FILE* file)
{
}

int ks_stream_create_from_bytes(ks_stream* stream, ks_bytes* bytes)
{
}

int ks_stream_read_u1(ks_stream* stream, uint8_t* value)
{
}
int ks_stream_read_u2le(ks_stream* stream, uint16_t* value)
{
}
int ks_stream_read_u4le(ks_stream* stream, uint32_t* value)
{
}
int ks_stream_read_u8le(ks_stream* stream, uint64_t* value)
{
}
int ks_stream_read_u2be(ks_stream* stream, uint16_t* value)
{
}
int ks_stream_read_u4be(ks_stream* stream, uint32_t* value)
{
}
int ks_stream_read_u8be(ks_stream* stream, uint64_t* value)
{
}

int ks_stream_read_s1(ks_stream* stream, uint8_t* value)
{
}
int ks_stream_read_s2le(ks_stream* stream, uint16_t* value)
{
}
int ks_stream_read_s4le(ks_stream* stream, uint32_t* value)
{
}
int ks_stream_read_s8le(ks_stream* stream, uint64_t* value)
{
}
int ks_stream_read_s2be(ks_stream* stream, uint16_t* value)
{
}
int ks_stream_read_s4be(ks_stream* stream, uint32_t* value)
{
}
int ks_stream_read_s8be(ks_stream* stream, uint64_t* value)
{
}

int ks_stream_read_bits_le(ks_stream* stream, int width, uint32_t* value)
{
}
int ks_stream_read_bits_be(ks_stream* stream, int width, uint32_t* value)
{
}

int ks_stream_read_bytes(ks_stream* stream, int len, ks_bytes* bytes)
{
}
