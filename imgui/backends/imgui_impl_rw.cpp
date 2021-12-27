// dear imgui: Renderer Backend for DirectX9
// This needs to be used along with a Platform Backend (e.g. Win32)

// Implemented features:
//  [X] Renderer: User texture binding. Use 'LPDIRECT3DTEXTURE9' as ImTextureID. Read the FAQ about ImTextureID!
//  [X] Renderer: Support for large meshes (64k+ vertices) with 16-bit indices.

// You can use unmodified imgui_impl_* files in your project. See examples/ folder for examples of using this.
// Prefer including the entire imgui/ repository into your project (either as a copy or as a submodule), and only build the backends you need.
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// CHANGELOG
// (minor and older changes stripped away, please see git history for details)
//  2021-06-29: Reorganized backend to pull data from a single structure to facilitate usage with multiple-contexts (all g_XXXX access changed to bd->XXXX).
//  2021-06-25: DirectX9: Explicitly disable texture state stages after >= 1.
//  2021-05-19: DirectX9: Replaced direct access to ImDrawCmd::TextureId with a call to ImDrawCmd::GetTexID(). (will become a requirement)
//  2021-04-23: DirectX9: Explicitly setting up more graphics states to increase compatibility with unusual non-default states.
//  2021-03-18: DirectX9: Calling IDirect3DStateBlock9::Capture() after CreateStateBlock() as a workaround for state restoring issues (see #3857).
//  2021-03-03: DirectX9: Added support for IMGUI_USE_BGRA_PACKED_COLOR in user's imconfig file.
//  2021-02-18: DirectX9: Change blending equation to preserve alpha in output buffer.
//  2019-05-29: DirectX9: Added support for large mesh (64K+ vertices), enable ImGuiBackendFlags_RendererHasVtxOffset flag.
//  2019-04-30: DirectX9: Added support for special ImDrawCallback_ResetRenderState callback to reset render state.
//  2019-03-29: Misc: Fixed erroneous assert in ImGui_ImplDX9_InvalidateDeviceObjects().
//  2019-01-16: Misc: Disabled fog before drawing UI's. Fixes issue #2288.
//  2018-11-30: Misc: Setting up io.BackendRendererName so it can be displayed in the About Window.
//  2018-06-08: Misc: Extracted imgui_impl_dx9.cpp/.h away from the old combined DX9+Win32 example.
//  2018-06-08: DirectX9: Use draw_data->DisplayPos and draw_data->DisplaySize to setup projection matrix and clipping rectangle.
//  2018-05-07: Render: Saving/restoring Transform because they don't seem to be included in the StateBlock. Setting shading mode to Gouraud.
//  2018-02-16: Misc: Obsoleted the io.RenderDrawListsFn callback and exposed ImGui_ImplDX9_RenderDrawData() in the .h file so you can call it yourself.
//  2018-02-06: Misc: Removed call to ImGui::Shutdown() which is not available from 1.60 WIP, user needs to call CreateContext/DestroyContext themselves.

#include "imgui.h"
#include "imgui_impl_rw.h"
#include "RenderWare.h"
#include "../GTADef.h"

