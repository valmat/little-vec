#pragma once
#include <cstring>
#include <cmath>

using dist_float_t = float;

struct DistFun final
{
    using dist_func_t = dist_float_t(*)(dist_float_t*, dist_float_t*, size_t) noexcept;

public:
    static size_t get_index(const char* name) noexcept
    {
        for (size_t i = 1; i < _num_fun; ++i) {
            if (std::strcmp(_names[i], name) == 0)
                return i;
        }
        return 0;
    }

    static const char* get_name(size_t idx) noexcept
    {
        return (idx < _num_fun) ? _names[idx] : nullptr;
    }

    static dist_func_t get_func(const char* name) noexcept
    {
        return _funcs[get_index(name)];
    }

    static dist_func_t get_func(size_t idx) noexcept
    {
        return (idx < _num_fun) ? _funcs[idx] : nullptr;
    }

private:
    static constexpr size_t _num_fun = 4;

    static const char* _names[_num_fun];
    static dist_func_t _funcs[_num_fun];

    static dist_float_t cos_dist(dist_float_t* a, dist_float_t* b, size_t len) noexcept
    {
        dist_float_t dot = 0.0f, norm_a = 0.0f, norm_b = 0.0f;
        for (size_t i = 0; i < len; ++i) {
            dot += a[i] * b[i];
            norm_a += a[i] * a[i];
            norm_b += b[i] * b[i];
        }
        if (norm_a == 0.0f || norm_b == 0.0f) return 1.0f;
        return 1.0f - (dot / (std::sqrt(norm_a) * std::sqrt(norm_b)));
    }

    static dist_float_t l1_dist(dist_float_t* a, dist_float_t* b, size_t len) noexcept
    {
        dist_float_t sum = 0.0f;
        for (size_t i = 0; i < len; ++i)
            sum += std::abs(a[i] - b[i]);
        return sum;
    }

    static dist_float_t l2_dist(dist_float_t* a, dist_float_t* b, size_t len) noexcept
    {
        dist_float_t sum = 0.0f;
        for (size_t i = 0; i < len; ++i) {
            dist_float_t d = a[i] - b[i];
            sum += d * d;
        }
        return std::sqrt(sum);
    }
};

// Определения статических членов класса
const char* DistFun::_names[DistFun::_num_fun] = { nullptr, "cos", "l1", "l2" };
DistFun::dist_func_t DistFun::_funcs[DistFun::_num_fun] = {
    nullptr, &DistFun::cos_dist, &DistFun::l1_dist, &DistFun::l2_dist
};
