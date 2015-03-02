#include "Common.h"
#include "D2DRenderer.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

template<class T>
static inline void SafeRelease(T **pUnk)
{
	if (*pUnk)
	{
		(*pUnk)->Release();
		(*pUnk) = nullptr;
	}
}

static inline D2D1_COLOR_F COLORREF_TO_D2DCOLOR(COLORREF color, int nAlpha = 255)
{
	return D2D1::ColorF(GetRValue(color) / 255.0f, GetGValue(color) / 255.0f, GetBValue(color) / 255.0f, nAlpha / 255.0f);
}

static inline D2D1_RECT_F RECT_TO_D2DRECT(RECT rt)
{
	return D2D1::RectF(rt.left, rt.top, rt.right, rt.bottom);
}

D2DRenderer::D2DRenderer(HWND hWnd)
: BaseRenderer(hWnd)
, m_pRenderTarget(nullptr)
, m_pD2DFactory(nullptr)
, m_pColorBrush(nullptr)
, m_pTextFormat(nullptr)
, m_pDWriteFactory(nullptr)
{
}

D2DRenderer::~D2DRenderer()
{
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_pD2DFactory);
	SafeRelease(&m_pColorBrush);
	SafeRelease(&m_pTextFormat);
	SafeRelease(&m_pDWriteFactory);
};

bool D2DRenderer::Initialize()
{
	HRESULT hr = S_OK;

	if (!m_hWnd)
	{
		hr = E_FAIL;
		goto exit;
	}

	CreateDeviceDependentResource();

exit:
	return hr == S_OK;
}

void D2DRenderer::Clear(COLORREF dwClearColor)
{
	m_pRenderTarget->Clear(COLORREF_TO_D2DCOLOR(dwClearColor));
}

void D2DRenderer::DrawText(tstring const &str, LPRECT lpRect, COLORREF color, tstring const &strFont, int nFontSize)
{
	m_pColorBrush->SetColor(COLORREF_TO_D2DCOLOR(color));

	IDWriteTextFormat *pTextFormat = nullptr;
	m_pDWriteFactory->CreateTextFormat(
		strFont.c_str(),
		nullptr,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		nFontSize,
		_T(""),
		&pTextFormat
		);

	pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

	m_pRenderTarget->DrawText(str.c_str(), str.length(), pTextFormat, RECT_TO_D2DRECT(*lpRect), m_pColorBrush);
	SafeRelease(&pTextFormat);
}

void D2DRenderer::DrawRect(LPRECT lpRect, COLORREF dwColorStroke, int nWidth, BOOL bFill, COLORREF dwColorFill)
{
	D2D1_RECT_F rt = RECT_TO_D2DRECT(*lpRect);
	if (bFill)
	{
		m_pColorBrush->SetColor(COLORREF_TO_D2DCOLOR(dwColorFill));
		m_pRenderTarget->FillRectangle(rt, m_pColorBrush);
	}

	m_pColorBrush->SetColor(COLORREF_TO_D2DCOLOR(dwColorStroke));
	m_pRenderTarget->DrawRectangle(rt, m_pColorBrush, nWidth);
}

void D2DRenderer::DrawRoundRect(LPRECT lpRect, COLORREF dwColorStroke, int nWidth, BOOL bFill, COLORREF dwColorFill, int nRW, int nRH)
{
	D2D1_ROUNDED_RECT rt;
	rt.rect = RECT_TO_D2DRECT(*lpRect);
	rt.radiusX = nRW;
	rt.radiusY = nRH;

	if (bFill)
	{
		m_pColorBrush->SetColor(COLORREF_TO_D2DCOLOR(dwColorFill));	
		m_pRenderTarget->FillRoundedRectangle(rt, m_pColorBrush);
	}

	m_pColorBrush->SetColor(COLORREF_TO_D2DCOLOR(dwColorStroke));
	m_pRenderTarget->DrawRoundedRectangle(rt, m_pColorBrush, nWidth);
}


void D2DRenderer::Present(HWND hWnd)
{

}

void D2DRenderer::DrawLine(int x1, int y1, int x2, int y2, COLORREF dwColor, int nWidth /*= 1*/)
{
	m_pColorBrush->SetColor(COLORREF_TO_D2DCOLOR(dwColor));
	m_pRenderTarget->DrawLine(D2D1::Point2F(x1, y1), D2D1::Point2F(x2, y2), m_pColorBrush, nWidth);
}

bool D2DRenderer::BeginPaint()
{
	if (SUCCEEDED(CreateDeviceResource()))
	{
		m_pRenderTarget->BeginDraw();
	}
	
	return true;
}

void D2DRenderer::EndPaint()
{
	if (m_pRenderTarget)
	{
		HRESULT hr = m_pRenderTarget->EndDraw();
		if (hr == D2DERR_RECREATE_TARGET)
		{
			// discard resource
			DiscardDeviceResource();
		}
	}
}

HRESULT D2DRenderer::CreateDeviceResource()
{
	HRESULT hr = S_OK;
	if (!m_hWnd)
	{
		hr = E_FAIL;
		goto exit;
	}

	if (!m_pRenderTarget)
	{
		GetClientRect(m_hWnd, &m_rtWnd);
		D2D1_SIZE_U size = D2D1::SizeU(
			m_rtWnd.right - m_rtWnd.left,
			m_rtWnd.bottom - m_rtWnd.top
			);

		hr = m_pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hWnd, size), &m_pRenderTarget);
		if (FAILED(hr))
		{
			goto exit;
		}

		hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pColorBrush);
		if (FAILED(hr))
		{
			goto exit;
		}
	}

exit:
	return hr;
}
HRESULT D2DRenderer::CreateDeviceDependentResource()
{
	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
	if (FAILED(hr))
	{
		goto exit;
	}

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_ISOLATED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&m_pDWriteFactory));
	if (FAILED(hr))
	{
		goto exit;
	}

	hr = m_pDWriteFactory->CreateTextFormat(
		_T("SimHei"),
		nullptr,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		40,
		_T(""),
		&m_pTextFormat
		);

exit:
	return hr;
}

void D2DRenderer::DiscardDeviceResource()
{
	SafeRelease(&m_pColorBrush);
	SafeRelease(&m_pRenderTarget);
}