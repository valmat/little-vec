#pragma once

class MergedIterable final
{
public:
    MergedIterable(std::string_view prefix, uint index, const std::vector<std::string_view>& ids) noexcept
        : _prefix(prefix), _index(index), _ids(ids) {}

    class iterator final
    {
        std::string_view _prefix;
        uint _index;
        const std::vector<std::string_view>& _ids;
        size_t _pos;

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = std::string;
        using difference_type = std::ptrdiff_t;
        using pointer = const std::string*;
        using reference = const std::string&;

        iterator(std::string_view prefix, uint index, const std::vector<std::string_view>& ids, size_t pos) noexcept : 
            _prefix(prefix),
            _index(index),
            _ids(ids),
            _pos(pos)
        {}

        value_type operator*() const noexcept
        {
            return merge_args(_prefix, _index, _ids[_pos]);
        }

        iterator& operator++() noexcept
        {
            ++_pos;
            return *this;
        }

        iterator operator++(int) noexcept
        {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const iterator& other) const noexcept
        {
            return _pos == other._pos && &_ids == &other._ids;
        }

        bool operator!=(const iterator& other) const noexcept
        {
            return !(*this == other);
        }
    };

    iterator begin() const noexcept
    {
        return iterator(_prefix, _index, _ids, 0);
    }

    iterator end() const noexcept
    {
        return iterator(_prefix, _index, _ids, _ids.size());
    }

private:
    std::string_view _prefix;
    uint _index;
    const std::vector<std::string_view>& _ids;
};
