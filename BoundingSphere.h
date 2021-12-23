#pragma once
#include "CommonD.h"
#include "Frustum.h"

namespace Math
{
	class BoundingSphere
	{
	public:
		XMFLOAT3 Center;
		float Radius;

		ContainmentType Contains(XMFLOAT3 point);
		ContainmentType Contains(Frustum frustum);
		bool Intersects(XMFLOAT4 plane);
	};
}
