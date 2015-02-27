#include "Common.h"
#include "GdiRenderer.h"


GdiRenderer::GdiRenderer(HWND hWnd)
: BaseRenderer(hWnd)
, m_hMemoryDC(nullptr)
, m_hBitmap(nullptr)
, m_hBitmapOld(nullptr)
{
}


GdiRenderer::~GdiRenderer()
{
	if (m_hBitmapOld)
	{
		if (m_hMemoryDC)
		{
			::SelectObject(m_hMemoryDC, m_hBitmapOld);
		}
		m_hBitmapOld = nullptr;
	}

	if (m_hBitmap)
	{
		::DeleteObject(m_hBitmap);
		m_hBitmap = nullptr;
	}

	if (m_hMemoryDC)
	{
		::DeleteDC(m_hMemoryDC);
		m_hMemoryDC = nullptr;
	}
}


void GdiRenderer::Clear(DWORD dwClearColor)
{
	HBRUSH hBrush = ::CreateSolidBrush(dwClearColor);
	::FillRect(m_hMemoryDC, &m_rtWnd, hBrush);
	::DeleteObject(hBrush);
}

bool GdiRenderer::Initialize()
{
	if (!m_hWnd)
	{
		return false;
	}

	GetClientRect(m_hWnd, &m_rtWnd);

	HDC hDC = ::GetDC(m_hWnd);

	m_hMemoryDC = ::CreateCompatibleDC(hDC);
	if (!m_hMemoryDC)
	{
		return false;
	}

	m_hBitmap = ::CreateCompatibleBitmap(hDC, m_rtWnd.right, m_rtWnd.bottom);
	m_hBitmapOld = ::SelectObject(m_hMemoryDC, m_hBitmap);

	::ReleaseDC(m_hWnd, hDC);

	return true;
}

void GdiRenderer::DrawText(tstring const &str, LPRECT lpRect, COLORREF color, tstring const &strFont, int nFontSize)
{
	HFONT hFont = ::CreateFont(
		nFontSize,					// nHeight
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
		strFont.c_str()
		);
	HGDIOBJ hOldFont = ::SelectObject(m_hMemoryDC, hFont);

	::SetTextColor(m_hMemoryDC, color);
	::SetBkMode(m_hMemoryDC, TRANSPARENT);
	
	::DrawText(m_hMemoryDC, str.c_str(), -1, lpRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	::SelectObject(m_hMemoryDC, hOldFont);
	::DeleteObject(hFont);
}

void GdiRenderer::DrawLine(int x1, int y1, int x2, int y2, COLORREF color, int nWidth/* = 1*/)
{
	HPEN hPen = ::CreatePen(PS_SOLID, nWidth, color);
	HGDIOBJ hOldPen = ::SelectObject(m_hMemoryDC, hPen);
	::MoveToEx(m_hMemoryDC, x1, y1, nullptr);
	::LineTo(m_hMemoryDC, x2, y2);

	::SelectObject(m_hMemoryDC, hOldPen);
	::DeleteObject(hPen);
}

void GdiRenderer::DrawRect(LPRECT lpRect, COLORREF dwColorStroke, int nWidth, BOOL bFill, COLORREF dwColorFill)
{
	HBRUSH hBrush = nullptr;
	HGDIOBJ hOldBrush = nullptr;

	if (bFill)
	{
		hBrush = ::CreateSolidBrush(dwColorFill);
		hOldBrush = ::SelectObject(m_hMemoryDC, hBrush);
	}
	else
	{
		hBrush = (HBRUSH)::GetStockObject(NULL_BRUSH);
		hOldBrush = ::SelectObject(m_hMemoryDC, hBrush);
	}

	HPEN hPen = ::CreatePen(PS_SOLID, nWidth, dwColorStroke);
	HGDIOBJ hOldPen = ::SelectObject(m_hMemoryDC, hPen);
	Rectangle(m_hMemoryDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
	
	::SelectObject(m_hMemoryDC, hOldPen);
	::DeleteObject(hPen);

	::SelectObject(m_hMemoryDC, hOldBrush);
	::DeleteObject(hBrush);
}

void GdiRenderer::DrawRoundRect(LPRECT lpRect, COLORREF dwColorStroke, int nWidth, BOOL bFill, COLORREF dwColorFill, int nRW, int nRH)
{
	HBRUSH hBrush = nullptr;
	HGDIOBJ hOldBrush = nullptr;

	if (bFill)
	{
		hBrush = ::CreateSolidBrush(dwColorFill);
		hOldBrush = ::SelectObject(m_hMemoryDC, hBrush);
	}
	else
	{
		hBrush = (HBRUSH)::GetStockObject(NULL_BRUSH);
		hOldBrush = ::SelectObject(m_hMemoryDC, hBrush);
	}

	HPEN hPen = ::CreatePen(PS_SOLID, nWidth, dwColorStroke);
	HGDIOBJ hOldPen = ::SelectObject(m_hMemoryDC, hPen);
	RoundRect(m_hMemoryDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom, nRW, nRH);

	::SelectObject(m_hMemoryDC, hOldPen);
	::DeleteObject(hPen);

	::SelectObject(m_hMemoryDC, hOldBrush);
	::DeleteObject(hBrush);
}


void GdiRenderer::Present(HWND hWnd)
{
	if (hWnd == nullptr)
	{
		hWnd = m_hWnd;
	}

	HDC hDC = ::GetDC(hWnd);

	if (hDC)
	{
		::BitBlt(hDC, 0, 0, m_rtWnd.right, m_rtWnd.bottom, m_hMemoryDC, 0, 0, SRCCOPY);
		::ReleaseDC(hWnd, hDC);
	}
	else
	{
		DWORD err = ::GetLastError();
	}
}