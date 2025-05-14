#include "VecDbOpts.h"
 
VecDbOpts::VecDbOpts(const RocksServer::IniConfigs& cfg) noexcept
{
    _db_counter_key = cfg.get("db_counter_key", _db_counter_key);
    _db_key         = cfg.get("db_key",         _db_key);
    _vec_key        = cfg.get("vec_key",        _vec_key);
    _payload_key    = cfg.get("payload_key",    _payload_key);

    _max_dim        = cfg.get("max_dim",        _max_dim);
    _top_k          = cfg.get("top_k",          _top_k);
    _json_indent    = cfg.get("json_indent",    _json_indent);
}
