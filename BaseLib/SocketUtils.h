#pragma once
#include <WS2tcpip.h>
#include <Windows.h>
#include <cstdint>

#include "IPEndPoint.h"

namespace BaseLib
{
	namespace Utils
	{
		class SocketUtils
		{
		public:
			static IPEndPoint GetLocalIP(const SOCKET& socket);
			static uint16_t GetLocalPort(const SOCKET& socket);

			static IPEndPoint GetRemoteIP(const SOCKET& socket);
			static uint16_t GetRemotePort(const SOCKET& socket);
		};
	}
}