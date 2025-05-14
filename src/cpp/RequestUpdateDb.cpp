#include <iostream>
#include <string>
#include "RequestUpdateDb.h"
#include "dist_fun.h"
#include "utils_rocks.h"
#include "req_utils.h"

void RequestUpdateDb::run(const ProtocolInPost &in, const ProtocolOut &out) noexcept
{
    auto parsed = RequestUtils::init_meta(in, out, _db.get());
    if (!parsed) [[unlikely]] return;
    auto [js, db_name, meta] = std::move(parsed.value());

    uint dist_index = meta->dist;
    {
        std::string_view dist_name;
        auto it_dist = js.find("dist");
        if (it_dist != js.end() && it_dist->is_string()) {
            dist_name  = it_dist->get<std::string_view>();
            dist_index = DistFun::get_index(dist_name);
        } else if (it_dist != js.end() && !it_dist->is_string()) {
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
