#include <iostream>
#include <rocksserver/api.h>
#include "VecDbOpts.h"

#pragma once
 

class VecDb final
{

public:
    VecDb(const VecDbOpts& opts) noexcept;
};

