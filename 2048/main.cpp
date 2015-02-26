#include "Common.h"
#include "GameLooper.h"

int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	GameLooper looper;
	if (looper.Initialize())
	{
		looper.EnterLoop();
	}

	return 0;
}