#include <fmt/format.h>

#include "IPEndPoint.h"

namespace BaseLib
{
	const IPEndPoint IPEndPoint::Empty = IPEndPoint("0.0.0.0", 0);

	byte* IPEndPoint::GetAddressBytes() const
	{
		byte* ret = new byte[4];

		sscanf_s(IP.c_str(),
			"%hhu.%hhu.%hhu.%hhu",
			&ret[0], &ret[1], &ret[2], &ret[3]);

		return ret;
	}

	std::string IPEndPoint::ToString() const
	{
		return fmt::format("{0}:{1}", IP, Port);
	}

	std::string IPEndPoint::AddressFromUInt32(uint32_t address)
	{
		byte addressBytes[4];

		addressBytes[0] = address & 0x000000FF;
		addressBytes[1] = (address & 0x0000FF00) >> 8;
		addressBytes[2] = (address & 0x00FF0000) >> 16;
		addressBytes[3] = (address & 0xFF000000) >> 24;

		char address_cstr[16];
		snprintf(address_cstr, sizeof(address_cstr),
			"%hhu.%hhu.%hhu.%hhu",
			addressBytes[0], addressBytes[1], addressBytes[2], addressBytes[3]);

		return std::string(address_cstr);
	}

	bool IPEndPoint::operator==(const IPEndPoint& obj) const
	{
		return (IP == obj.IP) && (Port == obj.Port);
	}

	bool IPEndPoint::operator!=(const IPEndPoint& obj) const
	{
		return !(*this == obj);
	}

	bool IPEndPoint::IsEmpty() const
	{
		return (*this == Empty);
	}
}