#pragma once
#include "BaseRenderer.h"
#include "ObjectPool.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <map>

class D3DXFontPool : public ObjectPool<std::pair<int, tstring>, LPD3DXFONT>
{
protected:
	virtual LPD3DXFONT CreateNewObj(std::pair<int, tstring> key)
	{
		LPD3DXFONT pFont = nullptr;
		LPDIRECT3DDEVICE9 pDevice = (LPDIRECT3DDEVICE9)m_pContext;
		D3DXCreateFont(pDevice, key.first, 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, \
					CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, key.second.c_str(), &pFont);

		return pFont;
	}
	virtual void DestroyObj(LPD3DXFONT pFont)
	{
		SafeRelease(&pFont);
	}
};

class D3D9Renderer : public BaseRenderer
{
public:
	D3D9Renderer(HWND hWnd);
	~D3D9Renderer();
	virtual bool Initialize();
	virtual void Clear(COLORREF dwClearColor);
	virtual void DrawText(tstring const &str, LPRECT lpRect, COLORREF color, tstring const &strFont, int nFontSize);
	virtual void DrawLine(int x1, int y1, int x2, int y2, COLORREF dwColor, int nWidth = 1);
	virtual void DrawRect(LPRECT lpRect, COLORREF dwColorStroke, int nWidth, BOOL bFill, COLORREF dwColorFill);
	virtual void DrawRoundRect(LPRECT lpRect, COLORREF dwColorStroke, int nWidth, BOOL bFill, COLORREF dwColorFill, int nRW, int nRH);
	virtual bool BeginPaint();
	virtual void EndPaint();
	virtual void Present(HWND hWnd);

private:
	LPDIRECT3D9 m_pD3D9;
	LPDIRECT3DDEVICE9 m_pD3DDevice9;
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBufferLine;
	D3DXFontPool m_fontPool;
};

