#include <iostream>
#include <string>
#include "RequestSetVectors.h"
#include "dist_fun.h"
#include "utils_rocks.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

void RequestSetVectors::run(const ProtocolInPost &in, const ProtocolOut &out) noexcept
{
    if (!in.isPost() || in.isEmpty()) {
        out.setCode(422);
        return;
    }

    auto body = in.key().ToStringView();

    std::cout << body << std::endl;

    json j = json::parse(body, nullptr, false);
    if (j.is_discarded() || !j.is_object()) {
        set_error(out, "Invalid JSON.");
        return;
    }

    // Проверка наличия и корректности поля "db_name"
    auto it_db_name = j.find("db_name");
    if (it_db_name == j.end() || !it_db_name->is_string()) {
        set_error(out, "Missing or invalid 'db_name' key.");
        return;
    }
    std::string db_name = it_db_name->get<std::string>();
    if (db_name.empty()) {
        set_error(out, "'db_name' must not be empty.");
        return;
    }

    // Проверка наличия и корректности поля "data"
    auto it_data = j.find("data");
    if (it_data == j.end() || !it_data->is_array()) {
        set_error(out, "Missing or invalid 'data' key. Must be an array.");
        return;
    }

    for (const auto& item : *it_data) {
        if (!item.is_object()) {
            set_error(out, "Each item in 'data' array must be an object.");
            return;
        }

        // Проверка поля "id"
        auto it_id = item.find("id");
        if (it_id == item.end() || !it_id->is_string()) {
            set_error(out, "Missing or invalid 'id' key in 'data' item.");
            return;
        }
        std::string id = it_id->get<std::string>();
        if (id.empty()) {
            set_error(out, "'id' must not be empty.");
            return;
        }

        // Проверка поля "vector"
        auto it_vector = item.find("vector");
        if (it_vector == item.end() || !it_vector->is_array()) {
            set_error(out, "Missing or invalid 'vector' key in 'data' item.");
            return;
        }

        // XXX TODO: проверять размерность
        std::vector<float> vector_data;
        for (const auto& val : *it_vector) {
            if (!val.is_number()) {
                set_error(out, "All elements of 'vector' must be numeric.");
                return;
            }
            vector_data.push_back(val.get<float>());
        }

        if (vector_data.empty()) {
            set_error(out, "'vector' must not be empty.");
            return;
        }

        // Поле "payload" опционально, если есть - проверим, что это объект
        auto it_payload = item.find("payload");
        if (it_payload != item.end() && !it_payload->is_object()) {
            set_error(out, "Invalid 'payload' key in 'data' item. Must be an object.");
            return;
        }

        // Выводим данные для отладки
        std::cout << "db_name: " << db_name << std::endl;
        std::cout << "id: " << id << std::endl;
        std::cout << "vector size: " << vector_data.size() << std::endl;

        if (it_payload != item.end()) {
            std::cout << "payload: " << it_payload->dump() << std::endl;
        } else {
            std::cout << "payload: (none)" << std::endl;
        }

        // Здесь далее будет логика сохранения/обновления вектора в БД
        // ...
    }

    // Если всё прошло успешно, отправляем ответ клиенту
    json response;
    response["status"] = "success";
    response["message"] = "Vectors processed successfully.";
    out.setStr(response.dump());

}
