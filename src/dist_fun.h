#pragma once
#include <cstddef>
#include <string_view>

using dist_float_t = float;

struct DistFun final
{
    using dist_func_t = dist_float_t(*)(const dist_float_t*, const dist_float_t*, size_t) noexcept;
    static constexpr size_t default_index = 1;

    static size_t get_index(const char* name) noexcept;
    static size_t get_index(std::string_view name) noexcept;
    static const char* get_name(size_t idx) noexcept;
    static dist_func_t get_func(const char* name) noexcept;
    static dist_func_t get_func(size_t idx) noexcept;

private:
    static constexpr size_t _num_fun = 6;
    static const char* _names[_num_fun];
    static dist_func_t _funcs[_num_fun];
};
