#include "AABB.h"
#include "Frustum.h"

namespace Math
{
	AABB::AABB()
	{}

	AABB::AABB(XMFLOAT3 min, XMFLOAT3 max)
	{
		Min = min;
		Max = max;
	}

	AABB::AABB(XMVECTOR min, XMVECTOR max)
	{
		XMStoreFloat3(&Min, min);
		XMStoreFloat3(&Max, max);
	}

	void AABB::CreateFromPoints(XMFLOAT3 points[], size_t count)
	{
		XMVECTOR minVec = XMVectorSet(points[0].x, points[0].y, points[0].z, 1.0);
		XMVECTOR maxVec = XMVectorSet(points[0].x, points[0].y, points[0].z, 1.0);

		for(int i = 0; i < count; i++)
		{
			XMVECTOR point = XMLoadFloat3(&points[i]);
			minVec = XMVectorMin(minVec, point);
			maxVec = XMVectorMax(maxVec, point);
		}

		XMStoreFloat3(&Min, minVec);
		XMStoreFloat3(&Max, maxVec);
	}

	XMFLOAT3* AABB::GetCorners()
	{
		XMFLOAT3 corners[]
		{
			{Min.x, Min.y, Min.z},
			{Max.x, Min.y, Min.z},
			{Min.x, Max.y, Min.z},
			{Max.x, Max.y, Min.z},
			{Min.x, Min.y, Max.z},
			{Max.x, Min.y, Max.z},
			{Min.x, Max.y, Max.z},
			{Max.x, Max.y, Max.z}
		};
		
		return corners;
	}

	bool AABB::Intersects(XMFLOAT3 point)
	{
		if(point.x < Min.x ||
		   point.x > Max.x ||
		   point.y < Min.y ||
		   point.y > Max.y ||
		   point.z < Min.z ||
		   point.z > Max.z)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	bool AABB::Intersects(XMFLOAT4 plane)
	{
		bool falseNegative = false;
		float d = std::max(Min.x * plane.x, Max.x * plane.x)
				+ std::max(Min.y * plane.y, Max.y * plane.y)
				+ std::max(Min.z * plane.z, Max.z * plane.z)
				+ plane.w;

		return d > 0.0;

		//bool intersect = false;
		//if(plane.x * Min.x + plane.y * Min.y + plane.z * Min.z + plane.w > 0.0)
		//	intersect = true;
		//if(plane.x * Max.x + plane.y * Min.y + plane.z * Min.z + plane.w > 0.0)
		//	intersect = true;
		//if(plane.x * Min.x + plane.y * Max.y + plane.z * Min.z + plane.w > 0.0)
		//	intersect = true;
		//if(plane.x * Max.x + plane.y * Max.y + plane.z * Min.z + plane.w > 0.0)
		//	intersect = true;
		//if(plane.x * Min.x + plane.y * Min.y + plane.z * Max.z + plane.w > 0.0)
		//	intersect = true;
		//if(plane.x * Max.x + plane.y * Min.y + plane.z * Max.z + plane.w > 0.0)
		//	intersect = true;
		//if(plane.x * Min.x + plane.y * Max.y + plane.z * Max.z + plane.w > 0.0)
		//	intersect = true;
		//if(plane.x * Max.x + plane.y * Max.y + plane.z * Max.z + plane.w > 0.0)
		//	intersect = true;

		//return intersect;

		//XMFLOAT3 PP = Min; 
		//if(plane.x >= 0)
		//	PP.x = Max.x;
		//if(plane.y >= 0)
		//	PP.y = Max.y;
		//if(plane.z >= 0)
		//	PP.z = Max.z;

		//XMVECTOR V = XMLoadFloat3(&PP);
		//XMVECTOR P = XMLoadFloat4(&plane);

		//float d = XMVectorGetX(XMPlaneDotCoord(P, V));
		//return d > 0.0;
	}

	bool AABB::Contains(Frustum frustum)
	{
		/* TODO: bad done here need a fix.
		 * Because the question is not if frustum contains box but the reverse and
		 * this is not the same.
		 */
		int i;
		ContainmentType contained;
		auto corners = frustum.GetCorners();

		bool result;

		// First we check if frustum is in box.
		for(i = 0; i < 8; i += 1)
		{
			result = Intersects( corners[i]);
			if(result==false)
			{
				break;
			}
		}

		// This means we checked all the corners and they were all contain or instersect
		if(i == 8)
		{
			contained= ContainmentType::CONTAINS;
		}

		// If i is not equal to zero, we can fastpath and say that this box intersects
		if(i != 0)
		{
			contained =ContainmentType::INTERSECTS;
		}


		/* If we get here, it means the first (and only) point we checked was
		 * actually contained in the frustum. So we assume that all other points
		 * will also be contained. If one of the points is disjoint, we can
		 * exit immediately saying that the result is Intersects
		 */
		i += 1;
		for(; i < 8; i += 1)
		{
			result = Intersects( corners[i]);
			if(result==false)
			{
				contained = ContainmentType::INTERSECTS;
			}

		}


		return contained == ContainmentType::CONTAINS;

		/* If we get here, then we know all the points were actually contained,
		 * therefore result is Contains.
		 */
		return false;
	}

}
