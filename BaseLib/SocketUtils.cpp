#pragma comment(lib,"ws2_32.lib")

#include "SocketUtils.h"

namespace BaseLib
{
	namespace Utils
	{
		IPEndPoint SocketUtils::GetLocalIP(const SOCKET& socket)
		{
            if (socket != INVALID_SOCKET)
            {
                sockaddr_in localAddress;
                socklen_t addressLength = sizeof(sockaddr_in);
                getsockname(socket, (sockaddr*)&localAddress, &addressLength);
                char str[INET_ADDRSTRLEN];
                return BaseLib::IPEndPoint(inet_ntop(AF_INET, &localAddress.sin_addr, str, INET_ADDRSTRLEN), ntohs(localAddress.sin_port));
            }

            return BaseLib::IPEndPoint::Empty;
		}

        uint16_t SocketUtils::GetLocalPort(const SOCKET& socket)
        {
            if (socket != INVALID_SOCKET)
            {
                sockaddr_in localAddress;
                socklen_t addressLength = sizeof(sockaddr_in);
                getsockname(socket, (sockaddr*)&localAddress, &addressLength);
                return ntohs(localAddress.sin_port);
            }

            return 0;
        }

        IPEndPoint SocketUtils::GetRemoteIP(const SOCKET& socket)
        {
            if (socket != INVALID_SOCKET)
            {
                sockaddr_in remoteAddress;
                socklen_t addressLength = sizeof(sockaddr_in);
                getpeername(socket, (sockaddr*)&remoteAddress, &addressLength);
                char str[INET_ADDRSTRLEN];
                return BaseLib::IPEndPoint(inet_ntop(AF_INET, &remoteAddress.sin_addr, str, INET_ADDRSTRLEN), ntohs(remoteAddress.sin_port));
            }

            return BaseLib::IPEndPoint::Empty;
        }

        uint16_t SocketUtils::GetRemotePort(const SOCKET& socket)
        {
            if (socket != INVALID_SOCKET)
            {
                sockaddr_in remoteAddress;
                socklen_t addressLength = sizeof(sockaddr_in);
                getpeername(socket, (sockaddr*)&remoteAddress, &addressLength);
                return ntohs(remoteAddress.sin_port);
            }

            return 0;
        }

	}
}