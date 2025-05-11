#pragma once
#include <iostream>
#include <rocksserver/api.h>
#include "VecDbOpts.h"

using namespace RocksServer;

class VecDb final
{
    VecDbOpts _opts;
    RocksDBWrapper& _db;

public:
    VecDb(const VecDbOpts& opts, RocksDBWrapper& db) noexcept;

    bool create_db(const std::string& db_name, int db_dim, int dist_index) noexcept;

    


};

