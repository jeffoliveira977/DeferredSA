#pragma once
#include "CommonD.h"
#include "AABB.h"
namespace Math
{
	class BoundingSphere;
	class AABB;
	class Ray
	{
	public:
		XMVECTOR Position;
		XMVECTOR Direction;
	};

	class Frustum
	{
		XMFLOAT3 Corners[8];
		std::vector<XMFLOAT4> Planes;
		AABB BoundBox;
	public:
		XMFLOAT3* GetCorners();
		XMFLOAT4* GetPlanes();
		AABB GetBoundingBox();

		void SetMatrix(XMMATRIX matrix);
		void SetMatrix(XMFLOAT4X4 matrix);

		Ray ComputeIntersectionLine(FXMVECTOR p1, FXMVECTOR p2);
		XMVECTOR ComputeIntersection(FXMVECTOR& plane, Ray& ray);
		XMFLOAT3 IntersectionPoint(XMFLOAT4 a, XMFLOAT4 b, XMFLOAT4 c);

		bool Intersects(AABB aabb);
		bool Intersects(BoundingSphere sphere);
	};
}

