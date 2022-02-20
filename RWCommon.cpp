#include "RWCommon.h"

 D3DPRESENT_PARAMETERS& Present = *(D3DPRESENT_PARAMETERS*)0xC9C040;
rwD3D9AdapterInformation& _RwD3D9AdapterInformation = *(rwD3D9AdapterInformation*)0x00C9BCE0;
uint32_t& _RwD3DAdapterIndex = *(uint32_t*)0xC97C24;
HWND* WindowHandle = (HWND*)0xC97C1C;
IDirect3D9*& _RwDirect3DObject = *(IDirect3D9**)0xC97C20;
LPDIRECT3DDEVICE9& _RwD3DDevice = *(IDirect3DDevice9**)0xC97C28;
IDirect3DSurface9*& RwD3D9RenderSurface = (*(IDirect3DSurface9**)0xC97C30);
IDirect3DSurface9*& RwD3D9DepthStencilSurface = (*(IDirect3DSurface9**)0xC97C2C);
D3DMATRIX& _RwD3D9D3D9ViewTransform = (*(D3DMATRIX*)0xC9BC80);
D3DMATRIX& _RwD3D9D3D9ProjTransform = (*(D3DMATRIX*)0x8E2458);
RwInt32& _RwD3D9ZBufferDepth = *(RwInt32*)0x00C9BEFC;
D3DCAPS9& _RwD3D9DeviceCaps = *(D3DCAPS9*)0xC9BF00;
D3DDEVTYPE& _RwD3DAdapterType = *(D3DDEVTYPE*)0X8E2428;

IDirect3DSurface9*& CurrentDepthStencilSurface = *(IDirect3DSurface9**)0xC9808C;
 IDirect3DSurface9* (&CurrentRenderSurface)[4] = *(IDirect3DSurface9 * (*)[4])0x0C98090;
int _rwpathisabsolute(const RwChar* path)
{
    return ((*path == SLASH) || (RwIsAlpha(*path) && (path[1] == ':')));
}

RwBool RwFexist(const RwChar* name)
{
    struct stat st;

    int result = stat(name, &st);

    return (result == 0);
}

__inline RwBool GetColorBlockColorsDXT1(const DXTColBlock* pBlock,
    Color8888* col_0, Color8888* col_1,
    Color8888* col_2, Color8888* col_3,
    RwUInt16* wrd)
{
    RwBool  hasAlpha = FALSE;

    /* There are 4 methods to use - see the Time_ functions. */
    /* 1st = shift = does normal approach per byte for color comps */
    /* 2nd = use freak variable bit field color565 for component extraction */
    /* 3rd = use super-freak RwUInt32 adds BEFORE shifting the color components */
    /*  This lets you do only 1 add per color instead of 3 RwUInt8 adds and */
    /*  might be faster */
    /* Call RunTimingSession() to run each of them & output result to txt file */

    /* freak variable bit structure method */
    /* normal math */
    /* This method is fastest */
    const Color565* pCol;

    pCol = (const Color565*)&(pBlock->col0);

    col_0->a = 0xff;
    col_0->r = pCol->nRed;
    col_0->r <<= 3;             /* shift to full precision */
    col_0->g = pCol->nGreen;
    col_0->g <<= 2;
    col_0->b = pCol->nBlue;
    col_0->b <<= 3;

    pCol = (const Color565*)&(pBlock->col1);
    col_1->a = 0xff;
    col_1->r = pCol->nRed;
    col_1->r <<= 3;             /* shift to full precision */
    col_1->g = pCol->nGreen;
    col_1->g <<= 2;
    col_1->b = pCol->nBlue;
    col_1->b <<= 3;


    if (pBlock->col0 > pBlock->col1)
    {
        /* Four-color block: derive the other two colors.     */
        /* 00 = color_0, 01 = color_1, 10 = color_2, 11 = color_3 */
        /* These two bit codes correspond to the 2-bit fields  */
        /* stored in the 64-bit block. */

        *wrd = ((RwUInt16)col_0->r * 2 + (RwUInt16)col_1->r) / 3;
        /* no +1 for rounding */
        /* as bits have been shifted to 888 */
        col_2->r = (RwUInt8)*wrd;

        *wrd = ((RwUInt16)col_0->g * 2 + (RwUInt16)col_1->g) / 3;
        col_2->g = (RwUInt8)*wrd;

        *wrd = ((RwUInt16)col_0->b * 2 + (RwUInt16)col_1->b) / 3;
        col_2->b = (RwUInt8)*wrd;
        col_2->a = 0xff;

        *wrd = ((RwUInt16)col_0->r + (RwUInt16)col_1->r * 2) / 3;
        col_3->r = (RwUInt8)*wrd;

        *wrd = ((RwUInt16)col_0->g + (RwUInt16)col_1->g * 2) / 3;
        col_3->g = (RwUInt8)*wrd;

        *wrd = ((RwUInt16)col_0->b + (RwUInt16)col_1->b * 2) / 3;
        col_3->b = (RwUInt8)*wrd;
        col_3->a = 0xff;
    }
    else
    {
        /* Three-color block: derive the other color. */
        /* 00 = color_0,  01 = color_1,  10 = color_2,   */
        /* 11 = transparent. */
        /* These two bit codes correspond to the 2-bit fields  */
        /* stored in the 64-bit block.  */

        /* explicit for each component, unlike some refrasts... */

        *wrd = ((RwUInt16)col_0->r + (RwUInt16)col_1->r) / 2;
        col_2->r = (RwUInt8)*wrd;
        *wrd = ((RwUInt16)col_0->g + (RwUInt16)col_1->g) / 2;
        col_2->g = (RwUInt8)*wrd;
        *wrd = ((RwUInt16)col_0->b + (RwUInt16)col_1->b) / 2;
        col_2->b = (RwUInt8)*wrd;
        col_2->a = 0xff;

        col_3->r = 0x00;        /* black color to indicate alpha */
        col_3->g = 0x00;
        col_3->b = 0x00;
        col_3->a = 0x00;

        hasAlpha = TRUE;
    }

    return(hasAlpha);
}

