#include "ShaderConstant.h"

/*
 * Copyright 2008 Luis Busquets
 * Copyright 2009 Matteo Bruni
 * Copyright 2011 Travis Athougies
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */


#define NONAMELESSUNION
#define NONAMELESSSTRUCT

#include "windef.h"
#include "wingdi.h"
#include "objbase.h"
#include "d3dcommon.h"
#include "d3dcompiler.h"

#include "winbase.h"
#include "wingdi.h"
#include "winuser.h"
#include "d3dx9.h"


LPCSTR WINAPI D3DX_GetPixelShaderProfile(LPDIRECT3DDEVICE9 device)
{
    D3DCAPS9 caps;

    if(!device) return NULL;

    IDirect3DDevice9_GetDeviceCaps(device, &caps);

    switch(caps.PixelShaderVersion)
    {
        case D3DPS_VERSION(1, 1):
            return "ps_1_1";

        case D3DPS_VERSION(1, 2):
            return "ps_1_2";

        case D3DPS_VERSION(1, 3):
            return "ps_1_3";

        case D3DPS_VERSION(1, 4):
            return "ps_1_4";

        case D3DPS_VERSION(2, 0):
            if((caps.PS20Caps.NumTemps >= 22) &&
                (caps.PS20Caps.Caps & D3DPS20CAPS_ARBITRARYSWIZZLE) &&
               (caps.PS20Caps.Caps & D3DPS20CAPS_GRADIENTINSTRUCTIONS) &&
               (caps.PS20Caps.Caps & D3DPS20CAPS_PREDICATION) &&
               (caps.PS20Caps.Caps & D3DPS20CAPS_NODEPENDENTREADLIMIT) &&
               (caps.PS20Caps.Caps & D3DPS20CAPS_NOTEXINSTRUCTIONLIMIT))
            {
                return "ps_2_a";
            }
            if((caps.PS20Caps.NumTemps >= 32) &&
                (caps.PS20Caps.Caps & D3DPS20CAPS_NOTEXINSTRUCTIONLIMIT))
            {
                return "ps_2_b";
            }
            return "ps_2_0";

        case D3DPS_VERSION(3, 0):
            return "ps_3_0";
    }

    return NULL;
}

UINT WINAPI D3DX_GetShaderSize(const DWORD* byte_code)
{
    const DWORD* ptr = byte_code;

    if(!ptr) return 0;

    /* Look for the END token, skipping the VERSION token */
    while(*++ptr != D3DSIO_END)
    {
        /* Skip comments */
        if((*ptr & D3DSI_OPCODE_MASK) == D3DSIO_COMMENT)
        {
            ptr += ((*ptr & D3DSI_COMMENTSIZE_MASK) >> D3DSI_COMMENTSIZE_SHIFT);
        }
    }
    ++ptr;

    /* Return the shader size in bytes */
    return (ptr - byte_code) * sizeof(*ptr);
}

DWORD WINAPI D3DX_GetShaderVersion(const DWORD* byte_code)
{
    return byte_code ? *byte_code : 0;
}

LPCSTR WINAPI D3DX_GetVertexShaderProfile(LPDIRECT3DDEVICE9 device)
{
    D3DCAPS9 caps;

    if(!device) return NULL;

    IDirect3DDevice9_GetDeviceCaps(device, &caps);

    switch(caps.VertexShaderVersion)
    {
        case D3DVS_VERSION(1, 1):
            return "vs_1_1";
        case D3DVS_VERSION(2, 0):
            if((caps.VS20Caps.NumTemps >= 13) &&
                (caps.VS20Caps.DynamicFlowControlDepth == 24) &&
               (caps.VS20Caps.Caps & D3DPS20CAPS_PREDICATION))
            {
                return "vs_2_a";
            }
            return "vs_2_0";
        case D3DVS_VERSION(3, 0):
            return "vs_3_0";
    }

    return NULL;
}

HRESULT WINAPI D3DX_FindShaderComment(CONST DWORD* byte_code, DWORD fourcc, LPCVOID* data, UINT* size)
{
    CONST DWORD* ptr = byte_code;


    if(data) *data = NULL;
    if(size) *size = 0;

    if(!byte_code)
        return D3DERR_INVALIDCALL;

    while(*++ptr != D3DSIO_END)
    {
        /* Check if it is a comment */
        if((*ptr & D3DSI_OPCODE_MASK) == D3DSIO_COMMENT)
        {
            DWORD comment_size = (*ptr & D3DSI_COMMENTSIZE_MASK) >> D3DSI_COMMENTSIZE_SHIFT;

            /* Check if this is the comment we are looking for */
            if(*(ptr + 1) == fourcc)
            {
                UINT ctab_size = (comment_size - 1) * sizeof(DWORD);
                LPCVOID ctab_data = ptr + 2;
                if(size)
                    *size = ctab_size;
                if(data)
                    *data = ctab_data;

                return D3D_OK;
            }
            ptr += comment_size;
        }
    }

    return S_FALSE;
}


