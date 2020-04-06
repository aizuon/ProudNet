#pragma once

namespace Nettention
{
	namespace Proud
	{
		class SysUtil
		{
		public:
			template<typename T>
			static void Swap(T& lhs, T& rhs)
			{
				T temp = lhs;
				lhs = rhs;
				rhs = temp;
			}
		};
	}
}