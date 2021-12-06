#include "mesh.h"


CMesh::CMesh(IDirect3DDevice9* p_pDX3D)
{
	m_iPrimitivesCount = 0;
	m_pPrimitives = NULL;
	m_p3DDevice = p_pDX3D;
}

CMesh::~CMesh(void)
{
	ReleaseBuffers();
}

HRESULT CMesh::Draw()
{
	HRESULT hr;
	if(!m_pPrimitives || !m_iPrimitivesCount)
		return S_FALSE;


	_rwD3D9SetFVF(D3DFVF_GRAPH3DVERTEX);

	RwD3D9SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	for(int i = 0; i < m_iPrimitivesCount; i++){
		RwD3D9SetStreamSource(0, m_pPrimitives[i].VertexBuffer, 0,
			sizeof(GRAPH3DVERTEXSTRUCT));

		_rwD3D9DrawPrimitive(m_pPrimitives[i].PrimitiveType,
			0, m_pPrimitives[i].dwPrimitiveCount);

	}
	RwD3D9SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	return S_OK;
}

HRESULT CMesh::CreateVertexBuffers(int p_iPrimitivesCount)
{

	HRESULT hr;
	if(!m_pPrimitives || m_iPrimitivesCount < p_iPrimitivesCount)
		return S_FALSE;

	for(int i = 0; i < p_iPrimitivesCount; i++){
		m_pPrimitives[i].dwVertexCount = 
			CalcVertexCount(m_pPrimitives[i].PrimitiveType, m_pPrimitives[i].dwPrimitiveCount);
		hr = m_p3DDevice->CreateVertexBuffer(
			sizeof(GRAPH3DVERTEXSTRUCT) * m_pPrimitives[i].dwVertexCount, 
			0, D3DFVF_GRAPH3DVERTEX,
			D3DPOOL_MANAGED/*D3DPOOL_DEFAULT*/, &m_pPrimitives[i].VertexBuffer, NULL);
		if(FAILED(hr)){
			ReleaseBuffers();
			break;
		}
	}

	return hr;
}

DWORD CMesh::CalcVertexCount(D3DPRIMITIVETYPE p_PrimitiveType, DWORD p_dwPrimitiveCount)
{
	switch(p_PrimitiveType){
	case D3DPT_POINTLIST:
		return p_dwPrimitiveCount;
	case D3DPT_LINELIST:
		return p_dwPrimitiveCount * 2;
	case D3DPT_LINESTRIP:
		return p_dwPrimitiveCount + 1;
	case D3DPT_TRIANGLELIST:
		return  p_dwPrimitiveCount * 3;
	case D3DPT_TRIANGLESTRIP:
		return  p_dwPrimitiveCount + 2;
	case D3DPT_TRIANGLEFAN:
		return  p_dwPrimitiveCount + 2;
	default:
		return  0;
	};	
}

void CMesh::CalcSphereVertex(float p_fPitch, float p_fYaw, float p_fRadius, 
						GRAPH3DVERTEXSTRUCT* p_pVertex, 
						bool bSetNormal, bool bSetTexture)
{
	float cospitch = cos(p_fPitch);
	float sinpitch = sin(p_fPitch);
	float cosyaw = cos(p_fYaw);
	float sinyaw = sin(p_fYaw);
	float r = p_fRadius * cospitch;
	p_pVertex->z = p_fRadius * sinpitch;
	p_pVertex->x = r * cosyaw;
	p_pVertex->y = r * sinyaw;
	/*p_pVertex->nz = sinpitch;
	p_pVertex->nx = cospitch * cosyaw;
	p_pVertex->ny = cospitch * sinyaw;*/
	p_pVertex->tv = (p_fPitch + D3DXToRadian(90)) / D3DXToRadian(180);
	p_pVertex->tu = p_fYaw / D3DXToRadian(360);
}

