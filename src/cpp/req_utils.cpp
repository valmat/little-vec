#include "req_utils.h"
#include "utils.h"
#include "dist_fun.h"
#include "utils_rocks.h"

std::optional<VecRequest> RequestUtils::init_vec(const ProtocolInPost &in, const ProtocolOut &out, VecDb* db) noexcept {
    if (!in.isPost() || in.isEmpty()) [[unlikely]] {
        out.setCode(422);
        return std::nullopt;
    }

    json j = json::parse(in.key().ToStringView(), nullptr, false);
    if (j.is_discarded() || !j.is_object()) [[unlikely]] {
        set_error(out, "Invalid JSON.");
        return std::nullopt;
    }

    auto it_db_name = j.find("db_name");
    if (it_db_name == j.end() || !it_db_name->is_string() || it_db_name->get<std::string_view>().empty()) [[unlikely]] {
        set_error(out, "Missing or invalid 'db_name' key.");
        return std::nullopt;
    }

    std::string_view db_name = it_db_name->get<std::string_view>();
    auto meta = db->get_meta(db_name);
    if (!meta.has_value()) [[unlikely]] {
        set_error(out, "Data base doesn't exist.");
        return std::nullopt;
    }

    return VecRequest{ std::move(j), db_name, meta };
}

size_t RequestUtils::top_k_dist_vec(const json& js, const VecDbOpts& opts, DbMeta& meta, const ProtocolOut &out) noexcept
{
    size_t top_k = opts.top_k();
    auto it_top_k = js.find("top_k");
    if (it_top_k != js.end() && !it_top_k->is_number_integer()) [[unlikely]] {
        set_error(out, "Value of 'top_k' must be integer.");
        return 0;
    } else if (it_top_k != js.end()) [[unlikely]] {
        top_k = it_top_k->get<size_t>();
        if (top_k == 0) [[unlikely]] {
            set_error(out, "Invalid 'top_k' value. Must be more than 0.");
            return 0;
        }        
    }

    auto it_dist = js.find("dist");
    if (it_dist != js.end() && it_dist->is_string()) {
        std::string_view dist_name = it_dist->get<std::string_view>();
        uint dist_index = DistFun::get_index(dist_name);
        if (dist_index == 0) [[unlikely]] {
            set_error(out, "Invalid 'dist' value. Unsupported distance function");
            return 0;
        }
        meta.dist = dist_index;
    } else if (it_dist != js.end() && !it_dist->is_string()) {
        set_error(out, "Invalid 'dist' key: type must be 'string'.");
        return 0;
    }
    return top_k;
}

bool RequestUtils::validate_vector(const json& js, size_t dim, std::vector<float>& out_vector, const ProtocolOut &out) noexcept {
    auto it_vector = js.find("vector");
    if (!it_vector->is_array() || it_vector->size() != dim) [[unlikely]] {
        set_error(out, "Vector must be a numeric array with correct dimension.");
        return false;
    }

    out_vector.reserve(dim);
    for (const auto& val : *it_vector) {
        if (!val.is_number()) [[unlikely]] {
            set_error(out, "All elements of 'vector' must be numeric.");
            return false;
        }
        out_vector.push_back(val.get<float>());
    }
    return true;
}

void to_json(json& j, const SearchResult& data) noexcept
{
    j = json{
        {"id", data.id},
        {"distance", data.distance}
    };

    if (!data.payload.empty()) {
        json js_payload = json::parse(data.payload, nullptr, false);
        if (!js_payload.is_discarded()) [[likely]] {
            j["payload"] = js_payload;
        }
    }
}
