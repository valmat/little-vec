#include "utils.h"
#include "dist_fun.h"
#include "utils_rocks.h"
#include "req_validator.h"

static constexpr size_t max_ids_size = 10000;

std::optional<InitReqData> ReqValidator::init(const ProtocolInPost &in, const ProtocolOut &out) noexcept
{
    if (!in.isPost() || in.isEmpty()) [[unlikely]] {
        out.setCode(422);
        return std::nullopt;
    }

    json js = json::parse(in.key().ToStringView(), nullptr, false);
    if (js.is_discarded() || !js.is_object()) [[unlikely]] {
        set_error(out, "Invalid JSON.");
        return std::nullopt;
    }

    auto it_db_name = js.find("db_name");
    if (it_db_name == js.end() || !it_db_name->is_string() || it_db_name->get<std::string_view>().empty()) [[unlikely]] {
        set_error(out, "Missing or invalid 'db_name' key.");
        return std::nullopt;
    }

    std::string_view db_name = it_db_name->get<std::string_view>();

    return InitReqData{ std::move(js), db_name };
}

std::optional<InitReqDataExt> ReqValidator::init_meta(const ProtocolInPost &in, const ProtocolOut &out, VecDb* db) noexcept
{
    auto parsed = init(in, out);
    if (!parsed) [[unlikely]] return {};
    auto [js, db_name] = std::move(parsed.value());

    auto meta = db->get_meta(db_name);
    if (!meta.has_value()) [[unlikely]] {
        set_error(out, "Data base doesn't exist.");
        return std::nullopt;
    }

    return InitReqDataExt{ std::move(js), db_name, meta };
}

size_t ReqValidator::top_k(const json& js, size_t value, const ProtocolOut &out) noexcept
{
    auto it_top_k = js.find("top_k");
    if (it_top_k != js.end() && !it_top_k->is_number_integer()) [[unlikely]] {
        set_error(out, "Value of 'top_k' must be integer.");
        return 0;
    } else if (it_top_k != js.end()) [[unlikely]] {
        value = it_top_k->get<size_t>();
        if (value == 0) [[unlikely]] {
            set_error(out, "Invalid 'top_k' value. Must be more than 0.");
            return 0;
        }        
    }
    return value;
}

size_t ReqValidator::dist(const json& js, uint& dist_index, const ProtocolOut &out) noexcept
{
    auto it_dist = js.find("dist");
    if (it_dist != js.end() && it_dist->is_string()) {
        std::string_view dist_name = it_dist->get<std::string_view>();
        dist_index = DistFun::get_index(dist_name);
        if (dist_index == 0) [[unlikely]] {
            set_error(out, "Invalid 'dist' value. Unsupported distance function");
            return false;
        }
    } else if (it_dist != js.end() && !it_dist->is_string()) {
        set_error(out, "Invalid 'dist' key: type must be 'string'.");
        return false;
    }
    return true;
}

uint ReqValidator::dim(const json& js, const ProtocolOut &out) noexcept
{
    auto it_dim = js.find("dim");
    if (it_dim == js.end() || !it_dim->is_number_integer()) [[unlikely]] {
        set_error(out, "Missing or invalid 'dim' key.");
        return 0;
    }
    uint db_dim = it_dim->get<uint>();
    if (db_dim == 0) [[unlikely]] {
        set_error(out, "Invalid 'dim' value.");
        return 0;
    }
    return db_dim;
}

bool ReqValidator::data_array(json::const_iterator it, json::const_iterator end, const ProtocolOut &out) noexcept
{
    if (it == end || !it->is_array() || it->empty()) [[unlikely]] {
        set_error(out, "Missing or invalid 'data' key. Must be a non-empty array.");
        return false;
    }
    return true;
}


bool ReqValidator::vecs_array(
    json::const_iterator it,
    json::const_iterator end,
    size_t dim,
    std::vector<std::vector<float>>& out_vectors,
    const ProtocolOut &out) noexcept
{
    if (it == end || !it->is_array() || it->empty()) [[unlikely]] {
        set_error(out, "Missing or invalid 'vectors' key. Must be a non-empty array.");
        return false;
    }

    out_vectors.reserve(it->size());
    for (const auto& item : *it) {
        if (!item.is_object()) [[unlikely]] {
            set_error(out, "Each item in 'data' must be an object.");
            return false;
        }

        std::vector<float> vector_data;
        if ( !ReqValidator::vector(item , dim, vector_data, out) ) [[unlikely]] {
            return false;
        }

        out_vectors.emplace_back(std::move(vector_data));
    }

    return true;
}

bool ReqValidator::vector(const json& js, size_t dim, std::vector<float>& out_vector, const ProtocolOut &out) noexcept
{
    auto it_vector = js.find("vector");
    if (it_vector == js.end()) [[unlikely]] {
        set_error(out, "Missing or invalid 'vector' key.");
        return false;
    }    
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

bool ReqValidator::ids(const json& js, std::vector<std::string_view>& out_vector, const ProtocolOut &out) noexcept
{
    auto it_ids = js.find("ids");
    if (it_ids == js.end()) [[unlikely]] {
        set_error(out, "Missing or invalid 'ids' key.");
        return false;
    }    
    if (!it_ids->is_array() || it_ids->size() > max_ids_size) [[unlikely]] {
        set_error(out, "Vector 'ids' must be a string array.");
        return false;
    }

    out_vector.reserve(it_ids->size());
    for (const auto& val : *it_ids) {
        if (!val.is_string()) [[unlikely]] {
            set_error(out, "All elements of 'ids' must be stings.");
            return false;
        }
        out_vector.push_back(val.get<std::string_view>());
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

void to_json(json& j, const DataUnit& data) noexcept
{
    j = json{
        {"id", data.id},
    };

    if (!data.payload.empty()) {
        json js_payload = json::parse(data.payload, nullptr, false);
        if (!js_payload.is_discarded()) [[likely]] {
            j["payload"] = js_payload;
        }
    }
}

void to_json(json& j, const DistancesUnit& data) noexcept
{
    json distances_json = json::array();
    distances_json.get_ref<json::array_t&>().reserve(data.distances.size());

    for (auto d : data.distances) {
        distances_json.push_back({{"distance", d}});
    }    

    j = json{
        {"id", data.id},
        {"distances", std::move(distances_json)}
    };

    if (!data.payload.empty()) {
        json js_payload = json::parse(data.payload, nullptr, false);
        if (!js_payload.is_discarded()) [[likely]] {
            j["payload"] = js_payload;
        }
    }
}