static void free_constant(ctab_constant* constant)
{
    if(constant->constants)
    {
        UINT i, count = constant->desc.Elements > 1 ? constant->desc.Elements : constant->desc.StructMembers;

        for(i = 0; i < count; ++i)
        {
            free_constant(&constant->constants[i]);
        }
        HeapFree(GetProcessHeap(), 0, constant->constants);
    }
}

static void free_constant_table(ID3DXConstantTableImpl* table)
{
    if(table->constants)
    {
        UINT i;

        for(i = 0; i < table->desc.Constants; ++i)
        {
            free_constant(&table->constants[i]);
        }
        HeapFree(GetProcessHeap(), 0, table->constants);
    }
    HeapFree(GetProcessHeap(), 0, table->ctab);
}


static inline int is_vertex_shader(DWORD version)
{
    return (version & 0xFFFF0000) == 0xFFFE0000;
}

static DWORD calc_bytes(D3DXCONSTANT_DESC* desc)
{
    if(desc->RegisterSet != D3DXRS_FLOAT4 && desc->RegisterSet != D3DXRS_SAMPLER)
    {
    }
    return 4 * desc->Elements * desc->Rows * desc->Columns;
}

static inline  ctab_constant* constant_from_handle(D3DXHANDLE handle)
{
    return (ctab_constant*)handle;
}

static inline D3DXHANDLE handle_from_constant( ctab_constant *constant)
{
    return (D3DXHANDLE)constant;
}

static ctab_constant* get_constant_by_name(ID3DXConstantTableImpl*, ctab_constant*, LPCSTR);

static ctab_constant* get_constant_element_by_name(ctab_constant* constant, LPCSTR name)
{
    UINT element;
    LPCSTR part;

    if(!name || !*name) return NULL;

    element = atoi(name);
    part = strchr(name, ']') + 1;

    if(constant->desc.Elements > element)
    {
        struct ctab_constant* c = constant->constants ? &constant->constants[element] : constant;

        switch(*part++)
        {
            case '.':
                return get_constant_by_name(NULL, c, part);

            case '[':
                return get_constant_element_by_name(c, part);

            case '\0':
                return c;

            default:
                break;
        }
    }

    return NULL;
}

static  ctab_constant* get_constant_by_name(ID3DXConstantTableImpl* table,
                                            ctab_constant* constant, LPCSTR name)
{
    UINT i, count, length;
    struct ctab_constant* handles;
    LPCSTR part;

    if(!name || !*name) return NULL;

    if(!constant)
    {
        count = table->desc.Constants;
        handles = table->constants;
    }
    else
    {
        count = constant->desc.StructMembers;
        handles = constant->constants;
    }

    length = strcspn(name, "[.");
    part = name + length;

    for(i = 0; i < count; i++)
    {
        if(strlen(handles[i].desc.Name) == length && !strncmp(handles[i].desc.Name, name, length))
        {
            switch(*part++)
            {
                case '.':
                    return get_constant_by_name(NULL, &handles[i], part);

                case '[':
                    return get_constant_element_by_name(&handles[i], part);

                default:
                    return &handles[i];
            }
        }
    }

    return NULL;
}

static  ctab_constant* is_valid_sub_constant(ctab_constant* parent, ctab_constant* constant)
{
    UINT i, count;

    /* all variable have at least elements = 1, but no elements */
    if(!parent->constants) return NULL;

    if(parent->desc.Elements > 1) count = parent->desc.Elements;
    else count = parent->desc.StructMembers;

    for(i = 0; i < count; ++i)
    {
        if(&parent->constants[i] == constant)
            return constant;

        if(is_valid_sub_constant(&parent->constants[i], constant))
            return constant;
    }

    return NULL;
}

static inline  ctab_constant* is_valid_constant(ID3DXConstantTableImpl* table, D3DXHANDLE handle)
{
    struct ctab_constant* c = constant_from_handle(handle);
    UINT i;

    if(!c) return NULL;

    for(i = 0; i < table->desc.Constants; ++i)
    {
        if(&table->constants[i] == c)
            return c;

        if(is_valid_sub_constant(&table->constants[i], c))
            return c;
    }

    return NULL;
}

static inline ctab_constant* get_valid_constant(ID3DXConstantTableImpl* table, D3DXHANDLE handle)
{
    struct ctab_constant* constant = is_valid_constant(table, handle);

    if(!constant) constant = get_constant_by_name(table, NULL, handle);

    return constant;
}

