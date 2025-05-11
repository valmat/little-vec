#include <iostream>
#include <string>
#include <rocksserver/api.h>
#include "RequestCreateDb.h"
#include "dist_fun.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

void set_error(const ProtocolOut &out, const char* msg) noexcept
{
    out.setStr(R"({"error": ")").setStr(msg).setStr(R"("})").setCode(422);
}

void RequestCreateDb::run(const ProtocolInPost &in, const ProtocolOut &out) noexcept
{
    if(!in.isPost() || in.isEmpty()) {
        out.setCode(422);
        return;
    }
    auto body = in.key().ToStringView();

    std::cout << body << std::endl;

    std::string db_name;
    int db_dim;
    int dist_index = DistFun::default_index;

    {
        json j = json::parse(body, nullptr, false);
        if (j.is_discarded() || !j.is_object()) {
            set_error(out, "Invalid JSON.");
            return;
        }

        auto it_name = j.find("name");
        if (it_name == j.end() || !it_name->is_string()) {
            set_error(out, "Missing or invalid 'name' key.");
            return;
        }
        db_name = it_name->get<std::string>();

        auto it_dim = j.find("dim");
        if (it_dim == j.end() || !it_dim->is_number_integer()) {
            set_error(out, "Missing or invalid 'dim' key.");
            return;
        }
        db_dim = it_dim->get<int>();

        std::string dist_name;
        auto it_dist = j.find("dist");
        if (it_dist != j.end() && it_dist->is_string()) {
            dist_name  = it_dist->get<std::string>();
            dist_index = DistFun::get_index(dist_name.c_str());
        } else if (it_dist != j.end() && !it_dist->is_string()) {
            set_error(out, "Invalid 'dist' key: type must be 'string'.");
            return;
        }        
    }

    if (db_dim <= 0) {
        set_error(out, "Invalid 'dim' value.");
        return;
    }
    if (dist_index == 0) {
        set_error(out, "Invalid 'dist' value. Unsupported distance function");
        return;
    }
    if (db_name.empty()) {
        set_error(out, "DB name mast not be empty");
        return;
    }    
    

    std::cout << "db_name: " << db_name << std::endl;
    std::cout << "db_dim: " << db_dim << std::endl;
    std::cout << "dist_index: " << dist_index << std::endl;

    // out.setStr(body);
}
