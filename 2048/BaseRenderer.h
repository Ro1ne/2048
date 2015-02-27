#ifndef _BASE_RENDERER_H_
#define _BASE_RENDERER_H_

class BaseRenderer
{
public:
	BaseRenderer(HWND hWnd);
	virtual ~BaseRenderer() { };
	virtual bool Initialize() = 0;
	virtual void Clear(COLORREF dwClearColor) = 0;
	virtual void DrawText(tstring const &str, LPRECT lpRect, COLORREF color, tstring const &strFont, int nFontSize) = 0;
	virtual void DrawLine(int x1, int y1, int x2, int y2, COLORREF dwColor, int nWidth = 1) = 0;
	virtual void DrawRect(LPRECT lpRect, COLORREF dwColorStroke, int nWidth, BOOL bFill, COLORREF dwColorFill) = 0;
	virtual void DrawRoundRect(LPRECT lpRect, COLORREF dwColorStroke, int nWidth, BOOL bFill, COLORREF dwColorFill, int nRW, int nRH) = 0;
	virtual void Present(HWND hWnd) = 0;
	virtual bool BeginPaint() = 0;
	virtual void EndPaint() = 0;
protected:
	RECT m_rtWnd;
	HWND m_hWnd;
};

#endif //_BASE_RENDERER_H_