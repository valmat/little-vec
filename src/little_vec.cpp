/**
 *  common.cpp
 *
 *  Simple example of RocksServer plugin
 *
 *  @author valmat <ufabiz@gmail.com>
 *  @github https://github.com/valmat/rocksserver
 */

#include <iostream>
#include <memory>
#include <rocksserver/api.h>
#include "VecDbOpts.h"
#include "VecDb.h"

#include "RequestCreateDb.h"
#include "RequestUpdateDb.h"
#include "RequestDeleteDb.h"
#include "RequestSetVectors.h"
#include "RequestDelVectors.h"
#include "RequestSearchVectors.h"

using namespace RocksServer;

/*
 * Create plugin
 * 
 * You can use one of the following forms of macro PLUGIN with one, two or three arguments:
 *
 * PLUGIN(Extension extension, RocksDBWrapper& db, const IniConfigs& cfg)
 * PLUGIN(Extension extension, RocksDBWrapper& db)
 * PLUGIN(Extension extension)
 *
 * The macro `PLUGIN` is defined in rocksserver/api.h. 
 * Instead of using the macro, you can write
 * `extern "C" void plugin(...)` if you like
 *
 * @param extension  object of Extension
 * @param db         wrapped object of RocksDB
 * @param cfg        Reference to configuration settings
 */

PLUGIN(Extension extension, RocksDBWrapper& db, const RocksServer::IniConfigs& cfg)
{

    VecDbOpts opts(cfg);
    auto vec_db = std::make_shared<VecDb>(opts, db);

    extension
        .bind("/create_vecdb",   new RequestCreateDb(vec_db))
        .bind("/update_vecdb",   new RequestUpdateDb(vec_db))
        .bind("/delete_vecdb",   new RequestDeleteDb(vec_db))
        .bind("/set_vectors",    new RequestSetVectors(vec_db))
        .bind("/delete_vectors", new RequestDelVectors(vec_db))
        .bind("/search_vectors", new RequestSearchVectors(vec_db))
        ;
}
