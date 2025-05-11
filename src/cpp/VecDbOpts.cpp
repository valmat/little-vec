#include "VecDbOpts.h"
 
VecDbOpts::VecDbOpts(const RocksServer::IniConfigs& cfg) noexcept
{
    std::cout << "VecDbOpts::VecDbOpts" << std::endl;

    _db_counter_key = cfg.get("db_counter_key", _db_counter_key);
    _db_key         = cfg.get("db_key",         _db_key);
    _vec_key        = cfg.get("vec_key",        _vec_key);
    _payload_key    = cfg.get("payload_key",    _payload_key);

    _max_dim        = cfg.get("max_dim",        _max_dim);



    std::cout << "db_counter_key: [" << _db_counter_key << "]" << std::endl;
    std::cout << "db_key:         [" << _db_key         << "]" << std::endl;
    std::cout << "vec_key:        [" << _vec_key        << "]" << std::endl;
    std::cout << "payload_key:    [" << _payload_key    << "]" << std::endl;
    std::cout << "max_dim:        [" << _max_dim        << "]" << std::endl;
}
