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
    if ( !ReqValidator::dist(js, dist_index, out) ) [[unlikely]] return;

    if (const char* err = _db->create_db(db_name, db_dim, dist_index); err != nullptr) [[unlikely]] {
        set_error(out, err);
        return;
    }
    out.setStr(R"({"success": true})");    
}
