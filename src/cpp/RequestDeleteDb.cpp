#include <iostream>
#include <string>
#include "RequestDeleteDb.h"
#include "utils_rocks.h"


#include <nlohmann/json.hpp>
using json = nlohmann::json;

void RequestDeleteDb::run(const ProtocolInPost &in, const ProtocolOut &out) noexcept
{
    if (!in.isPost() || in.isEmpty()) [[unlikely]] {
        out.setCode(422);
        return;
    }
    auto body = in.key().ToStringView();

    std::cout << body << std::endl;

    std::string_view db_name;
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
    }

    if (db_name.empty()) [[unlikely]] {
        set_error(out, "DB name must not be empty");
        return;
    }

    if (const char* err = _db->delete_db(db_name); err != nullptr) [[unlikely]] {
        set_error(out, err);
        return;
    }
    out.setStr(R"({"success": true})");    
}
