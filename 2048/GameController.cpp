#include "Common.h"
#include "GameController.h"
#include "GameLooper.h"
#include "GameBoard.h"
#include "GameRenderer.h"
#include <deque>
#include <algorithm>

GameController::GameController(GameLooper *pLooper)
: m_pLooper(pLooper)
, m_pGameBoard(nullptr)
, m_nTotalScore(0)
{
}


GameController::~GameController()
{
	if (m_pGameBoard)
	{
		delete m_pGameBoard;
		m_pGameBoard = nullptr;
	}
}

bool GameController::Initialize()
{
	if (!m_pLooper)
	{
		return false;
	}

	m_nTotalScore = 0;

	// Initialize Game Board
	m_pGameBoard = new GameBoard(GAMEBOARD_WIDTH, GAMEBOARD_HEIGHT);
	if (!m_pGameBoard || !m_pGameBoard->Initialize())
	{
		return false;
	}

	for (int y = 0; y < m_pGameBoard->GetWidth(); ++y)
	{
		for (int x = 0; x < m_pGameBoard->GetHeight(); ++x)
		{
			m_pGameBoard->SetBlock(x, y, {0});
		}
	}

	// Put Initial value
	int nRandX = rand() % m_pGameBoard->GetWidth();
	int nRandY = rand() % m_pGameBoard->GetHeight();
	m_pGameBoard->SetBlock(nRandX, nRandY, { 2 });

	HWND hWnd = m_pLooper->GetHWND();
	if (!hWnd)
	{
		return false;
	}

	// Initialize Game Renderer
	m_pGameRenderer = new GameRenderer(hWnd);
	if (!m_pGameRenderer || !m_pGameRenderer->Initialize())
	{
		return false;
	}

	return true;
}

void GameController::HandleKey(UINT nKeyCode)
{
	switch (nKeyCode)
	{
	case VK_ESCAPE:
		if (m_pLooper)
		{
			m_pLooper->Quit();
		}
		break;

	case VK_UP:
		ProcessKey(kUp);
		break;
	case VK_DOWN:
		ProcessKey(kDown);
		break;
	case VK_LEFT:
		ProcessKey(kLeft);
		break;
	case VK_RIGHT:
		ProcessKey(kRight);
		break;

	default:
		break;
	}
}

void GameController::ProcessKey(KPAD_DIR kDir)
{
	auto ProcessLine = [](std::vector<GameBlock *> const &vpBlocks, std::vector<GameBlock> &vResults, BOOL bBackward, INT64 &nTotalScore)
	{
		// process row
		std::for_each(vpBlocks.begin(), vpBlocks.end(), [&vResults](GameBlock const *pBlock)
		{
			if (pBlock->m_nValue != 0)
			{
				vResults.push_back(*pBlock);
			}
		});

	
		if (vResults.size() > 0)
		{
			std::deque<GameBlock> vResTmp;
			if (!bBackward)
			{	
				for (int i = 0; i < vResults.size(); ++i)
				{
					if (i == vResults.size() - 1)
					{
						vResTmp.push_back(vResults[i]);
						break;
					}

					if (vResults[i].m_nValue != vResults[i + 1].m_nValue)
					{
						vResTmp.push_back(vResults[i]);
					}
					else
					{
						int nValue = vResults[i].m_nValue + vResults[i + 1].m_nValue;
						nTotalScore += nValue;
						vResTmp.push_back({ nValue });
						++i;
					}
				}
			}
			else
			{
				for (int i = vResults.size() - 1; i >= 0; --i)
				{
					if (i == 0)
					{
						vResTmp.push_front(vResults[i]);
						break;
					}

					if (vResults[i].m_nValue != vResults[i - 1].m_nValue)
					{
						vResTmp.push_front(vResults[i]);
					}
					else
					{
						int nValue = vResults[i].m_nValue + vResults[i - 1].m_nValue;
						nTotalScore += nValue;
						vResTmp.push_front({ nValue });
						--i;
					}
				}	
			}

			vResults.clear();
			std::copy(vResTmp.begin(), vResTmp.end(), std::back_inserter(vResults));
		}
	};

	auto CopyBack = [](std::vector<GameBlock *> vpBlocks, std::vector<GameBlock> const &vResults, bool bBackward)
	{
		if (bBackward)
		{
			auto itr = vResults.rbegin();
			for (int i = vpBlocks.size() - 1; i >= 0; --i)
			{
				if (itr != vResults.rend())
				{
					vpBlocks[i]->m_nValue = itr->m_nValue;
					++itr;
				}
				else
				{
					vpBlocks[i]->m_nValue = 0;
				}
			}
		}
		else
		{
			auto itr = vResults.begin();
			for (int i = 0; i < vpBlocks.size(); ++i)
			{
				if (itr != vResults.end())
				{
					vpBlocks[i]->m_nValue = itr->m_nValue;
					++itr;
				}
				else
				{
					vpBlocks[i]->m_nValue = 0;
				}
			}
		}
	};

	if (kDir == kLeft || kDir == kRight)
	{
		// row	
		for (int y = 0; y < m_pGameBoard->GetWidth(); ++y)
		{
			std::vector<GameBlock *> vpBlocks;
			for (int x = 0; x < m_pGameBoard->GetHeight(); ++x)
			{
				GameBlock &block = m_pGameBoard->GetBlock(x, y);
				vpBlocks.push_back(&block);
			}

			// process row
			std::vector<GameBlock> vResults;
			ProcessLine(vpBlocks, vResults, kDir == kRight, m_nTotalScore);
			CopyBack(vpBlocks, vResults, kDir == kRight);
		}
	}

	if (kDir == kUp || kDir == kDown)
	{
		// row	
		for (int x = 0; x < m_pGameBoard->GetWidth(); ++x)
		{
			std::vector<GameBlock *> vpBlocks;
			for (int y = 0; y < m_pGameBoard->GetHeight(); ++y)
			{
				GameBlock &block = m_pGameBoard->GetBlock(x, y);
				vpBlocks.push_back(&block);
			}

			// process row
			std::vector<GameBlock> vResults;
			ProcessLine(vpBlocks, vResults, kDir == kDown, m_nTotalScore);
			CopyBack(vpBlocks, vResults, kDir == kDown);
		}
	}

	// Check if win
	for (int x = 0; x < m_pGameBoard->GetWidth(); ++x)
	{
		for (int y = 0; y < m_pGameBoard->GetHeight(); ++y)
		{
			GameBlock &block = m_pGameBoard->GetBlock(x, y);
			if (block.m_nValue == GAME_WON_SCORE)
			{
				// Render once
				GameMain();

				if (IDYES == ::MessageBox(m_pLooper->GetHWND(), _T("Congratulations! Continue?"), _T("You Win"), MB_YESNO))
				{
					Initialize();
				}
				else
				{
					m_pLooper->Quit();
				}

				return;
			}
		}
	}

	// Generate new element
	std::vector<std::pair<int, int> > vEmpty;
	for (int x = 0; x < m_pGameBoard->GetWidth(); ++x)
	{
		for (int y = 0; y < m_pGameBoard->GetHeight(); ++y)
		{
			GameBlock &block = m_pGameBoard->GetBlock(x, y);
			if (block.m_nValue == 0)
			{
				vEmpty.push_back({ x, y });
			}
		}
	}

	if (vEmpty.size() == 0)
	{
		// Game over...
		if (IDYES == ::MessageBox(m_pLooper->GetHWND(), _T("Game Over. Continue?"), GAME_WND_TITLE, MB_YESNO))
		{
			Initialize();
		}
		else
		{
			m_pLooper->Quit();
		}
	}
	else
	{
		int nRand = rand() % vEmpty.size();

		int nValue = rand() % 100;
		int nGenerated = 2;
		if (nValue < 10 * (10 - GAME_LEVEL))
		{
			nGenerated = 4;
		}

		m_pGameBoard->SetBlock(vEmpty[nRand].first, vEmpty[nRand].second, { nGenerated });
	}
}