struct CUSTOMVERTEX
{
    float    pos[3];
    D3DCOLOR col;
    float    uv[2];
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

#ifdef IMGUI_USE_BGRA_PACKED_COLOR
#define IMGUI_COL_TO_DX9_ARGB(_COL)     (_COL)
#else
#define IMGUI_COL_TO_DX9_ARGB(_COL)     (((_COL) & 0xFF00FF00) | (((_COL) & 0xFF0000) >> 16) | (((_COL) & 0xFF) << 16))
#endif
#include"../CommonD.h"

static RwIm2DVertex* g_vertbuf = 0;
static int g_vertbufSize;

// Render function.
void ImGui_ImplRW_RenderDrawData(ImDrawData* draw_data)
{
    // Avoid rendering when minimized
    if(draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
        return;

    if(g_vertbuf == NULL || g_vertbufSize < draw_data->TotalVtxCount)
    {
        if(g_vertbuf)
        {
            RwFree(g_vertbuf);
            g_vertbuf = NULL;
        }
        g_vertbufSize = draw_data->TotalVtxCount + 5000;
        g_vertbuf = (RwIm2DVertex*)RwMalloc(g_vertbufSize * sizeof(RwIm2DVertex), 0);
    }

    RwCamera* camera = (RwCamera*)RWSRCGLOBAL(curCamera);
    RwIm2DVertex* vtx_dst2 = g_vertbuf;
    float recipZ = 1.0f / camera->nearPlane;

    for(int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawVert* vtx_src = cmd_list->VtxBuffer.Data;
        for(int i = 0; i < cmd_list->VtxBuffer.Size; i++)
        {
            vtx_dst2[i].x = vtx_src[i].pos.x + -0.5f;
            vtx_dst2[i].y = vtx_src[i].pos.y + 0.5f;
            vtx_dst2[i].z = 0.0f;
            vtx_dst2[i].rhw = recipZ;
            vtx_dst2[i].emissiveColor = IMGUI_COL_TO_DX9_ARGB(vtx_src[i].col);
            vtx_dst2[i].u = vtx_src[i].uv.x;
            vtx_dst2[i].v = vtx_src[i].uv.y;
        }
        vtx_dst2 += cmd_list->VtxBuffer.Size;
    }

    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
    RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
    ImVec2 clip_off = draw_data->DisplayPos;
    int global_vtx_offset = 0;
    for(int n = 0; n < draw_data->CmdListsCount; n++)
    {
        int global_idx_offset = 0;
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        for(int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if(pcmd->UserCallback != NULL)
            {
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                RwRaster* raster = (RwRaster*)pcmd->TextureId;
                if(raster)
                {
                    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, raster);
                    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
                }
                else
                    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, NULL);

                const RECT r = {(LONG)(pcmd->ClipRect.x - clip_off.x), (LONG)(pcmd->ClipRect.y - clip_off.y), (LONG)(pcmd->ClipRect.z - clip_off.x), (LONG)(pcmd->ClipRect.w - clip_off.y)};

                GetD3DDevice()->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
                GetD3DDevice()->SetScissorRect(&r);
                RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, g_vertbuf + global_vtx_offset, cmd_list->VtxBuffer.Size,
                                             cmd_list->IdxBuffer.Data + global_idx_offset, pcmd->ElemCount);
                GetD3DDevice()->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

            }
            global_idx_offset += pcmd->ElemCount;
        }
        global_vtx_offset += cmd_list->VtxBuffer.Size;
    }
}

bool ImGui_ImplRW_Init(IDirect3DDevice9* device)
{
    return true;
}

void ImGui_ImplRW_Shutdown()
{
}

RwRaster* g_FontTexture = 0;
static bool ImGui_ImplRW_CreateFontsTexture()
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();

    unsigned char* pixels;
    int width, height, bytes_per_pixel;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixel);

    // Convert RGBA32 to BGRA32 (because RGBA32 is not well supported by DX9 devices)
#ifndef IMGUI_USE_BGRA_PACKED_COLOR
    if(io.Fonts->TexPixelsUseColors)
    {
        ImU32* dst_start = (ImU32*)ImGui::MemAlloc((size_t)width * height * bytes_per_pixel);
        for(ImU32* src = (ImU32*)pixels, *dst = dst_start, *dst_end = dst_start + (size_t)width * height; dst < dst_end; src++, dst++)
            *dst = IMGUI_COL_TO_DX9_ARGB(*src);
        pixels = (unsigned char*)dst_start;
    }
#endif

    RwImage* image;
    image = RwImageCreate(width, height, 32);
    RwImageAllocatePixels(image);
    for(int y = 0; y < height; y++)
        memcpy(image->cpPixels + image->stride * y, pixels + width * bytes_per_pixel * y, width * bytes_per_pixel);

    RwRaster* raster = RwRasterCreate(width, height,32, rwRASTERTYPETEXTURE);
    g_FontTexture = RwRasterSetFromImage(raster, image);
    RwImageDestroy(image);

    // Store our identifier
    io.Fonts->SetTexID((ImTextureID)g_FontTexture);
#ifndef IMGUI_USE_BGRA_PACKED_COLOR
    if(io.Fonts->TexPixelsUseColors)
        ImGui::MemFree(pixels);
#endif
    return true;
}

bool ImGui_ImplRW_CreateDeviceObjects()
{
    if (!ImGui_ImplRW_CreateFontsTexture())
        return false;
    return true;
}

void ImGui_ImplRW_NewFrame()
{
    if(!g_FontTexture)
        ImGui_ImplRW_CreateDeviceObjects();
}
