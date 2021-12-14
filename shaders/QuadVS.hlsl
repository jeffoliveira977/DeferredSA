struct VS
{
    float4 Position : POSITION;
    float4 TexCoord : TEXCOORD0;
};

VS main(float4 Position : POSITION, float2 TexCoord : TEXCOORD0)
{
    VS output;

    output.Position = Position;
    output.TexCoord = float4(TexCoord, 0, 1);
    
    return output;
}