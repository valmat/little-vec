#include <iostream>
#include <string>
#include <vector>
#include "RequestSearchByVector.h"
#include "utils_rocks.h"
#include "units.h"
#include "req_validator.h"

void RequestSearchByVector::run(const ProtocolInPost &in, const ProtocolOut &out) noexcept
{
    auto parsed = ReqValidator::init_meta(in, out, _db.get());
    if ( !parsed ) [[unlikely]] return;
    auto [js, db_name, meta] = std::move(parsed.value());

    size_t top_k = ReqValidator::top_k(js, _db->opts().top_k(), out);
    if ( !top_k ) [[unlikely]] return;
    if ( !ReqValidator::dist(js, meta->dist, out) ) [[unlikely]] return;

    
    std::vector<float> vector;
    if ( !ReqValidator::vector(js , meta->dim, vector, out) ) [[unlikely]] {return;}

    json results {
        {"nearest", _db->search_vec(meta, vector, top_k)}
    };

    out.setStr(results.dump(_db->opts().json_indent()));
}
