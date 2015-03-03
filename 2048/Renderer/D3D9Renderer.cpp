#include "Common.h"

#ifdef BUILD_WITH_D3D9
#include "D3D9Renderer.h"
#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")

#ifndef _M_X64
#pragma comment(lib, "x86\\d3dx9.lib")
#else
#pragma comment(lib, "x64\\d3dx9.lib")
#endif

static inline D3DCOLOR COLORREF_TO_D3DCOLOR(COLORREF color, int nAlpha = 255)
{
	return D3DCOLOR_ARGB(nAlpha, GetRValue(color), GetGValue(color), GetBValue(color));
}

#define D3DFVF_CUSTOM D3DFVF_XYZ | D3DFVF_DIFFUSE

struct CustomVertex
{
	float x, y, z;
	D3DCOLOR dwColor;
};


D3D9Renderer::D3D9Renderer(HWND hWnd)
: BaseRenderer(hWnd)
, m_pD3D9(nullptr)
, m_pD3DDevice9(nullptr)
, m_pVertexBufferLine(nullptr)
{
}

D3D9Renderer::~D3D9Renderer()
{
	m_fontPool.Clear();

	SafeRelease(&m_pD3D9);
	SafeRelease(&m_pD3DDevice9);
}

bool D3D9Renderer::Initialize()
{
	if (!m_hWnd)
	{
		return false;
	}

	GetClientRect(m_hWnd, &m_rtWnd);

	m_pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (!m_pD3D9)
	{
		return false;
	}

	D3DPRESENT_PARAMETERS param = {};
	param.Windowed = TRUE;
	param.hDeviceWindow = m_hWnd;
	param.SwapEffect = D3DSWAPEFFECT_DISCARD;
	param.EnableAutoDepthStencil = TRUE;
	param.AutoDepthStencilFormat = D3DFMT_D24S8;
	HRESULT hr = m_pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &param, &m_pD3DDevice9);
	if (FAILED(hr))
	{
		return false;
	}

	m_fontPool.Initialize(m_pD3DDevice9);

	m_pD3DDevice9->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pD3DDevice9->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pD3DDevice9->SetRenderState(D3DRS_CLIPPING, FALSE);

	// setup matrix
	D3DXMATRIX projectionMatrix;
	D3DXMatrixIdentity(&projectionMatrix);
	D3DXMatrixPerspectiveLH(&projectionMatrix, m_rtWnd.right - m_rtWnd.left, m_rtWnd.bottom - m_rtWnd.top, 1.0f, 10.0f);

	D3DXMATRIX worldMatrix;
	D3DXMatrixIdentity(&worldMatrix);

	D3DXMATRIX translationMatrix;
	D3DXMatrixTranslation(&translationMatrix, -(m_rtWnd.right - m_rtWnd.left) / 2.0f, -(m_rtWnd.bottom - m_rtWnd.top) / 2.0f, 0.0f);
	D3DXMATRIX scalingMatrix;
	D3DXMatrixScaling(&scalingMatrix, 1.0f, -1.0f, 1.0f);
	D3DXMatrixMultiply(&worldMatrix, &translationMatrix, &scalingMatrix);

	D3DXMATRIX viewMatrix;
	D3DXMatrixIdentity(&viewMatrix);

	D3DXVECTOR3 eye = { 0.0f, 0.0f, -1.0f };
	D3DXVECTOR3 at = {0.0f, 0.0f, 0.0f};
	D3DXVECTOR3 up = {0.0f, 1.0f, 0.0f};
	D3DXMatrixLookAtLH(&viewMatrix, &eye, &at, &up);

	D3DXVECTOR4 vec = { 0.0f, 0.0f, 0.0f, 1.0f };
	D3DXVec4Transform(&vec, &vec, &worldMatrix);
	D3DXVec4Transform(&vec, &vec, &viewMatrix);
	D3DXVec4Transform(&vec, &vec, &projectionMatrix);

	m_pD3DDevice9->SetTransform(D3DTS_VIEW, &viewMatrix);
	m_pD3DDevice9->SetTransform(D3DTS_WORLD, &worldMatrix);
	m_pD3DDevice9->SetTransform(D3DTS_PROJECTION, &projectionMatrix);

	return true;
}

void D3D9Renderer::Clear(COLORREF dwClearColor)
{
	m_pD3DDevice9->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, COLORREF_TO_D3DCOLOR(dwClearColor), 1.0f, 0);
}

void D3D9Renderer::DrawText(tstring const &str, LPRECT lpRect, COLORREF color, tstring const &strFont, int nFontSize)
{
	LPD3DXFONT pFont = nullptr;
	pFont = m_fontPool.GetObject(std::make_pair(nFontSize, strFont));

	if (pFont == nullptr)
	{
		return;
	}

	pFont->DrawText(nullptr, str.c_str(), str.length(), lpRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE, COLORREF_TO_D3DCOLOR(color));
}

void D3D9Renderer::DrawLine(int x1, int y1, int x2, int y2, COLORREF dwColor, int nWidth/* = 1*/)
{
	static const float fzValue = 0.01f;
	CustomVertex customVertice[] =
	{
		x1, y1, fzValue, COLORREF_TO_D3DCOLOR(dwColor),
		x2, y2, fzValue, COLORREF_TO_D3DCOLOR(dwColor)
	};

	m_pD3DDevice9->SetFVF(D3DFVF_CUSTOM);
	m_pD3DDevice9->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, &customVertice, sizeof(CustomVertex));
}

void D3D9Renderer::DrawRect(LPRECT lpRect, COLORREF dwColorStroke, int nWidth, BOOL bFill, COLORREF dwColorFill)
{
	static const float fzValue = 0.01f;
	CustomVertex customVertice[] =
	{
		lpRect->left, lpRect->bottom, fzValue, COLORREF_TO_D3DCOLOR(dwColorStroke),
		lpRect->left, lpRect->top, fzValue, COLORREF_TO_D3DCOLOR(dwColorStroke),
		lpRect->right, lpRect->bottom, fzValue, COLORREF_TO_D3DCOLOR(dwColorStroke),
		lpRect->right, lpRect->top, fzValue, COLORREF_TO_D3DCOLOR(dwColorStroke),
	};

	m_pD3DDevice9->SetFVF(D3DFVF_CUSTOM);
	m_pD3DDevice9->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &customVertice, sizeof(CustomVertex));
}

void D3D9Renderer::DrawRoundRect(LPRECT lpRect, COLORREF dwColorStroke, int nWidth, BOOL bFill, COLORREF dwColorFill, int nRW, int nRH)
{
	// call DrawRect temporarily, will be implement later
	DrawRect(lpRect, dwColorStroke, nWidth, bFill, dwColorFill);
}

bool D3D9Renderer::BeginPaint()
{
	return S_OK == m_pD3DDevice9->BeginScene();
}

void D3D9Renderer::EndPaint()
{
	m_pD3DDevice9->EndScene();
}

void D3D9Renderer::Present(HWND hWnd)
{
	m_pD3DDevice9->Present(nullptr, nullptr, nullptr, nullptr);
}

#endif