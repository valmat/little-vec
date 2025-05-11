#include "units.h"
#include <charconv>

std::optional<DbMeta> DbMeta::deserialize(std::string_view data) noexcept
{
    DbMeta result;

    auto parse_uint = [](std::string_view str, uint& value) noexcept -> bool {
        auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
        return ec == std::errc();
    };

    size_t pos1 = data.find(':');
    if (pos1 == std::string_view::npos) [[unlikely]] {
        return {};
    }

    size_t pos2 = data.find(':', pos1 + 1);
    if (pos2 == std::string_view::npos) [[unlikely]] {
        return {};
    }

    if (!parse_uint(data.substr(0, pos1), result.dim)) [[unlikely]]
        return {};

    if (!parse_uint(data.substr(pos1 + 1, pos2 - pos1 - 1), result.dist)) [[unlikely]]
        return {};

    if (!parse_uint(data.substr(pos2 + 1), result.index)) [[unlikely]]
        return {};

    return result;
}
