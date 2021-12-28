//-------------------------------
// Structs
//-------------------------------
struct VertexShaderInput
{
    float4 Position : POSITION0;
    float2 TexCoord : TEXCOORD0;
};

struct VertexShaderOutput
{
    float4 Position : POSITION0;
    float4 TexCoord : TEXCOORD0;
    float2 TexCoordHalfBuffer : TEXCOORD1;
};

static float2 GBufferPixelSize = { 0.5f / 1920.0f, 0.5f / 1080.0f };

float RandomTile : register(c0);

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output = (VertexShaderOutput) 0;
	
    output.Position = input.Position;
    output.TexCoord.xy = input.TexCoord + GBufferPixelSize;
    output.TexCoord.zw = input.TexCoord * 100.0f;
	
    output.TexCoordHalfBuffer.xy = input.TexCoord + GBufferPixelSize;
	
    return output;
}