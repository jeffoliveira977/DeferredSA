#pragma once

#include "CommonD.h"
class CDX3D;

#define D3DFVF_GRAPH3DVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL |D3DFVF_TEX1)

typedef struct sGRAPH3DVERTEXSTRUCT{
    float x, y, z;
	float nx, ny, nz;
	float tu, tv;   
} GRAPH3DVERTEXSTRUCT;

typedef struct {
	IDirect3DVertexBuffer9* VertexBuffer; // Vertex Data Buffer 
	D3DPRIMITIVETYPE PrimitiveType; // Primitive Type
	DWORD dwPrimitiveCount; // Primitive Element count
	DWORD dwVertexCount;  // Vertext Count
}PRIMITIVESTRUCT;

class CMesh
{
private:
	
	void CalcSphereVertex(float p_fPitch, float p_fYaw, float p_fRadius, 
						GRAPH3DVERTEXSTRUCT* p_pVertex, 
						bool bSetNormal = true, bool bSetTexture = true);
	void ReleaseBuffers();
	int m_iPrimitivesCount;
	PRIMITIVESTRUCT* m_pPrimitives;
protected:
	IDirect3DDevice9* m_p3DDevice;
	// Create Buffers
	HRESULT CreateVertexBuffers(int p_iPrimitivesCount);
	DWORD CalcVertexCount(D3DPRIMITIVETYPE p_PrimitiveType, DWORD p_dwPrimitiveCount);
public:
	CMesh(IDirect3DDevice9* p_pDX3D);
	virtual ~CMesh(void);

	// Draw the Mesh
	virtual HRESULT Draw();

	// Create Sphere
	HRESULT CreateSphere(float p_fRadius, unsigned int p_iSteps,
		bool p_bUseNormals = true, bool p_bUseTexture = true);
};