__inline void GetColorBlockColors(const DXTColBlock* pBlock,
    Color8888* col_0, Color8888* col_1,
    Color8888* col_2, Color8888* col_3,
    RwUInt16* wrd)
{
    /* There are 4 methods to use - see the Time_ functions. */
    /* 1st = shift = does normal approach per byte for color comps */
    /* 2nd = use freak variable bit field color565 for component extraction */
    /* 3rd = use super-freak RwUInt32 adds BEFORE shifting the color components */
    /*  This lets you do only 1 add per color instead of 3 RwUInt8 adds and */
    /*  might be faster */
    /* Call RunTimingSession() to run each of them & output result to txt file */

    /* freak variable bit structure method */
    /* normal math */
    /* This method is fastest */
    const Color565* pCol;

    pCol = (const Color565*)&(pBlock->col0);

    col_0->a = 0xff;
    col_0->r = pCol->nRed;
    col_0->r <<= 3;             /* shift to full precision */
    col_0->g = pCol->nGreen;
    col_0->g <<= 2;
    col_0->b = pCol->nBlue;
    col_0->b <<= 3;

    pCol = (const Color565*)&(pBlock->col1);
    col_1->a = 0xff;
    col_1->r = pCol->nRed;
    col_1->r <<= 3;             /* shift to full precision */
    col_1->g = pCol->nGreen;
    col_1->g <<= 2;
    col_1->b = pCol->nBlue;
    col_1->b <<= 3;

    if (pBlock->col0 > pBlock->col1)
    {
        /* Four-color block: derive the other two colors.     */
        /* 00 = color_0, 01 = color_1, 10 = color_2, 11 = color_3 */
        /* These two bit codes correspond to the 2-bit fields  */
        /* stored in the 64-bit block. */

        *wrd = ((RwUInt16)col_0->r * 2 + (RwUInt16)col_1->r) / 3;
        /* no +1 for rounding */
        /* as bits have been shifted to 888 */
        col_2->r = (RwUInt8)*wrd;

        *wrd = ((RwUInt16)col_0->g * 2 + (RwUInt16)col_1->g) / 3;
        col_2->g = (RwUInt8)*wrd;

        *wrd = ((RwUInt16)col_0->b * 2 + (RwUInt16)col_1->b) / 3;
        col_2->b = (RwUInt8)*wrd;
        col_2->a = 0xff;

        *wrd = ((RwUInt16)col_0->r + (RwUInt16)col_1->r * 2) / 3;
        col_3->r = (RwUInt8)*wrd;

        *wrd = ((RwUInt16)col_0->g + (RwUInt16)col_1->g * 2) / 3;
        col_3->g = (RwUInt8)*wrd;

        *wrd = ((RwUInt16)col_0->b + (RwUInt16)col_1->b * 2) / 3;
        col_3->b = (RwUInt8)*wrd;
        col_3->a = 0xff;
    }
    else
    {
        /* Three-color block: derive the other color. */
        /* 00 = color_0,  01 = color_1,  10 = color_2,   */
        /* 11 = transparent. */
        /* These two bit codes correspond to the 2-bit fields  */
        /* stored in the 64-bit block.  */

        /* explicit for each component, unlike some refrasts... */

        *wrd = ((RwUInt16)col_0->r + (RwUInt16)col_1->r) / 2;
        col_2->r = (RwUInt8)*wrd;
        *wrd = ((RwUInt16)col_0->g + (RwUInt16)col_1->g) / 2;
        col_2->g = (RwUInt8)*wrd;
        *wrd = ((RwUInt16)col_0->b + (RwUInt16)col_1->b) / 2;
        col_2->b = (RwUInt8)*wrd;
        col_2->a = 0xff;

        col_3->r = 0x00;        /* random color to indicate alpha */
        col_3->g = 0xff;
        col_3->b = 0xff;
        col_3->a = 0x00;
    }

    return;
}

