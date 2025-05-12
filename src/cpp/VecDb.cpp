#include "VecDb.h"
#include "dist_fun.h"
#include "merge_args.h"


VecDb::VecDb(const VecDbOpts& opts, RocksDBWrapper& db) noexcept :
    _opts(opts),
    _db(db)
{
    std::cout << "VecDb::VecDb" << std::endl;

    // std::cout << "_opts.db_key():"  << _opts.db_key() << std::endl;
    // std::cout << "_opts.vec_key():" << std::move(_opts).vec_key() << std::endl;

}

const char* VecDb::create_db(std::string_view db_name, uint db_dim, uint dist_index) noexcept
{
    if ( db_dim > _opts.max_dim() ) [[unlikely]] {
        return "The maximum dimension size has been exceeded.";
    }

    auto key = merge_args(_opts.db_key(), db_name);
    if ( _db.keyExist(key) ) [[unlikely]] {
        return "Data Base already exists.";
    }

    if ( !_db.incr( _opts.db_counter_key() ) ) [[unlikely]] {
        return "Internal RocksDB error: couldn't increment counter.";
    }

    auto db_index = _db.get(_opts.db_counter_key());
    auto val = merge_args(db_dim, dist_index, db_index);

    // std::cout << "key: "  << key << std::endl;
    // std::cout << "val: "  << val << std::endl;

    if (!_db.set(key, val)) [[unlikely]] {
        return "Internal RocksDB error: couldn't set meta data.";
    }

    return nullptr;
}

const char* VecDb::update_db(std::string_view db_name, uint dist_index) noexcept
{
    auto meta = get_meta(db_name);
    if ( !meta.has_value() ) [[unlikely]] {
        return "Data Base doesn't exist.";
    }
    return update_db(db_name, meta, dist_index);
}
const char* VecDb::update_db(std::string_view db_name, std::optional<DbMeta> meta, uint dist_index) noexcept
{
    if ( meta->dist == dist_index ) [[unlikely]] {
        return "Nothing changed.";
    }

    meta->dist = dist_index;

    auto key = merge_args(_opts.db_key(), db_name);
    auto val = merge_args(meta->dim, dist_index, meta->index);

    if (!_db.set(key, val)) [[unlikely]] {
        return "Internal RocksDB error: couldn't set meta data";
    }

    return 0;
}

const char* VecDb::delete_db(std::string_view db_name) noexcept
{
    auto meta = get_meta(db_name);
    if ( !meta.has_value() ) [[unlikely]] {
        return "Data Base doesn't exist.";
    }

    auto vec_prefix = merge_args(_opts.vec_key(), meta->index, nullptr);
    auto payload_prefix = merge_args(_opts.payload_key(), meta->index, nullptr);


    Batch batch;
    auto iter(_db.newIter());
    
    // // Iterate over prefixed keys
    for (iter->Seek(vec_prefix); iter->Valid() && iter->key().starts_with(vec_prefix); iter->Next()) {
        if (iter->status().ok()) [[likely]] {
            batch.del(iter->key());
        }
    }
    iter->Reset();
    for (iter->Seek(payload_prefix); iter->Valid() && iter->key().starts_with(payload_prefix); iter->Next()) {
        if (iter->status().ok()) [[likely]] {
            batch.del(iter->key());
        }
    }

    // Apply the delete-batch to the RocksDB
    if (!_db.commit(batch)) [[unlikely]] {
        return "Internal RocksDB error: couldn't commit delete batch.";
    }

    auto key = merge_args(_opts.db_key(), db_name);

    if (!_db.del(key)) [[unlikely]] {
        return "Internal RocksDB error: couldn't delete meta data.";
    }

    return nullptr;
}


std::optional<DbMeta> VecDb::get_meta(std::string_view db_name) noexcept
{
    auto key = merge_args(_opts.db_key(), db_name);
    std::string value;
    if (!_db.keyExist(key, value)) [[unlikely]] {
        return {};
    }

    return DbMeta::deserialize(value);
}

const char* VecDb::set_vec(
    std::optional<DbMeta> meta,
    std::string_view id,
    const std::vector<char>& vector_serialized,
    std::string_view payload) noexcept
{
    auto key = merge_args(_opts.vec_key(), meta->index, id);
    rocksdb::Slice value(vector_serialized.data(), vector_serialized.size());
    if (!_db.set(key, value)) [[unlikely]] {
        return "Internal RocksDB error: couldn't set vector data.";
    }

    key = merge_args(_opts.payload_key(), meta->index, id);
    if (!_db.set(key, payload)) [[unlikely]] {
        return "Internal RocksDB error: couldn't set payload data.";
    }

    return nullptr;
}


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

const char* VecDb::del_vec(
    std::optional<DbMeta> meta,
    const std::vector<std::string_view>& ids) noexcept
{
    Batch batch;
    batch.del(MergedIterable(_opts.vec_key(), meta->index, ids));
    batch.del(MergedIterable(_opts.payload_key(), meta->index, ids));

    // Apply the delete-batch to the RocksDB
    if (!_db.commit(batch)) [[unlikely]] {
        return "Internal RocksDB error: couldn't commit delete batch.";
    }

    return nullptr;
}
