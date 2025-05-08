#pragma once
#include <iostream>
#include <rocksserver/api.h>
 

class VecDbOpts final
{

public:
    VecDbOpts(const RocksServer::IniConfigs& cfg) noexcept;
};

