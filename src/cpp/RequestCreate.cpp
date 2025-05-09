#include <iostream>
#include <string>
#include <rocksserver/api.h>
#include "RequestCreate.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

void RequestCreate::run(const ProtocolInPost &in, const ProtocolOut &out) noexcept
{
    if(!in.isPost() || in.isEmpty()) {
        out.setCode(422);
        return;
    }
    auto body = in.key().ToStringView();

    std::cout << body << std::endl;

    std::string db_name;
    int db_dim;
    std::string dist_name;

    try {
        auto j = json::parse(body);

        db_name = j.at("name").get<std::string>();
        db_dim = j.at("dim").get<int>();
        dist_name = j.at("dist").get<std::string>();

    } catch (const json::exception& e) {
        out.setCode(422);
        return;
    }

    std::cout << "db_name: " << db_name << std::endl;
    std::cout << "db_dim: " << db_dim << std::endl;
    std::cout << "dist_name: " << dist_name << std::endl;

    

    out.setStr(body);
}