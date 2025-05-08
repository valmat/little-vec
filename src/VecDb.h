#pragma once
#include <iostream>
#include <rocksserver/api.h>
#include "VecDbOpts.h"

class VecDb final
{

public:
    VecDb(const VecDbOpts& opts) noexcept;
};

