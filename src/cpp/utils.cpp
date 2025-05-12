#include "utils.h"
#include <cstring> // memcpy


template<typename T>
char* serialize_buf(const T* in_data, size_t in_size, char* out_data) noexcept
{
    std::memcpy(out_data, in_data, in_size * sizeof(T));
    return out_data + in_size * sizeof(T);
}

template<typename T>
T* deserialize_buf(const char* in_data, size_t in_size, T* out_data) noexcept
{
    std::memcpy(out_data, in_data, in_size * sizeof(T));
    return out_data + in_size;
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


// Явное инстанцирование шаблонов для float и double
template char* serialize_buf<float>(const float*, size_t, char*) noexcept;
template float* deserialize_buf<float>(const char*, size_t, float*) noexcept;

template char* serialize_buf<double>(const double*, size_t, char*) noexcept;
template double* deserialize_buf<double>(const char*, size_t, double*) noexcept;

template size_t serialize_buf_bytes_len<float>(size_t in_size) noexcept;
template size_t serialize_buf_bytes_len<double>(size_t in_size) noexcept;


template char* serialize_val<float>(float in_val, char* out_data) noexcept;
template char* serialize_val<double>(double in_val, char* out_data) noexcept;
