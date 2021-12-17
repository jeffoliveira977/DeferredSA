struct VSOUTPUT_BLUR
{
    float4 vPosition : POSITION;
    float2 vTexCoord : TEXCOORD0;
};

// Gaussian filter vertex shader
VSOUTPUT_BLUR main(float4 inPosition : POSITION, float2 inTexCoord : TEXCOORD0)
{
	// Output struct
    VSOUTPUT_BLUR OUT = (VSOUTPUT_BLUR) 0;

	// Output the position
    OUT.vPosition = float4(inPosition.xyz, 1.0);

	// Output the texture coordinates
    OUT.vTexCoord = inTexCoord;
    return OUT;
}