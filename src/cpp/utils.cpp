#include "utils.h"
#include <cstring> // memcpy


template<typename T>
char* serialize_buf(const T* in_data, size_t in_size, char* out_data) noexcept
{
    std::memcpy(out_data, in_data, in_size * sizeof(T));
    return out_data + in_size * sizeof(T);
}

template<typename T>
T* deserialize_buf(const char* in_data, size_t out_size, T* out_data) noexcept
{
    std::memcpy(out_data, in_data, out_size * sizeof(T));
    return out_data + out_size;
}

template<typename T>
size_t serialize_buf_bytes_len(size_t vec_size) noexcept
{
    return vec_size * sizeof(T);
}

template<typename T>
char* serialize_val(T in_val, char* out_data) noexcept
{
    return serialize_buf(&in_val, 1, out_data);
}

template<typename T>
std::vector<T> deserialize_to_vec(const char* in_data, size_t out_size) noexcept
{
    std::vector<T> vector_data;
    vector_data.resize(out_size);

    deserialize_buf(in_data, out_size, vector_data.data());
    return vector_data;
}


template char* serialize_buf<float>(const float*, size_t, char*) noexcept;
template float* deserialize_buf<float>(const char*, size_t, float*) noexcept;
template size_t serialize_buf_bytes_len<float>(size_t) noexcept;
template char* serialize_val<float>(float, char*) noexcept;
template std::vector<float> deserialize_to_vec<float>(const char*, size_t) noexcept;
