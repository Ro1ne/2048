#include "Common.h"
#include "GameLooper.h"
#include "GameController.h"
#include <windowsx.h>

extern "C" IMAGE_DOS_HEADER __ImageBase;
HINSTANCE g_hInstance = (HINSTANCE)&__ImageBase;

GameLooper::GameLooper()
: m_pGameController(nullptr)
, bGameLooping(false)
, m_hWnd(nullptr)
{
}


GameLooper::~GameLooper()
{
	if (m_pGameController)
	{
		delete m_pGameController;
		m_pGameController = nullptr;
	}
}


void GameLooper::EnterLoop()
{
	bGameLooping = true;

	MSG msg;
	while (bGameLooping)
	{
		if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}

			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		GameMain();
	}

	bGameLooping = false;
}

void GameLooper::GameMain()
{
	if (m_pGameController)
	{
		m_pGameController->GameMain();
	}
}

bool GameLooper::Initialize()
{
	WNDCLASS wndCls = {};
	wndCls.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndCls.hInstance = g_hInstance;
	wndCls.lpszClassName = GAME_WND_CLASS;
	wndCls.style = CS_HREDRAW | CS_VREDRAW;
	wndCls.lpfnWndProc = WindowProc;

	if (!::RegisterClass(&wndCls))
	{
		return false;
	}

	HWND hWnd = ::CreateWindow(GAME_WND_CLASS, GAME_WND_TITLE, WS_OVERLAPPEDWINDOW & (~WS_MAXIMIZEBOX & ~WS_SIZEBOX),\
							   CW_USEDEFAULT, CW_USEDEFAULT, GAMEWINDOW_WIDTH, GAMEWINDOW_HEIGHT, nullptr, nullptr, g_hInstance, this);
	if (!hWnd)
	{
		return false;
	}
	m_hWnd = hWnd;

	// Initialize Game Controller
	m_pGameController = new GameController(this);
	if (!m_pGameController || !m_pGameController->Initialize())
	{
		return false;
	}

	::ShowWindow(hWnd, SW_SHOW);

	return true;
}


void GameLooper::Quit()
{
	bGameLooping = false;
}

LRESULT CALLBACK GameLooper::WindowProc(HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam)
{
	static GameLooper *pGameLooper = nullptr; 
	if (nMessage == WM_CREATE)
	{
		LPCREATESTRUCT lpCS = reinterpret_cast<LPCREATESTRUCT>(lParam);
		pGameLooper = static_cast<GameLooper *>(lpCS->lpCreateParams);
	}

	if (!pGameLooper || !pGameLooper->WindowProc(nMessage, wParam, lParam))
	{
		return ::DefWindowProc(hWnd, nMessage, wParam, lParam);;
	}

	return 0;
}

bool GameLooper::WindowProc(UINT nMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hDC = nullptr;
	PAINTSTRUCT ps;
	bool bHandled = false;
	switch (nMessage)
	{
	case WM_DESTROY:
		Quit();
		bHandled = true;
		break;

	case WM_ERASEBKGND:
		bHandled = true;
		break;

	case WM_PAINT:
		if (m_hWnd)
		{
			hDC = ::BeginPaint(m_hWnd, &ps);

			m_pGameController->GameMain();

			::EndPaint(m_hWnd, &ps);
			bHandled = true;
		}

		break;

	case WM_KEYDOWN:
		if (m_pGameController)
		{
			m_pGameController->HandleKey(wParam);
			bHandled = true;
		}
		break;
	}

	return bHandled;
}