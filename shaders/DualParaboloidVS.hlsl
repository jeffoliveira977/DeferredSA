row_major float4x4 World: register(c0);
row_major float4x4 View : register(c4);
row_major float4x4 Projection : register(c8);
float4 FogData : register(c12);

#define NearPlane FogData.x
#define FarPlane  FogData.y

struct VS_DualInput
{
    float4 Position : POSITION0;
    float4 Color    : COLOR0;
    float2 Texcoord : TEXCOORD0;
};

struct VS_DualOutput
{
    float4 Position : POSITION0;
    float4 Color    : COLOR0;
    float2 Texcoord : TEXCOORD0;
    float DualZ     : TEXCOORD1;
};

VS_DualOutput main(VS_DualInput input)
{
    VS_DualOutput output;
    
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
    float L = length(output.Position.xyz);
    output.Position = output.Position / L;
    output.DualZ = output.Position.z / L;
    output.Position.z = (L - NearPlane) / (FarPlane - NearPlane);
    output.Position.w = 1.0f;
    
    ////determine the distance between (0,0,0) and the vertex
    //float L = length(output.Position.xyz);

    ////divide the vertex position by the distance  
    //output.Position = output.Position / L;
    //output.DualZ = output.Position.z;

    ////add the reflected vector to find the normal vector 
    //output.Position.z = output.Position.z + 1;

    ////divide x coordinate by the new z-value 
    //output.Position.x = output.Position.x / output.Position.z;
   
    ////divide y coordinate by the new z-value  
    //output.Position.y = output.Position.y / output.Position.z;
  
    ////scale the depth to [0, 1]  
    //output.Position.z = (L - NearPlane) / (FarPlane - NearPlane);
    //output.Position.w = 1;
        
    output.Texcoord = input.Texcoord;
    output.Color = input.Color;
    return output;
    
}