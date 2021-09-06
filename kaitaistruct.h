#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK2(expr, err) \
   if (expr) { printf("%s:%d - %s\n", __FILE__, __LINE__, err); return 1; }

#define CHECK(expr) \
   CHECK2(expr, "")

typedef int bool;

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

typedef struct ks_stream_internal_
{
    bool is_file;
    FILE* file;
    uint8_t* data;
    uint64_t start;
    uint64_t length;
    uint64_t pos;
    uint64_t bits;
    int bits_left;
} ks_stream_internal;

typedef struct ks_handle_internal_
{
    ks_stream_internal stream;
    /* Might need to add parent/rootdata pointer as well... */
    int pos;
    void* data;
    ks_type type;
    int type_size;
    void* write_func; /* To write back */
    uint64_t last_size; /* To make sure the size when writing back isn't too big */
} ks_handle_internal;

typedef struct ks_bytes_internal_
{
    ks_stream_internal stream;
    uint64_t pos;
    int length;
} ks_bytes_internal;

typedef struct ks_handle_
{
    unsigned char reserved[sizeof(ks_handle_internal)];
} ks_handle;

typedef struct ks_stream_
{
    unsigned char reserved[sizeof(ks_stream_internal)];
} ks_stream;

typedef struct ks_bytes_
{
    unsigned char reserved[sizeof(ks_bytes_internal)];
} ks_bytes;

#ifndef KS_DEPEND_ON_INTERNALS
    #define ks_stream_internal_ ks_stream_internal_DO_NOT_USE_THIS
    #define ks_stream_internal  ks_stream_internal_DO_NOT_USE_THIS
    #define ks_handle_internal_ ks_handle_internal_DO_NOT_USE_THIS
    #define ks_handle_internal  ks_handle_internal_DO_NOT_USE_THIS
    #define ks_bytes_internal_  ks_bytes_internal_DO_NOT_USE_THIS
    #define ks_bytes_internal   ks_bytes_internal_DO_NOT_USE_THIS
#endif

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

int ks_init_handle(ks_handle* handle, ks_stream* stream, void* data, ks_type type, int type_size);


