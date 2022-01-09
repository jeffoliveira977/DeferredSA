
struct VertexShaderOutputMeshBased
{
    float4 Position : POSITION0;
    float4 TexCoordScreenSpace : TEXCOORD4;
};

struct VertexShaderInput
{
    float4 Position : POSITION0;
    float2 TexCoord : TEXCOORD0;
};

float4x4 WorldViewProj : register(c17);

VertexShaderOutputMeshBased main(VertexShaderInput input)
{
    VertexShaderOutputMeshBased output = (VertexShaderOutputMeshBased) 0;
    output.Position = mul(input.Position, WorldViewProj);

	//we will compute our texture coords based on pixel position further
    output.TexCoordScreenSpace = output.Position;
    return output;
}