#pragma once
#include <Windows.h>

#include "NetMessage.h"

namespace Nettention
{
    namespace Proud
    {
        class NetZip
        {
        public:
            static NetMessage CompressPacket(NetMessage& message);
            static byte* CompressData(byte* message, size_t inSize, size_t& outSize);
            static NetMessage DecompressPacket(NetMessage& message);
        };
    }
}