#include <iostream>
#include <string>
#include "RequestCreateDb.h"
#include "dist_fun.h"
#include "utils_rocks.h"
#include "req_validator.h"

void RequestCreateDb::run(const ProtocolInPost &in, const ProtocolOut &out) noexcept
{
    auto parsed = ReqValidator::init(in, out);
    if (!parsed) [[unlikely]] return;
    auto [js, db_name] = std::move(parsed.value());

    uint db_dim = ReqValidator::dim(js, out);
    if (!db_dim) [[unlikely]] return;

    uint dist_index = DistFun::default_index;
    {
        auto it_dist = js.find("dist");
        if (it_dist != js.end() && it_dist->is_string()) {
            std::string_view dist_name = it_dist->get<std::string_view>();
            dist_index = DistFun::get_index(dist_name);
            if (dist_index == 0) [[unlikely]] {
                set_error(out, "Invalid 'dist' value. Unsupported distance function");
                return;
            }            
        } else if (it_dist != js.end() && !it_dist->is_string()) {
            set_error(out, "Invalid 'dist' key: type must be 'string'.");
            return;
        }
    }

    if (const char* err = _db->create_db(db_name, db_dim, dist_index); err != nullptr) [[unlikely]] {
        set_error(out, err);
        return;
    }
    out.setStr(R"({"success": true})");    
}
