#include "Collider.h"


float Collider::EPSILON = 0.0f;

Collider::Frustum Collider::ComputeFrustumFromViewProj(XMFLOAT4X4 matrix)
{
	
	Frustum frustum;

	/*XMVECTOR vertices[8] =
	{
		{-1.0f, +1.0f, +0.0f, +1.0f},
		{+1.0f, +1.0f, +0.0f, +1.0f},
		{+1.0f, -1.0f, +0.0f, +1.0f},
		{-1.0f, -1.0f, +0.0f, +1.0f},
		{-1.0f, +1.0f, +1.0f, +1.0f},
		{+1.0f, +1.0f, +1.0f, +1.0f},
		{+1.0f, -1.0f, +1.0f, +1.0f},
		{-1.0f, -1.0f, +1.0f, +1.0f},
	};

	XMVECTOR det;
	XMMATRIX invM = XMMatrixInverse(&det, XMLoadFloat4x4(&M));

	for (int i = 0; i < 8; i++)
	{

		vertices[i] = XMVector3Transform(vertices[i], invM);
		 vertices[i] = vertices[i] * XMVectorReciprocal(XMVectorSplatW(vertices[i]));
	}

	XMStoreFloat4(&frustum.Planes[0], XMPlaneFromPoints(vertices[0], vertices[4], vertices[7]));
	XMStoreFloat4(&frustum.Planes[1], XMPlaneFromPoints(vertices[0], vertices[1], vertices[5]));
	XMStoreFloat4(&frustum.Planes[2], XMPlaneFromPoints(vertices[1], vertices[6], vertices[5]));
	XMStoreFloat4(&frustum.Planes[3], XMPlaneFromPoints(vertices[2], vertices[3], vertices[7]));
	XMStoreFloat4(&frustum.Planes[4], XMPlaneFromPoints(vertices[0], vertices[3], vertices[2]));
	XMStoreFloat4(&frustum.Planes[5], XMPlaneFromPoints(vertices[4], vertices[5], vertices[6]));

	return frustum;*/
	

	frustum.Planes[0].x = -matrix._13;
	frustum.Planes[0].y = -matrix._23;
	frustum.Planes[0].z = -matrix._33;
	frustum.Planes[0].w = -matrix._43;

	frustum.Planes[1].x = -matrix._14 + matrix._13;
	frustum.Planes[1].y = -matrix._24 + matrix._23;
	frustum.Planes[1].z = -matrix._34 + matrix._33;
	frustum.Planes[1].w = -matrix._44 + matrix._43;

	frustum.Planes[2].x = -matrix._14 - matrix._11;
	frustum.Planes[2].y = -matrix._24 - matrix._21;
	frustum.Planes[2].z = -matrix._34 - matrix._31;
	frustum.Planes[2].w = -matrix._44 - matrix._41;

	frustum.Planes[3].x = -matrix._14 + matrix._11;
	frustum.Planes[3].y = -matrix._24 + matrix._21;
	frustum.Planes[3].z = -matrix._34 + matrix._31;
	frustum.Planes[3].w = -matrix._44 + matrix._41;

	frustum.Planes[4].x = -matrix._14 + matrix._12;
	frustum.Planes[4].y = -matrix._24 + matrix._22;
	frustum.Planes[4].z = -matrix._34 + matrix._32;
	frustum.Planes[4].w = -matrix._44 + matrix._42;

	frustum.Planes[5].x = -matrix._14 - matrix._12;
	frustum.Planes[5].y = -matrix._24 - matrix._22;
	frustum.Planes[5].z = -matrix._34 - matrix._32;
	frustum.Planes[5].w = -matrix._44 - matrix._42;

	// Normalize the plane equations.
	for(int i = 0; i < 6; ++i)
	{
		XMVECTOR v = XMPlaneNormalize(XMLoadFloat4(&frustum.Planes[i]));
		XMStoreFloat4(&frustum.Planes[i], v);
	}

	return frustum;
}

bool Collider::IsIntersectSphereWithFrustum(Sphere sphere, Frustum frustum)
{
	for(int i = 0; i < 6; i++)
	{

		XMVECTOR p = XMLoadFloat4(&frustum.Planes[i]);
		XMVECTOR v = XMLoadFloat3(&sphere.Center);

		float dist = XMVectorGetX(XMPlaneDotCoord(p, v));

		if(-dist > sphere.Radius + EPSILON) return false;

	}

	return true;
}

