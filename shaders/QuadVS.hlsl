struct VS
{
    float4 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 FrustumRay : TEXCOORD1;
};

float3 FrustumCorners[4] : register(c0);

float3 GetFrustumRay(in float4 Position)
{
   float3 vCamVecLerpT = (Position.x > 0) ? FrustumCorners[1].xyz : FrustumCorners[0].xyz;
   float3 vCamVecLerpB = (Position.x > 0) ? FrustumCorners[3].xyz : FrustumCorners[2].xyz;
   return (Position.y < 0) ? vCamVecLerpB.xyz : vCamVecLerpT.xyz;
}

float3 GetFrustumRay(in float2 texCoord)
{
    float index = texCoord.x + (texCoord.y * 2);
    return FrustumCorners[index];
}

VS main(float4 Position : POSITION, float2 TexCoord : TEXCOORD0)
{
    VS output;
    
    output.Position.x = Position.x - (1.0f / 1366.0f);
    output.Position.y = Position.y + (1.0f / 768.0f);
    output.Position.z = Position.z;
    output.Position.w = 1.0f;
    output.TexCoord = TexCoord;   
    output.FrustumRay = GetFrustumRay(TexCoord);
    
    return output;
}