HRESULT CMesh::CreateSphere(float p_fRadius, unsigned int p_iSteps,
		bool p_bUseNormals, bool p_bUseTexture)
{
	float pi = 3.1415926535f;

	ReleaseBuffers();
	m_pPrimitives = new PRIMITIVESTRUCT[3];
	if(!m_pPrimitives)
		return ERROR_NOT_ENOUGH_MEMORY;
	m_iPrimitivesCount = 3;
	// Sphere Bottom
	m_pPrimitives[0].PrimitiveType =  D3DPT_TRIANGLEFAN;
	m_pPrimitives[0].dwPrimitiveCount = 2 * p_iSteps;
	// Sphere Middle
	m_pPrimitives[1].PrimitiveType =  D3DPT_TRIANGLESTRIP;
	m_pPrimitives[1].dwPrimitiveCount = 4 * p_iSteps * (p_iSteps - 2);
	// Sphere Top
	m_pPrimitives[2].PrimitiveType =  D3DPT_TRIANGLEFAN;
	m_pPrimitives[2].dwPrimitiveCount = 2 * p_iSteps;
	
	HRESULT hr = CreateVertexBuffers(3);
	if(FAILED(hr)){
		ReleaseBuffers(); // Buffers have already been released
		return hr;
	}
	float anglestep = pi / p_iSteps;
	
	GRAPH3DVERTEXSTRUCT* pData;

	// Sphere Bottom
	hr = m_pPrimitives[0].VertexBuffer->Lock(0, 0, (void**)&pData, 0);
	if (FAILED(hr)) {
		return hr;
	}
	// Bottom Vertex
	CalcSphereVertex(-pi / 2, 0, p_fRadius, pData);
	// Rest of triangle fan vertexes
	for(int i = p_iSteps * 2; i >= 0; i--){
		CalcSphereVertex(- pi / 2 + anglestep, 
						 anglestep * i, p_fRadius, 
						 &pData[p_iSteps * 2 - i + 1]);
	}
	
	hr = m_pPrimitives[0].VertexBuffer->Unlock();
	if(FAILED(hr)){
		return hr;
	}

	// Sphere Middle
	hr = m_pPrimitives[1].VertexBuffer->Lock(0, 0, (void**)&pData, 0);
	if(FAILED(hr)){
		return hr;
	}
	DWORD v = 0; // vertex index
	// First vertex of triangle strip
	CalcSphereVertex(-pi / 2 + 1 * anglestep, 0, p_fRadius,
			&pData[v++]);
	// Rest of triangle strip vertexes
	for(unsigned int i = 1; i < p_iSteps - 1; i++){
		for(unsigned int j = 1; j <= 2 * p_iSteps; j++){
			CalcSphereVertex(-pi / 2 + i * anglestep,
				(i - 1) * 2 * pi + j * anglestep, 
				p_fRadius, &pData[v++]);
			CalcSphereVertex(-pi / 2 + (i + 1) * anglestep,
				(i - 1) * 2 * pi + j * anglestep, 
				p_fRadius, &pData[v++]);
		}
	}
	// Last vertex of triangle strip
	CalcSphereVertex(pi / 2 - anglestep, anglestep, p_fRadius,
						&pData[v++]);
	m_pPrimitives[1].VertexBuffer->Unlock();
	if(FAILED(hr)){
		return hr;
	}

	// Sphere Top
	hr = m_pPrimitives[2].VertexBuffer->Lock(0, 0, (void**)&pData, 0);
	if(FAILED(hr)){
		return hr;
	}
	// Top Vertex
	CalcSphereVertex(pi / 2, 0, p_fRadius, pData);
	// Rest of triangle fan vertexes
	for(unsigned int i = 0; i <= p_iSteps * 2; i++){
		CalcSphereVertex(pi / 2 - anglestep, 
						 anglestep * i, p_fRadius, &pData[i + 1]);
	}
	
	m_pPrimitives[2].VertexBuffer->Unlock();
	if(FAILED(hr)){
		return hr;
	}
	return S_OK;
}

void CMesh::ReleaseBuffers()
{
	if (m_pPrimitives) {
		for(int i = 0; i < m_iPrimitivesCount; i++){
			if(m_pPrimitives[i].VertexBuffer)
				m_pPrimitives[i].VertexBuffer->Release();
		}
		delete[] m_pPrimitives;
	}
	m_pPrimitives = NULL;
	m_iPrimitivesCount = 0;
}

