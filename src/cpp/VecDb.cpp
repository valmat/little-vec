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

int VecDb::create_db(const std::string& db_name, int db_dim, int dist_index) noexcept
{
    auto key = merge_args(_opts.db_key(), db_name);
    if( _db.keyExist(key) ) {
        return 1;
    }

    if ( !_db.incr( _opts.db_counter_key() ) ) {
        return 2;
    }

    auto db_index = _db.get(_opts.db_counter_key());
    auto val = merge_args(db_dim, dist_index, db_index);

    // std::cout << "key: "  << key << std::endl;
    // std::cout << "val: "  << val << std::endl;

    if (!_db.set(key, val)) {
        return 3;
    }

    return 0;
}
