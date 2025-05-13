#pragma once
#include <string>
#include <optional>

struct DbMeta final
{
    uint dim   = 0u;
    uint dist  = 0u;
    uint index = 0u;

    static std::optional<DbMeta> deserialize(std::string_view data) noexcept;
};

struct SearchData final
{
    std::string id;
    float distance;
    std::string payload;
};
