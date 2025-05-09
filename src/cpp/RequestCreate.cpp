#include <rocksserver/api.h>
#include "RequestCreate.h"


void RequestCreate::run(const ProtocolInPost &in, const ProtocolOut &out) noexcept
{
    if(!in.isPost() || in.isEmpty()) {
        out.setCode(422);
        return;
    }
    auto body = in.key().ToStringView();

    std::cout << body << std::endl;

    out.setStr(body);
}