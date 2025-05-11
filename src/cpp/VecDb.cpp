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

int VecDb::create_db(const std::string& db_name, uint db_dim, uint dist_index) noexcept
{
    if ( db_dim > _opts.max_dim() ) {
        return 1;
    }

    auto key = merge_args(_opts.db_key(), db_name);
    if( _db.keyExist(key) ) {
        return 2;
    }

    if ( !_db.incr( _opts.db_counter_key() ) ) {
        return 3;
    }

    auto db_index = _db.get(_opts.db_counter_key());
    auto val = merge_args(db_dim, dist_index, db_index);

    // std::cout << "key: "  << key << std::endl;
    // std::cout << "val: "  << val << std::endl;

    if (!_db.set(key, val)) {
        return 4;
    }

    return 0;
}

int VecDb::update_db(const std::string& db_name, uint dist_index) noexcept
{
    auto meta = get_meta(db_name);
    if ( !meta.has_value() ) {
        return 1;
    }
    return update_db(db_name, meta, dist_index);
}
int VecDb::update_db(const std::string& db_name, std::optional<DbMeta> meta, uint dist_index) noexcept
{
    if ( meta->dist == dist_index ) {
        return 2;
    }

    meta->dist = dist_index;

    auto key = merge_args(_opts.db_key(), db_name);
    auto val = merge_args(meta->dim, dist_index, meta->index);

    if (!_db.set(key, val)) {
        return 3;
    }

    return 0;
}

int VecDb::delete_db(const std::string& db_name) noexcept
{
    auto meta = get_meta(db_name);
    if ( !meta.has_value() ) {
        return 1;
    }

    auto vec_prefix = merge_args(_opts.vec_key(), meta->index, nullptr);
    auto payload_prefix = merge_args(_opts.payload_key(), meta->index, nullptr);


    Batch batch;
    auto iter(_db.newIter());
    
    // // Iterate over prefixed keys
    for (iter->Seek(vec_prefix); iter->Valid() && iter->key().starts_with(vec_prefix); iter->Next()) {
        if(iter->status().ok()) {
            batch.del(iter->key());
        }
    }
    iter->Reset();
    for (iter->Seek(payload_prefix); iter->Valid() && iter->key().starts_with(payload_prefix); iter->Next()) {
        if(iter->status().ok()) {
            batch.del(iter->key());
        }
    }

    // Apply the delete-batch to the RocksDB
    if(!_db.commit(batch)) {
        return 2;
    }

    auto key = merge_args(_opts.db_key(), db_name);

    if(!_db.del(key)) {
        return 3;
    }

    return 0;
}


std::optional<DbMeta> VecDb::get_meta(std::string_view db_name) noexcept
{
    auto key = merge_args(_opts.db_key(), db_name);
    std::string value;
    if (!_db.keyExist(key, value)) {
        return {};
    }

    return DbMeta::deserialize(value);
}
