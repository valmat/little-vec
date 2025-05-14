#pragma once
#include <iostream>
#include <string>
#include <optional>
#include <vector>
#include <rocksserver/api.h>
#include "units.h"
#include "VecDbOpts.h"

using namespace RocksServer;

class VecDb final
{
    VecDbOpts _opts;
    RocksDBWrapper& _db;

public:
    VecDb(const VecDbOpts& opts, RocksDBWrapper& db) noexcept;

    const char* create_db(std::string_view db_name, uint db_dim, uint dist_index) noexcept;
    const char* update_db(std::string_view db_name, uint dist_index) noexcept;
    const char* update_db(std::string_view db_name, std::optional<DbMeta> meta, uint dist_index) noexcept;
    const char* delete_db(std::string_view db_name) noexcept;

    std::optional<DbMeta> get_meta(std::string_view db_name) noexcept;


    const char* set_vec(
        std::optional<DbMeta> meta,
        std::string_view id,
        const std::vector<char>& vector_serialized,
        std::string_view payload) noexcept;

    const char* del_vec(
        std::optional<DbMeta> meta,
        const std::vector<std::string_view>& ids) noexcept;

    std::vector<SearchResult> search_vec(
        std::optional<DbMeta> meta,
        const std::vector<float>& vector, 
        size_t top_k) noexcept;

    const char* search_batch_vec(
        std::optional<DbMeta> meta,
        const std::vector<std::vector<float>>& vectors, 
        size_t top_k) noexcept;
};
