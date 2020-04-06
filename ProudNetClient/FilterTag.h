#pragma once
#include <Windows.h>
#include <cstdint>

namespace Nettention
{
    namespace Proud
    {
        class FilterTag
        {
        public:
            static uint16_t CreateFilterTag(uint32_t srcID, uint32_t destID);
            static bool ShouldBeFiltered(uint16_t filterTag, uint32_t srcID, uint32_t destID);
        };
    }
}