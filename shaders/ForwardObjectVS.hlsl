#include "FillGBuffer.hlsl"

VS_DeferredOutput VS_Deferred(VS_Input input)
{
    VS_DeferredOutput output;
    VSFillGBuffer(input, output);
    
    return output;
}