__inline RwBool DecodeColorBlockCheckColor3(RwUInt32* pImPos, const DXTColBlock* pColorBlock,
    RwInt32 width,
    RwUInt32* col_0, RwUInt32* col_1,
    RwUInt32* col_2, RwUInt32* col_3)
{
    /* width is width of image in pixels */
    RwInt32     r, n;
    RwBool      usedThird = FALSE;

    /* r steps through lines in y */
    for (r = 0; r < 4; r++, pImPos += width - 4)   /* no width*4 as RwUInt32 ptr inc will *4 */
    {
        RwUInt32 bits = pColorBlock->row[r];

        /* width * 4 bytes per pixel per line */
        /* each j dxtc row is 4 lines of pixels */

        /* pImPos = (RwUInt32*)((RwUInt32)pBase + i*16 + (r+j*4) * m_nWidth * 4 ); */

        /* n steps through pixels */
        for (n = 0; n < 4; n++)
        {
            switch (bits & 0x3)
            {
            case 0:
                *pImPos = *col_0;
                pImPos++;       /* increment to next RwUInt32 */
                break;
            case 1:
                *pImPos = *col_1;
                pImPos++;
                break;
            case 2:
                *pImPos = *col_2;
                pImPos++;
                break;
            case 3:
                usedThird = TRUE;
                *pImPos = *col_3;
                pImPos++;
                break;
            default:
                pImPos++;
                break;
            }

            bits >>= 2;
        }
    }

    return(usedThird);
}

__inline void DecodeColorBlock(RwUInt32* pImPos, const DXTColBlock* pColorBlock,
    RwInt32 width,
    RwUInt32* col_0, RwUInt32* col_1,
    RwUInt32* col_2, RwUInt32* col_3)
{
    /* width is width of image in pixels */
    RwInt32     r, n;

    /* r steps through lines in y */
    for (r = 0; r < 4; r++, pImPos += width - 4)   /* no width*4 as RwUInt32 ptr inc will *4 */
    {
        RwUInt32 bits = pColorBlock->row[r];

        /* width * 4 bytes per pixel per line */
        /* each j dxtc row is 4 lines of pixels */

        /* pImPos = (RwUInt32*)((RwUInt32)pBase + i*16 + (r+j*4) * m_nWidth * 4 ); */

        /* n steps through pixels */
        for (n = 0; n < 4; n++)
        {
            switch (bits & 0x3)
            {
            case 0:
                *pImPos = *col_0;
                pImPos++;       /* increment to next RwUInt32 */
                break;
            case 1:
                *pImPos = *col_1;
                pImPos++;
                break;
            case 2:
                *pImPos = *col_2;
                pImPos++;
                break;
            case 3:
                *pImPos = *col_3;
                pImPos++;
                break;
            default:
                pImPos++;
                break;
            }

            bits >>= 2;
        }
    }

    return;
}

__inline RwBool DXT1BlockHasAlpha(const DXTColBlock* pColorBlock)
{
    /* width is width of image in pixels */
    RwInt32     r, n;


    /* r steps through lines in y */
    for (r = 0; r < 4; r++)
    {
        RwUInt32 bits = pColorBlock->row[r];

        /* n steps through pixels */
        for (n = 0; n < 4; n++)
        {
            if ((bits & 0x3) == 0x3)
            {
                /* this color is the only one with non-255 alpha */
                return(TRUE);
            }

            bits >>= 2;
        }
    }

    return(FALSE);
}

__inline RwBool DXT1BlockNonSquareHasAlpha(const DXTColBlock* pColorBlock, RwInt32 width, RwInt32 height)
{
    /* width is width of image in pixels */
    RwInt32     r, n;

    /* r steps through lines in y */
    for (r = 0; r < height; r++)
    {
        RwUInt32 bits = pColorBlock->row[r];

        /* n steps through pixels */
        for (n = 0; n < width; n++)
        {
            if ((bits & 0x3) == 0x3)
            {
                /* this color is the only one with non-255 alpha */
                return(TRUE);
            }

            bits >>= 2;
        }
    }

    return(FALSE);
}

