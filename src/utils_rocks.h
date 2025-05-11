#pragma once
#include <rocksserver/api.h>
using namespace RocksServer;

void set_error(const ProtocolOut &out, const char* msg) noexcept;
