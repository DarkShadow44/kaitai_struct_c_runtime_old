#define KS_DEPEND_ON_INTERNALS
#include "kaitaistruct.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

#define REVERSE_FUNC(type) \
    static void reverse_##type(type* data, int len) { \
        int start = 0, end = len - 1; \
        while (start < end) { \
            type temp = data[start]; \
            data[start] = data[end]; \
            data[end] = temp; \
            start++; \
            end--; \
        } \
    }

REVERSE_FUNC(uint8_t);

ks_stream* ks_stream_create_from_file(FILE* file, ks_config* config)
{
    ks_stream* ret;

    if (!file)
    {
        return 0;
    }

    ret = calloc(1, sizeof(ks_stream));
    ret->config = calloc(1, sizeof(ks_config));
    *ret->config = *config;
    ret->is_file = 1;
    ret->file = file;
    ret->err = calloc(1, sizeof(int));

    fseek(file, 0, SEEK_END);
    ret->length = ftell(file);
    return ret;
}

ks_stream* ks_stream_create_from_bytes(ks_bytes* bytes)
{
    ks_stream* ret = calloc(1, sizeof(ks_stream));

    ret->config = bytes->stream->config;
    ret->is_file = bytes->stream->is_file;
    if (bytes->data_direct)
    {
        ret->data  = bytes->data_direct;
        ret->is_file = 0;
    }
    else
    {
        ret->file = bytes->stream->file;
        ret->data = bytes->stream->data;
    }
    ret->start = bytes->pos;
    ret->length = bytes->length;
    ret->err = bytes->stream->err;
    ret->parent = bytes->stream;

    return ret;
}

ks_stream* ks_stream_create_from_memory(uint8_t* data, int len, ks_config* config)
{
    ks_stream* ret = calloc(1, sizeof(ks_stream));

    ret->config = calloc(1, sizeof(ks_config));
    *ret->config = *config;
    ret->is_file = 0;
    ret->data = data;
    ret->length = len;
    ret->err = calloc(1, sizeof(int));

    return ret;
}

ks_stream* ks_stream_get_root(ks_stream* stream)
{
    while (stream->parent)
    {
        stream = stream->parent;
    }
    return stream;
}

ks_bool ks_stream_is_eof(ks_stream* stream)
{
    return stream->pos == stream->length && stream->bits_left == 0;
}

uint64_t ks_stream_get_pos(ks_stream* stream)
{
    return stream->pos;
}

uint64_t ks_stream_get_length(ks_stream* stream)
{
    return stream->length;
}

void ks_stream_seek(ks_stream* stream, uint64_t pos)
{
    CHECK2(pos > stream->length, "End of stream", VOID);
    stream->pos = pos;
}

static void stream_read_bytes_nomove(const ks_stream* stream, uint64_t pos, uint64_t len, uint8_t* bytes)
{
    CHECK2(pos + len > stream->length, "End of stream", VOID);
    if (stream->is_file)
    {
        int success = fseek(stream->file, stream->start + pos, SEEK_SET);
        size_t read = fread(bytes, 1, len, stream->file);
        CHECK2(success != 0, "Failed to seek", VOID);
        CHECK2(len != read, "Failed to read", VOID);
    }
    else
    {
        memcpy(bytes, stream->data + stream->start + pos, len);
    }
}

static void stream_read_bytes(ks_stream* stream, uint64_t len, void* bytes)
{
    CHECK(stream_read_bytes_nomove(stream, stream->pos, len, bytes), VOID);
    stream->pos += len;
}

static int64_t stream_read_int(ks_stream* stream, int len, ks_bool big_endian)
{
    uint8_t bytes[8];
    int64_t ret = 0;
    int i;

    CHECK(stream_read_bytes(stream, len, bytes), 0);

    if (big_endian)
    {
        reverse_uint8_t(bytes, len);
    }

    for (i = 0; i < len; i++)
    {
        ret += (int64_t)bytes[i] << (i*8);
    }
    return ret;
}

