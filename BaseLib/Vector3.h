#pragma once

namespace BaseLib
{
	struct Vector3
	{
		Vector3(float x, float y, float z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
		}

		float x;
		float y;
		float z;
	};
}