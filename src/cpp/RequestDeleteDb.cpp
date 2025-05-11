#include <iostream>
#include <string>
#include "RequestDeleteDb.h"
#include "utils_rocks.h"


#include <nlohmann/json.hpp>
using json = nlohmann::json;

void RequestDeleteDb::run(const ProtocolInPost &in, const ProtocolOut &out) noexcept
{
    if(!in.isPost() || in.isEmpty()) {
        out.setCode(422);
        return;
    }
    auto body = in.key().ToStringView();

    std::cout << body << std::endl;

    std::string db_name;
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
    }

    if (db_name.empty()) {
        set_error(out, "DB name must not be empty");
        return;
    }    
    
    std::cout << "db_name: " << db_name << std::endl;

    // out.setStr(body);
}