static inline void set_float_shader_constant(ID3DXConstantTableImpl* table, IDirect3DDevice9* device,
                                             UINT register_index, const FLOAT* data, UINT count)
{
    if(is_vertex_shader(table->desc.Version))
        IDirect3DDevice9_SetVertexShaderConstantF(device, register_index, data, count);
    else
        IDirect3DDevice9_SetPixelShaderConstantF(device, register_index, data, count);
}

static ULONG WINAPI ID3DXConstantTableImpl_Release(ID3DXConstantTableImpl* This)
{
    ULONG ref = InterlockedDecrement(&This->ref);

    if(!ref)
    {
        free_constant_table(This);
        HeapFree(GetProcessHeap(), 0, This);
    }

    return ref;
}

/*** ID3DXBuffer methods ***/
static LPVOID WINAPI ID3DXConstantTableImpl_GetBufferPointer(ID3DXConstantTableImpl* This)
{
    return This->ctab;
}

static DWORD WINAPI ID3DXConstantTableImpl_GetBufferSize(ID3DXConstantTableImpl* This)
{
    return This->size;
}

/*** ID3DXConstantTable methods ***/
static HRESULT WINAPI ID3DXConstantTableImpl_GetDesc(ID3DXConstantTableImpl* This, D3DXCONSTANTTABLE_DESC* desc)
{

    if(!desc)
        return D3DERR_INVALIDCALL;

    *desc = This->desc;

    return D3D_OK;
}

static HRESULT WINAPI ID3DXConstantTableImpl_GetConstantDesc(ID3DXConstantTableImpl* This, D3DXHANDLE constant,
                                                             D3DXCONSTANT_DESC* desc, UINT* count)
{
    ctab_constant* c = get_valid_constant(This, constant);

    if(!c)
    {
        return D3DERR_INVALIDCALL;
    }

    if(desc) *desc = c->desc;
    if(count) *count = 1;

    return D3D_OK;
}

static UINT WINAPI ID3DXConstantTableImpl_GetSamplerIndex(ID3DXConstantTableImpl* This, D3DXHANDLE constant)
{
    ctab_constant* c = get_valid_constant(This, constant);

    if(!c || c->desc.RegisterSet != D3DXRS_SAMPLER)
    {
        return (UINT)-1;
    }

    return c->desc.RegisterIndex;
}

static D3DXHANDLE WINAPI ID3DXConstantTableImpl_GetConstant(ID3DXConstantTableImpl* This, D3DXHANDLE constant, UINT index)
{
    ctab_constant* c;

    if(constant)
    {
        c = get_valid_constant(This, constant);
        if(c && index < c->desc.StructMembers)
        {
            c = &c->constants[index];
            return handle_from_constant(c);
        }
    }
    else
    {
        if(index < This->desc.Constants)
        {
            c = &This->constants[index];
            return handle_from_constant(c);
        }
    }

    return NULL;
}

static D3DXHANDLE WINAPI ID3DXConstantTableImpl_GetConstantByName(ID3DXConstantTableImpl* This, D3DXHANDLE constant, LPCSTR name)
{
    ctab_constant* c = get_valid_constant(This, constant);

    c = get_constant_by_name(This, c, name);

    return handle_from_constant(c);
}

static D3DXHANDLE WINAPI ID3DXConstantTableImpl_GetConstantElement(ID3DXConstantTableImpl* This, D3DXHANDLE constant, UINT index)
{
    ctab_constant* c = get_valid_constant(This, constant);


    if(c && index < c->desc.Elements)
    {
        if(c->desc.Elements > 1) c = &c->constants[index];
        return handle_from_constant(c);
    }

    return NULL;
}