static ks_bool is_big_endian(void)
{
    int n = 1;
    return *(char *)&n == 0;
}

static float stream_read_float(ks_stream* stream, ks_bool big_endian)
{
    float value;
    uint8_t bytes[sizeof(value)];

    CHECK(stream_read_bytes(stream, sizeof(bytes), bytes), 0);

    if (big_endian != is_big_endian())
    {
        reverse_uint8_t(bytes, sizeof(bytes));
    }

    memcpy(&value, bytes, sizeof(bytes));
    return value;
}

static double stream_read_double(ks_stream* stream, ks_bool big_endian)
{
    double value;
    uint8_t bytes[sizeof(value)];

    CHECK(stream_read_bytes(stream, sizeof(bytes), bytes), 0);

    if (big_endian != is_big_endian())
    {
        reverse_uint8_t(bytes, sizeof(bytes));
    }

    memcpy(&value, bytes, sizeof(bytes));
    return value;
}


uint8_t ks_stream_read_u1(ks_stream* stream)
{
    int64_t temp;
    CHECK(temp = stream_read_int(stream, 1, 0), 0);
    return temp;
}

uint16_t ks_stream_read_u2le(ks_stream* stream)
{
    int64_t temp;
    CHECK(temp = stream_read_int(stream, 2, 0), 0);
    return temp;
}
uint32_t ks_stream_read_u4le(ks_stream* stream)
{
    int64_t temp;
    CHECK(temp = stream_read_int(stream, 4, 0), 0);
    return temp;
}

uint64_t ks_stream_read_u8le(ks_stream* stream)
{
    int64_t temp;
    CHECK(temp = stream_read_int(stream, 8, 0), 0);
    return temp;
}

uint16_t ks_stream_read_u2be(ks_stream* stream)
{
    int64_t temp;
    CHECK(temp = stream_read_int(stream, 2, 1), 0);
    return temp;
}

uint32_t ks_stream_read_u4be(ks_stream* stream)
{
    int64_t temp;
    CHECK(temp = stream_read_int(stream, 4, 1), 0);
    return temp;
}

uint64_t ks_stream_read_u8be(ks_stream* stream)
{
    int64_t temp;
    CHECK(temp = stream_read_int(stream, 8, 1), 0);
    return temp;
}

int8_t ks_stream_read_s1(ks_stream* stream)
{
    int64_t temp;
    CHECK(temp = stream_read_int(stream, 1, 0), 0);
    return temp;
}

int16_t ks_stream_read_s2le(ks_stream* stream)
{
    int64_t temp;
    CHECK(temp = stream_read_int(stream, 2, 0), 0);
    return temp;
}

int32_t ks_stream_read_s4le(ks_stream* stream)
{
    int64_t temp;
    CHECK(temp = stream_read_int(stream, 4, 0), 0);
    return temp;
}

int64_t ks_stream_read_s8le(ks_stream* stream)
{
    int64_t temp;
    CHECK(temp = stream_read_int(stream, 8, 0), 0);
    return temp;
}

int16_t ks_stream_read_s2be(ks_stream* stream)
{
    int64_t temp;
    CHECK(temp = stream_read_int(stream, 2, 1), 0);
    return temp;
}

int32_t ks_stream_read_s4be(ks_stream* stream)
{
    int64_t temp;
    CHECK(temp = stream_read_int(stream, 4, 1), 0);
    return temp;
}

int64_t ks_stream_read_s8be(ks_stream* stream)
{
    int64_t temp;
    CHECK(temp = stream_read_int(stream, 8, 1), 0);
    return temp;
}

static uint64_t get_mask_ones(int n) {
    if (n == 64)
        return 0xFFFFFFFFFFFFFFFF;
    return ((uint64_t) 1 << n) - 1;
}

