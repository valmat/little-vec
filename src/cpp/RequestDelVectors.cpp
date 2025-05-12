#include <iostream>
#include <string>
#include "RequestDelVectors.h"
#include "dist_fun.h"
#include "utils_rocks.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

void RequestDelVectors::run(const ProtocolInPost &in, const ProtocolOut &out) noexcept
{
    if (!in.isPost() || in.isEmpty()) [[unlikely]] {
        out.setCode(422);
        return;
    }

    auto body = in.key().ToStringView();

    std::cout << body << std::endl;

    std::string db_name;
    std::vector<std::string> vector_ids;

    {
        json j = json::parse(body, nullptr, false);
        if (j.is_discarded() || !j.is_object()) [[unlikely]] {
            set_error(out, "Invalid JSON.");
            return;
        }

        // Проверяем наличие и корректность поля "db_name"
        auto it_db_name = j.find("db_name");
        if (it_db_name == j.end() || !it_db_name->is_string()) [[unlikely]] {
            set_error(out, "Missing or invalid 'db_name' key.");
            return;
        }
        db_name = it_db_name->get<std::string>();
        if (db_name.empty()) [[unlikely]] {
            set_error(out, "'db_name' must not be empty.");
            return;
        }

        // Проверяем наличие и корректность поля "data"
        auto it_data = j.find("data");
        if (it_data == j.end() || !it_data->is_array()) [[unlikely]] {
            set_error(out, "Missing or invalid 'data' key. Expected array.");
            return;
        }

        // Парсим массив идентификаторов
        for (const auto& item : *it_data) {
            if (!item.is_object()) [[unlikely]] {
                set_error(out, "Invalid item in 'data': expected object.");
                return;
            }
            auto it_id = item.find("id");
            if (it_id == item.end() || !it_id->is_string()) [[unlikely]] {
                set_error(out, "Missing or invalid 'id' key in 'data' item.");
                return;
            }
            std::string id = it_id->get<std::string>();
            if (id.empty()) [[unlikely]] {
                set_error(out, "'id' in 'data' item must not be empty.");
                return;
            }
            vector_ids.push_back(id);
        }

        if (vector_ids.empty()) [[unlikely]] {
            set_error(out, "'data' array must contain at least one item.");
            return;
        }
    }

    // Выводим для отладки
    std::cout << "db_name: " << db_name << std::endl;
    std::cout << "vector_ids to delete:" << std::endl;
    for (const auto& id : vector_ids) {
        std::cout << " - " << id << std::endl;
    }

    // Здесь будет логика удаления векторов по идентификаторам из базы данных
    // ...

    // out.setStr(body);
}
