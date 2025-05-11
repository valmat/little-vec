#pragma once
#include <rocksserver/api.h>
#include <memory>
#include "VecDb.h"

using namespace RocksServer;

class RequestDelVectors final : public RequestBase<ProtocolInPost, ProtocolOut>
{
public:
    RequestDelVectors(std::shared_ptr<VecDb> db) noexcept : _db(db) {}

    virtual void run(const ProtocolInPost &in, const ProtocolOut &out) noexcept final;

    virtual ~RequestDelVectors() = default;
private:

    std::shared_ptr<VecDb> _db;
};