bool Collider::IsIntersectPointWithFrustum(FXMVECTOR point, Frustum frustum)
{

	for(int i = 0; i < 6; i++)
	{

		XMVECTOR p = XMLoadFloat4(&frustum.Planes[i]);

		float dist = XMVectorGetX(XMPlaneDotCoord(p, point));

		if(dist < 0.0f) return false;
	}


	return true;
}

Collider::Ray Collider::ComputePickRayFromScreen(int x, int y, int width, int height, XMFLOAT4X4 P)
{
	float vx = (2.0 * x / width - 1.0) / P(0, 0);
	float vy = (-2.0 * y / height + 1.0) / P(1, 1);

	Ray ray;
	ray.Origin = XMFLOAT3(0.0f, 0.0f, 0.0f);
	ray.Dir = XMFLOAT3(vx, vy, 1.0f);

	return ray;
}

bool Collider::IsIntersectSphereWithRay(Ray ray, Sphere sphere)
{

	XMVECTOR dir = XMLoadFloat3(&ray.Dir);
	XMVECTOR origin = XMLoadFloat3(&ray.Origin);

	float rad = sphere.Radius;
	XMVECTOR center = XMLoadFloat3(&sphere.Center);

	XMVECTOR m = origin - center;
	float b = XMVectorGetX(XMVector3Dot(dir, m));
	float c = XMVectorGetX(XMVector3Dot(m, m)) - rad * rad;

	float D = b * b - c;

	if(D < 0) return false;

	float t1 = -b + sqrt(b * b - c);
	float t2 = -b - sqrt(b * b - c);

	if(t1 > 0) return true;
	if(t2 > 0) return true;


	return false;
}

bool Collider::IsIntersectTriangleWithRay(Ray ray, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, float& t)
{
	XMVECTOR dir = XMLoadFloat3(&ray.Dir); 
	XMVECTOR origin = XMLoadFloat3(&ray.Origin);


	XMVECTOR e1 = v1 - v0;
	XMVECTOR e2 = v2 - v0;

	XMVECTOR m = origin - v0;

	XMVECTOR x1 = XMVector3Cross(m, e1);
	XMVECTOR x2 = XMVector3Cross(dir, e2);

	float _t = XMVectorGetX(XMVector3Dot(e2, x1) / XMVector3Dot(e1, x2));
	float u = XMVectorGetX(XMVector3Dot(m, x2) / XMVector3Dot(e1, x2));
	float v = XMVectorGetX(XMVector3Dot(dir, x1) / XMVector3Dot(e1, x2));

	if(u >= 0 && v >= 0 && 1 >= u + v)
	{
		t = _t;
		return true;
	}

	return false;
}

bool Collider::IsIntersectSphereWithSphere(Sphere s1, Sphere s2)
{
	XMVECTOR c1 = XMLoadFloat3(&s1.Center);
	XMVECTOR c2 = XMLoadFloat3(&s2.Center);

	float dist = XMVectorGetX(XMVector3Length(c1 - c2));

	if(dist <= s1.Radius + s2.Radius)
	{
		return true;
	}

	return false;
}

bool Collider::IsIntersectSphereWithPoint(Sphere s, XMVECTOR p)
{

	XMVECTOR c = XMLoadFloat3(&s.Center); 

	float dist = XMVectorGetX(XMVector3Length(p - c));

	if(s.Radius >= dist)
		return true;

	return false;
}


Collider::Sphere Collider::ComputeSphere(FXMVECTOR v1, FXMVECTOR v2, FXMVECTOR v3, CXMVECTOR v4)
{
	Sphere s;
	s.Radius = XMVectorGetX(XMVector3Length(v4 - v1)) / 2.0f;
	XMStoreFloat3(&s.Center, (v1 + v2 + v3 + v4) / 4.0f);

	return s;
}