void ks_stream_align_to_byte(ks_stream* stream)
{
    stream->bits = 0;
    stream->bits_left = 0;
}

static uint64_t stream_read_bits(ks_stream* stream, int n, ks_bool big_endian)
{
    int i;
    uint64_t value;
    uint64_t mask = get_mask_ones(n); /* raw mask with required number of 1s, starting from lowest bit */
    int bits_needed = n - stream->bits_left;
    if (bits_needed > 0)
    {
        uint8_t buf[8];
        int bytes_needed = ((bits_needed - 1) / 8) + 1;
        CHECK2(bytes_needed > 8, "More than 8 bytes requested", 0);
        CHECK(stream_read_bytes(stream, bytes_needed, buf), 0);
        for (i = 0; i < bytes_needed; i++)
        {
            uint8_t b = buf[i];
            if (big_endian)
            {
                stream->bits <<= 8;
                stream->bits |= b;
            }
            else
            {
                stream->bits |= (((uint64_t)b) << stream->bits_left);
            }
            stream->bits_left += 8;
        }
    }

    if (big_endian)
    {
        /* shift mask to align with highest bits available in @bits */
        int shift_bits = stream->bits_left - n;
        mask <<= shift_bits;

        /* derive reading result */
        value = (stream->bits & mask) >> shift_bits;
        /* clear top bits that we've just read => AND with 1s */
        stream->bits_left -= n;
        mask = get_mask_ones(stream->bits_left);
        stream->bits &= mask;
    }
    else
    {
        /* derive reading result */
        value = stream->bits & mask;
        /* remove bottom bits that we've just read by shifting */
        stream->bits >>= n;
        stream->bits_left -= n;
    }
    return value;
}

float ks_stream_read_f4le(ks_stream* stream)
{
    float temp;
    CHECK(temp = stream_read_float(stream, 0), 0);
    return temp;
}

float ks_stream_read_f4be(ks_stream* stream)
{
    float temp;
    CHECK(temp = stream_read_float(stream, 1), 0);
    return temp;
}

double ks_stream_read_f8le(ks_stream* stream)
{
    double temp;
    CHECK(temp = stream_read_double(stream, 0), 0);
    return temp;
}

double ks_stream_read_f8be(ks_stream* stream)
{
    double temp;
    CHECK(temp = stream_read_double(stream, 1), 0);
    return temp;
}

uint64_t ks_stream_read_bits_le(ks_stream* stream, int width)
{
    uint64_t temp;
    CHECK(temp = stream_read_bits(stream, width, 0), 0);
    return temp;
}

uint64_t ks_stream_read_bits_be(ks_stream* stream, int width)
{
    uint64_t temp;
    CHECK(temp = stream_read_bits(stream, width, 1), 0);
    return temp;
}

ks_bytes* ks_stream_read_bytes(ks_stream* stream, int len)
{
    ks_bytes* ret = calloc(1, sizeof(ks_bytes));

    CHECK2(stream->pos + len > stream->length, "End of stream", ret);

    ret->length = len;
    ret->stream = stream;
    ret->pos = stream->pos;

    stream->pos += len;

    return ret;
}

ks_bytes* ks_stream_read_bytes_term(ks_stream* stream, uint8_t terminator, ks_bool include, ks_bool consume, ks_bool eos_error)
{
    ks_bytes* ret = calloc(1, sizeof(ks_bytes));
    uint8_t byte;
    uint64_t start = stream->pos;
    uint64_t len;
    ret->_handle = ks_handle_create(stream, ret, KS_TYPE_BYTES, sizeof(ks_bytes));
    do
    {
        CHECK(stream_read_bytes(stream, 1, &byte), ret);
    } while(byte != terminator);

    len = stream->pos - start;
    if (!consume)
    {
        stream->pos--;
    }
    if (!include)
    {
        len--;
    }

    ret->length = len;
    ret->stream = stream;
    ret->pos = start;

    return ret;
}

