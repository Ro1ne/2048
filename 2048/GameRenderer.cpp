#include "Common.h"
#include "GameRenderer.h"


GameRenderer::GameRenderer(HWND hWnd)
: m_hWnd(hWnd)
, m_hMemoryDC(nullptr)
, m_hBitmap(nullptr)
, m_hBitmapOld(nullptr)
{
}


GameRenderer::~GameRenderer()
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


void GameRenderer::Clear(DWORD dwClearColor)
{
	HDC hDC = LockRenderTarget();

	if (!hDC)
	{
		return;
	}

	HBRUSH hBrush = ::CreateSolidBrush(dwClearColor);
	::FillRect(hDC, &m_rtWnd, hBrush);
	::DeleteObject(hBrush);

	UnlockRenderTarget(hDC);
}

bool GameRenderer::Initialize()
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

HDC GameRenderer::LockRenderTarget()
{
	return m_hMemoryDC;
}

void GameRenderer::UnlockRenderTarget(HDC hDC)
{
}

void GameRenderer::DrawText(tstring const &str, LPRECT lpRect, COLORREF color, tstring const &strFont, int nFontSize)
{
	HDC hDC = LockRenderTarget();

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
	HGDIOBJ hOldFont = ::SelectObject(hDC, hFont);

	::SetTextColor(hDC, color);
	::SetBkMode(hDC, TRANSPARENT);
	
	::DrawText(hDC, str.c_str(), -1, lpRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	::SelectObject(hDC, hOldFont);
	::DeleteObject(hFont);

	UnlockRenderTarget(hDC);
}

void GameRenderer::DrawLine(int x1, int y1, int x2, int y2, COLORREF color, int nWidth/* = 1*/)
{
	HDC hDC = LockRenderTarget();

	HPEN hPen = ::CreatePen(PS_SOLID, nWidth, color);
	HGDIOBJ hOldPen = ::SelectObject(hDC, hPen);
	::MoveToEx(hDC, x1, y1, nullptr);
	::LineTo(hDC, x2, y2);

	::SelectObject(hDC, hOldPen);
	::DeleteObject(hPen);
	UnlockRenderTarget(hDC);
}

void GameRenderer::DrawRect(LPRECT lpRect, COLORREF dwColorStroke, int nWidth, BOOL bFill, COLORREF dwColorFill)
{
	HDC hDC = LockRenderTarget();

	HBRUSH hBrush = nullptr;
	HGDIOBJ hOldBrush = nullptr;

	if (bFill)
	{
		hBrush = ::CreateSolidBrush(dwColorFill);
		hOldBrush = ::SelectObject(hDC, hBrush);
	}
	else
	{
		hBrush = (HBRUSH)::GetStockObject(NULL_BRUSH);
		hOldBrush = ::SelectObject(hDC, hBrush);
	}

	HPEN hPen = ::CreatePen(PS_SOLID, nWidth, dwColorStroke);
	HGDIOBJ hOldPen = ::SelectObject(hDC, hPen);
	Rectangle(hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
	
	::SelectObject(hDC, hOldPen);
	::DeleteObject(hPen);

	::SelectObject(hDC, hOldBrush);
	::DeleteObject(hBrush);

	UnlockRenderTarget(hDC);
}

void GameRenderer::DrawRoundRect(LPRECT lpRect, COLORREF dwColorStroke, int nWidth, BOOL bFill, COLORREF dwColorFill, int nRW, int nRH)
{
	HDC hDC = LockRenderTarget();

	HBRUSH hBrush = nullptr;
	HGDIOBJ hOldBrush = nullptr;

	if (bFill)
	{
		hBrush = ::CreateSolidBrush(dwColorFill);
		hOldBrush = ::SelectObject(hDC, hBrush);
	}
	else
	{
		hBrush = (HBRUSH)::GetStockObject(NULL_BRUSH);
		hOldBrush = ::SelectObject(hDC, hBrush);
	}

	HPEN hPen = ::CreatePen(PS_SOLID, nWidth, dwColorStroke);
	HGDIOBJ hOldPen = ::SelectObject(hDC, hPen);
	RoundRect(hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom, nRW, nRH);

	::SelectObject(hDC, hOldPen);
	::DeleteObject(hPen);

	::SelectObject(hDC, hOldBrush);
	::DeleteObject(hBrush);

	UnlockRenderTarget(hDC);
}


void GameRenderer::Present(HWND hWnd)
{
	HDC hMemoryDC = LockRenderTarget();
	
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

	UnlockRenderTarget(hMemoryDC);
}