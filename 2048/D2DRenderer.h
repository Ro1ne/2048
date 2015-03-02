#pragma once

#include <d2d1.h>
#include <dwrite.h>
#include "BaseRenderer.h"
#include "ObjectPool.h"

class TextFormatPool : public ObjectPool<std::pair<int, tstring>, IDWriteTextFormat *>
{
protected:
	virtual  IDWriteTextFormat * CreateNewObj(std::pair<int, tstring> key)
	{
		IDWriteTextFormat *pFormat = nullptr;
		IDWriteFactory *pFactory = (IDWriteFactory *)m_pContext;
		pFactory->CreateTextFormat(
			key.second.c_str(),
			nullptr,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			key.first,
			_T(""),
			&pFormat
			);

		return pFormat;
	}
	virtual void DestroyObj(IDWriteTextFormat *pTextformat)
	{
		SafeRelease(&pTextformat);
	}
};

class D2DRenderer : public BaseRenderer
{
public:
	D2DRenderer(HWND hWnd);
	virtual ~D2DRenderer();
	virtual bool Initialize();
	virtual void Clear(COLORREF dwClearColor);
	virtual void DrawText(tstring const &str, LPRECT lpRect, COLORREF color, tstring const &strFont, int nFontSize);
	virtual void DrawLine(int x1, int y1, int x2, int y2, COLORREF dwColor, int nWidth = 1);
	virtual void DrawRect(LPRECT lpRect, COLORREF dwColorStroke, int nWidth, BOOL bFill, COLORREF dwColorFill);
	virtual void DrawRoundRect(LPRECT lpRect, COLORREF dwColorStroke, int nWidth, BOOL bFill, COLORREF dwColorFill, int nRW, int nRH);
	virtual void Present(HWND hWnd);
	virtual bool BeginPaint();
	virtual void EndPaint();

private:
	HRESULT CreateDeviceResource();
	HRESULT CreateDeviceDependentResource();
	void DiscardDeviceResource();
protected:
	ID2D1Factory *m_pD2DFactory;
	ID2D1HwndRenderTarget *m_pRenderTarget;
	ID2D1SolidColorBrush *m_pColorBrush;
	IDWriteTextFormat *m_pTextFormat;
	IDWriteFactory *m_pDWriteFactory;
	TextFormatPool m_textformatPool;
};

