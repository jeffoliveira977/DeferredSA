#pragma once
#include <Windows.h>
#include "CommonD.h"

struct ctab_constant
{
    D3DXCONSTANT_DESC desc;
    ctab_constant* constants;
};

struct ID3DXConstantTableImpl
{
    char* ctab;
    uint32_t size;
    D3DXCONSTANTTABLE_DESC desc;
     ctab_constant* constants;
};

class ConstantTable
{
    ID3DXConstantTableImpl* mConstantTable;
protected:
    HRESULT Initialize(void* byteCode);
    void Release();
public:
    void SetValue(const std::string constant, void* data, uint32_t bytes);

    void SetBool(const std::string constant, bool b);
    void SetBoolArray(const std::string constant, bool* b, uint32_t count);

    void SetInt(const std::string constant, int n);
    void SetIntArray(const std::string constant, int* n, uint32_t count);

    void SetFloat(const std::string constant, float f);
    void SetFloatArray(const std::string constant, float* f, uint32_t count);


    void SetVector(const std::string constant, XMVECTOR* vector);
    void SetVectorArray(const std::string constant, XMVECTOR* vector, uint32_t count);

    void SetMatrix(const std::string constant, XMMATRIX* matrix);
    void SetMatrixArray(const std::string constant, XMMATRIX* matrix, uint32_t count);
    void SetMatrixPointerArray(const std::string constant, XMMATRIX** matrix, uint32_t count);
    void SetMatrixTranspose(const std::string constant, XMMATRIX* matrix);
    void SetMatrixTransposeArray(const std::string constant, XMMATRIX* matrix, uint32_t count);
    void SetMatrixTransposePointerArray(const std::string constant, XMMATRIX** matrix, uint32_t count);
};