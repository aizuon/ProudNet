#include <sstream>
#include <iomanip>
#include <algorithm>

#include "ByteArrayUtils.h"

namespace BaseLib
{
    namespace Utils
    {
        std::string ByteArrayUtils::ToHexString(byte* byteArray, size_t length)
        {
            std::stringstream ss;
            ss << std::hex;

            for (size_t i = 0; i < length; i++)
                ss << std::setw(2) << std::setfill('0') << (int)byteArray[i] << " ";

            std::string output = ss.str();
            std::transform(output.begin(), output.end(), output.begin(), std::toupper);

            return output;
        }
    }
}