#include <iostream>
#include <string>
#include <vector>
#include "RequestSearchVectors.h"
#include "dist_fun.h"
#include "utils_rocks.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

void RequestSearchVectors::run(const ProtocolInPost &in, const ProtocolOut &out) noexcept
{
    if (!in.isPost() || in.isEmpty()) [[unlikely]] {
        out.setCode(422);
        return;
    }

    const json j = json::parse(in.key().ToStringView(), nullptr, false);
    if (j.is_discarded() || !j.is_object()) [[unlikely]] {
        set_error(out, "Invalid JSON.");
        return;
    }

    // Проверка db_name
    auto it_db_name = j.find("db_name");
    if (it_db_name == j.end() || !it_db_name->is_string()) [[unlikely]] {
        set_error(out, "Missing or invalid 'db_name' key.");
        return;
    }
    std::string_view db_name = it_db_name->get<std::string_view>();
    if (db_name.empty()) [[unlikely]] {
        set_error(out, "'db_name' must not be empty.");
        return;
    }
    auto meta = _db->get_meta(db_name);
    if( !meta.has_value() ) [[unlikely]] {
        set_error(out, "Data base doesn't exist.");
        return;
    }

    size_t top_k = _opts.top_k();
    auto it_top_k = j.find("top_k");
    if (it_top_k != j.end() && !it_top_k->is_number_integer()) [[unlikely]] {
        set_error(out, "Value of 'top_k' must be integer.");
        return;
    } else if (it_top_k != j.end()) [[unlikely]] {
        top_k = it_top_k->get<size_t>();
    }
    

    // Проверка data
    auto it_data = j.find("data");
    if (it_data == j.end() || !it_data->is_array() || it_data->empty()) [[unlikely]] {
        set_error(out, "Missing or invalid 'data' key. Must be a non-empty array.");
        return;
    }

    // Парсим первый элемент массива data
    auto &first_item = it_data->at(0);
    if (!first_item.is_object()) [[unlikely]] {
        set_error(out, "Each item in 'data' must be an object.");
        return;
    }

    // Проверка vector
    auto it_vector = first_item.find("vector");
    if (it_vector == first_item.end() || !it_vector->is_array() || it_vector->empty()) [[unlikely]] {
        set_error(out, "Missing or invalid 'vector' key. Must be a non-empty array.");
        return;
    }

    std::vector<float> vector_data;
    vector_data.reserve(it_vector->size());

    for (const auto &v : *it_vector) {
        if (!v.is_number()) [[unlikely]] {
            set_error(out, "All elements in 'vector' must be numeric.");
            return;
        }
        vector_data.push_back(v.get<float>());
    }

    // Проверка extra (опциональное поле)
    json extra_data = nullptr;
    auto it_extra = first_item.find("extra");
    if (it_extra != first_item.end()) {
        extra_data = *it_extra; // сохраняем extra как есть
    }

    // Отладочный вывод
    std::cout << "db_name: " << db_name << std::endl;
    std::cout << "top_k: " << top_k << std::endl;
    std::cout << "vector size: " << vector_data.size() << std::endl;
    if (!extra_data.is_null()) {
        std::cout << "extra data provided." << std::endl;
    } else {
        std::cout << "no extra data provided." << std::endl;
    }

    // Здесь далее будет логика поиска векторов и формирования ответа.
    // Пока просто возвращаем обратно полученные данные для теста:
    json response;
    response["db_name"] = db_name;
    response["top_k"] = top_k;
    response["vector"] = vector_data;
    if (!extra_data.is_null()) {
        response["extra"] = extra_data;
    }

    out.setStr(response.dump());
}
