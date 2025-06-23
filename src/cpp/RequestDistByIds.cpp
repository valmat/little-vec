#include <vector>
#include "RequestDistByIds.h"
#include "utils_rocks.h"
#include "req_validator.h"

void RequestDistByIds::run(const ProtocolInPost &in, const ProtocolOut &out) noexcept
{
    auto parsed = ReqValidator::init_meta(in, out, _db.get());
    if ( !parsed ) [[unlikely]] return;
    auto [js, db_name, meta] = std::move(parsed.value());

    std::vector<std::string_view> ids;
    if ( !ReqValidator::ids(js, ids, out) ) [[unlikely]] {return;}

    if ( !ReqValidator::dist(js, meta->dist, out) ) [[unlikely]] return;

    std::vector<std::vector<float>> vectors;
    auto it_data = js.find("vectors");
    if ( !ReqValidator::vecs_array(it_data, js.end(), meta->dim, vectors, out) ) [[unlikely]] return;

    auto results = json(_db->get_distances(meta, ids, vectors));

    for(auto it = results.begin(); it != results.end(); ++it) {
        auto it_dists = it->find("distances");

        for (size_t i = 0; i < it_data->size(); ++i) {
            auto& src_item = (*it_data)[i];
            if (auto it_extra = src_item.find("extra"); it_extra != src_item.end()) {
                (*it_dists)[i]["extra"] = *it_extra;
            }
        }
    }

    out.setStr(json{{"data", results}}.dump(_db->opts().json_indent()));
}
