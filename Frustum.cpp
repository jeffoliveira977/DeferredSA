#include "Frustum.h"
#include "AABB.h"
#include "BoundingSphere.h"

namespace Math
{
	XMFLOAT3* Frustum::GetCorners()
	{
		return Corners;
	}

	XMFLOAT4* Frustum::GetPlanes()
	{
		return Planes.data();
	}

	AABB Frustum::GetBoundingBox()
	{
		return BoundBox;
	}

	void Frustum::SetMatrix(XMMATRIX matrix, bool rh)
	{
		XMFLOAT4X4 m4x4;
		XMStoreFloat4x4(&m4x4, matrix);
		SetMatrix(m4x4, rh);
	}

	void Frustum::SetMatrix(XMFLOAT4X4 matrix, bool rh)
	{
		Planes.clear();

		XMFLOAT4 planes[6];
		planes[0].x = matrix.m[0][3] + matrix.m[0][0];
		planes[0].y = matrix.m[1][3] + matrix.m[1][0];
		planes[0].z = matrix.m[2][3] + matrix.m[2][0];
		planes[0].w = matrix.m[3][3] + matrix.m[3][0];
		planes[1].x = matrix.m[0][3] - matrix.m[0][0];
		planes[1].y = matrix.m[1][3] - matrix.m[1][0];
		planes[1].z = matrix.m[2][3] - matrix.m[2][0];
		planes[1].w = matrix.m[3][3] - matrix.m[3][0];
		planes[2].x = matrix.m[0][3] + matrix.m[0][1];
		planes[2].y = matrix.m[1][3] + matrix.m[1][1];
		planes[2].z = matrix.m[2][3] + matrix.m[2][1];
		planes[2].w = matrix.m[3][3] + matrix.m[3][1];
		planes[3].x = matrix.m[0][3] - matrix.m[0][1];
		planes[3].y = matrix.m[1][3] - matrix.m[1][1];
		planes[3].z = matrix.m[2][3] - matrix.m[2][1];
		planes[3].w = matrix.m[3][3] - matrix.m[3][1];
		planes[4].x = matrix.m[0][3] + matrix.m[0][2];
		planes[4].y = matrix.m[1][3] + matrix.m[1][2];
		planes[4].z = matrix.m[2][3] + matrix.m[2][2];
		planes[4].w = matrix.m[3][3] + matrix.m[3][2];
		planes[5].x = matrix.m[0][3] - matrix.m[0][2];
		planes[5].y = matrix.m[1][3] - matrix.m[1][2];
		planes[5].z = matrix.m[2][3] - matrix.m[2][2];
		planes[5].w = matrix.m[3][3] - matrix.m[3][2];

		for (int i = 0; i < 6; i++)
		{
			XMVECTOR vector = XMVectorSet(planes[i].x, planes[i].y, planes[i].z, planes[i].w);
			XMVECTOR length = XMVector3Length(vector);

			XMStoreFloat4(&planes[i], XMVectorDivide(vector, length));
		}

		for (int i = 0; i < 6; i++)
			Planes.push_back(planes[i]);

#if 0
		Corners[0] = IntersectionPoint(Planes[0], Planes[2], Planes[4]);
		Corners[1] = IntersectionPoint(Planes[0], Planes[3], Planes[4]);
		Corners[2] = IntersectionPoint(Planes[0], Planes[3], Planes[5]);
		Corners[3] = IntersectionPoint(Planes[0], Planes[2], Planes[5]);
		Corners[4] = IntersectionPoint(Planes[1], Planes[2], Planes[4]);
		Corners[5] = IntersectionPoint(Planes[1], Planes[3], Planes[4]);
		Corners[6] = IntersectionPoint(Planes[1], Planes[3], Planes[5]);
		Corners[7] = IntersectionPoint(Planes[1], Planes[2], Planes[5]);
#else
		Ray ray = ComputeIntersectionLine(XMLoadFloat4(&Planes[0]), XMLoadFloat4(&Planes[2]));
		XMStoreFloat3(&Corners[0], ComputeIntersection(XMLoadFloat4(&Planes[4]), ray));
		XMStoreFloat3(&Corners[3], ComputeIntersection(XMLoadFloat4(&Planes[5]), ray));

		ray = ComputeIntersectionLine(XMLoadFloat4(&Planes[3]), XMLoadFloat4(&Planes[0]));
		XMStoreFloat3(&Corners[1], ComputeIntersection(XMLoadFloat4(&Planes[4]), ray));
		XMStoreFloat3(&Corners[2], ComputeIntersection(XMLoadFloat4(&Planes[5]), ray));

		ray = ComputeIntersectionLine(XMLoadFloat4(&Planes[2]), XMLoadFloat4(&Planes[1]));
		XMStoreFloat3(&Corners[4], ComputeIntersection(XMLoadFloat4(&Planes[4]), ray));
		XMStoreFloat3(&Corners[7], ComputeIntersection(XMLoadFloat4(&Planes[5]), ray));

		ray = ComputeIntersectionLine(XMLoadFloat4(&Planes[1]), XMLoadFloat4(&Planes[3]));
		XMStoreFloat3(&Corners[5], ComputeIntersection(XMLoadFloat4(&Planes[4]), ray));
		XMStoreFloat3(&Corners[6], ComputeIntersection(XMLoadFloat4(&Planes[5]), ray));
#endif
		BoundBox.CreateFromPoints(Corners, 8);

		//Planes.clear();

		//// Right-handed
		//XMVECTOR vertices[8] =
		//{
		//	{-1.0f, +1.0f, +0.0f, +1.0f},
		//	{+1.0f, +1.0f, +0.0f, +1.0f},
		//	{+1.0f, -1.0f, +0.0f, +1.0f},
		//	{-1.0f, -1.0f, +0.0f, +1.0f},
		//	{-1.0f, +1.0f, +1.0f, +1.0f},
		//	{+1.0f, +1.0f, +1.0f, +1.0f},
		//	{+1.0f, -1.0f, +1.0f, +1.0f},
		//	{-1.0f, -1.0f, +1.0f, +1.0f},
		//};

		//// Left-handed
		////XMVECTOR vertices[8] =
		////{                                            //                         7--------6
		////	XMVectorSet(1.0f, -1.0f, 0.0f, 1.0f),      //                        /|       /|
		////	XMVectorSet(-1.0f, -1.0f, 0.0f, 1.0f),     //     Y ^               / |      / |
		////	XMVectorSet(1.0f, 1.0f, 0.0f, 1.0f),       //     | _              3--------2  |
		////	XMVectorSet(-1.0f, 1.0f, 0.0f, 1.0f),      //     | /' Z           |  |     |  |
		////	XMVectorSet(1.0f, -1.0f, 1.0f, 1.0f),      //     |/               |  5-----|--4
		////	XMVectorSet(-1.0f, -1.0f, 1.0f, 1.0f),     //     + ---> X         | /      | /
		////	XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f),       //                      |/       |/
		////	XMVectorSet(-1.0f, 1.0f, 1.0f, 1.0f),      //                      1--------0
		////};

		//XMVECTOR det;
		//XMMATRIX invM = XMMatrixInverse(&det, XMLoadFloat4x4(&matrix));

		//for(int i = 0; i < 8; i++)
		//{
		//	vertices[i] = XMVector3TransformCoord(vertices[i], invM);
		//	XMStoreFloat3(&Corners[i], vertices[i]);
		//}

		//BoundBox.CreateFromPoints(Corners, 8);

		//XMFLOAT4 planes[6];

		//// Right-handed
		//XMStoreFloat4(&planes[0], XMPlaneFromPoints(vertices[0], vertices[1], vertices[2]));
		//XMStoreFloat4(&planes[1], XMPlaneFromPoints(vertices[4], vertices[7], vertices[5]));
		//XMStoreFloat4(&planes[2], XMPlaneFromPoints(vertices[4], vertices[5], vertices[1]));
		//XMStoreFloat4(&planes[3], XMPlaneFromPoints(vertices[7], vertices[3], vertices[6]));
		//XMStoreFloat4(&planes[4], XMPlaneFromPoints(vertices[4], vertices[0], vertices[7]));
		//XMStoreFloat4(&planes[5], XMPlaneFromPoints(vertices[5], vertices[6], vertices[1]));

		//// Left-handed 
		///*XMStoreFloat4(&planes[0], XMPlaneFromPoints(vertices[0], vertices[4], vertices[2]));
		//XMStoreFloat4(&planes[1], XMPlaneFromPoints(vertices[1], vertices[3], vertices[5]));
		//XMStoreFloat4(&planes[2], XMPlaneFromPoints(vertices[3], vertices[2], vertices[7]));
		//XMStoreFloat4(&planes[3], XMPlaneFromPoints(vertices[1], vertices[5], vertices[0]));
		//XMStoreFloat4(&planes[4], XMPlaneFromPoints(vertices[5], vertices[7], vertices[4]));
		//XMStoreFloat4(&planes[5], XMPlaneFromPoints(vertices[1], vertices[0], vertices[3]));*/

		//for(int i = 0; i < 6; i++)
		//	Planes.push_back(planes[i]);
	}

