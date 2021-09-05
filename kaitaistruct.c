#define KS_DEPEND_ON_INTERNALS
#include "kaitaistruct.h"

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

int ks_stream_create_from_file(ks_stream** stream, FILE* file)
{
    ks_stream* ret = calloc(1, sizeof(ks_stream));

    ret->is_file = 1;
    ret->file = file;

    fseek(file, 0, SEEK_END);
    ret->length = ftell(file);
    *stream = ret;
    return 0;
}

int ks_stream_create_from_bytes(ks_stream** stream, ks_bytes* bytes)
{
    ks_stream* ret = calloc(1, sizeof(ks_stream));

    ret->parent = bytes->stream;
    ret->is_file = ret->parent->is_file;
    ret->file = ret->parent->file;
    ret->data = ret->parent->data;
    ret->start = bytes->pos;
    ret->length = bytes->length;
    *stream = ret;
    return 0;
}

int ks_stream_create_from_memory(ks_stream** stream, uint8_t* data, int len)
{
    ks_stream* ret = calloc(1, sizeof(ks_stream));

    ret->is_file = 1;
    ret->data = data;
    ret->length = len;
    *stream = ret;
    return 0;
}

static int stream_read_bytes(ks_stream* stream, int len, uint8_t* bytes)
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
    stream->pos += len;
    return 0;
}

static int stream_read_int(ks_stream* stream, int len, bool big_endian, int64_t* value)
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

static int stream_read_bits(ks_stream* stream, int n, uint64_t* value, bool big_endian)
{
    uint64_t mask = get_mask_ones(n); // raw mask with required number of 1s, starting from lowest bit
    int bits_needed = n - stream->bits_left;
    if (bits_needed > 0)
    {
        char buf[8];
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

int ks_stream_read_bits_le(ks_stream* stream, int width, uint64_t* value)
{
    return stream_read_bits(stream, width, value, 0);
}

int ks_stream_read_bits_be(ks_stream* stream, int width, uint64_t* value)
{
    return stream_read_bits(stream, width, value, 1);
}

int ks_stream_read_bytes(ks_stream* stream, int len, ks_bytes** bytes)
{
    ks_bytes* ret;

    CHECK2(stream->pos + len > stream->length, "End of stream");

    ret = calloc(1, sizeof(ks_bytes));
    ret->length = len;
    ret->stream = stream;
    ret->pos = stream->pos;
    *bytes = ret;

    stream->pos += len;
    return 0;
}

int ks_bytes_destroy(ks_bytes* bytes)
{
    free(bytes);
    return 0;
}

int ks_stream_destroy(ks_stream* stream)
{
    free(stream);
    return 0;
}

int ks_allocate_handle(ks_handle** handle, ks_stream* stream, void* data, ks_type type, int type_size)
{
    ks_handle* ret = calloc(1, sizeof(ks_handle));

    ret->stream = stream;
    ret->pos = stream->pos;
    ret->data = data;
    ret->type = type;
    ret->type_size = type_size;

    *handle = ret;
    return 0;
}

int ks_allocate_handle_array(ks_handle** handle, ks_stream* stream, void* data, ks_type type, int type_size, void* array_data, int64_t array_size)
{
    ks_handle* ret = calloc(1, sizeof(ks_handle));

    ret->stream = stream;
    ret->pos = stream->pos;
    ret->data = data;
    ret->type = type;
    ret->type_size = type_size;
    ret->array_data = array_data;
    ret->array_size = array_size;

    *handle = ret;
    return 0;
}

int ks_destroy_handle(ks_handle* handle)
{
    free(handle);
    return 0;
}

