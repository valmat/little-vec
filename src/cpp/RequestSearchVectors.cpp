#include <iostream>
#include <string>
#include "RequestSearchVectors.h"
#include "dist_fun.h"
#include "utils_rocks.h"


#include <nlohmann/json.hpp>
using json = nlohmann::json;

void RequestSearchVectors::run(const ProtocolInPost &in, const ProtocolOut &out) noexcept
{
    if(!in.isPost() || in.isEmpty()) {
        out.setCode(422);
        return;
    }
    auto body = in.key().ToStringView();

    std::cout << body << std::endl;

    // out.setStr(body);
}