	Ray Frustum::ComputeIntersectionLine(FXMVECTOR p1, FXMVECTOR p2)
	{
		Ray ray;
		ray.Direction = XMVector3Cross(p1, p2);
		XMVECTOR lengthSquared = XMVector3LengthSq(ray.Direction);
		ray.Position = XMVector3Cross((-XMVectorGetW(p1) * p2) + (XMVectorGetW(p2) * p1), ray.Direction) / lengthSquared;

		return ray;
	}

	XMVECTOR Frustum::ComputeIntersection(FXMVECTOR& plane, Ray& ray)
	{
		float value = (-XMVectorGetW(plane) - XMVectorGetX(XMVector3Dot(plane, ray.Position))) / XMVectorGetX(XMVector3Dot(plane, ray.Direction));

		return (ray.Position + (ray.Direction * value));
	}

	XMFLOAT3 Frustum::IntersectionPoint(XMFLOAT4 plane1, XMFLOAT4 plane2, XMFLOAT4 plane3)
	{
		XMVECTOR a = XMLoadFloat4(&plane1);
		XMVECTOR b = XMLoadFloat4(&plane2);
		XMVECTOR c = XMLoadFloat4(&plane3);

		XMVECTOR v1, v2, v3;
		XMVECTOR cross;

		cross = XMVector3Cross(b, c);

		float f;
		f = XMVectorGetX(XMVector3Dot(a, cross));
		f *= -1.0f;

		cross = XMVector3Cross(b, c);
		v1 = cross * XMVectorGetW(a);

		cross = XMVector3Cross(c, a);
		v2 = cross * XMVectorGetW(b);

		cross = XMVector3Cross(a, b);
		v3 = cross * XMVectorGetW(c);

		XMFLOAT3 result;	
		XMStoreFloat3(&result,(v1 + v2 + v3) / f);
		return result;
	}

