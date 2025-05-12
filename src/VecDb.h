#pragma once
#include <iostream>
#include <optional>
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

    const char* create_db(const std::string& db_name, uint db_dim, uint dist_index) noexcept;
    const char* update_db(const std::string& db_name, uint dist_index) noexcept;
    const char* update_db(const std::string& db_name, std::optional<DbMeta> meta, uint dist_index) noexcept;
    const char* delete_db(const std::string& db_name) noexcept;

    std::optional<DbMeta> get_meta(std::string_view db_name) noexcept;
};

