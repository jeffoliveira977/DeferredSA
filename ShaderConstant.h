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