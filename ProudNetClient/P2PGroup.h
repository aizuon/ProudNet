#pragma once
#include <cstdint>
#include <concurrent_unordered_map.h>

#include "P2PMember.h"

namespace Nettention
{
    namespace Proud
    {
        class P2PGroup
        {
        public:
            uint32_t HostId = 0;
            concurrency::concurrent_unordered_map<uint32_t, P2PMember*> P2PMembers;
        };
    }
}