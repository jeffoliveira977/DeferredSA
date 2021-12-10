sampler2D Scene : register(s0);

static float2 PixelCoordsDownFilter[16] =
{
    { 1.5, -1.5 },
    { 1.5, -0.5 },
    { 1.5, 0.5 },
    { 1.5, 1.5 },
    { 0.5, -1.5 },
    { 0.5, -0.5 },
    { 0.5, 0.5 },
    { 0.5, 1.5 },
    { -0.5, -1.5 },
    { -0.5, -0.5 },
    { -0.5, 0.5 },
    { -0.5, 1.5 },
    { -1.5, -1.5 },
    { -1.5, -0.5 },
    { -1.5, 0.5 },
    { -1.5, 1.5 },
};

float4 main(in float2 Tex : TEXCOORD0) : COLOR0
{
    float4 Color = 0;

    for (int i = 0; i < 16; i++)
    {
        Color += tex2D(Scene, Tex + PixelCoordsDownFilter[i].xy);
    }

    return Color / 16;
}

