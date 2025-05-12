#include "utils.h"
#include <cstring> // memcpy


template<typename T>
char* serialize_vec(const T* in_data, size_t in_size, char* out_data) noexcept
{
    std::memcpy(out_data, in_data, in_size * sizeof(T));
    return out_data + in_size * sizeof(T);
}

template<typename T>
T* deserialize_vec(const char* in_data, size_t in_size, T* out_data) noexcept
{
    std::memcpy(out_data, in_data, in_size * sizeof(T));
    return out_data + in_size;
}

template<typename T>
size_t serialize_vec_bytes_len(size_t vec_size) noexcept
{
    return vec_size * sizeof(T);
}


// Явное инстанцирование шаблонов для float и double
template char* serialize_vec<float>(const float*, size_t, char*) noexcept;
template float* deserialize_vec<float>(const char*, size_t, float*) noexcept;

template char* serialize_vec<double>(const double*, size_t, char*) noexcept;
template double* deserialize_vec<double>(const char*, size_t, double*) noexcept;

template size_t serialize_vec_bytes_len<float>(size_t in_size) noexcept;
template size_t serialize_vec_bytes_len<double>(size_t in_size) noexcept;
