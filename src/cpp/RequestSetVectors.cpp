#include <iostream>
#include <string>
#include "RequestSetVectors.h"
#include "dist_fun.h"
#include "utils_rocks.h"
#include "utils.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

void RequestSetVectors::run(const ProtocolInPost &in, const ProtocolOut &out) noexcept
{
    // Check if the request is a POST and not empty
    if (!in.isPost() || in.isEmpty()) [[unlikely]] {
        out.setCode(422); // Set HTTP code 422 (Unprocessable Entity)
        return;
    }
    auto body = in.key().ToStringView();

    // Try to parse the JSON body
    const json j = json::parse(body, nullptr, false);
    if (j.is_discarded() || !j.is_object()) [[unlikely]] {
        set_error(out, "Invalid JSON.");
        return;
    }

    // Check if the "db_name" field exists and is a string
    auto it_db_name = j.find("db_name");
    if (it_db_name == j.end() || !it_db_name->is_string()) [[unlikely]] {
        set_error(out, "Missing or invalid 'db_name' key.");
        return;
    }
    auto db_name = it_db_name->get<std::string_view>();
    if (db_name.empty()) [[unlikely]] {
        set_error(out, "'db_name' must not be empty.");
        return;
    }

    // Check if the database exists
    auto meta = _db->get_meta(db_name);
    if( !meta.has_value() ) [[unlikely]] {
        set_error(out, "Data base doesn't exist.");
        return;
    }

    // Check if the "data" field exists and is an array
    auto it_data = j.find("data");
    if (it_data == j.end() || !it_data->is_array()) [[unlikely]] {
        set_error(out, "Missing or invalid 'data' key. Must be an array.");
        return;
    }

    // Prepare a buffer for serializing vectors
    std::vector<char> vector_serialized;
    vector_serialized.resize(serialize_buf_bytes_len(meta->dim));    

    // Go through each item in the "data" array
    for (const auto& item : *it_data) {
        if (!item.is_object()) [[unlikely]] {
            set_error(out, "Each item in 'data' array must be an object.");
            return;
        }

        // Check if the "id" field exists and is a string
        auto it_id = item.find("id");
        if (it_id == item.end() || !it_id->is_string()) [[unlikely]] {
            set_error(out, "Missing or invalid 'id' key in 'data' item.");
            return;
        }
        auto id = it_id->get<std::string_view>();
        if (id.empty()) [[unlikely]] {
            set_error(out, "'id' must not be empty.");
            return;
        }

        // The "payload" field is optional. If it exists, convert it to a string.
        std::string payload{};
        auto it_payload = item.find("payload");
        if (it_payload != item.end()) {
            payload = it_payload->dump();
        }

        // Check if the "vector" field exists and is an array
        auto it_vector = item.find("vector");
        if (it_vector == item.end() || !it_vector->is_array()) [[unlikely]] {
            set_error(out, "Missing or invalid 'vector' key in 'data' item.");
            return;
        }

        // "vector" must not be empty
        if (it_vector->empty()) [[unlikely]] {
            set_error(out, "Value of 'vector' must not be empty.");
            return;
        }
        // The size of "vector" must match the database dimension
        if (it_vector->size() != meta->dim) [[unlikely]] {
            set_error(out, "Dimension of 'vector' must match db dimension.");
            return;
        }

        // Serialize the vector values
        auto vector_ser_it = vector_serialized.data();
        for (const auto& val : *it_vector) {
            if (!val.is_number()) [[unlikely]] {
                set_error(out, "All elements of 'vector' must be numeric.");
                return;
            }
            vector_ser_it = serialize_val(val.get<float>(), vector_ser_it);
        }

        // Save the vector and payload in the database
        if (const char* err = _db->set_vec(meta, id, vector_serialized, payload); err != nullptr) [[unlikely]] {
            set_error(out, err);
            return;
        }
    }

    // If everything is OK, return success
    out.setStr(R"({"success": true})");
}
