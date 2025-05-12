#pragma once
#include <cstddef>
#include <vector>

template<typename T = float>
char* serialize_vec(const T* in_data, size_t in_size, char* out_data) noexcept;

template<typename T = float>
T* deserialize_vec(const char* in_data, size_t in_size, T* out_data) noexcept;

template<typename T = float>
size_t serialize_vec_bytes_len(size_t vec_size) noexcept;

template<typename T = float>
char* serialize_val(T in_val, char* out_data) noexcept;
