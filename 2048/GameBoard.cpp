#include "GameBoard.h"

GameBoard::GameBoard(int nWidth, int nHeight)
: m_nWidth(nWidth)
, m_nHeight(nHeight)
{

}

GameBlock &GameBoard::GetBlock(int nX, int nY)
{
	return m_vBoard[nX][nY];
}

void GameBoard::SetBlock(int nX, int nY, GameBlock const &block)
{
	m_vBoard[nX][nY] = block;
}

bool GameBoard::Initialize()
{
	m_vBoard.resize(m_nHeight);

	for (auto &&row : m_vBoard)
	{
		row.resize(m_nWidth);
	}

	return true;
}
