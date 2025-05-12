#include <iostream>
#include <string>
#include "RequestUpdateDb.h"
#include "dist_fun.h"
#include "utils_rocks.h"


#include <nlohmann/json.hpp>
using json = nlohmann::json;

void RequestUpdateDb::run(const ProtocolInPost &in, const ProtocolOut &out) noexcept
{
    if (!in.isPost() || in.isEmpty()) [[unlikely]] {
        out.setCode(422);
        return;
    }
    auto body = in.key().ToStringView();

    std::string_view db_name;
    std::optional<DbMeta> meta;
    uint dist_index = DistFun::default_index;
    const json j = json::parse(body, nullptr, false);
    if (j.is_discarded() || !j.is_object()) [[unlikely]] {
        set_error(out, "Invalid JSON.");
        return;
    }

    {
        auto it_name = j.find("name");
        if (it_name == j.end() || !it_name->is_string()) [[unlikely]] {
            set_error(out, "Missing or invalid 'name' key.");
            return;
        }
        db_name = it_name->get<std::string_view>();
        if (db_name.empty()) [[unlikely]] {
            set_error(out, "DB name mast not be empty.");
            return;
        }

        meta = _db->get_meta(db_name);
        if( !meta.has_value() ) [[unlikely]] {
            set_error(out, "Data base doesn't exist.");
            return;
        }
        dist_index = meta->dist;

        std::string_view dist_name;
        auto it_dist = j.find("dist");
        if (it_dist != j.end() && it_dist->is_string()) {
            dist_name  = it_dist->get<std::string_view>();
            dist_index = DistFun::get_index(dist_name);
        } else if (it_dist != j.end() && !it_dist->is_string()) {
            set_error(out, "Invalid 'dist' key: type must be 'string'.");
            return;
        }
        if (dist_index == 0) [[unlikely]] {
            set_error(out, "Invalid 'dist' value. Unsupported distance function.");
            return;
        }
    }

    if (const char* err = _db->update_db(db_name, meta, dist_index); err != nullptr) [[unlikely]] {
        set_error(out, err);
        return;
    }
    out.setStr(R"({"success": true})");
}
