row_major float4x4 World : register(c0);
row_major float4x4 View : register(c4);
row_major float4x4 Projection : register(c8);
float3 CameraPosition : register(c12);

static float fDepthAttenuation = 0.6;
static float fDepthAttenuationPower = 1.5;

struct VSInput
{
    float4 Position : POSITION0;
    float4 Diffuse : COLOR0;
    float2 TexCoord : TEXCOORD0;
};

struct PSInput
{
    float4 Position : POSITION0;
    float4 Diffuse : COLOR0;
    float2 TexCoord : TEXCOORD0;
    float3 TexProj : TEXCOORD1;
    float2 MatDepth : TEXCOORD2;
};

float Linearize(float posZ)
{
    return Projection[3][2] / (posZ - Projection[2][2]);
}

float InvLinearize(float posZ)
{
    return (Projection[3][2] / posZ) + Projection[2][2];
}

PSInput main(VSInput VS)
{
    PSInput PS = (PSInput) 0;

    float4 worldPos = mul(float4(VS.Position.xyz, 1.0), World);
    float4 viewPos = mul(worldPos, View);

    PS.MatDepth = float2(viewPos.z, viewPos.w);
	
    PS.Position = mul(viewPos, Projection);
    float linDepth = viewPos.z / viewPos.w;
    float depthBias = InvLinearize(linDepth) - InvLinearize(linDepth - fDepthAttenuation);
    PS.Position.z -= depthBias * PS.Position.w;
    PS.Position.z = max(InvLinearize(Linearize(0)) * PS.Position.w, PS.Position.z);
	
    PS.TexCoord = VS.TexCoord;
    PS.Diffuse = VS.Diffuse;

    float projectedX = (0.5 * (PS.Position.w + PS.Position.x));
    float projectedY = (0.5 * (PS.Position.w - PS.Position.y));
    PS.TexProj = float3(projectedX, projectedY, PS.Position.w);
    return PS;
}