bool Collider::IsIntersectAABBWithFrustum(AABB aabb, Frustum frustum)
{
	XMFLOAT3 PP;

	PlaneIntersectionType result;
	for(int i = 0; i < 6; i++)
	{
		XMFLOAT3 positive = aabb.Min;
		XMFLOAT3 negative = aabb.Max;

		if(frustum.Planes[i].x >= 0.0)
		{
			positive.x = aabb.Max.x;
			negative.x = aabb.Min.x;
		}
		if(frustum.Planes[i].y >= 0.0)
		{
			positive.y = aabb.Max.y;
			negative.y = aabb.Min.y;
		}
		if(frustum.Planes[i].z >= 0.0)
		{
			positive.z = aabb.Max.z;
			negative.z = aabb.Min.z;
		}

		XMVECTOR V = XMLoadFloat3(&positive);
		XMVECTOR P = XMLoadFloat4(&frustum.Planes[i]);
		float d = XMVectorGetX(XMPlaneDotCoord(P, V));

		if(d < 0.0)
		{
			result = PlaneIntersectionType::BACK;
			break;
		}

		V = XMLoadFloat3(&negative);
		d = XMVectorGetX(XMPlaneDotCoord(P, V));

		if(d > 0.0)
		{
			result = PlaneIntersectionType::FRONT; 
			break;
		}

	}
	bool flag = false;
	switch(result)
	{
		case PlaneIntersectionType::FRONT:
			return false;

		case PlaneIntersectionType::INTERSECTING:
			flag = true;
			break;
	}
	ContainmentType type = flag ? ContainmentType::INTERSECTS : ContainmentType::CONTAINS;

	return type != ContainmentType::DISJOINT;
}

bool Collider::IsIntersectAABBWithRay(AABB aabb, Ray ray)
{

	float tStart = 0.0f;
	float tEnd = 9999999999.0f;

	if(ray.Dir.x == 0.0)
	{
		if(ray.Origin.x < aabb.Min.x || ray.Origin.x > aabb.Max.x)
			return false;
	}
	else
	{
		float tStartX = (aabb.Min.x - ray.Origin.x) / ray.Dir.x;
		float tEndX = (aabb.Max.x - ray.Origin.x) / ray.Dir.x;
		if(tStartX > tEndX)
			std::swap(tStartX, tEndX);

		if(tStart < tStartX) tStart = tStartX;
		tEnd = tEndX;

		if(tStart > tEnd) return false;
	}

	if(ray.Dir.y == 0.0)
	{
		if(ray.Origin.y < aabb.Min.y || ray.Origin.y > aabb.Max.y)
			return false;
	}
	else
	{
		float tStartY = (aabb.Min.y - ray.Origin.y) / ray.Dir.y;
		float tEndY = (aabb.Max.y - ray.Origin.y) / ray.Dir.y;
		if(tStartY > tEndY)
			std::swap(tStartY, tEndY);

		if(tStart < tStartY) tStart = tStartY;
		if(tEnd > tEndY) tEnd = tEndY;

		if(tStart > tEnd) return false;
	}

	if(ray.Dir.z == 0.0)
	{
		if(ray.Origin.z < aabb.Min.z || ray.Origin.z > aabb.Max.z)
			return false;
	}
	else
	{
		float tStartZ = (aabb.Min.z - ray.Origin.z) / ray.Dir.z;
		float tEndZ = (aabb.Max.z - ray.Origin.z) / ray.Dir.z;
		if(tStartZ > tEndZ)
			std::swap(tStartZ, tEndZ);

		if(tStart < tStartZ) tStart = tStartZ;
		if(tEnd > tEndZ) tEnd = tEndZ;

		if(tStart > tEnd) return false;
	}


	return true;
}

bool Collider::IsIntersectAABBWithSphere(AABB aabb, Sphere s)
{
	float d = 0.0f;

	if(s.Center.x < aabb.Min.x)
	{
		d += (aabb.Min.x - s.Center.x) * (aabb.Min.x - s.Center.x);
	}
	else if(s.Center.x > aabb.Max.x)
	{
		d += (aabb.Max.x - s.Center.x) * (aabb.Max.x - s.Center.x);
	}

	if(s.Center.y < aabb.Min.y)
	{
		d += (aabb.Min.y - s.Center.y) * (aabb.Min.y - s.Center.y);
	}
	else if(s.Center.y > aabb.Max.y)
	{
		d += (aabb.Max.y - s.Center.y) * (aabb.Max.y - s.Center.y);
	}

	if(s.Center.z < aabb.Min.z)
	{
		d += (aabb.Min.z - s.Center.z) * (aabb.Min.z - s.Center.z);
	}
	else if(s.Center.z > aabb.Max.z)
	{
		d += (aabb.Max.z - s.Center.z) * (aabb.Max.z - s.Center.z);
	}

	if(d > s.Radius* s.Radius)
		return false;


	return true;
}