ks_bytes* ks_stream_read_bytes_full(ks_stream* stream)
{
    ks_bytes* ret = calloc(1, sizeof(ks_bytes));

    ret->_handle = ks_handle_create(stream, ret, KS_TYPE_BYTES, sizeof(ks_bytes));
    ret->length = stream->length - stream->pos;
    ret->stream = stream;
    ret->pos = stream->pos;

    stream->pos = stream->length;

    return ret;
}

ks_bytes* ks_bytes_from_data(ks_stream* stream, uint64_t count, ...)
{
    ks_bytes* ret = calloc(1, sizeof(ks_bytes));
    va_list list;
    int i;

    ret->_handle = ks_handle_create(0, ret, KS_TYPE_BYTES, sizeof(ks_bytes));
    ret->length = count;
    ret->data_direct = calloc(1, count);
    ret->stream = stream;
    va_start(list, count);

    for (i = 0; i < count; i++)
    {
        ret->data_direct[i] = va_arg(list, int);
    }
    va_end(list);

    return ret;
}

ks_bytes* ks_bytes_create(ks_bytes* original, void* data, uint64_t length)
{
    ks_bytes* ret = calloc(1, sizeof(ks_bytes));
    ret->_handle = ks_handle_create(0, ret, KS_TYPE_BYTES, sizeof(ks_bytes));
    ret->length = length;
    ret->data_direct = data;
    ret->stream = original->stream;
    return ret;
}

uint64_t ks_bytes_get_length(const ks_bytes* bytes)
{
    return bytes->length;
}

int ks_bytes_get_data(const ks_bytes* bytes, void* data)
{
    const ks_stream *stream = bytes->stream;
    if (bytes->data_direct)
    {
        memcpy(data, bytes->data_direct, bytes->length);
    }
    else
    {
        stream_read_bytes_nomove(stream, bytes->pos, bytes->length, data);
    }
    return bytes->stream ? *bytes->stream->err : 0;
}

int64_t ks_bytes_get_at(const ks_bytes* bytes, uint64_t index)
{
    const ks_stream *stream = bytes->stream;
    if (index >= bytes->length)
    {
        return 0;
    }

    if (bytes->data_direct)
    {
        return ((uint8_t*)bytes->data_direct)[index];
    }
    else
    {
        uint8_t data;
        CHECK(stream_read_bytes_nomove(stream, bytes->pos + index, 1, &data), 0);
        return data;
    }
}

ks_bytes* ks_bytes_strip_right(ks_bytes* bytes, int pad)
{
    ks_bytes* ret = calloc(1, sizeof(ks_bytes));
    uint64_t len = bytes->length;

    ret->_handle = ks_handle_create(0, ret, KS_TYPE_BYTES, sizeof(ks_bytes));
    ret->data_direct = malloc(len);
    ret->stream = bytes->stream;
    if (ks_bytes_get_data(bytes, ret->data_direct) != 0)
    {
        ret->length = 0;
        return ret;
    }

    while (len > 0 && ret->data_direct[len - 1] == pad)
        len--;

    ret->length = len;
    return ret;
}

ks_bytes* ks_bytes_terminate(ks_bytes* bytes, int term, ks_bool include)
{
    ks_bytes* ret = calloc(1, sizeof(ks_bytes));
    uint64_t len = 0;
    uint64_t max_len = bytes->length;

    ret->_handle = ks_handle_create(0, ret, KS_TYPE_BYTES, sizeof(ks_bytes));
    ret->data_direct = malloc(max_len);
    ret->stream = bytes->stream;
    if (ks_bytes_get_data(bytes, ret->data_direct) != 0)
    {
        ret->length = 0;
        return ret;
    }

    while (len < max_len && ret->data_direct[len] != term)
        len++;

    if (include && len < max_len)
        len++;

    ret->length = len;
    return ret;
}

