#pragma once
#include <rocksserver/api.h>
#include <memory>
#include "VecDb.h"

using namespace RocksServer;

class RequestSearchByVector final : public RequestBase<ProtocolInPost, ProtocolOut>
{
public:
    RequestSearchByVector(std::shared_ptr<VecDb> db) noexcept : 
        _db(db)
    {}

    virtual void run(const ProtocolInPost &in, const ProtocolOut &out) noexcept final;

    virtual ~RequestSearchByVector() = default;
private:

    std::shared_ptr<VecDb> _db;
};
