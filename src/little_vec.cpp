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
#include "RequestSearchByVectors.h"
// #include "RequestSearchByVector.h"

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
        .bind("/vecdb/create",         new RequestCreateDb(vec_db))
        .bind("/vecdb/update",         new RequestUpdateDb(vec_db))
        .bind("/vecdb/delete",         new RequestDeleteDb(vec_db))
        .bind("/vectors/set",          new RequestSetVectors(vec_db))
        .bind("/vectors/delete",       new RequestDelVectors(vec_db))
        // .bind("/vectors/search",       new RequestSearchByVector(opts, vec_db))
        .bind("/vectors/batch_search", new RequestSearchByVectors(opts, vec_db))
        ;
}
