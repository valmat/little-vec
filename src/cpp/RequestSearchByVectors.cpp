#include <iostream>
#include <string>
#include <vector>
#include "RequestSearchByVectors.h"
#include "dist_fun.h"
#include "utils_rocks.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

void RequestSearchByVectors::run(const ProtocolInPost &in, const ProtocolOut &out) noexcept
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

    size_t top_k = _db->opts().top_k();
    auto it_top_k = j.find("top_k");
    if (it_top_k != j.end() && !it_top_k->is_number_integer()) [[unlikely]] {
        set_error(out, "Value of 'top_k' must be integer.");
        return;
    } else if (it_top_k != j.end()) [[unlikely]] {
        top_k = it_top_k->get<size_t>();
    }

    auto it_dist = j.find("dist");
    if (it_dist != j.end() && it_dist->is_string()) {
        std::string_view dist_name = it_dist->get<std::string_view>();
        uint dist_index = DistFun::get_index(dist_name);
        if (dist_index == 0) [[unlikely]] {
            set_error(out, "Invalid 'dist' value. Unsupported distance function");
            return;
        }
        meta->dist = dist_index;
    } else if (it_dist != j.end() && !it_dist->is_string()) {
        set_error(out, "Invalid 'dist' key: type must be 'string'.");
        return;
    }
    

    // Проверка data
    auto it_data = j.find("data");
    if (it_data == j.end() || !it_data->is_array() || it_data->empty()) [[unlikely]] {
        set_error(out, "Missing or invalid 'data' key. Must be a non-empty array.");
        return;
    }

    json results{};

    // "results": [
    //     {
    //         "nearest": [
    //             { "id": "123", "distance": 0.123, "payload": ... },
    //             ...
    //         ],
    //         "extra": ... // если extra был передан в запросе
    //     },
    //     ...
    // ]    

    std::vector<std::vector<float>> vectors;
    vectors.reserve(it_data->size());
    for (const auto& item : *it_data) {

        if (!item.is_object()) [[unlikely]] {
            set_error(out, "Each item in 'data' must be an object.");
            return;
        }

        // Проверка extra (опциональное поле)
        json extra_data = nullptr;
        
        if (auto it_extra = item.find("extra"); it_extra != item.end()) {
            results.push_back(json{{"extra", *it_extra}});
        } else {
            results.push_back(json{});
        }

        auto it_vector = item.find("vector");
        if (it_vector == item.end() || !it_vector->is_array() || it_vector->empty()) [[unlikely]] {
            set_error(out, "Missing or invalid 'vector' key. Must be a non-empty float array.");
            return;
        }
        if (it_vector->size() != meta->dim) [[unlikely]] {
            set_error(out, "Demention of vector must mutch to the Data Base Demention.");
            return;
        }

        std::vector<float> vector_data;
        vector_data.reserve(meta->dim);
        
        std::cout << "vector:" << std::endl;
        for (const auto &v : *it_vector) {
            if (!v.is_number()) [[unlikely]] {
                set_error(out, "All elements in 'vector' must be numeric.");
                return;
            }
            std::cout << " " << v.get<float>();
            vector_data.push_back(v.get<float>());
        }
        std::cout << std::endl;

        vectors.emplace_back(std::move(vector_data));
        

        std::cout << "item: " << item.dump() << std::endl;
    }



    std::cout << "db_name: " << db_name << std::endl;
    std::cout << "top_k: " << top_k << std::endl;
    // std::cout << "vector size: " << vector_data.size() << std::endl;

    // if (const char* err = _db->search_batch_vec(meta, vectors, top_k); err != nullptr) [[unlikely]] {
    //     set_error(out, err);
    //     return;
    // }

    // out.setStr(json({{"results", results}}).dump(_db->opts().json_indent()));
}
