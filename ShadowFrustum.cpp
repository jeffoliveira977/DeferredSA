#include "ShadowFrustum.h"
#include "GTADef.h"

//BoundingBox	ShadowFrustum::m_LightBBox[4];
//XMVECTOR	ShadowFrustum::m_boudingBoxCenter[4];
//XMMATRIX	ShadowFrustum::m_LightSpaceMatrix;
//XMVECTOR ShadowFrustum::m_vFrustumCorners[4][8];


void GetCameraFrustumCorners(RwCamera* camera, RwReal nearClip, RwReal farClip, XMVECTOR *corners)
{
    RwMatrix* LTM = RwFrameGetLTM(RwCameraGetFrame(camera));

    XMVECTOR position = XMLoadFloat3(reinterpret_cast<XMFLOAT3*>(&LTM->pos));
    XMVECTOR at = XMLoadFloat3(reinterpret_cast<XMFLOAT3*>(&LTM->at));
    XMVECTOR up = XMLoadFloat3(reinterpret_cast<XMFLOAT3*>(&LTM->up));
    //XMVECTOR right = XMLoadFloat3(reinterpret_cast<XMFLOAT3*>(&LTM->right));
    XMVECTOR right = XMVector3Cross(at, up);

    XMVECTOR farCenter  = position + at * farClip;
    XMVECTOR nearCenter = position + at * nearClip;

    XMVECTOR nearHeight = up * 2 * camera->viewWindow.y * nearClip;
    XMVECTOR nearWidth  = right * 2 * camera->viewWindow.x * nearClip;
    XMVECTOR farHeight  = up * 2 * camera->viewWindow.y * farClip;
    XMVECTOR farWidth   = right * 2 * camera->viewWindow.x * farClip;

    corners[0] = nearCenter - nearHeight - nearWidth;
    corners[1] = nearCenter + nearHeight - nearWidth;
    corners[2] = nearCenter + nearHeight + nearWidth;
    corners[3] = nearCenter - nearHeight + nearWidth;

    corners[4] = farCenter - farHeight - farWidth;
    corners[5] = farCenter + farHeight - farWidth; 
    corners[6] = farCenter + farHeight + farWidth;
    corners[7] = farCenter - farHeight + farWidth;
}


//#define RWCAMERA_TRANS