static HRESULT set_scalar_array(ID3DXConstantTableImpl* This, IDirect3DDevice9* device, D3DXHANDLE constant, const void* data,
                                UINT count, D3DXPARAMETER_TYPE type)
{
    D3DXCONSTANT_DESC desc;
    HRESULT hr;
    UINT i, desc_count = 1;
    float row[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    hr = ID3DXConstantTableImpl_GetConstantDesc(This, constant, &desc, &desc_count);
    if(FAILED(hr))
    {
        return D3DERR_INVALIDCALL;
    }

    if(desc.Class != D3DXPC_SCALAR)
        return D3D_OK;

    switch(desc.RegisterSet)
    {
        case D3DXRS_FLOAT4:
            for(i = 0; i < min(count, desc.RegisterCount); i++)
            {
                /* We need the for loop since each IDirect3DDevice9_Set*ShaderConstantF expects a float4 */
                switch(type)
                {
                    case D3DXPT_FLOAT:
                        row[0] = ((float*)data)[i];
                        break;
                    case D3DXPT_INT:
                        row[0] = (float)((int*)data)[i];
                        break;
                    case D3DXPT_BOOL:
                        row[0] = ((BOOL*)data)[i] ? 1.0f : 0.0f;
                        break;
                    default:
                        return D3DERR_INVALIDCALL;
                }
                set_float_shader_constant(This, device, desc.RegisterIndex + i, row, 1);
            }
            break;
        default:
            return E_NOTIMPL;
    }

    return D3D_OK;
}

static HRESULT set_vector_array(ID3DXConstantTableImpl* This, IDirect3DDevice9* device, D3DXHANDLE constant, const void* data,
                                UINT count, D3DXPARAMETER_TYPE type)
{
    D3DXCONSTANT_DESC desc;
    HRESULT hr;
    UINT i, j, desc_count = 1;
    float vec[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    hr = ID3DXConstantTableImpl_GetConstantDesc(This, constant, &desc, &desc_count);
    if(FAILED(hr))
    {
        return D3DERR_INVALIDCALL;
    }

    if(desc.Class == D3DXPC_MATRIX_ROWS || desc.Class == D3DXPC_MATRIX_COLUMNS)
        return D3D_OK;

    switch(desc.RegisterSet)
    {
        case D3DXRS_FLOAT4:
            for(i = 0; i < min(count, desc.RegisterCount); i++)
            {
                switch(type)
                {
                    case D3DXPT_FLOAT:
                        memcpy(vec, ((float*)data) + i * desc.Columns, desc.Columns * sizeof(float));
                        break;
                    case D3DXPT_INT:
                        for(j = 0; j < desc.Columns; j++)
                            vec[j] = (float)((int*)data)[i * desc.Columns + j];
                        break;
                    case D3DXPT_BOOL:
                        for(j = 0; j < desc.Columns; j++)
                            vec[j] = ((BOOL*)data)[i * desc.Columns + j] ? 1.0f : 0.0f;
                        break;
                    default:
                        return D3DERR_INVALIDCALL;
                }

                set_float_shader_constant(This, device, desc.RegisterIndex + i, vec, 1);
            }
            break;
        default:
            return E_NOTIMPL;
    }

    return D3D_OK;
}

static HRESULT set_float_matrix(FLOAT* matrix, const D3DXCONSTANT_DESC* desc,
                                UINT row_offset, UINT column_offset, UINT rows, UINT columns,
                                const void* data, D3DXPARAMETER_TYPE type, UINT src_columns)
{
    UINT i, j;

    switch(type)
    {
        case D3DXPT_FLOAT:
            for(i = 0; i < rows; i++)
            {
                for(j = 0; j < columns; j++)
                    matrix[i * row_offset + j * column_offset] = ((FLOAT*)data)[i * src_columns + j];
            }
            break;
        case D3DXPT_INT:
            for(i = 0; i < rows; i++)
            {
                for(j = 0; j < columns; j++)
                    matrix[i * row_offset + j * column_offset] = ((INT*)data)[i * src_columns + j];
            }
            break;
        default:
            return D3DERR_INVALIDCALL;
    }

    return D3D_OK;
}

static HRESULT set_matrix_array(ID3DXConstantTableImpl* This, IDirect3DDevice9* device, D3DXHANDLE constant, const void* data,
                                UINT count, D3DXPARAMETER_CLASS classa, D3DXPARAMETER_TYPE type, UINT rows, UINT columns)
{
    struct ctab_constant* c = get_valid_constant(This, constant);
    D3DXCONSTANT_DESC* desc;
    UINT registers_per_matrix, num_rows, num_columns, i;
    UINT row_offset = 1, column_offset = 1;
    const DWORD* data_ptr;
    FLOAT matrix[16] = {0.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 0.0f};

    if(!c)
    {
        return D3DERR_INVALIDCALL;
    }
    desc = &c->desc;

    if(desc->Class == D3DXPC_MATRIX_ROWS
       || desc->Class == D3DXPC_MATRIX_COLUMNS
       || desc->Class == D3DXPC_VECTOR
       || desc->Class == D3DXPC_SCALAR)
    {
        if(desc->Class == classa) row_offset = 4;
        else column_offset = 4;

        if(classa == D3DXPC_MATRIX_ROWS)
        {
            if(desc->Class == D3DXPC_VECTOR) return D3D_OK;

            num_rows = desc->Rows;
            num_columns = desc->Columns;
        }
        else
        {
            num_rows = desc->Columns;
            num_columns = desc->Rows;
        }

        registers_per_matrix = (desc->Class == D3DXPC_MATRIX_COLUMNS) ? desc->Columns : desc->Rows;
    }
    else
    {
        return E_NOTIMPL;
    }

    switch(desc->RegisterSet)
    {
        case D3DXRS_FLOAT4:
            data_ptr = (DWORD*)data;
            for(i = 0; i < count; i++)
            {
                HRESULT hr;

                if(registers_per_matrix * (i + 1) > desc->RegisterCount)
                    break;

                hr = set_float_matrix(matrix, desc, row_offset, column_offset, num_rows, num_columns, data_ptr, type, columns);
                if(FAILED(hr)) return hr;

                set_float_shader_constant(This, device, desc->RegisterIndex + i * registers_per_matrix, matrix, registers_per_matrix);

                data_ptr += rows * columns;
            }
            break;
        default:
            return E_NOTIMPL;
    }

    return D3D_OK;
}

static HRESULT set_matrix_pointer_array(ID3DXConstantTableImpl* This, IDirect3DDevice9* device, D3DXHANDLE constant,
                                        const D3DXMATRIX** data, UINT count, D3DXPARAMETER_CLASS classa)
{
    D3DXCONSTANT_DESC desc;
    HRESULT hr;
    UINT registers_per_matrix;
    UINT i, desc_count = 1;
    UINT num_rows, num_columns;
    UINT row_offset, column_offset;
    FLOAT matrix[16] = {0.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 0.0f};

    hr = ID3DXConstantTableImpl_GetConstantDesc(This, constant, &desc, &desc_count);
    if(FAILED(hr))
    {
        return D3DERR_INVALIDCALL;
    }

    if(desc.Class == D3DXPC_MATRIX_ROWS || desc.Class == D3DXPC_MATRIX_COLUMNS)
    {
        if(desc.Class == classa)
        {
            column_offset = 1;
            row_offset = 4;
        }
        else
        {
            column_offset = 4;
            row_offset = 1;
        }

        if(classa == D3DXPC_MATRIX_ROWS)
        {
            num_rows = desc.Rows;
            num_columns = desc.Columns;
        }
        else
        {
            num_rows = desc.Columns;
            num_columns = desc.Rows;
        }

        registers_per_matrix = (desc.Class == D3DXPC_MATRIX_ROWS) ? desc.Rows : desc.Columns;
    }
    else if(desc.Class == D3DXPC_SCALAR)
    {
        registers_per_matrix = 1;
        column_offset = 1;
        row_offset = 1;
        num_rows = desc.Rows;
        num_columns = desc.Columns;
    }
    else if(desc.Class == D3DXPC_VECTOR)
    {
        registers_per_matrix = 1;

        if(classa == D3DXPC_MATRIX_ROWS)
        {
            column_offset = 1;
            row_offset = 4;
            num_rows = desc.Rows;
            num_columns = desc.Columns;
        }
        else
        {
            column_offset = 4;
            row_offset = 1;
            num_rows = desc.Columns;
            num_columns = desc.Rows;
        }
    }
    else
    {
        return D3D_OK;
    }

    switch(desc.RegisterSet)
    {
        case D3DXRS_FLOAT4:
            for(i = 0; i < count; i++)
            {
                if(registers_per_matrix * (i + 1) > desc.RegisterCount)
                    break;

                hr = set_float_matrix(matrix, &desc, row_offset, column_offset, num_rows, num_columns, *data, D3DXPT_FLOAT, 4);
                if(FAILED(hr)) return hr;

                set_float_shader_constant(This, device, desc.RegisterIndex + i * registers_per_matrix, matrix, registers_per_matrix);

                data++;
            }
            break;
        default:
            return E_NOTIMPL;
    }

    return D3D_OK;
}

static HRESULT WINAPI ID3DXConstantTableImpl_SetDefaults(ID3DXConstantTableImpl* This, LPDIRECT3DDEVICE9 device)
{
    UINT i;

    if(!device)
        return D3DERR_INVALIDCALL;

    for(i = 0; i < This->desc.Constants; i++)
    {
        D3DXCONSTANT_DESC* desc = &This->constants[i].desc;

        if(!desc->DefaultValue)
            continue;

        set_float_shader_constant(This, device, desc->RegisterIndex, (float*)desc->DefaultValue, desc->RegisterCount);
    }

    return D3D_OK;
}

static HRESULT WINAPI ID3DXConstantTableImpl_SetValue(ID3DXConstantTableImpl* This, LPDIRECT3DDEVICE9 device,
                                                      D3DXHANDLE constant, LPCVOID data, UINT bytes)
{
    HRESULT hr;
    UINT elements;
    UINT count = 1;
    D3DXCONSTANT_DESC desc;

    if(!device || !constant || !data)
        return D3DERR_INVALIDCALL;

    hr = ID3DXConstantTableImpl_GetConstantDesc(This, constant, &desc, &count);
    if(FAILED(hr))
        return hr;

    elements = bytes / (desc.Bytes / desc.Elements);

    switch(desc.Class)
    {
        case D3DXPC_SCALAR:
            return set_scalar_array(This, device, constant, data, elements, desc.Type);
        case D3DXPC_VECTOR:
            return set_vector_array(This, device, constant, data, elements, desc.Type);
        case D3DXPC_MATRIX_ROWS:
        case D3DXPC_MATRIX_COLUMNS:
            return set_matrix_array(This, device, constant, data, elements,
                                    D3DXPC_MATRIX_ROWS, desc.Type, desc.Rows, desc.Columns);
        default:
            return D3DERR_INVALIDCALL;
    }
}

static HRESULT WINAPI ID3DXConstantTableImpl_SetBool(ID3DXConstantTableImpl* This, LPDIRECT3DDEVICE9 device,
                                                     D3DXHANDLE constant, BOOL b)
{
    return set_scalar_array(This, device, constant, &b, 1, D3DXPT_BOOL);
}

static HRESULT WINAPI ID3DXConstantTableImpl_SetBoolArray(ID3DXConstantTableImpl* This, LPDIRECT3DDEVICE9 device,
                                                          D3DXHANDLE constant, CONST BOOL* b, UINT count)
{
    return set_scalar_array(This, device, constant, b, count, D3DXPT_BOOL);
}

static HRESULT WINAPI ID3DXConstantTableImpl_SetInt(ID3DXConstantTableImpl* This, LPDIRECT3DDEVICE9 device, D3DXHANDLE constant, INT n)
{
    return set_scalar_array(This, device, constant, &n, 1, D3DXPT_INT);
}

static HRESULT WINAPI ID3DXConstantTableImpl_SetIntArray(ID3DXConstantTableImpl* This, LPDIRECT3DDEVICE9 device,
                                                         D3DXHANDLE constant, CONST INT* n, UINT count)
{
    return set_scalar_array(This, device, constant, n, count, D3DXPT_INT);
}

static HRESULT WINAPI ID3DXConstantTableImpl_SetFloat(ID3DXConstantTableImpl* This, LPDIRECT3DDEVICE9 device,
                                                      D3DXHANDLE constant, FLOAT f)
{
    return set_scalar_array(This, device, constant, &f, 1, D3DXPT_FLOAT);
}

static HRESULT WINAPI ID3DXConstantTableImpl_SetFloatArray(ID3DXConstantTableImpl* This, LPDIRECT3DDEVICE9 device,
                                                           D3DXHANDLE constant, CONST FLOAT *f, UINT count)
{
    return set_scalar_array(This, device, constant, f, count, D3DXPT_FLOAT);
}

static HRESULT WINAPI ID3DXConstantTableImpl_SetVector(ID3DXConstantTableImpl* This, LPDIRECT3DDEVICE9 device,
                                                       D3DXHANDLE constant, CONST D3DXVECTOR4 *vector)
{
    return set_vector_array(This, device, constant, vector, 1, D3DXPT_FLOAT);
}

static HRESULT WINAPI ID3DXConstantTableImpl_SetVectorArray(ID3DXConstantTableImpl* This, LPDIRECT3DDEVICE9 device,
                                                            D3DXHANDLE constant, CONST D3DXVECTOR4 *vector, UINT count)
{
    return set_vector_array(This, device, constant, vector, count, D3DXPT_FLOAT);
}

 HRESULT WINAPI ID3DXConstantTableImpl_SetMatrix(ID3DXConstantTableImpl* This, LPDIRECT3DDEVICE9 device,
                                                       D3DXHANDLE constant, CONST XMMATRIX *matrix)
{
    return set_matrix_array(This, device, constant, matrix, 1, D3DXPC_MATRIX_ROWS, D3DXPT_FLOAT, 4, 4);
}

static HRESULT WINAPI ID3DXConstantTableImpl_SetMatrixArray(ID3DXConstantTableImpl* This, LPDIRECT3DDEVICE9 device,
                                                            D3DXHANDLE constant, CONST D3DXMATRIX *matrix, UINT count)
{
    return set_matrix_array(This, device, constant, matrix, count, D3DXPC_MATRIX_ROWS, D3DXPT_FLOAT, 4, 4);
}

static HRESULT WINAPI ID3DXConstantTableImpl_SetMatrixPointerArray(ID3DXConstantTableImpl* This, LPDIRECT3DDEVICE9 device,
                                                                   D3DXHANDLE constant, CONST D3DXMATRIX **matrix, UINT count)
{
    return set_matrix_pointer_array(This, device, constant, matrix, count, D3DXPC_MATRIX_ROWS);
}

 HRESULT WINAPI ID3DXConstantTableImpl_SetMatrixTranspose(ID3DXConstantTableImpl* This, LPDIRECT3DDEVICE9 device,
                                                                D3DXHANDLE constant, CONST XMMATRIX*matrix)
{
    return set_matrix_array(This, device, constant, matrix, 1, D3DXPC_MATRIX_COLUMNS, D3DXPT_FLOAT, 4, 4);
}

static HRESULT WINAPI ID3DXConstantTableImpl_SetMatrixTransposeArray(ID3DXConstantTableImpl* This, LPDIRECT3DDEVICE9 device,
                                                                     D3DXHANDLE constant, CONST D3DXMATRIX* matrix, UINT count)
{
    return set_matrix_array(This, device, constant, matrix, count, D3DXPC_MATRIX_COLUMNS, D3DXPT_FLOAT, 4, 4);
}

static HRESULT WINAPI ID3DXConstantTableImpl_SetMatrixTransposePointerArray(ID3DXConstantTableImpl* This, LPDIRECT3DDEVICE9 device,
                                                                            D3DXHANDLE constant, CONST D3DXMATRIX** matrix, UINT count)
{
    return set_matrix_pointer_array(This, device, constant, matrix, count, D3DXPC_MATRIX_COLUMNS);
}

static HRESULT parse_ctab_constant_type(const char* ctab, DWORD typeoffset, struct ctab_constant* constant,
                                        BOOL is_element, WORD index, WORD max, DWORD* offset, DWORD nameoffset, UINT regset)
{
    const D3DXSHADER_TYPEINFO* type = (LPD3DXSHADER_TYPEINFO)(ctab + typeoffset);
    const D3DXSHADER_STRUCTMEMBERINFO* memberinfo = NULL;
    HRESULT hr = D3D_OK;
    UINT i, count = 0;
    WORD size = 0;

    constant->desc.DefaultValue = offset ? ctab + *offset : NULL;
    constant->desc.Class = *(D3DXPARAMETER_CLASS*)type->Class;
    constant->desc.Type = *(D3DXPARAMETER_TYPE*)type->Type;
    constant->desc.Rows = type->Rows;
    constant->desc.Columns = type->Columns;
    constant->desc.Elements = is_element ? 1 : type->Elements;
    constant->desc.StructMembers = type->StructMembers;
    constant->desc.Name = ctab + nameoffset;
    constant->desc.RegisterSet = (D3DXREGISTER_SET)regset;
    constant->desc.RegisterIndex = index;

    if(type->Elements > 1 && !is_element)
    {
        count = type->Elements;
    }
    else if((type->Class == D3DXPC_STRUCT) && type->StructMembers)
    {
        memberinfo = (D3DXSHADER_STRUCTMEMBERINFO*)(ctab + type->StructMemberInfo);
        count = type->StructMembers;
    }

    if(count)
    {
        constant->constants = (ctab_constant*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*constant->constants) * count);
        if(!constant->constants)
        {
            hr = E_OUTOFMEMORY;
            goto error;
        }

        for(i = 0; i < count; ++i)
        {
            hr = parse_ctab_constant_type(ctab, memberinfo ? memberinfo[i].TypeInfo : typeoffset,
                                          &constant->constants[i], memberinfo == NULL, index + size, max, offset,
                                          memberinfo ? memberinfo[i].Name : nameoffset, regset);
            if(hr != D3D_OK)
                goto error;

            size += constant->constants[i].desc.RegisterCount;
        }
    }
    else
    {
        WORD offsetdiff = 0;

        switch(type->Class)
        {
            case D3DXPC_SCALAR:
            case D3DXPC_VECTOR:
                offsetdiff = 1;
                size = 1;
                break;

            case D3DXPC_MATRIX_ROWS:
                size = is_element ? type->Rows : max(type->Rows, type->Columns);
                offsetdiff = type->Rows;
                break;

            case D3DXPC_MATRIX_COLUMNS:
                size = type->Columns;
                offsetdiff = type->Columns;
                break;

            case D3DXPC_OBJECT:
                size = 1;
                break;

            default:
                break;
        }

        /* offset in bytes => offsetdiff * components(4) * sizeof(DWORD) */
        if(offset) *offset += offsetdiff * 4 * 4;
    }

    constant->desc.RegisterCount = max(0, min(max - index, size));
    constant->desc.Bytes = calc_bytes(&constant->desc);

    return D3D_OK;

error:
    if(constant->constants)
    {
        for(i = 0; i < count; ++i)
        {
            free_constant(&constant->constants[i]);
        }
        HeapFree(GetProcessHeap(), 0, constant->constants);
        constant->constants = NULL;
    }

    return hr;
}

HRESULT WINAPI D3DX_GetShaderConstantTableEx(CONST DWORD* byte_code,
                                             DWORD flags,
                                             ID3DXConstantTableImpl** constant_table)
{
    ID3DXConstantTableImpl* object = NULL;
    HRESULT hr;
    LPCVOID data;
    UINT size;
    const D3DXSHADER_CONSTANTTABLE* ctab_header;
    D3DXSHADER_CONSTANTINFO* constant_info;
    DWORD i;

    if(!byte_code || !constant_table)
    {
        return D3DERR_INVALIDCALL;
    }

    hr = D3DXFindShaderComment(byte_code, MAKEFOURCC('C', 'T', 'A', 'B'), &data, &size);
    if(hr != D3D_OK)
    {
        return D3DXERR_INVALIDDATA;
    }

    if(size < sizeof(D3DXSHADER_CONSTANTTABLE))
    {
        return D3DXERR_INVALIDDATA;
    }

    ctab_header = (const D3DXSHADER_CONSTANTTABLE*)data;
    if(ctab_header->Size != sizeof(D3DXSHADER_CONSTANTTABLE))
    {
        return D3DXERR_INVALIDDATA;
    }

    object = (ID3DXConstantTableImpl*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*object));
    if(!object)
    {
        return E_OUTOFMEMORY;
    }

    object->ref = 1;

    object->ctab = (char*)HeapAlloc(GetProcessHeap(), 0, size);
    if(!object->ctab)
    {
        HeapFree(GetProcessHeap(), 0, object);
        return E_OUTOFMEMORY;
    }
    object->size = size;
    memcpy(object->ctab, data, object->size);

    object->desc.Creator = ctab_header->Creator ? object->ctab + ctab_header->Creator : NULL;
    object->desc.Version = ctab_header->Version;
    object->desc.Constants = ctab_header->Constants;

    object->constants = (ctab_constant*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                                  sizeof(*object->constants) * object->desc.Constants);
    if(!object->constants)
    {
        hr = E_OUTOFMEMORY;
        goto error;
    }

    constant_info = (LPD3DXSHADER_CONSTANTINFO)(object->ctab + ctab_header->ConstantInfo);
    for(i = 0; i < ctab_header->Constants; i++)
    {
        DWORD offset = constant_info[i].DefaultValue;

        hr = parse_ctab_constant_type(object->ctab, constant_info[i].TypeInfo,
                                      &object->constants[i], FALSE, constant_info[i].RegisterIndex,
                                      constant_info[i].RegisterIndex + constant_info[i].RegisterCount,
                                      offset ? &offset : NULL, constant_info[i].Name, constant_info[i].RegisterSet);
        if(hr != D3D_OK)
            goto error;
    }

    *constant_table = object;
    return D3D_OK;

error:
    free_constant_table(object);
    HeapFree(GetProcessHeap(), 0, object);

    return hr;
}