__inline void DecodeAlphaExplicit(RwUInt32* pImPos, const DXTAlphaBlockExplicit* pAlphaBlock,
    RwInt32 width, RwUInt32 alphazero)
{
    /* alphazero is a bit mask that when & with the image color */
    /*  will zero the alpha bits, so if the image DWORDs  are */
    /*  ARGB then alphazero will be 0x00ffffff or if */
    /*  RGBA then alphazero will be 0xffffff00 */
    /*  alphazero constructed automaticaly from field order of Color8888 structure */

    /* decodes to 32 bit format only */
    RwInt32 row, pix;

    RwUInt16 wrd;

    Color8888 col;

    col.r = col.g = col.b = 0;

    for (row = 0; row < 4; row++, pImPos += width - 4)
    {
        /* pImPow += pImPos += width-4 moves to next row down */

        wrd = pAlphaBlock->row[row];

        for (pix = 0; pix < 4; pix++)
        {
            /* zero the alpha bits of image pixel */
            *pImPos &= alphazero;

            col.a = wrd & 0x000f;       /* get only low 4 bits */
/*          col.a <<= 4;                // shift to full byte precision */
                                        /* NOTE:  with just a << 4 you'll never have alpha */
                                        /* of 0xff,  0xf0 is max so pure shift doesn't quite */
                                        /* cover full alpha range. */
                                        /* It's much cheaper than divide & scale though. */
                                        /* To correct for this, and get 0xff for max alpha, */
                                        /*  or the low bits back in after left shifting */
            col.a = col.a | (col.a << 4);  /* This allows max 4 bit alpha to be 0xff alpha */
                                            /*  in final image, and is crude approach to full  */
                                            /*  range scale */

            *pImPos |= *((RwUInt32*)&col);  /* or the bits into the prev. nulled alpha */

            wrd >>= 4;      /* move next bits to lowest 4 */

            pImPos++;       /* move to next pixel in the row */

        }
    }

    return;
}

__inline void DecodeAlpha3BitLinear(RwUInt32* pImPos, const DXTAlphaBlock3BitLinear* pAlphaBlock,
    RwInt32 width, RwUInt32 alphazero)
{
    const RwUInt32 mask = 0x00000007;       /* bits = 00 00 01 11 */

    RwUInt8     gBits[4][4];
    RwUInt16    gAlphas[8];
    RwUInt8     gACol[4][4];
    RwUInt32    bits;
    RwInt32     row, pix;

    gAlphas[0] = pAlphaBlock->alpha0;
    gAlphas[1] = pAlphaBlock->alpha1;

    /* 8-alpha or 6-alpha block?     */
    if (gAlphas[0] > gAlphas[1])
    {
        /* 8-alpha block:  derive the other 6 alphas.     */
        /* 000 = alpha_0, 001 = alpha_1, others are interpolated */

        gAlphas[2] = (6 * gAlphas[0] + gAlphas[1]) / 7;    /* bit code 010 */
        gAlphas[3] = (5 * gAlphas[0] + 2 * gAlphas[1]) / 7;    /* Bit code 011     */
        gAlphas[4] = (4 * gAlphas[0] + 3 * gAlphas[1]) / 7;    /* Bit code 100     */
        gAlphas[5] = (3 * gAlphas[0] + 4 * gAlphas[1]) / 7;    /* Bit code 101 */
        gAlphas[6] = (2 * gAlphas[0] + 5 * gAlphas[1]) / 7;    /* Bit code 110     */
        gAlphas[7] = (gAlphas[0] + 6 * gAlphas[1]) / 7;    /* Bit code 111 */
    }
    else
    {
        /* 6-alpha block:  derive the other alphas.     */
        /* 000 = alpha_0, 001 = alpha_1, others are interpolated */

        gAlphas[2] = (4 * gAlphas[0] + gAlphas[1]) / 5; /* Bit code 010 */
        gAlphas[3] = (3 * gAlphas[0] + 2 * gAlphas[1]) / 5; /* Bit code 011     */
        gAlphas[4] = (2 * gAlphas[0] + 3 * gAlphas[1]) / 5; /* Bit code 100     */
        gAlphas[5] = (gAlphas[0] + 4 * gAlphas[1]) / 5; /* Bit code 101 */
        gAlphas[6] = 0;                                     /* Bit code 110 */
        gAlphas[7] = 255;                                   /* Bit code 111 */
    }

    /* Decode 3-bit fields into array of 16 BYTES with same value */

    /* first two rows of 4 pixels each: */
    /* pRows = (Alpha3BitRows*) & ( pAlphaBlock->stuff[0] ); */

    bits = *((const RwUInt32*)&(pAlphaBlock->stuff[0]));

    gBits[0][0] = (RwUInt8)(bits & mask);
    bits >>= 3;
    gBits[0][1] = (RwUInt8)(bits & mask);
    bits >>= 3;
    gBits[0][2] = (RwUInt8)(bits & mask);
    bits >>= 3;
    gBits[0][3] = (RwUInt8)(bits & mask);
    bits >>= 3;
    gBits[1][0] = (RwUInt8)(bits & mask);
    bits >>= 3;
    gBits[1][1] = (RwUInt8)(bits & mask);
    bits >>= 3;
    gBits[1][2] = (RwUInt8)(bits & mask);
    bits >>= 3;
    gBits[1][3] = (RwUInt8)(bits & mask);

    /* now for last two rows: */

    bits = *((const RwUInt32*)&(pAlphaBlock->stuff[3]));      /* last 3 bytes */

    gBits[2][0] = (RwUInt8)(bits & mask);
    bits >>= 3;
    gBits[2][1] = (RwUInt8)(bits & mask);
    bits >>= 3;
    gBits[2][2] = (RwUInt8)(bits & mask);
    bits >>= 3;
    gBits[2][3] = (RwUInt8)(bits & mask);
    bits >>= 3;
    gBits[3][0] = (RwUInt8)(bits & mask);
    bits >>= 3;
    gBits[3][1] = (RwUInt8)(bits & mask);
    bits >>= 3;
    gBits[3][2] = (RwUInt8)(bits & mask);
    bits >>= 3;
    gBits[3][3] = (RwUInt8)(bits & mask);

    /* decode the codes into alpha values */
    for (row = 0; row < 4; row++)
    {
        for (pix = 0; pix < 4; pix++)
        {
            gACol[row][pix] = (RwUInt8)gAlphas[gBits[row][pix]];
        }
    }

    /* Write out alpha values to the image bits */
    for (row = 0; row < 4; row++, pImPos += width - 4)
    {
        /* pImPow += pImPos += width-4 moves to next row down */

        for (pix = 0; pix < 4; pix++)
        {
            /* zero the alpha bits of image pixel */
            *pImPos &= alphazero;

            *pImPos |= ((RwUInt32)gACol[row][pix]) << 24;  /* or the bits into the prev. nulled alpha */
            pImPos++;
        }
    }

    return;
}

