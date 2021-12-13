#pragma once
#include <Windows.h>
#include "CommonD.h"

struct ctab_constant
{
    D3DXCONSTANT_DESC desc;
    struct ctab_constant* constants;
};

struct ID3DXConstantTableImpl
{
    LONG ref;
    char* ctab;
    DWORD size;
    D3DXCONSTANTTABLE_DESC desc;
    struct ctab_constant* constants;
};


HRESULT WINAPI D3DX_GetShaderConstantTable(CONST DWORD* byte_code,
                                           ID3DXConstantTableImpl** constant_table);

HRESULT WINAPI D3DX_GetShaderConstantTableEx(CONST DWORD* byte_code,
                                              DWORD flags,
                                              ID3DXConstantTableImpl** constant_table);

HRESULT WINAPI ID3DXConstantTableImpl_SetMatrix(ID3DXConstantTableImpl* This, LPDIRECT3DDEVICE9 device,
                                                       D3DXHANDLE constant, CONST XMMATRIX* matrix);

HRESULT WINAPI ID3DXConstantTableImpl_SetMatrixTranspose(ID3DXConstantTableImpl* This, LPDIRECT3DDEVICE9 device,
                                                         D3DXHANDLE constant, CONST XMMATRIX* matrix);

HRESULT WINAPI ID3DXConstantTableImpl_SetVector(ID3DXConstantTableImpl* This, LPDIRECT3DDEVICE9 device,
                                                       D3DXHANDLE constant, CONST D3DXVECTOR4* vector);
HRESULT WINAPI ID3DXConstantTableImpl_SetFloat(ID3DXConstantTableImpl* This, LPDIRECT3DDEVICE9 device,
                                               D3DXHANDLE constant, FLOAT f);
HRESULT WINAPI ID3DXConstantTableImpl_SetValue(ID3DXConstantTableImpl* This, LPDIRECT3DDEVICE9 device,
                                                      D3DXHANDLE constant, LPCVOID data, UINT bytes);

class ConstantTable
{
    ID3DXConstantTableImpl* mConstantTable;
    LPDIRECT3DDEVICE9 mDevice;
protected:
    void Initialize(void* byteCode);
public:
    void SetValue(D3DXHANDLE constant, void* data, uint32_t bytes);

    void SetBool(D3DXHANDLE constant, bool b);
    void SetBoolArray(D3DXHANDLE constant, bool* b, uint32_t count);

    void SetInt(D3DXHANDLE constant, int n);
    void SetIntArray(D3DXHANDLE constant, int* n, uint32_t count);

    void SetFloat(D3DXHANDLE constant, float f);
    void SetFloatArray(D3DXHANDLE constant, float* f, uint32_t count);


    void SetVector(D3DXHANDLE constant, XMVECTOR* vector);
    void SetVectorArray(D3DXHANDLE constant, XMVECTOR* vector, uint32_t count);

    void SetMatrix(D3DXHANDLE constant, XMMATRIX* matrix);
    void SetMatrixArray(D3DXHANDLE constant, XMMATRIX* matrix, uint32_t count);
    void SetMatrixPointerArray(D3DXHANDLE constant, XMMATRIX** matrix, uint32_t count);
    void SetMatrixTranspose(D3DXHANDLE constant, XMMATRIX* matrix);
    void SetMatrixTransposeArray(D3DXHANDLE constant, XMMATRIX* matrix, uint32_t count);
    void SetMatrixTransposePointerArray(D3DXHANDLE constant, XMMATRIX** matrix, uint32_t count);
};