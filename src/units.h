#pragma once
#include <string>
#include <optional>
#include <limits>

struct DbMeta final
{
    uint dim   = 0u;
    uint dist  = 0u;
    uint index = 0u;

    static std::optional<DbMeta> deserialize(std::string_view data) noexcept;
};

struct SearchResult final
{
    std::string id{};
    float distance = std::numeric_limits<float>::max();
    std::string payload{};
};
