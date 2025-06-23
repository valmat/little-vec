#pragma once

#include <optional>
#include <string_view>
#include <vector>
#include <nlohmann/json.hpp>
#include <rocksserver/api.h>
#include "VecDb.h"
#include "units.h"

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
    static size_t top_k(const json& js, size_t initial_value, const ProtocolOut &out) noexcept;
    static size_t dist(const json& js, uint& dist_index, const ProtocolOut &out) noexcept;
    
    static uint dim(const json& js, const ProtocolOut &out) noexcept;

    static bool data_array(json::const_iterator it, json::const_iterator end, const ProtocolOut &out) noexcept;
    static bool vecs_array(
        json::const_iterator it,
        json::const_iterator end,
        size_t dim,
        std::vector<std::vector<float>>& out_vectors,
        const ProtocolOut &out) noexcept;
    
    static bool vector(const json& js, size_t dim, std::vector<float>& out_vector, const ProtocolOut &out) noexcept;
    static bool ids(const json& js, std::vector<std::string_view>& out_vector, const ProtocolOut &out) noexcept;
};

void to_json(json& j, const SearchResult& data) noexcept;
void to_json(json& j, const DataUnit& data) noexcept;
