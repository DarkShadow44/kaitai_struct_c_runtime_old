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
    ks_stream ret = {0};

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

    ret.is_file = bytes->stream.is_file;
    ret.file = bytes->stream.file;
    ret.data = bytes->stream.data;
    ret.start = bytes->pos;
    ret.length = bytes->length;

    memcpy(stream, &ret, sizeof(ks_stream));
    return 0;
}

int ks_stream_init_from_memory(ks_stream* stream, uint8_t* data, int len)
{
    ks_stream ret = {0};

    ret.is_file = 1;
    ret.data = data;
    ret.length = len;

    memcpy(stream, &ret, sizeof(ks_stream));
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

int ks_array_max_int(ks_handle* handle)
{
    ks_array_generic array;
    memcpy(&array, handle->data, sizeof(ks_array_generic)); /* Type punning */
    /* TODO */
    return 0;
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

