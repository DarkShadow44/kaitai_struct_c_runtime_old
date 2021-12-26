#ifndef KAITAI_STRUCT_H
#define KAITAI_STRUCT_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define VOID

#define CHECK2(expr, message, DEFAULT) \
    if (expr) { \
        *stream->err = 1; \
        printf("%s:%d - %s\n", __FILE__, __LINE__, message); \
        return DEFAULT; \
    }

#define CHECK(expr, DEFAULT) \
    expr; \
    if (*stream->err) { \
        printf("%s:%d\n", __FILE__, __LINE__); \
        return DEFAULT; \
    }

#define CHECKV(expr) \
    CHECK(expr, VOID)

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 8) || __clang__
#define FIELD(expr, field)                                          \
    ({                                                              \
        __auto_type expr_ = (expr);                                 \
        __auto_type ret = expr_->_internal->_get_##field(expr_);    \
        CHECKV(;);                                                  \
        ret;                                                        \
    })
#else
#define FIELD(expr, field) \
    expr->_internal->_get_##field(expr)
#endif

typedef char ks_bool;

typedef enum ks_type_
{
    KS_TYPE_UNKNOWN = 0,
    KS_TYPE_ARRAY_UINT,
    KS_TYPE_ARRAY_INT,
    KS_TYPE_ARRAY_FLOAT,
    KS_TYPE_ARRAY_STRING,
    KS_TYPE_ARRAY_BYTES,
    KS_TYPE_ARRAY_USERENUM,
    KS_TYPE_ARRAY_USERTYPE,
    KS_TYPE_BYTES,
    KS_TYPE_USERTYPE,
    KS_TYPE_STRING,
} ks_type;

#ifdef KS_DEPEND_ON_INTERNALS
#define KS_DO_NOT_USE(X) X
#else
#define KS_DO_NOT_USE(X) DO_NOT_USE_##X
#endif

typedef struct ks_bytes_ ks_bytes;
typedef struct ks_string_ ks_string;

typedef struct ks_custom_decoder_
{
    void* userdata;
    ks_bytes* (*decode)(void* userdata, ks_bytes* bytes);
} ks_custom_decoder;

typedef struct ks_config_
{
    ks_bytes* (*inflate)(ks_bytes* bytes);
    ks_string* (*str_decode)(ks_string* src, const char* src_enc);
} ks_config;

typedef struct ks_stream_
{
    int* err;
    ks_config* config;
    ks_bool KS_DO_NOT_USE(is_file);
    FILE* KS_DO_NOT_USE(file);
    uint8_t* KS_DO_NOT_USE(data);
    uint64_t KS_DO_NOT_USE(start);
    uint64_t KS_DO_NOT_USE(length);
    uint64_t KS_DO_NOT_USE(pos);
    uint64_t KS_DO_NOT_USE(bits);
    int KS_DO_NOT_USE(bits_left);
    struct ks_stream_* KS_DO_NOT_USE(parent);
} ks_stream;

typedef struct ks_handle_
{
    ks_stream* KS_DO_NOT_USE(stream);
    /* Might need to add parent/rootdata pointer as well... */
    int KS_DO_NOT_USE(pos);
    void* KS_DO_NOT_USE(data);
    ks_type KS_DO_NOT_USE(type);
    int KS_DO_NOT_USE(type_size);
    void* KS_DO_NOT_USE(write_func); /* To write back */
    uint64_t KS_DO_NOT_USE(last_size); /* To make sure the size when writing back isn't too big */
    ks_bool KS_DO_NOT_USE(temporary); /* To mark something allocated as temporary, e.g. strings */
} ks_handle;

typedef struct ks_bytes_
{
    ks_handle _handle;
    ks_stream* KS_DO_NOT_USE(stream);
    uint64_t KS_DO_NOT_USE(pos);
    uint64_t KS_DO_NOT_USE(length);
    uint8_t* KS_DO_NOT_USE(data_direct);
} ks_bytes;

typedef struct ks_string_
{
    ks_handle _handle;
    int64_t len;
    char* data;
} ks_string;

typedef struct ks_usertype_generic_
{
    ks_handle _handle;
    void* _internal;
    void* _parent;
} ks_usertype_generic;

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

typedef struct ks_array_float_
{
    ks_handle _handle;
    int64_t size;
    float* data;
} ks_array_float;

typedef struct ks_array_double_
{
    ks_handle _handle;
    int64_t size;
    double* data;
} ks_array_double;

typedef struct ks_array_string_
{
    ks_handle _handle;
    int64_t size;
    ks_string** data;
} ks_array_string;

