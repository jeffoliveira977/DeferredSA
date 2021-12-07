struct VS
{
    float4 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
};

VS main(VS input)
{
    VS output;

    output.Position = input.Position;
    output.TexCoord = input.TexCoord;
    
    return output;
}