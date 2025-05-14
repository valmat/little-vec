#include "RequestUpdateDb.h"
#include "dist_fun.h"
#include "utils_rocks.h"
#include "req_validator.h"

void RequestUpdateDb::run(const ProtocolInPost &in, const ProtocolOut &out) noexcept
{
    auto parsed = ReqValidator::init_meta(in, out, _db.get());
    if (!parsed) [[unlikely]] return;
    auto [js, db_name, meta] = std::move(parsed.value());

    uint dist_index = meta->dist;
    if ( !ReqValidator::dist(js, dist_index, out) ) [[unlikely]] return;

    if (const char* err = _db->update_db(db_name, meta, dist_index); err != nullptr) [[unlikely]] {
        set_error(out, err);
        return;
    }
    out.setStr(R"({"success": true})");
}
