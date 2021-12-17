struct VS
{
    float4 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
};

VS main(float4 Position : POSITION, float2 TexCoord : TEXCOORD0)
{
    VS output;

    output.Position = Position;
    output.TexCoord = TexCoord;
    
    return output;
}