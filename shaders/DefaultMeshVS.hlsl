#include "FillGBuffer.hlsl"

VS_DeferredOutput main(VS_Input input)
{
    VS_DeferredOutput output;
    VSFillGBuffer(input, output);
    
    return output;
}