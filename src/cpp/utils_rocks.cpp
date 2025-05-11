#include "utils_rocks.h"

void set_error(const ProtocolOut &out, const char* msg) noexcept
{
    out.setStr(R"({"error": ")").setStr(msg).setStr(R"("})").setCode(422);
}
