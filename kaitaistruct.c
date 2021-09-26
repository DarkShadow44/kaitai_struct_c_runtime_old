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

int ks_stream_init_from_file(ks_stream* stream, FILE* file, ks_config* config)
{
    ks_stream ret = {0};

    ret.config = *config;
    ret.is_file = 1;
    ret.file = file;

    fseek(file, 0, SEEK_END);
    ret.length = ftell(file);

    memcpy(stream, &ret, sizeof(ks_stream));
    return 0;
}

int ks_stream_init_from_bytes(ks_stream* stream, ks_bytes* bytes)
{
    ks_stream ret = {0};

    ret.config = bytes->stream.config;
    ret.is_file = bytes->stream.is_file;
    ret.file = bytes->stream.file;
    ret.data = bytes->stream.data;
    ret.start = bytes->pos;
    ret.length = bytes->length;

    memcpy(stream, &ret, sizeof(ks_stream));
    return 0;
}

int ks_stream_init_from_memory(ks_stream* stream, uint8_t* data, int len, ks_config* config)
{
    ks_stream ret = {0};

    ret.config = *config;
    ret.is_file = 1;
    ret.data = data;
    ret.length = len;

    memcpy(stream, &ret, sizeof(ks_stream));
    return 0;
}

static int stream_read_bytes_nomove(const ks_stream* stream, int len, uint8_t* bytes)
{
    CHECK2(stream->pos + len > stream->length, "End of stream");
    if (stream->is_file)
    {
        int success = fseek(stream->file, stream->start + stream->pos, SEEK_SET);
        size_t read = fread(bytes, 1, len, stream->file);
        CHECK2(success != 0, "Failed to seek");
        CHECK2(len != read, "Failed to read");
    }
    else
    {
        memcpy(bytes, stream->data + stream->start + stream->pos, len);
    }
    return 0;
}

static int stream_read_bytes(ks_stream* stream, int len, uint8_t* bytes)
{
    CHECK(stream_read_bytes_nomove(stream, len, bytes));
    stream->pos += len;
    return 0;
}

static int stream_read_int(ks_stream* stream, int len, ks_bool big_endian, int64_t* value)
{
    uint8_t bytes[8];
    int64_t ret = 0;

    CHECK(stream_read_bytes(stream, len, bytes));

    if (big_endian)
    {
        reverse_uint8_t(bytes, len);
    }

    for (int i = 0; i < len; i++)
    {
        ret += (int64_t)bytes[i] << (i*8);
    }
    *value = ret;
    return 0;
}

ks_bool is_big_endian(void)
{
    int n = 1;
    return *(char *)&n == 0;
}

static int stream_read_float(ks_stream* stream, ks_bool big_endian, float* value)
{
    uint8_t bytes[sizeof(*value)];
    int64_t ret = 0;

    CHECK(stream_read_bytes(stream, sizeof(bytes), bytes));

    if (big_endian != is_big_endian())
    {
        reverse_uint8_t(bytes, sizeof(bytes));
    }

    memcpy(value, bytes, sizeof(bytes));
    return 0;
}

static int stream_read_double(ks_stream* stream, ks_bool big_endian, double* value)
{
    uint8_t bytes[sizeof(*value)];
    int64_t ret = 0;

    CHECK(stream_read_bytes(stream, sizeof(bytes), bytes));

    if (big_endian != is_big_endian())
    {
        reverse_uint8_t(bytes, sizeof(bytes));
    }

    memcpy(value, bytes, sizeof(bytes));
    return 0;
}


int ks_stream_read_u1(ks_stream* stream, uint8_t* value)
{
    int64_t temp;
    CHECK(stream_read_int(stream, 1, 0, &temp));
    *value = (uint8_t)temp;
    return 0;
}

int ks_stream_read_u2le(ks_stream* stream, uint16_t* value)
{
    int64_t temp;
    CHECK(stream_read_int(stream, 2, 0, &temp));
    *value = (uint16_t)temp;
    return 0;
}
int ks_stream_read_u4le(ks_stream* stream, uint32_t* value)
{
    int64_t temp;
    CHECK(stream_read_int(stream, 4, 0, &temp));
    *value = (uint32_t)temp;
    return 0;
}

int ks_stream_read_u8le(ks_stream* stream, uint64_t* value)
{
    int64_t temp;
    CHECK(stream_read_int(stream, 8, 0, &temp));
    *value = (uint64_t)temp;
    return 0;
}

