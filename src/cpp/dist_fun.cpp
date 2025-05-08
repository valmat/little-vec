#include <cstring>
#include <cmath>
#include "dist_fun.h"

const char* DistFun::_names[DistFun::_num_fun] = { nullptr, "cos", "l1", "l2" };
DistFun::dist_func_t DistFun::_funcs[DistFun::_num_fun] = {
    nullptr, &DistFun::cos_dist, &DistFun::l1_dist, &DistFun::l2_dist
};

std::size_t DistFun::get_index(const char* name) noexcept
{
    for (std::size_t i = 1; i < _num_fun; ++i) {
        if (std::strcmp(_names[i], name) == 0)
            return i;
    }
    return 0;
}

const char* DistFun::get_name(std::size_t idx) noexcept
{
    return (idx < _num_fun) ? _names[idx] : nullptr;
}

DistFun::dist_func_t DistFun::get_func(const char* name) noexcept
{
    return _funcs[get_index(name)];
}

DistFun::dist_func_t DistFun::get_func(std::size_t idx) noexcept
{
    return (idx < _num_fun) ? _funcs[idx] : nullptr;
}

dist_float_t DistFun::cos_dist(dist_float_t* a, dist_float_t* b, std::size_t len) noexcept
{
    dist_float_t dot = 0.0f, norm_a = 0.0f, norm_b = 0.0f;
    for (std::size_t i = 0; i < len; ++i) {
        dot += a[i] * b[i];
        norm_a += a[i] * a[i];
        norm_b += b[i] * b[i];
    }
    if (norm_a == 0.0f || norm_b == 0.0f) return 1.0f;
    return 1.0f - (dot / (std::sqrt(norm_a) * std::sqrt(norm_b)));
}

dist_float_t DistFun::l1_dist(dist_float_t* a, dist_float_t* b, std::size_t len) noexcept
{
    dist_float_t sum = 0.0f;
    for (std::size_t i = 0; i < len; ++i)
        sum += std::abs(a[i] - b[i]);
    return sum;
}

dist_float_t DistFun::l2_dist(dist_float_t* a, dist_float_t* b, std::size_t len) noexcept
{
    dist_float_t sum = 0.0f;
    for (std::size_t i = 0; i < len; ++i) {
        dist_float_t d = a[i] - b[i];
        sum += d * d;
    }
    return std::sqrt(sum);
}