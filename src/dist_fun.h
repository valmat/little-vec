#pragma once
#include <iostream>
#include <cstring>
#include <math.h>

struct DistFun final
{
    using dist_func_t = float(*)(float*, float*, size_t) noexcept;

public:
    static int get_index(const char* name) noexcept
    {
        for (int i = 1; _names[i] != nullptr; ++i) {
            if (std::strcmp(_names[i], name) == 0)
                return i;
        }
        return 0;
    }

    static const char* get_name(size_t idx) noexcept
    {
        return (idx < size()) ? _names[idx] : nullptr;
    }

    // Получение указателя на функцию по имени
    static dist_func_t get_func(const char* name) noexcept
    {
        int idx = get_index(name);
        return (idx > 0) ? _funcs[idx] : nullptr;
    }

    // Получение указателя на функцию по индексу
    static dist_func_t get_func(size_t idx) noexcept
    {
        return (idx < size()) ? _funcs[idx] : nullptr;
    }

    // Количество функций
    static constexpr size_t size() noexcept
    {
        return 3;
    }

private:
    static constexpr size_t _num_fun = 3;

    // Статический массив имён функций
    static constexpr const char* _names[] = { nullptr, "cos", "l1", "l2", nullptr };

    // Реализации функций расстояния
    static float cos_dist(float* a, float* b, size_t len) noexcept
    {
        float dot = 0.0f, norm_a = 0.0f, norm_b = 0.0f;
        for (size_t i = 0; i < len; ++i) {
            dot += a[i] * b[i];
            norm_a += a[i] * a[i];
            norm_b += b[i] * b[i];
        }
        if (norm_a == 0.0f || norm_b == 0.0f) return 1.0f; // максимальное расстояние
        return 1.0f - (dot / (std::sqrt(norm_a) * std::sqrt(norm_b)));
    }

    static float l1_dist(float* a, float* b, size_t len) noexcept
    {
        float sum = 0.0f;
        for (size_t i = 0; i < len; ++i)
            sum += std::abs(a[i] - b[i]);
        return sum;
    }

    static float l2_dist(float* a, float* b, size_t len) noexcept
    {
        float sum = 0.0f;
        for (size_t i = 0; i < len; ++i) {
            float d = a[i] - b[i];
            sum += d * d;
        }
        return std::sqrt(sum);
    }

    // Статический массив указателей на функции
    static constexpr dist_func_t _funcs[] = {
        &cos_dist, &l1_dist, &l2_dist, nullptr
    };

};

// Определения статических членов класса
constexpr const char* DistFun::_names[];
constexpr DistFun::dist_func_t DistFun::_funcs[];