HRESULT WINAPI D3DX_GetShaderConstantTable(CONST DWORD* byte_code,
                                           ID3DXConstantTableImpl** constant_table)
{
    return D3DX_GetShaderConstantTableEx(byte_code, 0, constant_table);
}

HRESULT WINAPI D3DX_GetShaderSamplers(CONST DWORD* byte_code, LPCSTR* samplers, UINT* count)
{
    HRESULT hr;
    UINT i, sampler_count = 0;
    UINT size;
    LPCSTR data;
    const D3DXSHADER_CONSTANTTABLE* ctab_header;
    const D3DXSHADER_CONSTANTINFO* constant_info;

    if(count) *count = 0;

    hr = D3DX_FindShaderComment(byte_code, MAKEFOURCC('C', 'T', 'A', 'B'), (LPCVOID*)&data, &size);
    if(hr != D3D_OK) return D3D_OK;

    if(size < sizeof(D3DXSHADER_CONSTANTTABLE)) return D3D_OK;

    ctab_header = (const D3DXSHADER_CONSTANTTABLE*)data;
    if(ctab_header->Size != sizeof(*ctab_header)) return D3D_OK;

    constant_info = (D3DXSHADER_CONSTANTINFO*)(data + ctab_header->ConstantInfo);
    for(i = 0; i < ctab_header->Constants; i++)
    {
        const D3DXSHADER_TYPEINFO* type;

        type = (D3DXSHADER_TYPEINFO*)(data + constant_info[i].TypeInfo);

        if(type->Type == D3DXPT_SAMPLER
           || type->Type == D3DXPT_SAMPLER1D
           || type->Type == D3DXPT_SAMPLER2D
           || type->Type == D3DXPT_SAMPLER3D
           || type->Type == D3DXPT_SAMPLERCUBE)
        {
            if(samplers) samplers[sampler_count] = data + constant_info[i].Name;

            ++sampler_count;
        }
    }

    if(count) *count = sampler_count;

    return D3D_OK;
}