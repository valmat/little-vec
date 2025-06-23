#pragma once
#include <rocksserver/api.h>
#include <memory>
#include "VecDb.h"

using namespace RocksServer;

class RequestGetByIds final : public RequestBase<ProtocolInPost, ProtocolOut>
{
public:
    RequestGetByIds(std::shared_ptr<VecDb> db) noexcept : 
        _db(db)
    {}

    virtual void run(const ProtocolInPost &in, const ProtocolOut &out) noexcept final;

    virtual ~RequestGetByIds() = default;
private:

    std::shared_ptr<VecDb> _db;
};