ks_handle ks_handle_create(ks_stream* stream, void* data, ks_type type, int type_size)
{
    ks_handle ret = {0};

    ret.stream = stream;
    ret.pos = stream ? stream->pos : 0;
    ret.data = data;
    ret.type = type;
    ret.type_size = type_size;

    return ret;
}

static int64_t array_get_int(ks_handle* handle, void* data)
{
    switch(handle->type)
    {
        case KS_TYPE_ARRAY_UINT:
            switch(handle->type_size)
            {
                case 1:
                    return *(uint8_t*)data;
                case 2:
                    return *(uint16_t*)data;
                case 4:
                    return *(uint32_t*)data;
                case 8:
                    return *(uint64_t*)data;
            }
          case KS_TYPE_ARRAY_INT:
            switch(handle->type_size)
            {
                case 1:
                    return *(int8_t*)data;
                case 2:
                    return *(int16_t*)data;
                case 4:
                    return *(int32_t*)data;
                case 8:
                    return *(int64_t*)data;
            }
        default:
            break;
    }
    return 0;
}

static double array_get_float(ks_handle* handle, void* data)
{
   if (handle->type == KS_TYPE_ARRAY_FLOAT)
   {
       switch (handle->type_size)
       {
           case 4:
               return *(float*)data;
           case 8:
               return *(double*)data;
       }
   }
    return 0;
}

static ks_bool array_min_max_func(ks_handle* handle, ks_bool max, void* minmax, void* other)
{
    if (max)
    {
        switch (handle->type)
        {
            case KS_TYPE_ARRAY_UINT:
            case KS_TYPE_ARRAY_INT:
                return array_get_int(handle, other) > array_get_int(handle, minmax);
            case KS_TYPE_ARRAY_FLOAT:
                return array_get_float(handle, other) > array_get_float(handle, minmax);
            case KS_TYPE_ARRAY_STRING:
                return ks_string_compare(*(ks_string**)other, *(ks_string**)minmax) > 0;
            case KS_TYPE_ARRAY_BYTES:
                return ks_bytes_compare(*(ks_bytes**)other, *(ks_bytes**)minmax) > 0;
            default:
                break;
        }
    }
    else
    {
        switch (handle->type)
        {
            case KS_TYPE_ARRAY_UINT:
            case KS_TYPE_ARRAY_INT:
                return array_get_int(handle, other) < array_get_int(handle, minmax);
            case KS_TYPE_ARRAY_FLOAT:
                return array_get_float(handle, other) < array_get_float(handle, minmax);
            case KS_TYPE_ARRAY_STRING:
                return ks_string_compare(*(ks_string**)other, *(ks_string**)minmax) < 0;
            case KS_TYPE_ARRAY_BYTES:
                return ks_bytes_compare(*(ks_bytes**)other, *(ks_bytes**)minmax) < 0;
            default:
                break;
        }
    }
    return 0;
}

static void* array_min_max(ks_handle* handle, ks_bool max)
{
    char* pointer;
    ks_array_generic array;
    int i;

    memcpy(&array, handle->data, sizeof(ks_array_generic)); /* Type punning */

    if (array.size == 0)
    {
        return 0;
    }

    pointer = array.data;
    for (i = 1; i < array.size; i++)
    {
        char* data_new = array.data + (i * handle->type_size);
        if (array_min_max_func(handle, max, pointer, data_new))
        {
            pointer = data_new;
        }
    }
    return pointer;
}

int64_t ks_array_min_int(ks_handle* handle)
{
    void* ret = array_min_max(handle, 0);
    return array_get_int(handle, ret);
}

int64_t ks_array_max_int(ks_handle* handle)
{
    void* ret = array_min_max(handle, 1);
    return array_get_int(handle, ret);
}

double ks_array_min_float(ks_handle* handle)
{
    void* ret = array_min_max(handle, 0);
    return array_get_float(handle, ret);
}

