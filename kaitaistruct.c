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

int ks_stream_init_from_file(ks_stream* stream, FILE* file)
{
    ks_stream_internal* ret = (ks_stream_internal*)stream->reserved;

    memset(ret, 0, sizeof(ks_stream_internal));

    ret->is_file = 1;
    ret->file = file;

    fseek(file, 0, SEEK_END);
    ret->length = ftell(file);
    return 0;
}

int ks_stream_init_from_bytes(ks_stream* stream, ks_bytes* bytes)
{
    ks_stream_internal* ret = (ks_stream_internal*)stream->reserved;
    ks_bytes_internal* b = (ks_bytes_internal*)bytes->reserved;

    memset(ret, 0, sizeof(ks_stream_internal));

    ret->is_file = b->stream.is_file;
    ret->file = b->stream.file;
    ret->data = b->stream.data;
    ret->start = b->pos;
    ret->length = b->length;
    return 0;
}

int ks_stream_init_from_memory(ks_stream* stream, uint8_t* data, int len)
{
    ks_stream_internal* ret = (ks_stream_internal*)stream->reserved;

    memset(ret, 0, sizeof(ks_stream_internal));

    ret->is_file = 1;
    ret->data = data;
    ret->length = len;
    return 0;
}

static int stream_read_bytes(ks_stream* stream, int len, uint8_t* bytes)
{
    ks_stream_internal* s = (ks_stream_internal*)stream->reserved;
    CHECK2(s->pos + len > s->length, "End of stream");
    if (s->is_file)
    {
        int success = fseek(s->file, s->start + s->pos, SEEK_SET);
        size_t read = fread(bytes, 1, len, s->file);
        CHECK2(success != 0, "Failed to seek");
        CHECK2(len != read, "Failed to read");
    }
    else
    {
        memcpy(bytes, s->data + s->start + s->pos, len);
    }
    s->pos += len;
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
    ks_stream_internal* s = (ks_stream_internal*)stream->reserved;
    uint64_t mask = get_mask_ones(n); // raw mask with required number of 1s, starting from lowest bit
    int bits_needed = n - s->bits_left;
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
                s->bits <<= 8;
                s->bits |= b;
            }
            else
            {
                s->bits |= (((uint64_t)b) << s->bits_left);
            }
            s->bits_left += 8;
        }
    }

    if (big_endian)
    {
        // shift mask to align with highest bits available in @bits
        int shift_bits = s->bits_left - n;
        mask <<= shift_bits;

        // derive reading result
        *value = (s->bits & mask) >> shift_bits;
        // clear top bits that we've just read => AND with 1s
        s->bits_left -= n;
        mask = get_mask_ones(s->bits_left);
        s->bits &= mask;
    }
    else
    {
        // derive reading result
        *value = s->bits & mask;
        // remove bottom bits that we've just read by shifting
        s->bits >>= n;
        s->bits_left -= n;
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

int ks_stream_read_bytes(ks_stream* stream, int len, ks_bytes* bytes)
{
    ks_bytes_internal* ret = (ks_bytes_internal*)bytes->reserved;
    ks_stream_internal* s = (ks_stream_internal*)stream->reserved;

    CHECK2(s->pos + len > s->length, "End of stream");

    ret->length = len;
    ret->stream = *s;
    ret->pos = s->pos;

    s->pos += len;
    return 0;
}

int ks_init_handle(ks_handle* handle, ks_stream* stream, void* data, ks_type type, int type_size)
{
    ks_handle_internal* ret = (ks_handle_internal*)handle->reserved;
    ks_stream_internal* s = (ks_stream_internal*)stream->reserved;

    memset(ret, 0, sizeof(ks_handle_internal));

    ret->stream = *s;
    ret->pos = s->pos;
    ret->data = data;
    ret->type = type;
    ret->type_size = type_size;
    return 0;
}

int ks_array_max_int(ks_handle* handle)
{
    ks_handle_internal* h = (ks_handle_internal*)handle->reserved;
    ks_array_generic array;
    memcpy(&array, h->data, sizeof(ks_array_generic));
    /* TODO */
    return 0;
}

