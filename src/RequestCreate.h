#pragma once

using namespace RocksServer;

class RequestCreate final : public RequestBase<ProtocolInPost, ProtocolOut>
{
public:
    RequestCreate(RocksDBWrapper &rdb) noexcept : db(rdb) {}

    virtual void run(const ProtocolInPost &in, const ProtocolOut &out) noexcept final;

    virtual ~RequestCreate() = default;
private:
    RocksDBWrapper& db;
};
