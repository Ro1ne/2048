#include "Common.h"
#include "OpenGL11Renderer.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

OpenGL11Renderer::OpenGL11Renderer(HWND hWnd)
: BaseRenderer(hWnd)
, m_hGLRC(nullptr)
, m_hDC(nullptr)
{
}


OpenGL11Renderer::~OpenGL11Renderer()
{
	m_fontPool.Clear();
	m_bitmapFontListPool.Clear();

	wglMakeCurrent(nullptr, nullptr);
	
	if (m_hGLRC)
	{
		wglDeleteContext(m_hGLRC);
		m_hGLRC = nullptr;
	}

	if (m_hDC)
	{
		::ReleaseDC(m_hWnd, m_hDC);
		m_hDC = nullptr;
	}
}

bool OpenGL11Renderer::Initialize()
{
	if (!m_hWnd)
	{
		return false;
	}

	m_hDC = GetDC(m_hWnd);

	m_bitmapFontListPool.Initialize(m_hDC);

	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),  //  size of this pfd  
		1,                     // version number  
		PFD_DRAW_TO_WINDOW |   // support window  
		PFD_SUPPORT_OPENGL |   // support OpenGL  
		PFD_DOUBLEBUFFER,      // double buffered  
		PFD_TYPE_RGBA,         // RGBA type  
		24,                    // 24-bit color depth  
		0, 0, 0, 0, 0, 0,      // color bits ignored  
		0,                     // no alpha buffer  
		0,                     // shift bit ignored  
		0,                     // no accumulation buffer  
		0, 0, 0, 0,            // accum bits ignored  
		32,                    // 32-bit z-buffer      
		0,                     // no stencil buffer  
		0,                     // no auxiliary buffer  
		PFD_MAIN_PLANE,        // main layer  
		0,                     // reserved  
		0, 0, 0                // layer masks ignored  
	};

	int nPixelFormat = ChoosePixelFormat(m_hDC, &pfd);
	if (nPixelFormat == 0)
	{
		return false;
	}

	SetPixelFormat(m_hDC, nPixelFormat, &pfd);

	m_hGLRC = wglCreateContext(m_hDC);
	if (!m_hGLRC)
	{
		return false;
	}

	if (!wglMakeCurrent(m_hDC, m_hGLRC))
	{
		return false;
	}

	GetClientRect(m_hWnd, &m_rtWnd);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(m_rtWnd.left, m_rtWnd.right, m_rtWnd.bottom, m_rtWnd.top, -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	return true;
}

void OpenGL11Renderer::Clear(COLORREF dwClearColor)
{
	glClearColor(GetRValue(dwClearColor) / 255.0f, GetGValue(dwClearColor) / 255.0f, GetBValue(dwClearColor) / 255.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGL11Renderer::DrawText(tstring const &str, LPRECT lpRect, COLORREF color, tstring const &strFont, int nFontSize)
{
	HFONT hFont = m_fontPool.GetObject(std::make_pair(nFontSize, strFont));
	HGDIOBJ hOldFont = ::SelectObject(m_hDC, hFont);
	DWORD dwLists = m_bitmapFontListPool.GetObject(hFont);

	SIZE szText = {};
	GetTextExtentPoint32(m_hDC, str.c_str(), str.length(), &szText);

	glColor3ub(GetRValue(color), GetGValue(color), GetBValue(color));
	glRasterPos2f(lpRect->left + ((lpRect->right - lpRect->left) - szText.cx) / 2, lpRect->bottom - ((lpRect->bottom - lpRect->top) - szText.cy) / 2);
	glListBase(dwLists);
	glCallLists(str.length(), GL_UNSIGNED_SHORT, str.c_str());

	::SelectObject(m_hDC, hOldFont);
}

void OpenGL11Renderer::DrawLine(int x1, int y1, int x2, int y2, COLORREF dwColor, int nWidth/* = 1*/)
{
	static const float fzValue = 0.01f;
	glBegin(GL_LINES);
	glColor3ub(GetRValue(dwColor), GetGValue(dwColor), GetBValue(dwColor));
	glVertex3f(x1, y1, fzValue);
	glColor3ub(GetRValue(dwColor), GetGValue(dwColor), GetBValue(dwColor));
	glVertex3f(x2, y2, fzValue);
	glEnd();
}

void OpenGL11Renderer::DrawRect(LPRECT lpRect, COLORREF dwColorStroke, int nWidth, BOOL bFill, COLORREF dwColorFill)
{
	static const float fzValue = 0.01f;
	glBegin(GL_TRIANGLE_STRIP);
	glColor3ub(GetRValue(dwColorFill), GetGValue(dwColorFill), GetBValue(dwColorFill));
	glVertex3f(lpRect->left, lpRect->bottom, fzValue);
	glColor3ub(GetRValue(dwColorFill), GetGValue(dwColorFill), GetBValue(dwColorFill));
	glVertex3f(lpRect->left, lpRect->top, fzValue);
	glColor3ub(GetRValue(dwColorFill), GetGValue(dwColorFill), GetBValue(dwColorFill));
	glVertex3f(lpRect->right, lpRect->bottom, fzValue);
	glColor3ub(GetRValue(dwColorFill), GetGValue(dwColorFill), GetBValue(dwColorFill));
	glVertex3f(lpRect->right, lpRect->top, fzValue);
	glEnd();
}

void OpenGL11Renderer::DrawRoundRect(LPRECT lpRect, COLORREF dwColorStroke, int nWidth, BOOL bFill, COLORREF dwColorFill, int nRW, int nRH)
{
	// call DrawRect temporarily, will be implement later
	DrawRect(lpRect, dwColorStroke, nWidth, bFill, dwColorFill);
}

bool OpenGL11Renderer::BeginPaint()
{
	return true;
}
void OpenGL11Renderer::EndPaint()
{

}

void OpenGL11Renderer::Present(HWND hWnd)
{
	SwapBuffers(m_hDC);
}