typedef struct ks_array_bytes_
{
    ks_handle _handle;
    int64_t size;
    ks_bytes** data;
} ks_array_bytes;

typedef struct ks_array_any_
{
    ks_handle _handle;
    int64_t size;
    void** data;
} ks_array_any;

typedef struct ks_array_usertype_generic_
{
    ks_handle _handle;
    int64_t size;
    ks_usertype_generic** data;
} ks_array_usertype_generic;

ks_stream* ks_stream_create_from_file(FILE* file, ks_config* config);
ks_stream* ks_stream_create_from_memory(uint8_t* data, int len, ks_config* config);
ks_stream* ks_stream_create_from_bytes(ks_bytes* bytes);
ks_stream* ks_stream_get_root(ks_stream* stream);

uint8_t ks_stream_read_u1(ks_stream* stream);
uint16_t ks_stream_read_u2le(ks_stream* stream);
uint32_t ks_stream_read_u4le(ks_stream* stream);
uint64_t ks_stream_read_u8le(ks_stream* stream);
uint16_t ks_stream_read_u2be(ks_stream* stream);
uint32_t ks_stream_read_u4be(ks_stream* stream);
uint64_t ks_stream_read_u8be(ks_stream* stream);

int8_t ks_stream_read_s1(ks_stream* stream);
int16_t ks_stream_read_s2le(ks_stream* stream);
int32_t ks_stream_read_s4le(ks_stream* stream);
int64_t ks_stream_read_s8le(ks_stream* stream);
int16_t ks_stream_read_s2be(ks_stream* stream);
int32_t ks_stream_read_s4be(ks_stream* stream);
int64_t ks_stream_read_s8be(ks_stream* stream);

float ks_stream_read_f4le(ks_stream* stream);
float ks_stream_read_f4be(ks_stream* stream);
double ks_stream_read_f8le(ks_stream* stream);
double ks_stream_read_f8be(ks_stream* stream);

uint64_t ks_stream_read_bits_le(ks_stream* stream, int width);
uint64_t ks_stream_read_bits_be(ks_stream* stream, int width);
void ks_stream_align_to_byte(ks_stream* stream);

ks_bytes* ks_stream_read_bytes(ks_stream* stream, int len);
ks_bytes* ks_stream_read_bytes_term(ks_stream* stream, uint8_t terminator, ks_bool include, ks_bool consume, ks_bool eos_error);
ks_bytes* ks_stream_read_bytes_full(ks_stream* stream);
void ks_bytes_destroy(ks_bytes* bytes);
void ks_stream_destroy(ks_stream* stream);
ks_bool ks_stream_is_eof(ks_stream* stream);
uint64_t ks_stream_get_pos(ks_stream* stream);
uint64_t ks_stream_get_length(ks_stream* stream);
void ks_stream_seek(ks_stream* stream, uint64_t pos);

ks_bytes* ks_bytes_from_data(ks_stream* stream, uint64_t count, ...);
ks_bytes* ks_bytes_create(ks_bytes* original, void* data, uint64_t length);
uint64_t ks_bytes_get_length(const ks_bytes* bytes);
int ks_bytes_get_data(const ks_bytes* bytes, void* data);

ks_handle ks_handle_create(ks_stream* stream, void* data, ks_type type, int type_size);

ks_string* ks_string_concat(ks_string* s1, ks_string* s2);
void ks_string_destroy(ks_string* s);
ks_string* ks_string_from_int(int64_t i, int base);
int64_t ks_string_to_int(ks_string* str, int base);
ks_string* ks_string_from_bytes(ks_bytes* bytes, ks_string* encoding);
ks_string* ks_string_from_cstr(const char* data);
ks_string* ks_string_reverse(ks_string* str);
ks_array_int64_t* ks_array_int64_t_from_data(uint64_t count, ...);
ks_array_double* ks_array_double_from_data(uint64_t count, ...);
ks_array_string* ks_array_string_from_data(uint64_t count, ...);
ks_array_usertype_generic* ks_array_usertype_generic_from_data(uint64_t count, ...);
int64_t ks_array_min_int(ks_handle* handle);
int64_t ks_array_max_int(ks_handle* handle);
double ks_array_min_float(ks_handle* handle);
double ks_array_max_float(ks_handle* handle);
ks_string* ks_array_min_string(ks_handle* handle);
ks_string* ks_array_max_string(ks_handle* handle);
ks_bytes* ks_array_min_bytes(ks_handle* handle);
ks_bytes* ks_array_max_bytes(ks_handle* handle);
ks_bytes* ks_bytes_strip_right(ks_bytes* bytes, int pad);
ks_bytes* ks_bytes_terminate(ks_bytes* bytes, int term, ks_bool include);
int ks_string_compare(ks_string* left, ks_string* right);
int ks_bytes_compare(ks_bytes* left, ks_bytes* right);
ks_string* ks_string_substr(ks_string* str, int start, int end);
int64_t ks_bytes_min(ks_bytes* bytes);
int64_t ks_bytes_max(ks_bytes* bytes);
int64_t ks_bytes_get_at(const ks_bytes* bytes, uint64_t index);
int64_t ks_mod(int64_t a, int64_t b);
int64_t ks_div(int64_t a, int64_t b);
ks_bytes* ks_bytes_process_xor_int(ks_bytes* bytes, uint64_t xor_int, int count_xor_bytes);
ks_bytes* ks_bytes_process_xor_bytes(ks_bytes* bytes, ks_bytes* xor_bytes);
ks_bytes* ks_bytes_process_rotate_left(ks_bytes* bytes, int count);
void ks_bytes_set_error(ks_bytes* bytes, int err);
void ks_string_set_error(ks_string* bytes, int err);
ks_usertype_generic* ks_usertype_get_root(ks_usertype_generic* data);
int ks_usertype_get_depth(ks_usertype_generic* data);


