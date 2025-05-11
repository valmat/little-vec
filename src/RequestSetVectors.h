#pragma once
#include <rocksserver/api.h>
#include <memory>
#include "VecDb.h"

using namespace RocksServer;

class RequestSetVectors final : public RequestBase<ProtocolInPost, ProtocolOut>
{
public:
    RequestSetVectors(std::shared_ptr<VecDb> db) noexcept : _db(db) {}

    virtual void run(const ProtocolInPost &in, const ProtocolOut &out) noexcept final;

    virtual ~RequestSetVectors() = default;
private:

    std::shared_ptr<VecDb> _db;
};
