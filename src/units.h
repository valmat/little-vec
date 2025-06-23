#pragma once
#include <string>
#include <optional>
#include <limits>
#include <vector>

struct DbMeta final
{
    uint dim   = 0u;
    uint dist  = 0u;
    uint index = 0u; // DB index

    static std::optional<DbMeta> deserialize(std::string_view data) noexcept;
};

struct SearchResult final
{
    std::string id{};
    float distance = std::numeric_limits<float>::max();
    std::string payload{};
};

struct DataUnit final
{
    std::string id{};
    std::string payload{};
};

struct DistancesUnit final
{
    std::string id{};
    std::string payload{};
    std::vector<float> distances{};
};