RwBool _rwD3D9DecompressDXT1(RwInt32 width, RwInt32 height, RwUInt32* destPixels, const RwUInt16* srcPixels)
{
    /* This was hacked up pretty quick & slopily */
    /* decompresses to 32 bit format 0xARGB */
    const RwInt32  xblocks = width / 4;
    const RwInt32  yblocks = height / 4;

    RwUInt32* pBase = destPixels;
    RwUInt32* pImPos = pBase;            /* pos in decompressed data */


    const DXTColBlock* pBlock;

    Color8888 col_0, col_1, col_2, col_3;

    RwUInt16 wrd;
    RwInt32 i, j;
    RwBool  hasAlpha = FALSE;

    for (j = 0; j < yblocks; j++)
    {
        /* 8 bytes per block */
        pBlock = (const DXTColBlock*)((const RwUInt8*)srcPixels + j * xblocks * 8);

        for (i = 0; i < xblocks; i++, pBlock++)
        {
            /* inline func: */
            const RwBool hasAlphaBlock = GetColorBlockColorsDXT1(pBlock, &col_0, &col_1, &col_2, &col_3, &wrd);

            /* now decode the color block into the bitmap bits inline func: */
            pImPos = (RwUInt32*)((RwUInt8*)pBase + i * 16 + (j * 4) * width * 4);

            if (hasAlphaBlock)
            {
                hasAlpha |= DecodeColorBlockCheckColor3(pImPos, pBlock, width, (RwUInt32*)&col_0, (RwUInt32*)&col_1, (RwUInt32*)&col_2, (RwUInt32*)&col_3);
            }
            else
            {
                DecodeColorBlock(pImPos, pBlock, width, (RwUInt32*)&col_0, (RwUInt32*)&col_1, (RwUInt32*)&col_2, (RwUInt32*)&col_3);
            }
        }
    }

    return(hasAlpha);
}

