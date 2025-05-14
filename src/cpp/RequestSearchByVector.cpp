#include <iostream>
#include <string>
#include <vector>
#include "RequestSearchByVector.h"
#include "dist_fun.h"
#include "utils_rocks.h"
#include "units.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

static
void to_json(json& j, const SearchResult& data)
{
    j = json{
        {"id", data.id},
        {"distance", data.distance}
    };

    if (!data.payload.empty()) {
        json js_payload = json::parse(data.payload, nullptr, false);
        if (!js_payload.is_discarded()) [[likely]] {
            j["payload"] = js_payload;
        }
    }
}

void RequestSearchByVector::run(const ProtocolInPost &in, const ProtocolOut &out) noexcept
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
    
    auto it_vector = j.find("vector");
    if (it_vector == j.end() || !it_vector->is_array() || it_vector->empty()) [[unlikely]] {
        set_error(out, "Missing or invalid 'vector' key. Must be a non-empty float array.");
        return;
    }
    // std::cout << "it_vector->size(): " << it_vector->size() << std::endl;
    // std::cout << "meta->dim: " << meta->dim << std::endl;
    if (it_vector->size() != meta->dim) [[unlikely]] {
        set_error(out, "Demention of vector must mutch to the Data Base Demention.");
        return;
    }

    std::vector<float> vector;
    vector.reserve(meta->dim);
    for (const auto &v : *it_vector) {
        if (!v.is_number()) [[unlikely]] {
            set_error(out, "All elements in 'vector' must be numeric.");
            return;
        }
        vector.push_back(v.get<float>());
    }

    // std::cout << "db_name: " << db_name << std::endl;
    // std::cout << "top_k: " << top_k << std::endl;
    // std::cout << "vector size: " << vector.size() << std::endl;

    json results = _db->search_vec(meta, vector, top_k);

    out.setStr(results.dump(_opts.json_indent()));
}
