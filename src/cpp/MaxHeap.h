#pragma once

class MaxHeap final
{
    size_t _top_index = 0;
    float _top_dist = std::numeric_limits<float>::max();
    std::vector<SearchResult> _container;

public:

    MaxHeap(size_t top_k) noexcept :
        _container(top_k)
    {}

    void update(float distance, std::string_view key) noexcept
    {
        if (distance < _top_dist) [[unlikely]]  {

            _container[_top_index].id.assign(key.begin(), key.end());
            _container[_top_index].distance = distance;

            // finding new _top_dist and _top_index
            _top_dist = _container[0].distance;
            _top_index = 0;
            for (size_t i = 1; i < _container.size(); ++i) {
                if (_container[i].distance > _top_dist) {
                    _top_dist = _container[i].distance;
                    _top_index = i;
                }
            }
        }
    }

    std::vector<SearchResult>& container() & noexcept
    {
        return _container;
    }
    std::vector<SearchResult> container() && noexcept
    {
        return std::move(_container);
    }

    void shrink() noexcept
    {
        auto new_end = std::remove_if(_container.begin(), _container.end(), [](const SearchResult& item) {
            return item.id.empty();
        });
        _container.erase(new_end, _container.end());
    }

    void sort() noexcept
    {
        std::sort(_container.begin(), _container.end(), [](const SearchResult& a, const SearchResult& b) {
            return a.distance < b.distance;
        });    
    }

};