void _rwD3D9DecompressDXT3(RwInt32 width, RwInt32 height, RwUInt32* destPixels, const RwUInt16* srcPixels)
{
    const RwInt32 xblocks = width / 4;
    const RwInt32 yblocks = height / 4;

    RwUInt32* pBase = destPixels;
    RwUInt32* pImPos = pBase;            /* pos in decompressed data */

    const DXTColBlock* pBlock;
    const DXTAlphaBlockExplicit* pAlphaBlock;

    Color8888   col_0, col_1, col_2, col_3;
    RwUInt32    alphazero;

    RwUInt16 wrd;

    RwInt32 i, j;

    /* fill alphazero with appropriate value to zero out alpha when */
    /*  alphazero is ANDed with the image color 32 bit RwUInt32: */
    col_0.a = 0;
    col_0.r = col_0.g = col_0.b = 0xff;
    alphazero = *((RwUInt32*)&col_0);

    for (j = 0; j < yblocks; j++)
    {
        /* 8 bytes per block */
        /* 1 block for alpha, 1 block for color */
        pBlock = (const DXTColBlock*)((const RwUInt8*)srcPixels + j * xblocks * 16);

        for (i = 0; i < xblocks; i++, pBlock++)
        {
            /* inline */
            /* Get alpha block */

            pAlphaBlock = (const DXTAlphaBlockExplicit*)pBlock;

            /* inline func: */
            /* Get color block & colors */
            pBlock++;

            GetColorBlockColors(pBlock, &col_0, &col_1, &col_2, &col_3, &wrd);

            /* Decode the color block into the bitmap bits */
            /* inline func: */

            pImPos = (RwUInt32*)((RwUInt8*)pBase + i * 16 + (j * 4) * width * 4);


            DecodeColorBlock(pImPos, pBlock, width, (RwUInt32*)&col_0, (RwUInt32*)&col_1, (RwUInt32*)&col_2, (RwUInt32*)&col_3);

            /* Overwrite the previous alpha bits with the alpha block */
            /*  info */
            /* inline func: */
            DecodeAlphaExplicit(pImPos, pAlphaBlock, width, alphazero);
        }
    }

    return;
}

void _rwD3D9DecompressDXT5(RwInt32 width, RwInt32 height, RwUInt32* destPixels, const RwUInt16* srcPixels)
{
    const RwInt32 xblocks = width / 4;
    const RwInt32 yblocks = height / 4;

    RwUInt32* pBase = destPixels;
    RwUInt32* pImPos = pBase;            /* pos in decompressed data */

    const DXTColBlock* pBlock;
    const DXTAlphaBlock3BitLinear* pAlphaBlock;

    Color8888   col_0, col_1, col_2, col_3;
    RwUInt32    alphazero;

    RwUInt16 wrd;
    RwInt32 i, j;

    /* fill alphazero with appropriate value to zero out alpha when */
    /*  alphazero is ANDed with the image color 32 bit RwUInt32: */
    col_0.a = 0;
    col_0.r = col_0.g = col_0.b = 0xff;

    alphazero = *((RwUInt32*)&col_0);

    for (j = 0; j < yblocks; j++)
    {
        /* 8 bytes per block */
        /* 1 block for alpha, 1 block for color */
        pBlock = (const DXTColBlock*)((const RwUInt8*)srcPixels + j * xblocks * 16);

        for (i = 0; i < xblocks; i++, pBlock++)
        {
            /* inline */
            /* Get alpha block */

            pAlphaBlock = (const DXTAlphaBlock3BitLinear*)pBlock;

            /* inline func: */
            /* Get color block & colors */
            pBlock++;

            GetColorBlockColors(pBlock, &col_0, &col_1, &col_2, &col_3, &wrd);

            /* Decode the color block into the bitmap bits */
            /* inline func: */

            pImPos = (RwUInt32*)((RwUInt8*)pBase + i * 16 + (j * 4) * width * 4);

            DecodeColorBlock(pImPos, pBlock, width, (RwUInt32*)&col_0, (RwUInt32*)&col_1, (RwUInt32*)&col_2, (RwUInt32*)&col_3);

            /* Overwrite the previous alpha bits with the alpha block */
            /*  info */
            DecodeAlpha3BitLinear(pImPos, pAlphaBlock, width, alphazero);
        }
    }

    return;
}

