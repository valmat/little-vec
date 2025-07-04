#include <iostream>
#include <string>
#include <vector>
#include "RequestSearchByVectors.h"
#include "utils_rocks.h"
#include "req_validator.h"

void RequestSearchByVectors::run(const ProtocolInPost &in, const ProtocolOut &out) noexcept
{
    auto parsed = ReqValidator::init_meta(in, out, _db.get());
    if (!parsed) [[unlikely]] return;
    auto [js, db_name, meta] = std::move(parsed.value());

    size_t top_k = ReqValidator::top_k(js, _db->opts().top_k(), out);
    if ( !top_k ) [[unlikely]] return;
    if ( !ReqValidator::dist(js, meta->dist, out) ) [[unlikely]] return;
    
    // Checking vectors out
    std::vector<std::vector<float>> vectors;
    auto it_data = js.find("vectors");
    if ( !ReqValidator::vecs_array(it_data, js.end(), meta->dim, vectors, out) ) [[unlikely]] return;
    
    std::vector<std::vector<SearchResult>> search_results = _db->search_batch_vec(meta, vectors, top_k);
    
    std::vector<json> results(it_data->size());

    for (size_t i = 0; i < it_data->size(); ++i) {
        
        auto& src_item = (*it_data)[i];
        json item{{"data", std::move(search_results[i])}};
        
        if (auto it_extra = src_item.find("extra"); it_extra != src_item.end()) {
            item["extra"] = std::move(*it_extra);
        }

        results[i] = std::move(item);
    }

    out.setStr(json({{"results", std::move(results)}}).dump(_db->opts().json_indent()));
}
