#pragma once
#include <memory>
#include "VecDb.h"

using namespace RocksServer;

class RequestCreateDb final : public RequestBase<ProtocolInPost, ProtocolOut>
{
public:
    RequestCreateDb(std::shared_ptr<VecDb> db) noexcept : _db(db) {}

    virtual void run(const ProtocolInPost &in, const ProtocolOut &out) noexcept final;

    virtual ~RequestCreateDb() = default;
private:

    std::shared_ptr<VecDb> _db;
};