double ks_array_max_float(ks_handle* handle)
{
    void* ret = array_min_max(handle, 1);
    return array_get_float(handle, ret);
}

ks_string* ks_array_min_string(ks_handle* handle)
{
    void* ret = array_min_max(handle, 0);
    return *(ks_string**)ret;
}

ks_string* ks_array_max_string(ks_handle* handle)
{
    void* ret = array_min_max(handle, 1);
    return *(ks_string**)ret;
}

ks_bytes* ks_array_min_bytes(ks_handle* handle)
{
    void* ret = array_min_max(handle, 0);
    return *(ks_bytes**)ret;
}

ks_bytes* ks_array_max_bytes(ks_handle* handle)
{
    void* ret = array_min_max(handle, 1);
    return *(ks_bytes**)ret;
}

static int64_t bytes_minmax(ks_bytes* bytes, ks_bool max)
{
    uint8_t minmax;
    uint8_t* data;
    int i;

    if (bytes->length == 0)
    {
        return 0;
    }

    data = malloc(bytes->length);
    if (ks_bytes_get_data(bytes, data) != 0)
    {
        free(data);
        return 0;
    }
    minmax = data[0];

    for (i = 1; i < bytes->length; i++)
    {
        if (max)
        {
            if (data[i] > minmax)
            {
                minmax = data[i];
            }
        }
        else
        {
            if (data[i] < minmax)
            {
                minmax = data[i];
            }
        }
    }

    free(data);
    return minmax;
}

int64_t ks_bytes_min(ks_bytes* bytes)
{
    return bytes_minmax(bytes, 0);
}

int64_t ks_bytes_max(ks_bytes* bytes)
{
    return bytes_minmax(bytes, 1);
}

ks_string* ks_string_concat(ks_string* s1, ks_string* s2)
{
    ks_string* ret = calloc(1, sizeof(ks_string));
    ret->_handle = ks_handle_create(0, ret, KS_TYPE_STRING, sizeof(ks_string));
    ret->_handle.temporary = 1;
    ret->len = s1->len + s2->len;
    ret->data = calloc(1, ret->len + 1);
    memcpy(ret->data, s1->data, s1->len);
    memcpy(ret->data + s1->len, s2->data, s2->len);

    return ret;
}

ks_string* ks_string_from_int(int64_t i, int base)
{
    ks_string* ret = calloc(1, sizeof(ks_string));
    ret->_handle = ks_handle_create(0, ret, KS_TYPE_STRING, sizeof(ks_string));
    char buf[50] = {0};
    if (base == 10)
    {
        sprintf(buf, "%lld", (long long int)i);
    }
    else if (base == 16)
    {
        sprintf(buf, "%llx", (long long int)i);
    }
    
    ret->_handle.temporary = 1;
    ret->len = strlen(buf);
    ret->data = calloc(1, ret->len + 1);
    memcpy(ret->data, buf, ret->len);

    return ret;
}

int64_t ks_string_to_int(ks_string* str, int base)
{
    long long int i = 0;
    if (base == 10)
    {
        sscanf(str->data, "%lld", &i);
    }
    else if (base == 16)
    {
        sscanf(str->data, "%llx", &i);
    }
    else
    {
        return strtol(str->data, 0, base);
    }

    return i;
}

ks_string* ks_string_reverse(ks_string* str)
{
    int i;
    ks_string* ret = calloc(1, sizeof(ks_string));
    ret->_handle = ks_handle_create(0, ret, KS_TYPE_STRING, sizeof(ks_string));
    ret->_handle.temporary = 1;
    ret->len = str->len;
    ret->data = calloc(1, ret->len + 1);

    for (i = 0; i < str->len; i++)
    {
        ret->data[i] = str->data[str->len - i - 1];
    }
    return ret;
}

