#pragma once
#include <iostream>
#include <rocksserver/api.h>
 

class VecDbOpts final
{
    // 
    // Key Prefixes
    // 

    // Prefix for storing the database counter
    // storage format:
    // key: <db_counter_key>
    // value: <last db_counter value unsigned integer>
    std::string _db_counter_key = "db_counter";

    // Prefix for storing database metadata
    // storage format:
    // key: <db_key>:<db_name>
    // value: <dim>:<dist_fun_index>:<db_counter_index>
    std::string _db_key = "db";

    // Prefix for storing vectors
    // storage format:
    // key: <vec_key>:<db_counter_index>:<vec_id>
    // value: <vector data serialized>
    std::string _vec_key = "vec";

    // Prefix for storing vector payloads
    // storage format:
    // key: <payload_key>:<db_counter_index>:<vec_id>
    // value: <payload>
    std::string _payload_key = "pld";

    // 
    // Options
    // 

    // The maximum dimension size
    size_t _max_dim = 10000;

    // Default value for top_k parameter
    size_t _top_k = 5;

    // JSON indent
    int _json_indent = 2;

public:

    VecDbOpts(const RocksServer::IniConfigs& cfg) noexcept;

    const std::string& db_counter_key() const & noexcept {return _db_counter_key;}
    const std::string& db_key()         const & noexcept {return _db_key;}
    const std::string& vec_key()        const & noexcept {return _vec_key;}
    const std::string& payload_key()    const & noexcept {return _payload_key;}

    std::string db_counter_key() const && noexcept {return std::move(_db_counter_key);}
    std::string db_key()         const && noexcept {return std::move(_db_key);}
    std::string vec_key()        const && noexcept {return std::move(_vec_key);}
    std::string payload_key()    const && noexcept {return std::move(_payload_key);}

    size_t max_dim()  const noexcept {return _max_dim;}
    size_t top_k()    const noexcept {return _top_k;}
    int json_indent() const noexcept {return _json_indent;}

};
