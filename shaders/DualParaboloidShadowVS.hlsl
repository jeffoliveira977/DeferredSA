row_major float4x4 World : register(c0);
row_major float4x4 View : register(c4);
row_major float4x4 Projection : register(c8);

float4 FogData : register(c12);

#define NearPlane FogData.x
#define FarPlane  FogData.y

float g_fDir; // direction of hemisphere

 // DPDepth
struct VSDPDepthIn
{
    float4 Pos : POSITION;
};

struct PSDPDepthIn
{
    float4 Pos : POSITION;
    float ClipDepth : TEXCOORD1;
    float2 Depth : TEXCOORD2;
};

 // DPDepth-vertex-shader
PSDPDepthIn main(VSDPDepthIn In)
{
    PSDPDepthIn Out;
    Out.Pos = mul(In.Pos, World);
    Out.Pos = mul(Out.Pos, View);
  //  Out.Pos = mul(Out.Pos, Projection);
    
	 // transform vertex to DP-space
    Out.Pos = mul(In.Pos, World);
    Out.Pos = mul(Out.Pos, View);
    Out.Pos /= Out.Pos.w;
	
	 	
	 // because the origin is at 0 the proj-vector
	 // matches the vertex-position
    float fLength = length(Out.Pos.xyz);
	 
	 // normalize
    Out.Pos /= fLength;
	 
	 // save for clipping 	
    Out.ClipDepth = Out.Pos.z;
 	 	
	 // calc "normal" on intersection, by adding the 
	 // reflection-vector(0,0,1) and divide through 
	 // his z to get the texture coords
    Out.Pos.x /= Out.Pos.z + 1.0f;
    Out.Pos.y /= Out.Pos.z + 1.0f;
	
	 // set z for z-buffering and neutralize w
    Out.Pos.z = (fLength - NearPlane) / (FarPlane - NearPlane);
    Out.Pos.w = 1.0f;
    
    //float L = length(Out.Pos.xyz);
    //Out.Pos = Out.Pos / L;
    //Out.ClipDepth = Out.Pos.z / L;
    //Out.Pos.z = (L - NearPlane) / (FarPlane - NearPlane);
    //Out.Pos.w = 1.0f;
    
	 // DP-depth
    Out.Depth = Out.Pos.zw;
	
    return Out;
}