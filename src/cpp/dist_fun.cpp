#include <cstring>
#include <cmath>
#include "dist_fun.h"

static dist_float_t cos_dist(const dist_float_t* a, const dist_float_t* b, size_t len) noexcept;
static dist_float_t l1_dist(const dist_float_t* a, const dist_float_t* b, size_t len) noexcept;
static dist_float_t l2_dist(const dist_float_t* a, const dist_float_t* b, size_t len) noexcept;

const char* DistFun::_names[DistFun::_num_fun] = {
    nullptr,
    "cos",
    "l1",
    "l2"
};

DistFun::dist_func_t DistFun::_funcs[DistFun::_num_fun] = {
    nullptr,
    &cos_dist,
    &l1_dist,
    &l2_dist
};

size_t DistFun::get_index(const char* name) noexcept
{
    for (size_t i = 1; i < _num_fun; ++i) {
        if (std::strcmp(_names[i], name) == 0) {
            return i;
        }
    }
    return 0;
}

const char* DistFun::get_name(size_t idx) noexcept
{
    return (idx < _num_fun) ? _names[idx] : nullptr;
}

DistFun::dist_func_t DistFun::get_func(const char* name) noexcept
{
    return _funcs[get_index(name)];
}

DistFun::dist_func_t DistFun::get_func(size_t idx) noexcept
{
    return (idx < _num_fun) ? _funcs[idx] : nullptr;
}

dist_float_t cos_dist(const dist_float_t* a, const dist_float_t* b, size_t len) noexcept
{
    dist_float_t dot = 0.0f, norm_a = 0.0f, norm_b = 0.0f;
    for (size_t i = 0; i < len; ++i) {
        dot    += a[i] * b[i];
        norm_a += a[i] * a[i];
        norm_b += b[i] * b[i];
    }

    constexpr dist_float_t eps = std::numeric_limits<dist_float_t>::epsilon() * 100;

    if (norm_a <= eps && norm_b <= eps) [[unlikely]] {
        return 0.0f;
    }
    if (norm_a <= eps || norm_b <= eps) [[unlikely]] {
        return 1.0f;
    }

    return 1.0f - (dot / (std::sqrt(norm_a) * std::sqrt(norm_b)));
}

dist_float_t l1_dist(const dist_float_t* a, const dist_float_t* b, size_t len) noexcept
{
    dist_float_t sum = 0.0f;
    for (size_t i = 0; i < len; ++i) {
        sum += std::abs(a[i] - b[i]);
    }
    return sum;
}

dist_float_t l2_dist(const dist_float_t* a, const dist_float_t* b, size_t len) noexcept
{
    dist_float_t sum = 0.0f;
    for (size_t i = 0; i < len; ++i) {
        dist_float_t d = a[i] - b[i];
        sum += d * d;
    }
    return std::sqrt(sum);
}