RwBool D3D9RasterDXT1HasAlpha(RwRaster* raster)
{
    RwInt32 numMipLevels;
    RwInt32 i;

    /* Load the surfaces */
    numMipLevels = RwRasterGetNumLevels(raster);
    for (i = 0; i < numMipLevels; i++)
    {
        RwBool              squareBlocks = TRUE;
        RwInt32             sizeblockX, sizeblockY, totalBlocks;
        const DXTColBlock* pixels;

        pixels = (const DXTColBlock*)RwRasterLock(raster, (RwUInt8)i, rwRASTERLOCKREAD);

        if (raster->width < 4)
        {
            sizeblockX = raster->width;
            totalBlocks = 1;

            squareBlocks = FALSE;
        }
        else
        {
            sizeblockX = 4;
            totalBlocks = raster->width / 4;
        }

        if (raster->height < 4)
        {
            sizeblockY = raster->height;
            squareBlocks = FALSE;
        }
        else
        {
            sizeblockY = 4;
            totalBlocks *= raster->height / 4;
        }

        if (squareBlocks)
        {
            do
            {
                /*
                 * If the first color is greater than the second, it implies that
                 * only opaque texels are defined
                 */
                if (!(pixels->col0 > pixels->col1) &&
                    /* but not all of those have non-opaque alpha */
                    DXT1BlockHasAlpha(pixels))
                {
                    RwRasterUnlock(raster);

                    return(TRUE);
                }

                pixels++;
            } while (--totalBlocks);
        }
        else
        {
            do
            {
                /*
                 * If the first color is greater than the second, it implies that
                 * only opaque texels are defined
                 */
                if (!(pixels->col0 > pixels->col1) && DXT1BlockNonSquareHasAlpha(pixels, sizeblockX, sizeblockY))
                {
                    RwRasterUnlock(raster);

                    return(TRUE);
                }

                pixels++;
            } while (--totalBlocks);
        }

        RwRasterUnlock(raster);
    }

    return(FALSE);
}

RwUInt32 D3D9RasterGetSize(RwRaster* raster, RwUInt8 mipLevel)
{
    RwUInt32            blocksX, blocksY, totalBlocks;
    RwUInt32            width, height;
    RwUInt32            shift, size;
    _rwD3D9RasterExt* rasExt;


    rasExt = RASTEREXTFROMRASTER(raster);

    /* Calc mip level size */
    width = raster->width >> mipLevel;
    height = raster->height >> mipLevel;

    /* Clamp width and height to 1 */
    width = ((0 == width) ? 1 : width);
    height = ((0 == height) ? 1 : height);

    /* Is the raster a DXT compressed format */
    if (0 == rasExt->compressed)
    {
        size = width * height * (raster->depth >> 3);

        return(size);
    }
    else
    {
        /* A DXT compressed format */
        if (D3DFMT_DXT1 == rasExt->d3dFormat)
        {
            shift = 3; /* 64Bits / 8 = 8. 2^3 = 8 */
        }
        else
        {
            shift = 4; /* 128Bits / 8 = 16. 2^4 = 16 */
        }

        blocksX = width >> 2;
        blocksY = height >> 2;

        blocksX = ((0 == blocksX) ? 1 : blocksX);
        blocksY = ((0 == blocksY) ? 1 : blocksY);

        totalBlocks = blocksX * blocksY;

        size = totalBlocks << shift;

        return(size);
    }

    return(-1);
}

RwUInt32 D3D9DDSGetSize(RwInt32 ddsWidth, RwInt32 ddsHeight, RwInt32 ddsDepth, RwUInt8 ddsCompressed, D3DFORMAT ddsFormat, RwUInt8 mipLevel)
{
    RwUInt32            blocksX, blocksY, totalBlocks;
    RwUInt32            width, height;
    RwUInt32            shift, size;


    /* Calc mip level size */
    width = ddsWidth >> mipLevel;
    height = ddsHeight >> mipLevel;

    /* Clamp width and height to 1 */
    width = ((0 == width) ? 1 : width);
    height = ((0 == height) ? 1 : height);

    /* Is the raster a DXT compressed format */
    if (0 == ddsCompressed)
    {
        size = width * height * (ddsDepth >> 3);

        return(size);
    }
    else
    {
        /* A DXT compressed format */
        if (D3DFMT_DXT1 == ddsFormat)
        {
            shift = 3; /* 64Bits / 8 = 8. 2^3 = 8 */
        }
        else
        {
            shift = 4; /* 128Bits / 8 = 16. 2^4 = 16 */
        }

        blocksX = width >> 2;
        blocksY = height >> 2;

        blocksX = ((0 == blocksX) ? 1 : blocksX);
        blocksY = ((0 == blocksY) ? 1 : blocksY);

        totalBlocks = blocksX * blocksY;

        size = totalBlocks << shift;

        return (size);
    }

    return -1;
}

RwFreeList* RwFreeListCreate(int32_t entrySize, int32_t entriesPerBlock, int32_t alignment, uint32_t hint) {
    return ((RwFreeList * (__cdecl*)(int32_t, int32_t, int32_t, uint32_t))0x801980)(entrySize, entriesPerBlock, alignment, hint);
}

RwBool  _rwD3D9Im2DRenderLine(RwIm2DVertex* verts, RwInt32 numVerts, RwInt32 vert1, RwInt32 vert2)
{
    return ((RwBool(__cdecl*)(RwIm2DVertex*, RwInt32, RwInt32, RwInt32))0x007FB650)(verts, numVerts, vert1, vert2);
}

