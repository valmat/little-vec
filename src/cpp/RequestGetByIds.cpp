#include <vector>
#include "RequestGetByIds.h"
#include "utils_rocks.h"
#include "req_validator.h"

void RequestGetByIds::run(const ProtocolInPost &in, const ProtocolOut &out) noexcept
{
    out.setStr("test");
}
