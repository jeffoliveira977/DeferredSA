#include "BoundingSphere.h"

namespace Math
{
	BoundingSphere::BoundingSphere()
	{

	}

	BoundingSphere::BoundingSphere(XMFLOAT3 center, float radius)
	{
		Center = center;
		Radius = radius;
	}

	void BoundingSphere::CreateFromAABB(AABB aabb)
	{
		XMVECTOR min, max;
		min = XMLoadFloat3(&aabb.Min);
		max = XMLoadFloat3(&aabb.Max);

		XMVECTOR center = (min + max) * 0.5;
		Radius = XMVectorGetX(XMVector3Length(center - max));
		XMStoreFloat3(&Center, center);
	}

	ContainmentType BoundingSphere::Contains(XMFLOAT3 point)
	{
		float sqRadius = Radius * Radius;
		float sqDistance;

		sqDistance = XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&Center) - XMLoadFloat3(&point)));
		if(sqDistance > sqRadius)
		{
			return ContainmentType::DISJOINT;
		}
		else if(sqDistance < sqRadius)
		{
			return ContainmentType::CONTAINS;
		}
		return ContainmentType::INTERSECTS;
	}

	ContainmentType BoundingSphere::Contains(Frustum frustum)
	{
		// Check if all corners are in sphere.
		bool inside = true;

		XMFLOAT3* corners = frustum.GetCorners();

		for(int i = 0; i < 6; i++)
		{
			if(Contains(corners[i]) == ContainmentType::DISJOINT)
			{
				inside = false;
				break;
			}
		}

		if(inside)
		{
			return ContainmentType::CONTAINS;
		}

		// Check if the distance from sphere center to frustrum face is less than radius.
		double dmin = 0;
		if(dmin <= Radius * Radius)
		{
			return ContainmentType::INTERSECTS;
		}

		return ContainmentType::DISJOINT;
	}

	PlaneIntersectionType BoundingSphere::Intersects(XMFLOAT4 plane)
	{
		PlaneIntersectionType result;
		XMVECTOR P = XMLoadFloat4(&plane);
		XMVECTOR V = XMLoadFloat3(&Center);

	/*	float distance = XMVectorGetX(XMPlaneDotCoord(P, V));
		if(distance > Radius)
		{
			return false;
		}
		
		return true;*/

		//float distance = XMVectorGetX(XMPlaneDotCoord(P, V));

		float distance = XMVectorGetX(XMVector3Dot(P, V));
		distance += plane.w;

		if (distance > Radius)
		{
			result = PlaneIntersectionType::FRONT;
		}
		else if (distance < -Radius)
		{
			result = PlaneIntersectionType::BACK;
		}
		else
		{
			result = PlaneIntersectionType::INTERSECTING;
		}

		return result;
	}
}