#include "CScene.h"
void ShadowFrustum::DirectionalLightTransform(RwCamera* mainCam, const RW::V3d& lightDir, int shadowCascade)
{
    XMVECTOR lightDirection = XMVectorSet(lightDir.getX(), lightDir.getY(), lightDir.getZ(), 1.0);

    float lastSplitDist = 0.0;
    for(size_t i = 0; i < CascadeCount; i++)
    {
        #ifndef RWCAMERA_TRANS 
        {
            XMVECTOR corners[8];
            GetCameraFrustumCorners(mainCam, m_fShadowDistances[i], m_fShadowDistances[i + 1], corners);

            // Get frustum center
            XMVECTOR frustumCenter = XMVectorZero();
            for(uint32_t j = 0; j < 8; j++)
            {
                frustumCenter += corners[j];
            }
            frustumCenter /= 8.0f;

            XMVECTOR cameraUpVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);;
            if(XMVectorGetW(XMVectorAbs(XMVector3Dot(cameraUpVector, lightDirection))) > 0.9f)
                cameraUpVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

            // Create a temporary view matrix for the light
            XMVECTOR lookAt = frustumCenter - lightDirection;
            XMMATRIX lightRotation = XMMatrixLookAtRH(frustumCenter, lookAt, cameraUpVector);

            XMVECTOR minExtents;
            XMVECTOR maxExtents;

            bool stabilizeCascades = false;
            if(stabilizeCascades)
            {
                XMVECTOR radius = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
                for(uint32_t j = 0; j < 8; j++)
                {
                    auto distance = XMVector3Length(corners[j] - frustumCenter);
                    radius = XMVectorMax(radius, distance);
                }
                radius = XMVectorCeiling(radius * 16.0f) / 16.0f;

                maxExtents = radius;
                minExtents = -maxExtents;
            }
            else
            {
                minExtents = g_XMFltMax;
                maxExtents = g_XMFltMin;

                for(uint32_t j = 1; j < 8; j++)
                {
                    corners[j] = XMVector3Transform(corners[j], lightRotation);

                    minExtents = XMVectorMin(minExtents, corners[j]);
                    maxExtents = XMVectorMax(maxExtents, corners[j]);
                }

                // Adjust the min/max to accommodate the filtering size
                float texels = (static_cast<float>(ShadowSize) + FilterSize) / static_cast<float>(ShadowSize);
                XMVECTOR scale = XMVectorSet(texels, texels, 1.0f, 1.0f);
                minExtents *= scale;
                maxExtents *= scale;
            }

            XMStoreFloat3(&Desc[i].m_AABB.Min, minExtents);
            XMStoreFloat3(&Desc[i].m_AABB.Max, maxExtents);
            XMStoreFloat3(&Desc[i]. FrustumCenter, frustumCenter);

            XMFLOAT3 extents;
            XMStoreFloat3(&extents, maxExtents - minExtents);

           CVector coors= FindPlayerCoors(0);
            float longEdge = max(extents.x, extents.y);
            longEdge *= 0.5;
            Desc[i].NearClip = -(extents.z < 500.0 ? 500.0 : extents.z);
            Desc[i].FarClip = 50.0f + -Desc[i].NearClip;

            PrintMessage("%f", coors.z);

            // Get position of the shadow camera
            XMVECTOR shadowPosition = frustumCenter + lightDirection /** -XMVectorGetZ(minExtents)*/;

            Desc[i].lightViewMatrix = XMMatrixLookAtRH(shadowPosition,  frustumCenter, cameraUpVector);
            Desc[i].lightOrthoMatrix = XMMatrixOrthographicRH(longEdge, longEdge, Desc[i].NearClip, Desc[i].FarClip);
            
            if(stabilizeCascades)
            {
                // Create the rounding matrix, by projecting the world-space origin and determining
                // the fractional offset in texel space
               XMMATRIX vp = Desc[i].lightViewMatrix * Desc[i].lightOrthoMatrix;

                XMVECTOR origin = {0.0f, 0.0f, 0.0f, 1.0f};
                origin = XMVector3Transform(origin, vp);
                origin *= static_cast<float>(ShadowSize) / 2.0f;

                XMVECTOR rounded = XMVectorRound(origin) - origin;
                rounded *= 2.0f / static_cast<float>(ShadowSize);
                XMVectorSetZ(rounded, 0.0f);
                XMVectorSetW(rounded, 0.0f);

                Desc[i].lightOrthoMatrix.r[3] += rounded;
            }

            XMMATRIX projection = Desc[i].lightOrthoMatrix;
            //projection.r[0] *= 0.5;
            //projection.r[1] *= 0.5;
            Desc[i].m_FrustumCulling.SetMatrix(Desc[i].lightViewMatrix * projection);
        }
        #else
        {
            XMVECTOR right;
            XMVECTOR up;
            RwMatrix* cameraMatrix = RwFrameGetMatrix(RwCameraGetFrame(mainCam));

            lightDirection = XMVector3Normalize(lightDirection);
            right = XMVector3Cross(XMVectorSet(0, 1, 0, 0.0f), lightDirection);
            right = XMVector3Normalize(right);
            up = XMVector3Cross(right, lightDirection);
            up = XMVector3Normalize(up);

            // Build light matrix
            m_LightSpaceMatrix.r[0] = right;
            m_LightSpaceMatrix.r[1] = up;
            m_LightSpaceMatrix.r[2] = lightDirection;
            m_LightSpaceMatrix.r[3] = XMVectorSet(0, 0, 0, 1);

            float splitDist = m_fShadowDistances[i];
            XMVECTOR corners[8];
            GetCameraFrustumCorners(mainCam, m_fShadowDistances[i], m_fShadowDistances[i + 1], corners);

            XMVECTOR frustumCenter = XMVectorZero();
            for(uint32_t j = 0; j < 8; j++)
            {
                frustumCenter += corners[j];
            }
            frustumCenter /= 8.0f;

            XMVECTOR minExtents;
            XMVECTOR maxExtents;

            bool stabilizeCascades = true;

            if(stabilizeCascades)
            {
                minExtents = g_FltMax;
                maxExtents = g_FltMin;

                for(int j = 0; j < 8; j++)
                {
                    // Transform the AABB to Light space.
                    XMVECTOR frustumTranslation = XMVector3Transform(corners[j], m_LightSpaceMatrix);

                    //Calculates the min and max values of AABB.
                    minExtents = XMVectorMin(minExtents, frustumTranslation);
                    maxExtents = XMVectorMax(maxExtents, frustumTranslation);
                }
            }
            else
            {
                XMVECTOR radius = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
                for(uint32_t j = 0; j < 8; j++)
                {
                    auto distance = XMVector3Length(corners[j] - frustumCenter);
                    radius = XMVectorMax(radius, distance);
                }
                radius = XMVectorCeiling(radius * 16.0f) / 16.0f;

                maxExtents = radius;
                minExtents = -maxExtents;
            }

            // Adjust the min/max to accommodate the filtering size
            float texels = (static_cast<float>(ShadowSize) + 9.0f) / static_cast<float>(ShadowSize);
            XMVECTOR scale = XMVectorSet(texels, texels, 1.0f, 1.0f);
            minExtents *= scale;
            maxExtents *= scale;

            XMStoreFloat3(&Desc[i].m_AABB.Min, minExtents);
            XMStoreFloat3(&Desc[i].m_AABB.Max, maxExtents);
            XMStoreFloat3(&Desc[i].FrustumCenter, frustumCenter);

            lastSplitDist = m_fShadowDistances[i];
        }
        #endif
    }
}

