#include <Windows.h>
#include <cstdint>
#include <limits>
#include <string>

#include "NetUtil.h"

namespace Nettention
{
	namespace Proud
	{
		bool NetUtil::IsUnicastEndpoint(BaseLib::IPEndPoint& addrPort)
		{
			bool result = false;
			if (addrPort.Port == 0 || (uint16_t)addrPort.Port == (std::numeric_limits<uint16_t>::max)())
				return result;

			byte* addrBytes = addrPort.GetAddressBytes();

			switch (*(uint32_t*)addrBytes)
			{
			case 0:
			case (std::numeric_limits<uint32_t>::max)():
				result = false;
			default:
				result = true;
			}

			delete[] addrBytes;
			return result;
		}

		bool NetUtil::IsSameHost(BaseLib::IPEndPoint& a, BaseLib::IPEndPoint& b)
		{
			return (a.IP == b.IP);
		}

		bool NetUtil::IsSameLan(BaseLib::IPEndPoint& a, BaseLib::IPEndPoint& b)
		{
			byte* addressBytes1 = a.GetAddressBytes();
			byte* addressBytes2 = b.GetAddressBytes();
			bool result = false;
			if (addressBytes1[0] == addressBytes2[0] && addressBytes1[1] == addressBytes2[1])
				result = (addressBytes1[2] == addressBytes2[2]);

			delete[] addressBytes1;
			delete[] addressBytes2;
			return result;
		}
	}
}