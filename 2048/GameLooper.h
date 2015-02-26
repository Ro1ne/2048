#ifndef _GAME_LOOPER_H_
#define _GAME_LOOPER_H_

class GameController;
class GameLooper
{
public:
	GameLooper();
	~GameLooper();
	void EnterLoop();
	bool Initialize();
	void Quit();

	HWND GetHWND() const { return m_hWnd; }

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam);
	bool WindowProc(UINT nMessage, WPARAM wParam, LPARAM lParam);

private:
	void GameMain();

private:
	bool bGameLooping;
	HWND m_hWnd;
	GameController *m_pGameController;
};

#endif //_GAME_LOOPER_H_