#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef KS_DEPEND_ON_INTERNALS
typedef struct ks_handle_
{
} ks_handle;

typedef struct ks_stream_
{
} ks_stream;

typedef struct ks_bytes_
{
    uint8_t *data;
    uint32_t len;
} ks_bytes;
#else

struct ks_handle_;
typedef struct ks_handle_ ks_handle;

struct ks_stream_;
typedef struct ks_stream_ ks_stream;

struct ks_bytes_;
typedef struct ks_bytes_ ks_bytes;
#endif

int ks_stream_create_from_file(ks_stream* stream, FILE* file);
int ks_stream_create_from_bytes(ks_stream* stream, ks_bytes* bytes);

int ks_stream_read_u1(ks_stream* stream, uint8_t* value);
int ks_stream_read_u2le(ks_stream* stream, uint16_t* value);
int ks_stream_read_u4le(ks_stream* stream, uint32_t* value);
int ks_stream_read_u8le(ks_stream* stream, uint64_t* value);
int ks_stream_read_u2be(ks_stream* stream, uint16_t* value);
int ks_stream_read_u4be(ks_stream* stream, uint32_t* value);
int ks_stream_read_u8be(ks_stream* stream, uint64_t* value);

int ks_stream_read_s1(ks_stream* stream, uint8_t* value);
int ks_stream_read_s2le(ks_stream* stream, uint16_t* value);
int ks_stream_read_s4le(ks_stream* stream, uint32_t* value);
int ks_stream_read_s8le(ks_stream* stream, uint64_t* value);
int ks_stream_read_s2be(ks_stream* stream, uint16_t* value);
int ks_stream_read_s4be(ks_stream* stream, uint32_t* value);
int ks_stream_read_s8be(ks_stream* stream, uint64_t* value);

int ks_stream_read_bits_le(ks_stream* stream, int width, uint32_t* value);
int ks_stream_read_bits_be(ks_stream* stream, int width, uint32_t* value);

int ks_stream_read_bytes(ks_stream* stream, int len, ks_bytes* bytes);