/* Dynamic functions */

#ifdef KS_USE_ZLIB
#include <zlib.h>
static ks_bytes* ks_inflate(ks_bytes* bytes)
{
    uint64_t length_in = ks_bytes_get_length(bytes);
    uint8_t* data_in = 0;
    uint8_t* data_out = 0;
    uint64_t length_out = 0;
    z_stream strm = {0};
    uint8_t outbuffer[1024*64];
    int ret;

    data_in = (uint8_t*)malloc(length_in);
    ret = ks_bytes_get_data(bytes, data_in);
    if (ret != 0)
    {
        ks_bytes_set_error(bytes, 1);
        return 0;
    }

    if (inflateInit(&strm) != Z_OK)
        goto error;

    strm.next_in = (Bytef*)data_in;
    strm.avail_in = length_in;

    do {
        strm.next_out = outbuffer;
        strm.avail_out = sizeof(outbuffer);

        ret = inflate(&strm, 0);

        if (length_out < strm.total_out) {
            data_out = (uint8_t*)realloc(data_out, strm.total_out);
            memcpy(data_out + length_out, outbuffer, strm.total_out - length_out);
            length_out = strm.total_out;
        }
    } while (ret == Z_OK);

    if (ret != Z_STREAM_END)
        goto error;

    if (inflateEnd(&strm) != Z_OK)
        goto error;

    return ks_bytes_create(bytes, data_out, length_out);

 error:
    free(data_in);
    free(data_out);
    ks_bytes_set_error(bytes, 1);
    return 0;
}
#else
static ks_bytes* ks_inflate(ks_bytes* bytes)
{
    ks_bytes_set_error(bytes, 1);
    return 0;
}
#endif

#ifdef KS_USE_ICONV
#include <iconv.h>
#include <errno.h>
static ks_string* ks_str_decode(ks_string* src, const char* src_enc) {
    iconv_t cd = iconv_open("UTF-8", src_enc);
    size_t src_left = src->len;
    size_t dst_len = src->len * 2;
    char* dst = (char*)calloc(1, dst_len);
    char* dst_ptr = dst;
    char* src_ptr = src->data;
    size_t dst_left = dst_len;
    size_t res = -1;
    ks_string* ret;

    if (cd == (iconv_t) -1) {
        if (errno == EINVAL) {
            ks_string_set_error(src, 1);
        } else {
            ks_string_set_error(src, 1);
        }
    }

    while (res == (size_t) -1) {
        res = iconv(cd, &src_ptr, &src_left, &dst_ptr, &dst_left);
        if (res == (size_t) -1) {
            if (errno == E2BIG) {
                size_t dst_used = dst_len - dst_left;
                dst_left += dst_len;
                dst_len += dst_len;
                dst = (char*)realloc(dst, dst_len);
                memset(dst_ptr, 0, dst_left);
                dst_ptr = &dst[dst_used];
            } else {
                ks_string_set_error(src, 1);
            }
        }
    }

    if (iconv_close(cd) != 0) {
        ks_string_set_error(src, 1);
    }

    ret = ks_string_from_cstr(dst);
    free(dst);
    return ret;
}
#else
static ks_string* ks_str_decode(ks_string* src, const char* src_enc)
{
    return src;
}
#endif

static void ks_config_init(ks_config* config)
{
    config->inflate = ks_inflate;
    config->str_decode = ks_str_decode;
}

#endif
