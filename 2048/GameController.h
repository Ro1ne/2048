#ifndef _GAME_CONTROLLER_H_
#define _GAME_CONTROLLER_H_

class GameLooper;
class GameBoard;
class BaseRenderer;

class GameController
{
public:
	enum KPAD_DIR
	{
		kUp,
		kDown,
		kLeft,
		kRight
	};

	GameController(GameLooper *pLooper);
	~GameController();
	bool Initialize();
	void HandleKey(UINT nKeyCode);
	void ProcessKey(KPAD_DIR kDir);
	void GameMain();

protected:
	void DrawTitle();
	void DrawBoard();
private:
	GameLooper *m_pLooper;
	GameBoard *m_pGameBoard;
	BaseRenderer *m_pRenderer;
	INT64 m_nTotalScore;
};

#endif // _GAME_CONTROLLER_H_