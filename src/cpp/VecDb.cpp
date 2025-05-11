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

bool VecDb::create_db(const std::string& db_name, int db_dim, int dist_index) noexcept
{

    auto val = merge_args(db_name, db_dim, dist_index);

    std::cout << "val:"  << val << std::endl;


    return true;
}