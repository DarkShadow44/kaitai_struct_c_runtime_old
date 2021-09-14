#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK2(expr, err) \
   if (expr) { printf("%s:%d - %s\n", __FILE__, __LINE__, err); return 1; }

#define CHECK(expr) \
   CHECK2(expr, "")

#ifndef __cplusplus
typedef int bool;
#endif

typedef enum ks_type_
{
    KS_TYPE_UNKNOWN = 0,
    KS_TYPE_ARRAY_INT,
    KS_TYPE_ARRAY_FLOAT,
    KS_TYPE_ARRAY_STRING,
    KS_TYPE_ARRAY_USERENUM,
    KS_TYPE_ARRAY_USERTYPE,
    KS_TYPE_BYTES,
    KS_TYPE_USERTYPE,
} ks_type;

#ifdef KS_DEPEND_ON_INTERNALS
#define KS_DO_NOT_USE(X) X
#else
#define KS_DO_NOT_USE(X) DO_NOT_USE_##X
#endif

typedef struct ks_stream_
{
    bool KS_DO_NOT_USE(is_file);
    FILE* KS_DO_NOT_USE(file);
    uint8_t* KS_DO_NOT_USE(data);
    uint64_t KS_DO_NOT_USE(start);
    uint64_t KS_DO_NOT_USE(length);
    uint64_t KS_DO_NOT_USE(pos);
    uint64_t KS_DO_NOT_USE(bits);
    int KS_DO_NOT_USE(bits_left);
} ks_stream;

typedef struct ks_handle_
{
    ks_stream KS_DO_NOT_USE(stream);
    /* Might need to add parent/rootdata pointer as well... */
    int KS_DO_NOT_USE(pos);
    void* KS_DO_NOT_USE(data);
    ks_type KS_DO_NOT_USE(type);
    int KS_DO_NOT_USE(type_size);
    void* KS_DO_NOT_USE(write_func); /* To write back */
    uint64_t KS_DO_NOT_USE(last_size); /* To make sure the size when writing back isn't too big */
    bool KS_DO_NOT_USE(temporary); /* To mark something allocated as temporary, e.g. strings */
} ks_handle;

typedef struct ks_bytes_
{
    ks_stream KS_DO_NOT_USE(stream);
    uint64_t KS_DO_NOT_USE(pos);
    int KS_DO_NOT_USE(length);
} ks_bytes;

typedef struct ks_array_generic_
{
    ks_handle _handle;
    int64_t size;
    void* data;
} ks_array_generic;

typedef struct ks_array_uint8_t_
{
    ks_handle _handle;
    int64_t size;
    uint8_t* data;
} ks_array_uint8_t;

typedef struct ks_array_uint16_t_
{
    ks_handle _handle;
    int64_t size;
    uint16_t* data;
} ks_array_uint16_t;

typedef struct ks_array_uint32_t_
{
    ks_handle _handle;
    int64_t size;
    uint32_t* data;
} ks_array_uint32_t;

typedef struct ks_array_uint64_t_
{
    ks_handle _handle;
    int64_t size;
    uint64_t* data;
} ks_array_uint64_t;

typedef struct ks_array_int8_t_
{
    ks_handle _handle;
    int64_t size;
    int8_t* data;
} ks_array_int8_t;

typedef struct ks_array_int16_t_
{
    ks_handle _handle;
    int64_t size;
    int16_t* data;
} ks_array_int16_t;

typedef struct ks_array_int32_t_
{
    ks_handle _handle;
    int64_t size;
    int32_t* data;
} ks_array_int32_t;

typedef struct ks_array_int64_t_
{
    ks_handle _handle;
    int64_t size;
    int64_t* data;
} ks_array_int64_t;

typedef struct ks_array_float32_
{
    ks_handle _handle;
    int64_t size;
    float* data;
} ks_array_float32;

typedef struct ks_array_float64_
{
    ks_handle _handle;
    int64_t size;
    double* data;
} ks_array_float64;

typedef struct ks_string_
{
    ks_handle _handle;
    char* data;
    int64_t len;
} ks_string;

int ks_stream_init_from_file(ks_stream* stream, FILE* file);
int ks_stream_init_from_memory(ks_stream* stream, uint8_t* data, int len);
int ks_stream_init_from_bytes(ks_stream* stream, ks_bytes* bytes);

int ks_stream_read_u1(ks_stream* stream, uint8_t* value);
int ks_stream_read_u2le(ks_stream* stream, uint16_t* value);
int ks_stream_read_u4le(ks_stream* stream, uint32_t* value);
int ks_stream_read_u8le(ks_stream* stream, uint64_t* value);
int ks_stream_read_u2be(ks_stream* stream, uint16_t* value);
int ks_stream_read_u4be(ks_stream* stream, uint32_t* value);
int ks_stream_read_u8be(ks_stream* stream, uint64_t* value);

int ks_stream_read_s1(ks_stream* stream, int8_t* value);
int ks_stream_read_s2le(ks_stream* stream, int16_t* value);
int ks_stream_read_s4le(ks_stream* stream, int32_t* value);
int ks_stream_read_s8le(ks_stream* stream, int64_t* value);
int ks_stream_read_s2be(ks_stream* stream, int16_t* value);
int ks_stream_read_s4be(ks_stream* stream, int32_t* value);
int ks_stream_read_s8be(ks_stream* stream, int64_t* value);

int ks_stream_read_bits_le(ks_stream* stream, int width, uint64_t* value);
int ks_stream_read_bits_be(ks_stream* stream, int width, uint64_t* value);

int ks_stream_read_bytes(ks_stream* stream, int len, ks_bytes* bytes);
int ks_bytes_destroy(ks_bytes* bytes);
int ks_stream_destroy(ks_stream* stream);

int ks_handle_init(ks_handle* handle, ks_stream* stream, void* data, ks_type type, int type_size);

ks_string ks_string_concat(ks_string s1, ks_string s2);
int ks_string_destroy(ks_string s);
ks_string ks_string_from_int(int64_t i, int base);
