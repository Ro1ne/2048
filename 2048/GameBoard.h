#ifndef _GAME_BOARD_H_
#define _GAME_BOARD_H_

#include <vector>

struct GameBlock
{
	int m_nValue;
};

template <class T>
using GameBoardData = std::vector<std::vector<T >>;

class GameBoard
{
public:
	GameBoard(int nWidth, int nHeight);
	GameBlock &GetBlock(int nX, int nY);
	void SetBlock(int nX, int nY, GameBlock const &block);
	int GetWidth()  const { return m_nWidth; }
	int GetHeight()  const { return m_nHeight; }
	bool Initialize();
private:
	GameBoardData<GameBlock> m_vBoard;
	int m_nWidth;
	int m_nHeight;
};

#endif //_GAME_BOARD_H_
