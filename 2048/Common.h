#ifndef _COMMON_H_
#define _COMMON_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tchar.h>
#include <string>

#define GAMETITLE_HEIGHT 200
#define GAMEWINDOW_WIDTH 600
#define GAMEWINDOW_HEIGHT 800


#define GAMEBOARD_WIDTH 4
#define GAMEBOARD_HEIGHT 4

#define GAME_WND_CLASS _T("2048Wnd")
#define GAME_WND_TITLE _T("2048 Game")

#define GAME_LEVEL 8
#define GAME_WON_SCORE 2048

static const COLORREF g_aPalette[] = 
{
	RGB(238, 228, 218), // 2
	RGB(237, 224, 200), // 4
	RGB(244, 177, 121), // 8
	RGB(245, 149, 99), // 16
	RGB(246, 124, 95), // 32
	RGB(228, 88, 49), // 64
	RGB(218, 190, 101), // 128
	RGB(219, 189, 84), // 256
	RGB(219, 185, 66), // 512
	RGB(219, 182, 49), // 1024
	RGB(217, 181, 32) // 2048
};

static const UINT g_nPaletteSize = ARRAYSIZE(g_aPalette);

#ifndef _UNICODE
#define tstring std::string 
#define to_string std::string 
#else
#define tstring std::wstring 
#define to_string std::to_wstring 
#endif

#define USE_D2D_RENDERER

extern HINSTANCE g_hInstance;

#endif//_COMMON_H_
