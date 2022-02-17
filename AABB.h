#pragma once
#include "CommonD.h"

namespace Math
{
	class Frustum;
	class AABB
	{
	public:
		XMFLOAT3 Min;
		XMFLOAT3 Max;
		XMFLOAT3 Corners[8];

		AABB();
		AABB(XMFLOAT3 min, XMFLOAT3 max);
		AABB(XMVECTOR min, XMVECTOR max);
		void CreateFromPoints(XMFLOAT3 points[], size_t count);
		XMFLOAT3* GetCorners();

		void Transform(XMMATRIX matrix);
		void Transform(AABB &aabb, XMMATRIX matrix);
		bool Contains(Frustum frustum);
		bool Intersects(XMFLOAT3 point);
		bool Intersects(XMFLOAT4 plane);
	};
}
