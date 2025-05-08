#pragma once
#include <cstddef>

using dist_float_t = float;

struct DistFun final
{
    using dist_func_t = dist_float_t(*)(dist_float_t*, dist_float_t*, std::size_t) noexcept;

    static std::size_t get_index(const char* name) noexcept;
    static const char* get_name(std::size_t idx) noexcept;
    static dist_func_t get_func(const char* name) noexcept;
    static dist_func_t get_func(std::size_t idx) noexcept;

private:
    static constexpr std::size_t _num_fun = 4;
    static const char* _names[_num_fun];
    static dist_func_t _funcs[_num_fun];

    static dist_float_t cos_dist(dist_float_t* a, dist_float_t* b, std::size_t len) noexcept;
    static dist_float_t l1_dist(dist_float_t* a, dist_float_t* b, std::size_t len) noexcept;
    static dist_float_t l2_dist(dist_float_t* a, dist_float_t* b, std::size_t len) noexcept;
};