RwBool  _rwD3D9Im2DRenderTriangle(RwIm2DVertex* verts, RwInt32 numVerts, RwInt32 vert1, RwInt32 vert2, RwInt32 vert3)
{
    return ((RwBool(__cdecl*)(RwIm2DVertex*, RwInt32, RwInt32, RwInt32, RwInt32))0x007FB8E0)(verts, numVerts, vert1, vert2, vert3);
}

RwBool  _rwD3D9Im2DRenderPrimitive(RwPrimitiveType primType, RwIm2DVertex* verts, RwInt32 numVerts)
{
    return ((RwBool(__cdecl*)(RwPrimitiveType, RwIm2DVertex*, RwInt32))0x007FBB00)(primType, verts, numVerts);
}

RwBool  _rwD3D9Im2DRenderIndexedPrimitive(RwPrimitiveType primType, RwIm2DVertex* verts, RwInt32 numVerts, RwImVertexIndex* indices, RwInt32 numIndices)
{
    return ((RwBool(__cdecl*)(RwPrimitiveType, RwIm2DVertex*, RwInt32, RwImVertexIndex*, RwInt32))0x007FBD30)(primType, verts, numVerts, indices, numIndices);
}

RwBool _rwVectorSetMultFn(rwVectorMultFn multPoint, rwVectorsMultFn multPoints, rwVectorMultFn multVector, rwVectorsMultFn multVectors)
{
    return ((RwBool(__cdecl*)(rwVectorMultFn, rwVectorsMultFn, rwVectorMultFn, rwVectorsMultFn))0x007ED5F0)(multPoint, multPoints, multVector, multVectors);
}

RwV3d* _rwIntelSSEP4V3dTransformPoint(RwV3d* pointOut, const RwV3d* pointIn, const RwMatrix* matrix)
{
    return ((RwV3d * (__cdecl*)(RwV3d*, const RwV3d*, const RwMatrix*))0x0080D570)(pointOut, pointIn, matrix);
}

RwV3d* _rwIntelSSEP4V3dTransformPoints(RwV3d* pointsOut, const RwV3d* pointsIn, RwInt32 numPoints, const RwMatrix* matrix)
{
    return ((RwV3d * (__cdecl*)(RwV3d*, const RwV3d*, RwInt32, const RwMatrix*))0x0080D440)(pointsOut, pointsIn, numPoints, matrix);
}

RwV3d* _rwIntelSSEP4V3dTransformVector(RwV3d* vectorOut, const RwV3d* vectorIn, const RwMatrix* matrix)
{
    return ((RwV3d * (__cdecl*)(RwV3d*, const RwV3d*, const RwMatrix*))0x0080D600)(vectorOut, vectorIn, matrix);
}

RwV3d* _rwIntelSSEP4V3dTransformVectors(RwV3d* vectorsOut, const RwV3d* vectorsIn, RwInt32 numVectors, const RwMatrix* matrix)
{
    return ((RwV3d * (__cdecl*)(RwV3d*, const RwV3d*, RwInt32, const RwMatrix*))0x0080D4E0)(vectorsOut, vectorsIn, numVectors, matrix);
}

RwV3d* _rwIntelSSEV3dTransformPoint(RwV3d* pointOut, const RwV3d* pointIn, const RwMatrix* matrix)
{
    return ((RwV3d * (__cdecl*)(RwV3d*, const RwV3d*, const RwMatrix*))0x0080D280)(pointOut, pointIn, matrix);
}

RwV3d* _rwIntelSSEV3dTransformPoints(RwV3d* pointsOut, const RwV3d* pointsIn, RwInt32 numPoints, const RwMatrix* matrix)
{
    return ((RwV3d * (__cdecl*)(RwV3d*, const RwV3d*, RwInt32, const RwMatrix*))0x0080D070)(pointsOut, pointsIn, numPoints, matrix);
}

RwV3d* _rwIntelSSEV3dTransformVector(RwV3d* vectorOut, const RwV3d* vectorIn, const RwMatrix* matrix)
{
    return ((RwV3d * (__cdecl*)(RwV3d*, const RwV3d*, const RwMatrix*))0x0080D370)(vectorOut, vectorIn, matrix);
}

RwV3d* _rwIntelSSEV3dTransformVectors(RwV3d* vectorsOut, const RwV3d* vectorsIn, RwInt32 numVectors, const RwMatrix* matrix)
{
    return ((RwV3d * (__cdecl*)(RwV3d*, const RwV3d*, RwInt32, const RwMatrix*))0x0080D180)(vectorsOut, vectorsIn, numVectors, matrix);
}