	bool Frustum::Intersects(AABB aabb)
	{
		for(auto& plane : Planes)
		{
			if(aabb.Intersects(plane))
				continue;

			return false;
		}

		//// If false-positive then intersect frustum corners
		//int out;
		//out = 0; for(int i = 0; i < 8; i++) out += ((Corners[i].x < aabb.Min.x) ? 1 : 0); if(out == 8) return false;
		//out = 0; for(int i = 0; i < 8; i++) out += ((Corners[i].y > aabb.Max.y) ? 1 : 0); if(out == 8) return false;
		//out = 0; for(int i = 0; i < 8; i++) out += ((Corners[i].y < aabb.Min.y) ? 1 : 0); if(out == 8) return false;
		//out = 0; for(int i = 0; i < 8; i++) out += ((Corners[i].z > aabb.Max.z) ? 1 : 0); if(out == 8) return false;
		//out = 0; for(int i = 0; i < 8; i++) out += ((Corners[i].z < aabb.Min.z) ? 1 : 0); if(out == 8) return false;
		return true;
	}

	bool Frustum::Intersects(BoundingSphere sphere)
	{
		for (int i = 0; i < 6; i++)
		{
			XMVECTOR p = XMLoadFloat4(&Planes[i]);
			XMVECTOR v = XMLoadFloat3(&sphere.Center);

			float dist = XMVectorGetX(XMPlaneDotCoord(p, v));

			if (-dist > sphere.Radius) return false;

		}

		return true;
	}
}