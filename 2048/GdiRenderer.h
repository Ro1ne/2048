#ifndef _GDI_RENDERER_H_
#define _GDI_RENDERER_H_

#include "BaseRenderer.h"

class GdiRenderer : public BaseRenderer
{
public:
	GdiRenderer(HWND hWnd);
	~GdiRenderer();
	virtual bool Initialize();
	virtual void Clear(COLORREF dwClearColor);
	virtual void DrawText(tstring const &str, LPRECT lpRect, COLORREF color, tstring const &strFont, int nFontSize);
	virtual void DrawLine(int x1, int y1, int x2, int y2, COLORREF dwColor, int nWidth = 1);
	virtual void DrawRect(LPRECT lpRect, COLORREF dwColorStroke, int nWidth, BOOL bFill, COLORREF dwColorFill);
	virtual void DrawRoundRect(LPRECT lpRect, COLORREF dwColorStroke, int nWidth, BOOL bFill, COLORREF dwColorFill, int nRW, int nRH);
	virtual bool BeginPaint(){ return true; }
	virtual void EndPaint(){}
	virtual void Present(HWND hWnd);
private:
	HDC m_hMemoryDC;
	HBITMAP m_hBitmap;
	HGDIOBJ m_hBitmapOld;
};

#endif //_GDI_RENDERER_H_