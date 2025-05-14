#include <iostream>
#include <string>
#include <vector>
#include "RequestSearchByVector.h"
#include "dist_fun.h"
#include "utils_rocks.h"
#include "units.h"
#include "req_utils.h"

void RequestSearchByVector::run(const ProtocolInPost &in, const ProtocolOut &out) noexcept
{
    auto parsed = RequestUtils::init_vec(in, out, _db.get());
    if (!parsed) [[unlikely]] return;
    auto [js, db_name, meta] = std::move(parsed.value());


    size_t top_k = RequestUtils::top_k_dist_vec(js, _db->opts(), meta.value(), out);
    if (!top_k) [[unlikely]] return;

    
    std::vector<float> vector;
    if ( !RequestUtils::validate_vector(js , meta->dim, vector, out) ) [[unlikely]] {return;}


    json results {
        {"nearest", _db->search_vec(meta, vector, top_k)}
    };

    out.setStr(results.dump(_db->opts().json_indent()));
}
