#include <limits>
#include <type_traits>

#include "FilterTag.h"

namespace Nettention
{
    namespace Proud
    {
        uint16_t FilterTag::CreateFilterTag(uint32_t srcID, uint32_t destID)
        {
            uint16_t filterTag = 0;
            filterTag |= (uint16_t)((uint32_t)(destID | (uint32_t)((uint32_t)srcID << 8)) & (std::numeric_limits<byte>::max)());
            filterTag |= (uint16_t)((uint16_t)srcID << 8);
            return filterTag;
        }

        bool FilterTag::ShouldBeFiltered(uint16_t filterTag, uint32_t srcID, uint32_t destID)
        {
            if (filterTag == 0 && srcID == 1)
                return false;

            if (srcID == 0)
                return false;

            uint16_t correctFilterTag = CreateFilterTag(srcID, destID);

            return filterTag != correctFilterTag;
        }
    }
}