int ks_stream_read_u2be(ks_stream* stream, uint16_t* value)
{
    int64_t temp;
    CHECK(stream_read_int(stream, 2, 1, &temp));
    *value = (uint16_t)temp;
    return 0;
}

int ks_stream_read_u4be(ks_stream* stream, uint32_t* value)
{
    int64_t temp;
    CHECK(stream_read_int(stream, 4, 1, &temp));
    *value = (uint32_t)temp;
    return 0;
}

int ks_stream_read_u8be(ks_stream* stream, uint64_t* value)
{
    int64_t temp;
    CHECK(stream_read_int(stream, 8, 1, &temp));
    *value = (uint64_t)temp;
    return 0;
}

int ks_stream_read_s1(ks_stream* stream, int8_t* value)
{
    int64_t temp;
    CHECK(stream_read_int(stream, 1, 0, &temp));
    *value = (int8_t)temp;
    return 0;
}

int ks_stream_read_s2le(ks_stream* stream, int16_t* value)
{
    int64_t temp;
    CHECK(stream_read_int(stream, 2, 0, &temp));
    *value = (int16_t)temp;
    return 0;
}

int ks_stream_read_s4le(ks_stream* stream, int32_t* value)
{
    int64_t temp;
    CHECK(stream_read_int(stream, 4, 0, &temp));
    *value = (int32_t)temp;
    return 0;
}

int ks_stream_read_s8le(ks_stream* stream, int64_t* value)
{
    int64_t temp;
    CHECK(stream_read_int(stream, 8, 0, &temp));
    *value = (int64_t)temp;
    return 0;
}

int ks_stream_read_s2be(ks_stream* stream, int16_t* value)
{
    int64_t temp;
    CHECK(stream_read_int(stream, 2, 1, &temp));
    *value = (int16_t)temp;
    return 0;
}

int ks_stream_read_s4be(ks_stream* stream, int32_t* value)
{
    int64_t temp;
    CHECK(stream_read_int(stream, 4, 1, &temp));
    *value = (int32_t)temp;
    return 0;
}

int ks_stream_read_s8be(ks_stream* stream, int64_t* value)
{
    int64_t temp;
    CHECK(stream_read_int(stream, 8, 1, &temp));
    *value = (int64_t)temp;
    return 0;
}

static uint64_t get_mask_ones(int n) {
    if (n == 64)
        return 0xFFFFFFFFFFFFFFFF;
    return ((uint64_t) 1 << n) - 1;
}

static int stream_read_bits(ks_stream* stream, int n, uint64_t* value, ks_bool big_endian)
{
    uint64_t mask = get_mask_ones(n); // raw mask with required number of 1s, starting from lowest bit
    int bits_needed = n - stream->bits_left;
    if (bits_needed > 0)
    {
        uint8_t buf[8];
        int bytes_needed = ((bits_needed - 1) / 8) + 1;
        CHECK2(bytes_needed > 8, "More than 8 bytes requested");
        CHECK(stream_read_bytes(stream, bytes_needed, buf));
        for (int i = 0; i < bytes_needed; i++)
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
        // shift mask to align with highest bits available in @bits
        int shift_bits = stream->bits_left - n;
        mask <<= shift_bits;

        // derive reading result
        *value = (stream->bits & mask) >> shift_bits;
        // clear top bits that we've just read => AND with 1s
        stream->bits_left -= n;
        mask = get_mask_ones(stream->bits_left);
        stream->bits &= mask;
    }
    else
    {
        // derive reading result
        *value = stream->bits & mask;
        // remove bottom bits that we've just read by shifting
        stream->bits >>= n;
        stream->bits_left -= n;
    }
    return 0;
}

int ks_stream_read_f4le(ks_stream* stream, float* value)
{
    return stream_read_float(stream, 0, value);
}

int ks_stream_read_f4be(ks_stream* stream, float* value)
{
    return stream_read_float(stream, 1, value);
}

int ks_stream_read_f8le(ks_stream* stream, double* value)
{
    return stream_read_double(stream, 0, value);
}

int ks_stream_read_f8be(ks_stream* stream, double* value)
{
    return stream_read_double(stream, 1, value);
}

int ks_stream_read_bits_le(ks_stream* stream, int width, uint64_t* value)
{
    return stream_read_bits(stream, width, value, 0);
}

