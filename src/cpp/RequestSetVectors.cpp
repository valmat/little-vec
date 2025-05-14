#include <iostream>
#include <string>
#include "RequestSetVectors.h"
#include "utils_rocks.h"
#include "utils.h"
#include "req_validator.h"

void RequestSetVectors::run(const ProtocolInPost &in, const ProtocolOut &out) noexcept
{
    auto parsed = ReqValidator::init_meta(in, out, _db.get());
    if (!parsed) [[unlikely]] return;
    auto [js, db_name, meta] = std::move(parsed.value());


    // Check if the "data" field exists and is an array
    auto it_data = js.find("data");
    if ( !ReqValidator::data_array(it_data, js.end(), out) ) [[unlikely]] return;

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
