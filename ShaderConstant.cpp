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


#undef min
#undef max

#include "windef.h"
#include "wingdi.h"
#include "objbase.h"
#include "d3dcommon.h"
#include "d3dcompiler.h"

#include "winbase.h"
#include "wingdi.h"
#include "winuser.h"
#include "d3dx9.h"

HRESULT find_shader_comment(uint32_t* byte_code, uint32_t fourcc, void** data, uint32_t* size)
{
    uint32_t* ptr = byte_code;
    if(data) *data = NULL;
    if(size) *size = 0;

    if(!byte_code)
        return D3DERR_INVALIDCALL;

    while(*++ptr != D3DSIO_END)
    {
        /* Check if it is a comment */
        if((*ptr & D3DSI_OPCODE_MASK) == D3DSIO_COMMENT)
        {
            uint32_t comment_size = (*ptr & D3DSI_COMMENTSIZE_MASK) >> D3DSI_COMMENTSIZE_SHIFT;

            /* Check if this is the comment we are looking for */
            if(*(ptr + 1) == fourcc)
            {
                uint32_t ctab_size = (comment_size - 1) * sizeof(uint32_t);
                void* ctab_data = ptr + 2;
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

void free_constant(ctab_constant* constant)
{
    if(constant->constants)
    {
        uint32_t i, count = constant->desc.Elements > 1 ? constant->desc.Elements : constant->desc.StructMembers;

        for(i = 0; i < count; ++i)
        {
            free_constant(&constant->constants[i]);
        }
        delete[] constant->constants;
    }
}

void free_constant_table(ID3DXConstantTableImpl* table)
{
    if(table->constants)
    {
        uint32_t i;

        for(i = 0; i < table->desc.Constants; ++i)
        {
            free_constant(&table->constants[i]);
        }
        delete[] table->constants;
    }
    delete[] table->ctab;
}


inline int is_vertex_shader(uint32_t version)
{
    return (version & 0xFFFF0000) == 0xFFFE0000;
}

uint32_t calc_bytes(D3DXCONSTANT_DESC* desc)
{
    return 4 * desc->Elements * desc->Rows * desc->Columns;
}

inline ctab_constant* constant_from_handle(D3DXHANDLE handle)
{
    return (ctab_constant*)handle;
}

inline D3DXHANDLE handle_from_constant( ctab_constant *constant)
{
    return (D3DXHANDLE)constant;
}

ctab_constant* get_constant_by_name(ID3DXConstantTableImpl*, ctab_constant*, const char*);

ctab_constant* get_constant_element_by_name(ctab_constant* constant, const char* name)
{
    uint32_t element;
    const char* part;

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

ctab_constant* get_constant_by_name(ID3DXConstantTableImpl* table,
                                    ctab_constant* constant, const char* name)
{
    uint32_t count, length;
    ctab_constant* handles;
    const char* part;

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

    for(size_t i = 0; i < count; i++)
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

ctab_constant* is_valid_sub_constant(ctab_constant* parent, ctab_constant* constant)
{
    uint32_t i, count;

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

inline ctab_constant* is_valid_constant(ID3DXConstantTableImpl* table, D3DXHANDLE handle)
{
    ctab_constant* c = (ctab_constant*)handle;
    uint32_t i;

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

 inline ctab_constant* get_valid_constant(ID3DXConstantTableImpl* table, D3DXHANDLE handle)
{
    ctab_constant* constant = is_valid_constant(table, handle);

    if(!constant) 
        constant = get_constant_by_name(table, NULL, handle);

    return constant;
}

inline void set_float_shader_constant(ID3DXConstantTableImpl* table,
                                             uint32_t register_index, const void* data, uint32_t count)
{
    if(is_vertex_shader(table->desc.Version))
        _rwD3D9SetVertexShaderConstant(register_index, data, count);
    else
        _rwD3D9SetPixelShaderConstant(register_index, data, count);
}

 HRESULT ID3DXConstantTableImpl_GetConstantDesc(ID3DXConstantTableImpl* This, D3DXHANDLE constant,
                                                             D3DXCONSTANT_DESC* desc, uint32_t* count)
{
    ctab_constant* c = get_valid_constant(This, constant);

    if(!c)
    {
        return D3DERR_INVALIDCALL;
    }

    if(desc) 
        *desc = c->desc;

    if(count) 
        *count = 1;

    return D3D_OK;
}

D3DXHANDLE ID3DXConstantTableImpl_GetConstant(ID3DXConstantTableImpl* This, D3DXHANDLE constant, uint32_t index)
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

D3DXHANDLE WINAPI ID3DXConstantTableImpl_GetConstantByName(ID3DXConstantTableImpl* This, D3DXHANDLE constant, const char* name)
{
    ctab_constant* c = get_valid_constant(This, constant);

    c = get_constant_by_name(This, c, name);

    return handle_from_constant(c);
}

D3DXHANDLE WINAPI ID3DXConstantTableImpl_GetConstantElement(ID3DXConstantTableImpl* This, D3DXHANDLE constant, uint32_t index)
{
    ctab_constant* c = get_valid_constant(This, constant);


    if(c && index < c->desc.Elements)
    {
        if(c->desc.Elements > 1) c = &c->constants[index];
        return handle_from_constant(c);
    }

    return NULL;
}

HRESULT set_scalar_array(ID3DXConstantTableImpl* This, D3DXHANDLE constant, void* data,
                                uint32_t count, D3DXPARAMETER_TYPE type)
{
    D3DXCONSTANT_DESC desc;
    HRESULT hr;
    uint32_t i, desc_count = 1;
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
            for(i = 0; i < std::min(count, desc.RegisterCount); i++)
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
                        row[0] = ((bool*)data)[i] ? 1.0f : 0.0f;
                        break;
                    default:
                        return D3DERR_INVALIDCALL;
                }
                set_float_shader_constant(This, desc.RegisterIndex + i, row, 1);
            }
            break;
        default:
            return E_NOTIMPL;
    }

    return D3D_OK;
}

HRESULT set_vector_array(ID3DXConstantTableImpl* This, D3DXHANDLE constant,  void* data,
                                uint32_t count, D3DXPARAMETER_TYPE type)
{
    D3DXCONSTANT_DESC desc;
    HRESULT hr;
    uint32_t i, j, desc_count = 1;
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
            for(i = 0; i < std::min(count, desc.RegisterCount); i++)
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
                            vec[j] = ((bool*)data)[i * desc.Columns + j] ? 1.0f : 0.0f;
                        break;
                    default:
                        return D3DERR_INVALIDCALL;
                }

                set_float_shader_constant(This, desc.RegisterIndex + i, vec, 1);
            }
            break;
        default:
            return E_NOTIMPL;
    }

    return D3D_OK;
}

HRESULT set_float_matrix(float* matrix, const D3DXCONSTANT_DESC* desc,
                         uint32_t row_offset, uint32_t column_offset, uint32_t rows, uint32_t columns,
                         void* data, D3DXPARAMETER_TYPE type, uint32_t src_columns)
{
    uint32_t i, j;

    switch(type)
    {
        case D3DXPT_FLOAT:
            for(i = 0; i < rows; i++)
            {
                for(j = 0; j < columns; j++)
                    matrix[i * row_offset + j * column_offset] = ((float*)data)[i * src_columns + j];
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

HRESULT set_matrix_array(ID3DXConstantTableImpl* This, D3DXHANDLE constant, void* data,
                         uint32_t count, D3DXPARAMETER_CLASS classa, D3DXPARAMETER_TYPE type, uint32_t rows, uint32_t columns)
{
    ctab_constant* c = get_valid_constant(This, constant);
    D3DXCONSTANT_DESC* desc;
    uint32_t registers_per_matrix, num_rows, num_columns, i;
    uint32_t row_offset = 1, column_offset = 1;
    uint32_t* data_ptr;
    float matrix[16] = {0.0f, 0.0f, 0.0f, 0.0f,
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
            data_ptr = reinterpret_cast<uint32_t*>(data);
            for(i = 0; i < count; i++)
            {
                HRESULT hr;

                if(registers_per_matrix * (i + 1) > desc->RegisterCount)
                    break;

                hr = set_float_matrix(matrix, desc, row_offset, column_offset, num_rows, num_columns, data_ptr, type, columns);
                if(FAILED(hr)) return hr;

                set_float_shader_constant(This, desc->RegisterIndex + i * registers_per_matrix, matrix, registers_per_matrix);

                data_ptr += rows * columns;
            }
            break;
        default:
            return E_NOTIMPL;
    }

    return D3D_OK;
}

HRESULT set_matrix_pointer_array(ID3DXConstantTableImpl* This, D3DXHANDLE constant,
                                 XMMATRIX** data, uint32_t count, D3DXPARAMETER_CLASS classa)
{
    D3DXCONSTANT_DESC desc;
    HRESULT hr;
    uint32_t registers_per_matrix;
    uint32_t desc_count = 1;
    uint32_t num_rows, num_columns;
    uint32_t row_offset, column_offset;

    float matrix[16] = {0.0f, 0.0f, 0.0f, 0.0f,
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
            for(size_t i = 0; i < count; i++)
            {
                if(registers_per_matrix * (i + 1) > desc.RegisterCount)
                    break;

                hr = set_float_matrix(matrix, &desc, row_offset, column_offset, num_rows, num_columns, *data, D3DXPT_FLOAT, 4);
                if(FAILED(hr)) 
                    return hr;

                set_float_shader_constant(This, desc.RegisterIndex + i * registers_per_matrix, matrix, registers_per_matrix);

                data++;
            }
            break;
        default:
            return E_NOTIMPL;
    }

    return D3D_OK;
}

HRESULT ID3DXConstantTableImpl_SetDefaults(ID3DXConstantTableImpl* This)
{
    for(size_t i = 0; i < This->desc.Constants; i++)
    {
        D3DXCONSTANT_DESC* desc = &This->constants[i].desc;

        if(desc->DefaultValue == nullptr)
            continue;

        set_float_shader_constant(This, desc->RegisterIndex, (float*)desc->DefaultValue, desc->RegisterCount);
    }

    return D3D_OK;
}

HRESULT parse_ctab_constant_type(char* ctab, uint32_t typeoffset, ctab_constant* constant,
                                 bool is_element, uint16_t index, uint16_t maxl, uint32_t* offset, uint32_t nameoffset, uint32_t regset)
{
    D3DXSHADER_TYPEINFO* type = reinterpret_cast<D3DXSHADER_TYPEINFO*>(ctab + typeoffset);
    D3DXSHADER_STRUCTMEMBERINFO* memberinfo = nullptr;

    HRESULT hr = D3D_OK;

    uint32_t count = 0;
    uint16_t size = 0;

    constant->desc.DefaultValue = offset ? ctab + *offset : NULL;
    constant->desc.Class = static_cast<D3DXPARAMETER_CLASS>(type->Class);
    constant->desc.Type = static_cast<D3DXPARAMETER_TYPE>(type->Type);
    constant->desc.Rows = type->Rows;
    constant->desc.Columns = type->Columns;
    constant->desc.Elements = is_element ? 1 : type->Elements;
    constant->desc.StructMembers = type->StructMembers;
    constant->desc.Name = ctab + nameoffset;
    constant->desc.RegisterSet = static_cast<D3DXREGISTER_SET>(regset);
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
        constant->constants = new ctab_constant[sizeof(*constant->constants) * count];
        if(constant->constants == nullptr)
        {
            for(size_t i = 0; i < count; ++i)
            {
                free_constant(&constant->constants[i]);
            }

            return E_OUTOFMEMORY;
        }

        for(size_t i = 0; i < count; ++i)
        {
            hr = parse_ctab_constant_type(ctab, memberinfo ? memberinfo[i].TypeInfo : typeoffset,
                                          &constant->constants[i], memberinfo == NULL, index + size, maxl, offset,
                                          memberinfo ? memberinfo[i].Name : nameoffset, regset);
            if(hr != D3D_OK)
            {
                for(i = 0; i < count; ++i)
                {
                    free_constant(&constant->constants[i]);
                }
                delete[] constant->constants;
                constant->constants = NULL;

                return hr;
            }

            size += constant->constants[i].desc.RegisterCount;
        }
    }
    else
    {
        uint16_t offsetdiff = 0;

        switch(type->Class)
        {
            case D3DXPC_SCALAR:
            case D3DXPC_VECTOR:
                offsetdiff = 1;
                size = 1;
                break;

            case D3DXPC_MATRIX_ROWS:
                size = is_element ? type->Rows : std::max(type->Rows, type->Columns);
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

        /* offset in bytes => offsetdiff * components(4) * sizeof(uint32_t) */
        if(offset) *offset += offsetdiff * 4 * 4;
    }


    constant->desc.RegisterCount = std::max((uint16_t)0, std::min(uint16_t(maxl - index), size));
    constant->desc.Bytes = calc_bytes(&constant->desc);

    return D3D_OK;
}

HRESULT ConstantTable::Initialize(void* byteCode)
{
    ID3DXConstantTableImpl* object = nullptr;
    D3DXSHADER_CONSTANTTABLE* ctab_header = nullptr;
    D3DXSHADER_CONSTANTINFO* constant_info = nullptr;

    HRESULT hr;

    if(byteCode == nullptr)
    {
        return D3DERR_INVALIDCALL;
    }

    void* data = nullptr;
    uint32_t size;

    hr = find_shader_comment((uint32_t*)byteCode, MAKEFOURCC('C', 'T', 'A', 'B'), &data, &size);
    if(hr != D3D_OK)
    {
        return D3DXERR_INVALIDDATA;
    }

    if(size < sizeof(D3DXSHADER_CONSTANTTABLE))
    {
        return D3DXERR_INVALIDDATA;
    }

    ctab_header = reinterpret_cast <D3DXSHADER_CONSTANTTABLE*>(data);
    if(ctab_header->Size != sizeof(D3DXSHADER_CONSTANTTABLE))
    {
        return D3DXERR_INVALIDDATA;
    }

    object = new ID3DXConstantTableImpl[sizeof(*object)];
    if(object == nullptr)
    {
        return E_OUTOFMEMORY;
    }
    ZeroMemory(object, sizeof(*object));

    object->ctab = new char[size];
    if(object->ctab == nullptr)
    {
        delete[] object;
        return E_OUTOFMEMORY;
    }

    object->size = size;
    memcpy(object->ctab, data, object->size);

    object->desc.Creator = ctab_header->Creator ? object->ctab + ctab_header->Creator : NULL;
    object->desc.Version = ctab_header->Version;
    object->desc.Constants = ctab_header->Constants;

    object->constants = new ctab_constant[sizeof(*object->constants) * object->desc.Constants];

    if(object->constants == nullptr)
    {
        return E_OUTOFMEMORY;
        free_constant_table(object);
        delete[] object;
    }

    constant_info = (LPD3DXSHADER_CONSTANTINFO)(object->ctab + ctab_header->ConstantInfo);
    for(size_t i = 0; i < ctab_header->Constants; i++)
    {
        uint32_t offset = constant_info[i].DefaultValue;
        hr = parse_ctab_constant_type(object->ctab, constant_info[i].TypeInfo,
                                      &object->constants[i], false, constant_info[i].RegisterIndex,
                                      constant_info[i].RegisterIndex + constant_info[i].RegisterCount,
                                      offset ? &offset : NULL, constant_info[i].Name, constant_info[i].RegisterSet);
        if(hr != D3D_OK)
        {
            free_constant_table(object);
            delete[] object;
            return hr;
        }
    }

    mConstantTable = object;
    return D3D_OK;
}

void ConstantTable::Release()
{
    free_constant_table(mConstantTable);
    delete[] mConstantTable;
}

void ConstantTable::SetValue(D3DXHANDLE constant, void* data, uint32_t bytes)
{
    HRESULT hr;
    uint32_t elements;
    uint32_t count = 1;
    D3DXCONSTANT_DESC desc;

    if( !constant || !data)
        return;

    hr = ID3DXConstantTableImpl_GetConstantDesc(mConstantTable, constant, &desc, &count);
    if(FAILED(hr))
        return;

    elements = bytes / (desc.Bytes / desc.Elements);
    switch(desc.Class)
    {
        case D3DXPC_SCALAR:
            set_scalar_array(mConstantTable, constant, data, elements, desc.Type);
        case D3DXPC_VECTOR:
            set_vector_array(mConstantTable, constant, data, elements, desc.Type);
        case D3DXPC_MATRIX_ROWS:
        case D3DXPC_MATRIX_COLUMNS:
            set_matrix_array(mConstantTable, constant, data, elements,
                             D3DXPC_MATRIX_ROWS, desc.Type, desc.Rows, desc.Columns);
        default:
            return;
    }
}

void ConstantTable::SetBool(D3DXHANDLE constant, bool b)
{
    set_scalar_array(mConstantTable, constant, &b, 1, D3DXPT_BOOL);
}

void ConstantTable::SetBoolArray(D3DXHANDLE constant, bool* b, uint32_t count)
{
    set_scalar_array(mConstantTable, constant, &b, count, D3DXPT_BOOL);
}

void ConstantTable::SetInt(D3DXHANDLE constant, int n)
{
    set_scalar_array(mConstantTable, constant, &n, 1, D3DXPT_INT);
}

void ConstantTable::SetIntArray(D3DXHANDLE constant, int* n, uint32_t count)
{
    set_scalar_array(mConstantTable, constant, n, count, D3DXPT_INT);
}

void ConstantTable::SetFloat(D3DXHANDLE constant, float f)
{
    set_scalar_array(mConstantTable, constant, &f, 1, D3DXPT_FLOAT);
}

void ConstantTable::SetFloatArray(D3DXHANDLE constant, float* f, uint32_t count)
{
    set_scalar_array(mConstantTable, constant, f, count, D3DXPT_FLOAT);
}

void ConstantTable::SetVector(D3DXHANDLE constant, XMVECTOR* vector)
{
    set_vector_array(mConstantTable, constant, vector, 1, D3DXPT_FLOAT);
}

void ConstantTable::SetVectorArray(D3DXHANDLE constant, XMVECTOR* vector, uint32_t count)
{
    set_vector_array(mConstantTable, constant, vector, count, D3DXPT_FLOAT);
}

void ConstantTable::SetMatrix(D3DXHANDLE constant, XMMATRIX* matrix)
{
    set_matrix_array(mConstantTable, constant, matrix, 1, D3DXPC_MATRIX_ROWS, D3DXPT_FLOAT, 4, 4);
}

void ConstantTable::SetMatrixArray(D3DXHANDLE constant, XMMATRIX* matrix, uint32_t count)
{
    set_matrix_array(mConstantTable, constant, matrix, count, D3DXPC_MATRIX_ROWS, D3DXPT_FLOAT, 4, 4);
}

void ConstantTable::SetMatrixPointerArray(D3DXHANDLE constant, XMMATRIX** matrix, uint32_t count)
{
    set_matrix_pointer_array(mConstantTable, constant, matrix, count, D3DXPC_MATRIX_ROWS);
}

void ConstantTable::SetMatrixTranspose(D3DXHANDLE constant, XMMATRIX* matrix)
{
    set_matrix_array(mConstantTable, constant, matrix, 1, D3DXPC_MATRIX_COLUMNS, D3DXPT_FLOAT, 4, 4);
}

void ConstantTable::SetMatrixTransposeArray(D3DXHANDLE constant, XMMATRIX* matrix, uint32_t count)
{
    set_matrix_array(mConstantTable, constant, matrix, count, D3DXPC_MATRIX_COLUMNS, D3DXPT_FLOAT, 4, 4);
}

void ConstantTable::SetMatrixTransposePointerArray(D3DXHANDLE constant, XMMATRIX** matrix, uint32_t count)
{
    set_matrix_pointer_array(mConstantTable, constant, matrix, count, D3DXPC_MATRIX_COLUMNS);
}
