#include <iostream>
#include <string>
#include "RequestDelVectors.h"
#include "utils_rocks.h"
#include "req_validator.h"

void RequestDelVectors::run(const ProtocolInPost &in, const ProtocolOut &out) noexcept
{
    auto parsed = ReqValidator::init_meta(in, out, _db.get());
    if (!parsed) [[unlikely]] return;
    auto [js, db_name, meta] = std::move(parsed.value());

    std::vector<std::string_view> ids;
    {
        auto it_data = js.find("data");
        if ( !ReqValidator::data_array(it_data, js.end(), out) ) [[unlikely]] return;

        ids.reserve(it_data->size());

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
            auto id = it_id->get<std::string_view>();
            if (id.empty()) [[unlikely]] {
                set_error(out, "'id' in 'data' item must not be empty.");
                return;
            }
            ids.push_back(id);
        }
    }

    if (const char* err = _db->del_vec(meta, ids); err != nullptr) [[unlikely]] {
        set_error(out, err);
        return;
    }

    out.setStr(R"({"success": true})");
}