void ShadowFrustum::CalculateShadowDistances(const RwReal nearClip, const RwReal farClip)
{
    //m_fShadowDistances[0] = nearClip;
    //m_fShadowDistances[4] = farClip;

    ////compute each distance the way you like...
    //for(int i = 1; i < CascadeCount +1; i++)
    //    m_fShadowDistances[i] = nearClip + (farClip - nearClip) * pow((i / (float)CascadeCount), 2);

    //return;

    m_partitionMode = PartitionMode_Manual;
    if(m_partitionMode == PartitionMode_Manual)
    {
        m_fShadowDistances[0] = nearClip;
        m_fShadowDistances[1] = nearClip + DistanceCoefficients[0] * farClip;
        m_fShadowDistances[2] = nearClip + DistanceCoefficients[1] * farClip;
        m_fShadowDistances[3] = nearClip + DistanceCoefficients[2] * farClip;
        m_fShadowDistances[4] = nearClip + DistanceCoefficients[3] * farClip;
    }
    else if(m_partitionMode == PartitionMode_Logarithmic ||
            m_partitionMode == PartitionMode_PSSM)
    {

        float lambda = 1.0f;

        if(m_partitionMode == PartitionMode_PSSM)
            lambda = 0.95;

        float clipRange = farClip - nearClip;

        float minZ = nearClip + MinDistance * clipRange;
        float maxZ = nearClip + MaxDistance * clipRange;

        float range = maxZ - minZ;
        float ratio = maxZ / minZ;

        for(uint i = 0; i < CascadeCount+1; ++i)
        {
            float p = static_cast<float>(i + 1) / static_cast<float>(CascadeCount+1);
            float logScale = minZ * pow(ratio, p);
            float uniformScale = minZ + range * p;
            float d = lambda * (logScale - uniformScale) + uniformScale;
            m_fShadowDistances[i] = (d - nearClip) / clipRange;
        }

    }
}