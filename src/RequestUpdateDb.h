#pragma once
#include <rocksserver/api.h>
#include <memory>
#include "VecDb.h"

using namespace RocksServer;

class RequestUpdateDb final : public RequestBase<ProtocolInPost, ProtocolOut>
{
public:
    RequestUpdateDb(std::shared_ptr<VecDb> db) noexcept : _db(db) {}

    virtual void run(const ProtocolInPost &in, const ProtocolOut &out) noexcept final;

    virtual ~RequestUpdateDb() = default;
private:

    std::shared_ptr<VecDb> _db;
};
