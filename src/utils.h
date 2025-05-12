#pragma once
#include <cstddef>
#include <vector>

template<typename T = float>
char* serialize_buf(const T* in_data, size_t in_size, char* out_data) noexcept;

template<typename T = float>
T* deserialize_buf(const char* in_data, size_t out_size, T* out_data) noexcept;

template<typename T = float>
std::vector<T> deserialize_to_vec(const char* in_data, size_t out_size) noexcept;


template<typename T = float>
size_t serialize_buf_bytes_len(size_t vec_size) noexcept;

template<typename T = float>
char* serialize_val(T in_val, char* out_data) noexcept;
