#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <charconv>
#include <type_traits>

// Helper function to calculate the size of the argument
std::size_t arg_size(const std::string& s) noexcept {
    return s.size();
}

std::size_t arg_size(std::string_view sv) noexcept {
    return sv.size();
}

std::size_t arg_size(const std::vector<char>& v) noexcept {
    return v.size();
}

std::size_t arg_size(std::nullptr_t) noexcept {
    return 0;
}

std::size_t arg_size(const char* s) noexcept {
    return std::string_view(s).size();
}

template <typename T>
requires std::is_integral_v<T>
constexpr std::size_t arg_size(T value) noexcept {
    std::size_t size = 0;
    do {
        ++size;
        value /= 10;
    } while (value != 0);
    return size;
}

// Helper function to write the argument into a buffer
inline char* arg_write(char* dest, const std::string& s) noexcept {
    return std::copy(s.begin(), s.end(), dest);
}

inline char* arg_write(char* dest, std::string_view sv) noexcept {
    return std::copy(sv.begin(), sv.end(), dest);
}

inline char* arg_write(char* dest, const std::vector<char>& v) noexcept {
    return std::copy(v.begin(), v.end(), dest);
}

inline char* arg_write(char* dest, const char* s) noexcept {
    return std::copy(s, s + std::strlen(s), dest);
}

inline char* arg_write(char* dest, std::nullptr_t) noexcept {
    return dest;
}


template <typename T>
inline char* arg_write(char* dest, T value) noexcept {
    auto [ptr, ec] = std::to_chars(dest, dest + 20, value);
    return ptr;
}

template <typename... Args>
std::string merge_args(const Args&... args) noexcept {
    constexpr char separator = ':';
    constexpr std::size_t num_args = sizeof...(Args);
    if constexpr (num_args == 0) {
        return {};
    }

    const std::size_t total_size = (arg_size(args) + ...) + (num_args - 1);

    std::string result;
    result.resize(total_size);

    char* dest = result.data();
    std::size_t idx = 0;

    auto write_arg = [&](const auto& arg) noexcept {
        if (idx++ > 0) {
            *dest++ = separator;
        }
        dest = arg_write(dest, arg);
    };

    (write_arg(args), ...);

    return result;
}
