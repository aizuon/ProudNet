#include <limits>

#include "CRC32.h"

namespace BaseLib
{
    uint32_t CRC32::Hash(byte* data, size_t dataSize)
    {
        uint32_t crc = (std::numeric_limits<uint32_t>::max)();
        for (size_t offset = 0; offset < dataSize; offset++)
        {
            crc = (crc >> 8) ^ CRC32Table[(crc ^ data[offset]) & (std::numeric_limits<byte>::max)()];
        }
        crc = ~crc;
        return crc;
    }
}