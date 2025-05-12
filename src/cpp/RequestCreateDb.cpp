#include <iostream>
#include <string>
#include "RequestCreateDb.h"
#include "dist_fun.h"
#include "utils_rocks.h"


#include <nlohmann/json.hpp>
using json = nlohmann::json;

void RequestCreateDb::run(const ProtocolInPost &in, const ProtocolOut &out) noexcept
{
    if (!in.isPost() || in.isEmpty()) [[unlikely]] {
        out.setCode(422);
        return;
    }
    auto body = in.key().ToStringView();

    std::string_view db_name;
    uint db_dim;
    uint dist_index = DistFun::default_index;

    {
        const json j = json::parse(body, nullptr, false);
        if (j.is_discarded() || !j.is_object()) [[unlikely]] {
            set_error(out, "Invalid JSON.");
            return;
        }

        auto it_name = j.find("name");
        if (it_name == j.end() || !it_name->is_string()) [[unlikely]] {
            set_error(out, "Missing or invalid 'name' key.");
            return;
        }
        db_name = it_name->get<std::string_view>();

        auto it_dim = j.find("dim");
        if (it_dim == j.end() || !it_dim->is_number_integer()) [[unlikely]] {
            set_error(out, "Missing or invalid 'dim' key.");
            return;
        }
        db_dim = it_dim->get<uint>();

        std::string_view dist_name;
        auto it_dist = j.find("dist");
        if (it_dist != j.end() && it_dist->is_string()) {
            dist_name  = it_dist->get<std::string_view>();
            dist_index = DistFun::get_index(dist_name);
        } else if (it_dist != j.end() && !it_dist->is_string()) {
            set_error(out, "Invalid 'dist' key: type must be 'string'.");
            return;
        }        
    }

    if (db_dim == 0) [[unlikely]] {
        set_error(out, "Invalid 'dim' value.");
        return;
    }
    if (dist_index == 0) [[unlikely]] {
        set_error(out, "Invalid 'dist' value. Unsupported distance function");
        return;
    }
    if (db_name.empty()) [[unlikely]] {
        set_error(out, "DB name must not be empty");
        return;
    }    
    
    std::cout << "db_name: " << db_name << std::endl;
    std::cout << "db_dim: " << db_dim << std::endl;
    std::cout << "dist_index: " << dist_index << std::endl;

    if (const char* err = _db->create_db(db_name, db_dim, dist_index); err != nullptr) [[unlikely]] {
        set_error(out, err);
        return;
    }
    out.setStr(R"({"success": true})");    
}
