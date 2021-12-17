#pragma once
#include "CommonD.h"
#include "VertexBuffer.h"
#include "RwIndexBuffer.h"
#include "PixelShader.h"
#include "VertexShader.h"

class Immediate3D
{
public:
    static void Hook();
private:
    static VertexBuffer* mVertexBuffer;
    static RwIndexBuffer* mIndexBuffer;
    static VertexShader* mVertexShader;
    static PixelShader* mPixelShader;

    static RwBool Open();
    static void Close();
    static RwBool Render(RxPipelineNodeInstance* self, const RxPipelineNodeParam* params);
};