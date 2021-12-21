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
		BoundingSphere();
		BoundingSphere(XMFLOAT3 center, float radius);


		void CreateFromAABB(AABB aabb);
		ContainmentType Contains(XMFLOAT3 point);
		ContainmentType Contains(Frustum frustum);
		PlaneIntersectionType Intersects(XMFLOAT4 plane);
	};
}
