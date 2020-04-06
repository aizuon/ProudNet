#pragma once
#include "../BaseLib/IPEndPoint.h"

namespace Nettention
{
	namespace Proud
	{
		class NetUtil
		{
		public:
			static bool IsUnicastEndpoint(BaseLib::IPEndPoint& addrPort);
			static bool IsSameHost(BaseLib::IPEndPoint& a, BaseLib::IPEndPoint& b);
			static bool IsSameLan(BaseLib::IPEndPoint& a, BaseLib::IPEndPoint& b);
		};
	}
}