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

    int create_db(const std::string& db_name, uint db_dim, uint dist_index) noexcept;
    std::optional<DbMeta> get_meta(std::string_view db_name) noexcept;
};

