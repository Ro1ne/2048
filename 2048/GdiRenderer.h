#ifndef _GDI_RENDERER_H_
#define _GDI_RENDERER_H_

#include "BaseRenderer.h"
#include "ObjectPool.h"

class GDIFontPool : public ObjectPool<std::pair<int, tstring>, HFONT>
{
protected:
	virtual HFONT CreateNewObj(std::pair<int, tstring> key)
	{
		HFONT hFont = nullptr;
		hFont = ::CreateFont(
			key.first,					// nHeight
			0,                         // nWidth
			0,                         // nEscapement
			0,                         // nOrientation
			FW_NORMAL,                 // nWeight
			FALSE,                     // bItalic
			FALSE,                     // bUnderline
			0,                         // cStrikeOut
			DEFAULT_CHARSET,           // nCharSet
			OUT_DEFAULT_PRECIS,        // nOutPrecision
			CLIP_DEFAULT_PRECIS,       // nClipPrecision
			CLEARTYPE_QUALITY,         // nQuality
			DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
			key.second.c_str()
			);

		return hFont;
	}
	virtual void DestroyObj(HFONT hFont)
	{
		::DeleteObject(hFont);
	}
};
class GDIBrushPool : public ObjectPool<COLORREF, HBRUSH>
{
protected:
	virtual HBRUSH CreateNewObj(COLORREF key)
	{
		return ::CreateSolidBrush(key);
	}
	virtual void DestroyObj(HBRUSH hBrush)
	{
		::DeleteObject(hBrush);
	}
};

class GDIPenPool : public ObjectPool<std::pair<int, COLORREF>, HPEN>
{
protected:
	virtual HPEN CreateNewObj(std::pair<int, COLORREF> key)
	{
		return ::CreatePen(PS_SOLID, key.first, key.second);
	}
	virtual void DestroyObj(HPEN hPen)
	{
		::DeleteObject(hPen);
	}
};

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
	GDIFontPool m_fontPool;
	GDIPenPool m_penPool;
	GDIBrushPool m_brushPool;
};

#endif //_GDI_RENDERER_H_