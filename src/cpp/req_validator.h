#pragma once

#include <optional>
#include <string_view>
#include <vector>
#include <nlohmann/json.hpp>
#include <rocksserver/api.h>
#include "VecDb.h"
#include "units.h"
#include "utils_rocks.h"

using json = nlohmann::json;

struct InitReqData final
{
    json js;
    std::string_view db_name;
};
struct InitReqDataExt final
{
    json js;
    std::string_view db_name;
    std::optional<DbMeta> meta;
};

struct ReqValidator final
{
    static std::optional<InitReqData> init(const ProtocolInPost &in, const ProtocolOut &out) noexcept;
    static std::optional<InitReqDataExt> init_meta(const ProtocolInPost &in, const ProtocolOut &out, VecDb* db) noexcept;

    // Updates meta.dist and returns top_k
    static size_t top_k_dist_vec(const json& js, const VecDbOpts& opts, DbMeta& meta, const ProtocolOut &out) noexcept;
    static uint dim(const json& js, const ProtocolOut &out) noexcept;
    
    static bool validate_vector(const json& js, size_t dim, std::vector<float>& out_vector, const ProtocolOut &out) noexcept;
};

void to_json(json& j, const SearchResult& data) noexcept;
