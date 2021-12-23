 struct PSDPDepthIn
{
    float4 Pos : POSITION;
    float ClipDepth : TEXCOORD1;
    float2 Depth : TEXCOORD2;
};

// DPDepth-pixel-shader
float4 main(PSDPDepthIn In) : COLOR
{
   // float a = tex2D(DiffuseSampler, Texcoord).a;
    //clip(a - 0.5);
    
	 // clipping
    //clip(In.ClipDepth);
			
    return In.Depth.x / In.Depth.y;
}
	