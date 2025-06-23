#include <vector>
#include "RequestGetByIds.h"
#include "utils_rocks.h"
#include "req_validator.h"

void RequestGetByIds::run(const ProtocolInPost &in, const ProtocolOut &out) noexcept
{
    auto parsed = ReqValidator::init_meta(in, out, _db.get());
    if ( !parsed ) [[unlikely]] return;
    auto [js, db_name, meta] = std::move(parsed.value());

    std::vector<std::string_view> ids;
    if ( !ReqValidator::ids(js, ids, out) ) [[unlikely]] {return;}

    json results {
        {"data", _db->get_ids(meta, ids)}
    };


    out.setStr(results.dump(_db->opts().json_indent()));
}
