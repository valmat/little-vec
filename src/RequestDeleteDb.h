#pragma once
#include <rocksserver/api.h>
#include <memory>
#include "VecDb.h"

using namespace RocksServer;

class RequestDeleteDb final : public RequestBase<ProtocolInPost, ProtocolOut>
{
public:
    RequestDeleteDb(std::shared_ptr<VecDb> db) noexcept : _db(db) {}

    virtual void run(const ProtocolInPost &in, const ProtocolOut &out) noexcept final;

    virtual ~RequestDeleteDb() = default;
private:

    std::shared_ptr<VecDb> _db;
};
