#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct ks_handle_;
typedef struct ks_handle_ ks_handle;

struct ks_stream_;
typedef struct ks_stream_ ks_stream;

struct ks_bytes_;
typedef struct ks_bytes_ ks_bytes;

ks_stream* ks_stream_create_from_file(FILE* file);

int ks_stream_read_u1(ks_stream* stream, uint8_t* value, void* ignored1, void* ignored2);
int ks_stream_read_u2le(ks_stream* stream, uint16_t* value, void* ignored1, void* ignored2);
int ks_stream_read_u4le(ks_stream* stream, uint32_t* value, void* ignored1, void* ignored2);
int ks_stream_read_u8le(ks_stream* stream, uint64_t* value, void* ignored1, void* ignored2);
int ks_stream_read_u2be(ks_stream* stream, uint16_t* value, void* ignored1, void* ignored2);
int ks_stream_read_u4be(ks_stream* stream, uint32_t* value, void* ignored1, void* ignored2);
int ks_stream_read_u8be(ks_stream* stream, uint64_t* value, void* ignored1, void* ignored2);

int ks_stream_read_s1(ks_stream* stream, uint8_t* value, void* ignored1, void* ignored2);
int ks_stream_read_s2le(ks_stream* stream, uint16_t* value, void* ignored1, void* ignored2);
int ks_stream_read_s4le(ks_stream* stream, uint32_t* value, void* ignored1, void* ignored2);
int ks_stream_read_s8le(ks_stream* stream, uint64_t* value, void* ignored1, void* ignored2);
int ks_stream_read_s2be(ks_stream* stream, uint16_t* value, void* ignored1, void* ignored2);
int ks_stream_read_s4be(ks_stream* stream, uint32_t* value, void* ignored1, void* ignored2);
int ks_stream_read_s8be(ks_stream* stream, uint64_t* value, void* ignored1, void* ignored2);

int ks_stream_read_bits_le(int width, ks_stream* stream, uint32_t* value, void* ignored1, void* ignored2);
int ks_stream_read_bits_be(int width, ks_stream* stream, uint32_t* value, void* ignored1, void* ignored2);

