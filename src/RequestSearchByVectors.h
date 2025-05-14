#pragma once
#include <rocksserver/api.h>
#include <memory>
#include "VecDbOpts.h"
#include "VecDb.h"

using namespace RocksServer;

class RequestSearchByVectors final : public RequestBase<ProtocolInPost, ProtocolOut>
{
public:
    RequestSearchByVectors(std::shared_ptr<VecDb> db) noexcept : 
        _db(db)
    {}

    virtual void run(const ProtocolInPost &in, const ProtocolOut &out) noexcept final;

    virtual ~RequestSearchByVectors() = default;
private:

    std::shared_ptr<VecDb> _db;
};
