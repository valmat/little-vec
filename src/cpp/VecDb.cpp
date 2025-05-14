#include <queue>
#include <utility>
#include <algorithm>

#include "VecDb.h"
#include "utils.h"
#include "dist_fun.h"
#include "merge_args.h"
#include "MaxHeap.h"
#include "MergedIterable.h"

VecDb::VecDb(const VecDbOpts& opts, RocksDBWrapper& db) noexcept :
    _opts(opts),
    _db(db)
{}

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

    return delete_db(db_name, meta);
}

const char* VecDb::delete_db(std::string_view db_name, std::optional<DbMeta> meta) noexcept
{
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

inline
void turn_to_id(std::string &str, char delim = ':', int delim_count = 2) noexcept
{
    size_t pos = 0;
    int count = 0;

    // Find the position after the needed number of delimiters
    while (count < delim_count) {
        pos = str.find(delim, pos);
        if (pos == std::string::npos) {
            // If there are fewer delimiters than expected, clear the string
            str.clear();
            return;
        }
        ++pos; // Move past the found delimiter
        ++count;
    }

    // Remove all characters to the left of the found position
    str.erase(0, pos);
}

std::vector<SearchResult> VecDb::search_vec(
    std::optional<DbMeta> meta,
    const std::vector<float>& vector, 
    size_t top_k) noexcept
{
    auto iter(_db.newIter());

    auto dist_func = DistFun::get_func(meta->dist);
    size_t dim = meta->dim;

    std::vector<float> stored_vec(dim);
    float* data = stored_vec.data();

    MaxHeap max_heap(top_k);

    auto vec_prefix = merge_args(_opts.vec_key(), meta->index, nullptr);
    for (iter->Seek(vec_prefix); iter->Valid() && iter->key().starts_with(vec_prefix); iter->Next()) {
        if (iter->status().ok()) [[likely]] {
            deserialize_buf(iter->value().data(), dim, data);
            float dist = dist_func(vector.data(), data, dim);
            // const float* value = reinterpret_cast<const float*>(iter->value().data());
            // float dist = dist_func(vector.data(), value, dim);
            
            max_heap.update(dist, iter->key().ToStringView());
        }
    }

    max_heap.shrink();
    max_heap.sort();

    for (auto& item : max_heap.container()) {
        turn_to_id(item.id);
        auto payload_key = merge_args(_opts.payload_key(), meta->index, item.id);
        item.payload = _db.get(payload_key);
    }    

    return std::move(max_heap).container();
}

std::vector<std::vector<SearchResult>> VecDb::search_batch_vec(
    std::optional<DbMeta> meta,
    const std::vector<std::vector<float>>& vectors, 
    size_t top_k) noexcept
{
    auto iter(_db.newIter());

    auto dist_func = DistFun::get_func(meta->dist);
    size_t dim = meta->dim;
    size_t batch_size = vectors.size();

    // Create a MaxHeap for each vector in the batch
    std::vector<MaxHeap> heaps;
    heaps.reserve(batch_size);
    for (size_t i = 0; i < batch_size; ++i) {
        heaps.emplace_back(top_k);
    }

    std::vector<float> stored_vec(dim);
    float* data = stored_vec.data();

    auto vec_prefix = merge_args(_opts.vec_key(), meta->index, nullptr);
    for (iter->Seek(vec_prefix); iter->Valid() && iter->key().starts_with(vec_prefix); iter->Next()) {
        if (iter->status().ok()) [[likely]] {
            deserialize_buf(iter->value().data(), dim, data);

            // Calculate the distance to each vector in the batch and update the corresponding MaxHeap
            for (size_t i = 0; i < batch_size; ++i) {
                float dist = dist_func(vectors[i].data(), data, dim);
                heaps[i].update(dist, iter->key().ToStringView());
            }
        }
    }

    // Prepare the results
    std::vector<std::vector<SearchResult>> results(batch_size);
    for (size_t i = 0; i < batch_size; ++i) {
        heaps[i].shrink();
        heaps[i].sort();

        for (auto& item : heaps[i].container()) {
            turn_to_id(item.id);
            auto payload_key = merge_args(_opts.payload_key(), meta->index, item.id);
            item.payload = _db.get(payload_key);
        }

        results[i] = std::move(heaps[i]).container();
    }

    return results;
}
