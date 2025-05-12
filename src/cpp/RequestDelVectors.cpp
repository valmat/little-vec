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
    std::string_view db_name;
    std::vector<std::string_view> ids;
    std::optional<DbMeta> meta;
    const json j = json::parse(body, nullptr, false);
    {
        if (j.is_discarded() || !j.is_object()) [[unlikely]] {
            set_error(out, "Invalid JSON.");
            return;
        }

        auto it_db_name = j.find("db_name");
        if (it_db_name == j.end() || !it_db_name->is_string()) [[unlikely]] {
            set_error(out, "Missing or invalid 'db_name' key.");
            return;
        }
        db_name = it_db_name->get<std::string_view>();
        if (db_name.empty()) [[unlikely]] {
            set_error(out, "'db_name' must not be empty.");
            return;
        }
        meta = _db->get_meta(db_name);
        if( !meta.has_value() ) [[unlikely]] {
            set_error(out, "Data base doesn't exist.");
            return;
        }

        auto it_data = j.find("data");
        if (it_data == j.end() || !it_data->is_array()) [[unlikely]] {
            set_error(out, "Missing or invalid 'data' key. Expected array.");
            return;
        }
        if (it_data->empty()) [[unlikely]] {
            set_error(out, "'data' array must contain at least one item.");
            return;
        }

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
