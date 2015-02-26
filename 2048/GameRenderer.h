#ifndef _GAME_RENDERER_H_
#define _GAME_RENDERER_H_

class GameRenderer
{
public:
	GameRenderer(HWND hWnd);
	~GameRenderer();
	bool Initialize();
	void Clear(COLORREF dwClearColor);
	void DrawText(tstring const &str, LPRECT lpRect, COLORREF color, tstring const &strFont, int nFontSize);
	void DrawLine(int x1, int y1, int x2, int y2, COLORREF dwColor, int nWidth = 1);
	void DrawRect(LPRECT lpRect, COLORREF dwColorStroke, int nWidth, BOOL bFill, COLORREF dwColorFill);
	void DrawRoundRect(LPRECT lpRect, COLORREF dwColorStroke, int nWidth, BOOL bFill, COLORREF dwColorFill, int nRW, int nRH);
	HDC LockRenderTarget();
	void UnlockRenderTarget(HDC hDC);

	void Present(HWND hWnd);
private:
	HWND m_hWnd;
	HDC m_hMemoryDC;
	HBITMAP m_hBitmap;
	HGDIOBJ m_hBitmapOld;
	RECT m_rtWnd;
};

#endif //_GAME_RENDERER_H_