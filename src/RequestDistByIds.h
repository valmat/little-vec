#pragma once
#include <rocksserver/api.h>
#include <memory>
#include "VecDb.h"

using namespace RocksServer;

class RequestDistByIds final : public RequestBase<ProtocolInPost, ProtocolOut>
{
public:
    RequestDistByIds(std::shared_ptr<VecDb> db) noexcept : 
        _db(db)
    {}

    virtual void run(const ProtocolInPost &in, const ProtocolOut &out) noexcept final;

    virtual ~RequestDistByIds() = default;
private:

    std::shared_ptr<VecDb> _db;
};
