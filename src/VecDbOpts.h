#include <iostream>
#include <rocksserver/api.h>

#pragma once
 

class VecDbOpts final
{

public:
    VecDbOpts(const RocksServer::IniConfigs& cfg) noexcept;
};