void GameController::GameMain()
{
	if (!m_pGameBoard || !m_pGameRenderer)
	{
		return;
	}

	m_pGameRenderer->Clear(RGB(173, 160, 147));

	DrawTitle();
	DrawBoard();
}

void GameController::DrawTitle()
{
	RECT rt;
	GetClientRect(m_pLooper->GetHWND(), &rt);
	rt.bottom = GAMETITLE_HEIGHT / 2;

	m_pGameRenderer->DrawText(_T("2048 Game"), &rt, RGB(119, 110, 101), _T("Arial"), 60);

	rt.top = GAMETITLE_HEIGHT / 2;
	rt.bottom = rt.top + GAMETITLE_HEIGHT / 2;
	m_pGameRenderer->DrawText(_T("Score :") + std::to_string(m_nTotalScore), &rt,  RGB(255, 255, 255), _T("Arial"), 40);
}

void GameController::DrawBoard()
{
	RECT rt;
	GetClientRect(m_pLooper->GetHWND(), &rt);

	rt.top += GAMETITLE_HEIGHT;
	m_pGameRenderer->DrawRect(&rt, RGB(173, 160, 147), 1, TRUE, RGB(173, 160, 147));

	int nWidth = rt.right - rt.left;
	int nHeight = rt.bottom - rt.top;

	//
	int nBoxWidth = nWidth / m_pGameBoard->GetWidth();
	int nBoxHeight = nHeight / m_pGameBoard->GetHeight();

	RECT rtBox;
	SetRectEmpty(&rtBox);
	for (int y = 0; y < m_pGameBoard->GetWidth(); ++y)
	{
		for (int x = 0; x < m_pGameBoard->GetHeight(); ++x)
		{
			SetRect(&rtBox, x * nBoxWidth, y * nBoxHeight + GAMETITLE_HEIGHT, x * nBoxWidth + nBoxWidth, y * nBoxHeight + nBoxHeight + GAMETITLE_HEIGHT);

			COLORREF colorFrame = RGB(173, 160, 147);
			int nFrameWidth = 10;
			m_pGameRenderer->DrawRoundRect(&rtBox, colorFrame, 1, TRUE, colorFrame, 10, 10);

			InflateRect(&rtBox, -nFrameWidth / 2, -nFrameWidth / 2);
			int nValue = m_pGameBoard->GetBlock(x, y).m_nValue;

			COLORREF colorBox = RGB(188, 178, 164);

			if (nValue > 0)
			{
				int nIdx = log2(nValue);

				if (nIdx >= g_nPaletteSize)
				{
					nIdx = g_nPaletteSize - 1;
				}

				colorBox = g_aPalette[nIdx];
			}

			m_pGameRenderer->DrawRoundRect(&rtBox, colorBox, 1, TRUE, colorBox, 10, 10);
			if (nValue > 0)
			{
				m_pGameRenderer->DrawText(std::to_string(nValue), &rtBox, nValue > 4 ? RGB(255, 255, 255) : RGB(119, 110, 101), _T("Arial"), 60);
			}
		}
	}


	m_pGameRenderer->Present(nullptr);
}