#include "ShadowFrustum.h"
#include "GTADef.h"

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
void ShadowFrustum::DirectionalLightTransform(RwCamera* mainCam, CVector lightDir, int shadowCascade)
{
    XMVECTOR lightDirection = XMVectorSet(lightDir.x, lightDir.y, lightDir.z, 1.0f);

    float lastSplitDist = 0.0;
    for(size_t i = 0; i < CascadeCount; i++)
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
        mStabilizeCascades = false;
        if(mStabilizeCascades)
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

            for(uint32_t j = 0; j < 8; j++)
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

        XMFLOAT3 extents;
        XMStoreFloat3(&extents, maxExtents - minExtents);

        float longEdge = max(extents.x, extents.y);
        longEdge *= 0.5;
        float nearZ =-300;
        float farZ = 50.0f + -nearZ;
      //  PrintMessage("%f", farZ);

        // Get position of the shadow camera
        XMVECTOR shadowPosition = frustumCenter + lightDirection * -XMVectorGetZ(minExtents);

        Desc[i].mLightViewMatrix = XMMatrixLookAtRH(shadowPosition, frustumCenter, cameraUpVector);
        Desc[i].mLightOrthoMatrix = XMMatrixOrthographicRH(longEdge, longEdge, nearZ, farZ);
        if(mStabilizeCascades)
        {
            // Create the rounding matrix, by projecting the world-space origin and determining
            // the fractional offset in texel space
            XMMATRIX vp = Desc[i].mLightViewMatrix * Desc[i].mLightOrthoMatrix;

            XMVECTOR origin = {0.0f, 0.0f, 0.0f, 1.0f};
            origin = XMVector3Transform(origin, vp);
            origin *= static_cast<float>(ShadowSize) / 2.0f;

            XMVECTOR rounded = XMVectorRound(origin) - origin;
            rounded *= 2.0f / static_cast<float>(ShadowSize);
            XMVectorSetZ(rounded, 0.0f);
            XMVectorSetW(rounded, 0.0f);

            Desc[i].mLightOrthoMatrix.r[3] += rounded;
        }

        XMMATRIX projection = Desc[i].mLightOrthoMatrix;
       // projection.r[0] *= 0.5f;
       // projection.r[1] *= 0.5f;
        Desc[i].mFrustumCulling.SetMatrix(Desc[i].mLightViewMatrix * projection);
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
    m_fShadowDistances[0] = nearClip;
    /*for (auto i = 0; i < 4; i++)
    {
        m_fShadowDistances[i] = fNear * powf(farDist / fNear, i / (float)gShadowSettings.ShadowCascadeCount);
    }*/
  /*  m_fShadowDistances[1] = nearClip + farClip * mDistanceCoefficients[0];
    m_fShadowDistances[2] = nearClip + farClip * mDistanceCoefficients[1];
    m_fShadowDistances[3] = nearClip + farClip * mDistanceCoefficients[2];
    m_fShadowDistances[4] = farClip;
    return;*/
    m_partitionMode = PartitionMode_Logarithmic;
    if(m_partitionMode == PartitionMode_Manual)
    {
        m_fShadowDistances[0] = nearClip;
        m_fShadowDistances[1] = nearClip + mDistanceCoefficients[0] * farClip;
        m_fShadowDistances[2] = nearClip + mDistanceCoefficients[1] * farClip;
        m_fShadowDistances[3] = nearClip + mDistanceCoefficients[2] * farClip;
        m_fShadowDistances[4] = nearClip + mDistanceCoefficients[3] * farClip;
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