int ks_stream_read_bits_be(ks_stream* stream, int width, uint64_t* value)
{
    return stream_read_bits(stream, width, value, 1);
}

int ks_stream_read_bytes(ks_stream* stream, int len, ks_bytes* bytes)
{
    ks_bytes ret = {0};

    CHECK2(stream->pos + len > stream->length, "End of stream");

    ret.length = len;
    ret.stream = *stream;
    ret.pos = stream->pos;

    stream->pos += len;

    memcpy(bytes, &ret, sizeof(ks_bytes));
    return 0;
}

ks_bytes ks_bytes_from_data(uint64_t count, ...)
{
    ks_bytes ret = {0};
    va_list list;

    ret.length = count;
    ret.data_direct = calloc(1, count);
    va_start(list, count);

    for (int i = 0; i < count; i++)
    {
        ret.data_direct[i] = va_arg(list, int);
    }
    va_end(list);

    return ret;
}

int ks_bytes_get_length(const ks_bytes* bytes, uint64_t* length)
{
    *length = bytes->length;
    return 0;
}

int ks_bytes_get_data(const ks_bytes* bytes, uint8_t* data)
{
    if (bytes->data_direct)
    {
        memcpy(data, bytes->data_direct, bytes->length);
        return 0;
    }
    return stream_read_bytes_nomove(&bytes->stream, bytes->length, data);
}

int ks_handle_init(ks_handle* handle, ks_stream* stream, void* data, ks_type type, int type_size)
{
    ks_handle ret = {0};

    ret.stream = *stream;
    ret.pos = stream->pos;
    ret.data = data;
    ret.type = type;
    ret.type_size = type_size;

    memcpy(handle, &ret, sizeof(ks_handle));
    return 0;
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
                ks_string* str_minmax = (ks_string*)minmax;
                ks_string* str_other = (ks_string*)other;
                int len = min(str_minmax->len, str_other->len);
                return strncmp(str_other->data, str_minmax->data, len) > 0;
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
                return array_get_float(handle, other) > array_get_float(handle, minmax);
            case KS_TYPE_ARRAY_STRING:
                 ks_string* str_minmax = (ks_string*)minmax;
                ks_string* str_other = (ks_string*)other;
                int len = min(str_minmax->len, str_other->len);
                return strncmp(str_other->data, str_minmax->data, len) < 0;
        }
    }
    return 0;
}

static void* array_min_max(ks_handle* handle, ks_bool max)
{
    char* pointer;
    ks_array_generic array;
    memcpy(&array, handle->data, sizeof(ks_array_generic)); /* Type punning */

    if (array.size == 0)
    {
        return 0;
    }

    pointer = array.data;
    for (int i = 1; i < array.size; i++)
    {
        char* data_new = handle->data + (i * handle->type_size);
        if (array_min_max_func(handle, max, pointer, data_new))
        {
            pointer = data_new;
        }
    }
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

ks_string ks_array_min_string(ks_handle* handle)
{
    void* ret = array_min_max(handle, 0);
    return *(ks_string*)ret;
}

ks_string ks_array_max_string(ks_handle* handle)
{
    void* ret = array_min_max(handle, 1);
    return *(ks_string*)ret;
}

ks_string ks_string_concat(ks_string s1, ks_string s2)
{
    ks_string ret = {0};
    ret._handle.temporary = 1;
    ret.len = s1.len + s2.len;
    ret.data = calloc(1, ret.len + 1);
    memcpy(ret.data, s1.data, s1.len);
    memcpy(ret.data + s1.len, s2.data, s2.len);

    if (s1._handle.temporary)
    {
        ks_string_destroy(s1);
    }
    if (s2._handle.temporary)
    {
        ks_string_destroy(s2);
    }

    return ret;
}

ks_string ks_string_from_int(int64_t i, int base)
{
    char buf[50] = {0};
    if (base == 10)
    {
        sprintf(buf, "%lld", i);
    }
    else if (base == 16)
    {
        sprintf(buf, "%llx", i);
    }
    ks_string ret = {0};
    ret._handle.temporary = 1;
    ret.len = strlen(buf);
    ret.data = calloc(1, ret.len + 1);
    memcpy(ret.data, buf, ret.len);

    return ret;
}

int ks_string_destroy(ks_string s)
{
    free(s.data);
}

