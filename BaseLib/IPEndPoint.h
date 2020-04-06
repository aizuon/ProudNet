#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>

namespace BaseLib
{
	class IPEndPoint
	{
	public:
		IPEndPoint(const std::string& ip, uint16_t port)
		{
			IP = ip;
			Port = port;
		}

		IPEndPoint() = default;

		std::string IP;
		uint16_t Port = 0;

		byte* GetAddressBytes() const;

		std::string ToString() const;

		static std::string AddressFromUInt32(uint32_t address);

		static const IPEndPoint Empty;

		bool operator==(const IPEndPoint& obj) const;
		bool operator!=(const IPEndPoint& obj) const;

		bool IsEmpty() const;
	};
}