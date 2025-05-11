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


std::optional<DbMeta> VecDb::get_meta(std::string_view db_name) noexcept
{
    auto key = merge_args(_opts.db_key(), db_name);
    std::string value;
    if (!_db.keyExist(key, value)) {
        return {};
    }
    
    return DbMeta::deserialize(value);
}