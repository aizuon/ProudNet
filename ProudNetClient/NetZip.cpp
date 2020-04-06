#include <zlib.h>
#include <vector>

#include "NetZip.h"

namespace Nettention
{
    namespace Proud
    {
        NetMessage NetZip::CompressPacket(NetMessage& message)
        {
            size_t source_length = message.GetLength();
            uLong destination_length = compressBound(source_length);

            byte* destination_data = new byte[destination_length];

            compress2(destination_data, &destination_length, message.GetBuffer(), source_length, Z_DEFAULT_COMPRESSION);

            NetMessage ret(destination_data, destination_length);

            delete[] destination_data;

            return ret;
        }

        byte* NetZip::CompressData(byte* message, size_t inSize, size_t& outSize)
        {
            uLong destination_length = compressBound(inSize);

            byte* destination_data = new byte[destination_length];

            compress2(destination_data, (uLong*)&outSize, message, inSize, Z_DEFAULT_COMPRESSION);

            return destination_data;
        }

        NetMessage NetZip::DecompressPacket(NetMessage& message)
        {
            size_t source_length = message.GetLength();
            byte* destination_data = new byte[source_length];

            uLong destination_length = 0;
            uncompress2(destination_data, &destination_length, message.GetBuffer(), (uLong*)&source_length);

            NetMessage ret(destination_data, destination_length);

            delete[] destination_data;

            return ret;
        }
    }
}