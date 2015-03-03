#ifndef _OPENGL11_RENDERER_H_
#define _OPENGL11_RENDERER_H_

#include "BaseRenderer.h"
#include "GdiRenderer.h"
#include <Windows.h>
#include <gl\GL.h>
#include <assert.h>

class BitmapFontListsPool : public ObjectPool<HFONT, DWORD>
{
public:
	BitmapFontListsPool()
		:m_dwListsBase(1000)
	{

	}

protected:
	virtual DWORD CreateNewObj(HFONT key)
	{
		DWORD dwListsBase = m_dwListsBase;
		HDC hDC = (HDC)m_pContext;
		if (wglUseFontBitmaps(hDC, 0, 256, dwListsBase))
		{
			m_dwListsBase += 256;
		}
		return dwListsBase;
	}
	virtual void DestroyObj(DWORD dwLists)
	{
		glDeleteLists(dwLists, 256);
	}

private:
	DWORD m_dwListsBase;
};

class OpenGL11Renderer : public BaseRenderer
{
public:
	OpenGL11Renderer(HWND hWnd);
	~OpenGL11Renderer();
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
	HGLRC m_hGLRC;
	GDIFontPool m_fontPool;
	BitmapFontListsPool m_bitmapFontListPool;
	HDC m_hDC;
};

#endif //_OPENGL11_RENDERER_H_
