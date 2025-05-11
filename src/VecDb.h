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
};

