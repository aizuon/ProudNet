#pragma once
#include <cstdint>

namespace BaseLib
{
	class Version
	{
	public:
		Version(int32_t major, int32_t minor, int32_t build, int32_t revision)
		{
			Major = major;
			Minor = minor;
			Build = build;
			Revision = revision;
		}

		Version() = default;

		int32_t Major;
		int32_t Minor;
		int32_t Build;
		int32_t Revision;
	};
}