ks_string* ks_string_from_bytes(ks_bytes* bytes, ks_string* encoding)
{
    ks_string* tmp = calloc(1, sizeof(ks_string));
    ks_string* ret;

    tmp->_handle = ks_handle_create(0, tmp, KS_TYPE_STRING, sizeof(ks_string));
    tmp->_handle.temporary = 1;
    tmp->len = bytes->length;
    tmp->data = calloc(1, tmp->len + 1);
    if(ks_bytes_get_data(bytes, tmp->data) != 0)
    {
        tmp->len = 0;
    }

    ret = bytes->stream->config->str_decode(tmp, encoding->data);

    if (ret != tmp) {
        free(tmp);
    }

    return ret;
}

ks_string* ks_string_from_cstr(const char* data)
{
    ks_string* ret = calloc(1, sizeof(ks_string));
    ret->_handle = ks_handle_create(0, ret, KS_TYPE_STRING, sizeof(ks_string));
    ret->_handle.temporary = 1;
    ret->len = strlen(data);
    ret->data = calloc(1, ret->len + 1);
    memcpy(ret->data, data, ret->len);

    return ret;
}

ks_string* ks_string_substr(ks_string* str, int start, int end)
{
    ks_string* ret = calloc(1, sizeof(ks_string));
    ret->_handle = ks_handle_create(0, ret, KS_TYPE_STRING, sizeof(ks_string));
    ret->_handle.temporary = 1;
    ret->len = end - start;
    ret->data = calloc(1, ret->len + 1);
    memcpy(ret->data, str->data + start, ret->len);
    return ret;
}

#define ARRAY_FROM_DATA(type_array, type_element, type_enum) \
    type_array* ret = calloc(1, sizeof(type_array)); \
    va_list list; \
    int i; \
    ret->_handle = ks_handle_create(0, ret, type_enum, sizeof(type_element)); \
    ret->_handle.temporary = 1; \
    ret->size = count; \
    ret->data = calloc(ret->_handle.type_size, count); \
    va_start(list, count); \
    for (i = 0; i < count; i++) {  \
        ret->data[i] = va_arg(list, type_element);  \
    }  \
    va_end(list);  \
    return ret;

ks_array_int64_t* ks_array_int64_t_from_data(uint64_t count, ...)
{
    ARRAY_FROM_DATA(ks_array_int64_t, int64_t, KS_TYPE_ARRAY_INT);
}

ks_array_double* ks_array_double_from_data(uint64_t count, ...)
{
    ARRAY_FROM_DATA(ks_array_double, double, KS_TYPE_ARRAY_FLOAT);
}

ks_array_string* ks_array_string_from_data(uint64_t count, ...)
{
    ARRAY_FROM_DATA(ks_array_string, ks_string*, KS_TYPE_ARRAY_STRING);
}

ks_array_usertype_generic* ks_array_usertype_generic_from_data(uint64_t count, ...)
{
    ARRAY_FROM_DATA(ks_array_usertype_generic, ks_usertype_generic*, KS_TYPE_ARRAY_USERTYPE);
}

int ks_string_compare(ks_string* left, ks_string* right)
{
    return strcmp(left->data, right->data);
}

int ks_bytes_compare(ks_bytes* left, ks_bytes* right)
{
    int ret;
    uint8_t* data_left;
    uint8_t* data_right;
    uint64_t len;

    data_left = malloc(left->length);
    data_right = malloc(right->length);

    if (ks_bytes_get_data(left, data_left) != 0 || ks_bytes_get_data(right, data_right) != 0)
    {
        free(data_left);
        free(data_right);
        return 0;
    }

    len = min(left->length, right->length);

    ret = memcmp(data_left, data_right, len);

    if (ret == 0)
    {
        if (left->length < right->length)
        {
            ret = -1;
        }
        if (left->length > right->length)
        {
            ret = 1;
        }
    }

    free(data_left);
    free(data_right);
    return ret;
}

void ks_string_destroy(ks_string* s)
{
    free(s->data);
    free(s);
}

void ks_stream_destroy(ks_stream* stream)
{
    free(stream);
}

