float4 Diffuse : register(c0);

float4 main(float4 color : COLOR0) : COLOR0
{
    return Diffuse * color;
}