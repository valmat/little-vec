#include <iostream>
#include <string>
#include "RequestDeleteDb.h"
#include "utils_rocks.h"
#include "req_validator.h"

void RequestDeleteDb::run(const ProtocolInPost &in, const ProtocolOut &out) noexcept
{
    auto parsed = ReqValidator::init_meta(in, out, _db.get());
    if (!parsed) [[unlikely]] return;
    auto [js, db_name, meta] = std::move(parsed.value());

    if (const char* err = _db->delete_db(db_name, meta); err != nullptr) [[unlikely]] {
        set_error(out, err);
        return;
    }
    out.setStr(R"({"success": true})");    
}