int64_t ks_mod(int64_t a, int64_t b)
{
    int64_t r;
    if (b <= 0)
    {
        return 0;
    }
    r = a % b;
    if (r < 0) r += b;
    return r;
}

int64_t ks_div(int64_t a, int64_t b)
{
    int64_t ret = a / b;
    int64_t mod = a %b;
    if (a < 0 && b > 0 && mod != 0)
    {
        ret--;
    }
    return ret;
}

ks_bytes* ks_bytes_process_xor_int(ks_bytes* bytes, uint64_t xor_int, int count_xor_bytes)
{
    uint64_t i;
    ks_bytes* ret = calloc(1, sizeof(ks_bytes));

    ret->_handle = ks_handle_create(0, ret, KS_TYPE_BYTES, sizeof(ks_bytes));
    ret->_handle.temporary = 1;
    ret->length = bytes->length;
    ret->data_direct = calloc(1, bytes->length);
    ret->stream = bytes->stream;

    if (ks_bytes_get_data(bytes, ret->data_direct) != 0)
    {
        ret->length = 0;
        return ret;
    }

    for (i = 0; i < ret->length; i++)
    {
        ret->data_direct[i] ^= xor_int;
    }
    return ret;
}

ks_bytes* ks_bytes_process_xor_bytes(ks_bytes* bytes, ks_bytes* xor_bytes)
{
    uint64_t i;
    int xor_pos = 0;
    ks_bytes* ret = calloc(1, sizeof(ks_bytes));
    uint8_t* xor_data = malloc(xor_bytes->length);

    ret->_handle = ks_handle_create(0, ret, KS_TYPE_BYTES, sizeof(ks_bytes));
    ret->_handle.temporary = 1;
    ret->length = bytes->length;
    ret->data_direct = calloc(1, bytes->length);
    ret->stream = bytes->stream;

    if (ks_bytes_get_data(bytes, ret->data_direct) != 0 || ks_bytes_get_data(xor_bytes, xor_data) != 0)
    {
        free(xor_data);
        ret->length = 0;
        return ret;
    }

    for (i = 0; i < ret->length; i++)
    {
        ret->data_direct[i] ^= xor_data[xor_pos];
        xor_pos++;
        if (xor_pos >= xor_bytes->length)
        {
            xor_pos = 0;
        }
    }
    free(xor_data);
    return ret;
}

ks_bytes* ks_bytes_process_rotate_left(ks_bytes* bytes, int count)
{
    uint64_t i;
    ks_bytes* ret = calloc(1, sizeof(ks_bytes));

    ret->_handle = ks_handle_create(0, ret, KS_TYPE_BYTES, sizeof(ks_bytes));
    ret->_handle.temporary = 1;
    ret->length = bytes->length;
    ret->data_direct = calloc(1, bytes->length);
    ret->stream = bytes->stream;

    if (ks_bytes_get_data(bytes, ret->data_direct) != 0)
    {
        ret->length = 0;
        return ret;
    }

    for (i = 0; i < ret->length; i++)
    {
        uint64_t b = ret->data_direct[i];
        ret->data_direct[i] = (b << count) | (b >> (8 - count));
    }
    return ret;
}

void ks_bytes_set_error(ks_bytes* bytes, int err)
{
    int* error = bytes->stream->err;
    if (*error == 0)
    {
        *error = err;
    }
}

void ks_string_set_error(ks_string* str, int err)
{
    int* error = str->_handle.stream->err;
    if (*error == 0)
    {
        *error = err;
    }
}

ks_usertype_generic* ks_usertype_get_root(ks_usertype_generic* data)
{
    while (data->_parent)
    {
        data = data->_parent;
    }
    return data;
}

int ks_usertype_get_depth(ks_usertype_generic* data)
{
    int depth = 0;
    while (data->_parent)
    {
        depth++;
        data = data->_parent;
    }
    return depth;
}
