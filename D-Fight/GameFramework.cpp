#include "stdafx.h"
#include "GameFramework.h"

CGameFramework::CGameFramework()
{
	m_pRenderer = CRenderer::GetInstance();
}

CGameFramework::~CGameFramework()
{

}

CGameFramework* CGameFramework::GetInstance()
{
	static CGameFramework instance;
	return &instance;
}

int CGameFramework::Loop(GameInputs* inputs)
{
	// 경과 시간 계산
	int currentTime = glutGet(GLUT_ELAPSED_TIME);

	// 처음 실행 시 currentTime이 너무 커지는 거 방지
	if (m_fPrevTimeInMillisecond < 1.0f)
		currentTime = int(1000 / FRAME_LIMIT);

	int elapsedTime = currentTime - m_fPrevTimeInMillisecond;
	m_fPrevTimeInMillisecond = currentTime;
	float elapsedTimeInSec = (float)elapsedTime / 1000.f;

	// 프레임 제한 계산
	int waitTime = 0;
	if (elapsedTime < int(1000 / FRAME_LIMIT))
	{
		waitTime = int(1000 / FRAME_LIMIT) - elapsedTime;

		if (waitTime < 0)
			waitTime = 0;
	}

	// 과도한 경과 시간 증가 처리
	if (elapsedTimeInSec > MAX_ELAPSED_TIME)
		elapsedTimeInSec = MAX_ELAPSED_TIME;

	// 프레임 계산
	m_fSecond += (elapsedTimeInSec + ((float)waitTime / 1000.f));
	if (m_fSecond > 0.3)
	{
		int fps = (int)(1.0f / elapsedTimeInSec + ((float)waitTime / 1000.f));

		sprintf_s(m_cFpsText, sizeof(m_cFpsText), "E-Fight (%dfps)", fps);
		m_fSecond = 0.0f;
		glutSetWindowTitle(m_cFpsText);
	}


	Update(elapsedTimeInSec + ((float)waitTime / 1000.f), inputs);

	RenderScene();

	return waitTime;
}

void CGameFramework::ChangeState(CGameState* gameState, bool keep)
{
	std::cout << "Change State: \n";
	if (m_pGameStateStack.size() > 0) 
	{
		std::cout << " 현재 스테이트 종료\n";
		CGameState* tempState = m_pGameStateStack.top();
		tempState->Exit();
		m_pGameStateStack.pop();
		if (!keep) {
			delete tempState;
		}
	}
	std::cout << " 새로운 스테이트 시작\n";
	m_pGameStateStack.push(gameState);
	m_pGameStateStack.top()->Enter();
}

void CGameFramework::PushState(CGameState* gameState)
{
	std::cout << "push State: \n";
	if (m_pGameStateStack.size() > 0)
	{
		std::cout << " 현재 스테이트 멈춤\n";
		m_pGameStateStack.top()->Pause();
	}
	std::cout << " 새로운 스테이트 시작\n";
	m_pGameStateStack.push(gameState);
	m_pGameStateStack.top()->Enter();
}

void CGameFramework::PopState(bool keep)
{
	std::cout << "Pop State: \n";
	if (m_pGameStateStack.size() > 1)
	{
		std::cout << " 현재 스테이트 종료\n";
		CGameState* tempState = m_pGameStateStack.top();
		tempState->Exit();
		m_pGameStateStack.pop();
		if (!keep) {
			delete tempState;
		}
	}
	else
	{
		if (m_pGameStateStack.size() == 1)
			std::cout << " 스테이트가 1개 입니다.\n";
		if (m_pGameStateStack.size() == 0)
			std::cout << " 스테이트가 없습니다.\n";
	}

	if (m_pGameStateStack.size() > 0) 
	{
		std::cout << " 이전 스테이트 재시작\n";
		m_pGameStateStack.top()->Resume();
	}
	else
	{
		std::cout << " 스테이트가 없습니다.\n";
	}
}

void CGameFramework::ConvertWithPrevState()
{
	std::cout << "Convert With Prev State: \n";
	if (m_pGameStateStack.size() > 0)
	{
		std::cout << " 현재 스테이트 멈춤\n";
		CGameState* frontState = m_pGameStateStack.top();
		frontState->Pause();
		m_pGameStateStack.pop();
		
		CGameState* backState = m_pGameStateStack.top();
		m_pGameStateStack.pop();

		// 현재 스테이트와 이전 스테이트 순서 변경
		m_pGameStateStack.push(frontState);
		m_pGameStateStack.push(backState);

		std::cout << " 이전 스테이트 재시작\n";
		m_pGameStateStack.top()->Resume();
	}
	else
	{
		std::cout << " 스테이트가 1개 입니다.\n";
	}
}

void CGameFramework::PopPrevState(bool keep)
{
	std::cout << "Pop Prev State: \n";
	if (m_pGameStateStack.size() > 0)
	{
		CGameState* frontState = m_pGameStateStack.top();
		m_pGameStateStack.pop();

		CGameState* backState = m_pGameStateStack.top();
		m_pGameStateStack.pop();

		// 현재 스테이트와 이전 스테이트 순서 변경
		m_pGameStateStack.push(frontState);

		std::cout << " 이전 스테이트 제거\n";
		
		if (keep) {
			backState->Pause();
		}
		else {
			backState->Exit();
			delete backState;
		}
	}
}

void CGameFramework::SetWindowSize(int windowSizeX, int windowSizeY)
{
	m_iWindowSizeX = windowSizeX;
	m_iWindowSizeY = windowSizeY;

	m_pRenderer->SetDisplayResolution(m_iWindowSizeX, m_iWindowSizeY);
	m_pRenderer->SetTextureAndRenderBufferResize();
}

void CGameFramework::Update(float elapsedTimeInSec, GameInputs* inputs)
{
	// 프레임 가속
	//if (inputs->KeyLeftCtrl)
	//{
	//	elapsedTimeInSec *= 4;
	//}

	if (m_pGameStateStack.top() != NULL)
	{
		m_pGameStateStack.top()->ProcessInputs(elapsedTimeInSec, inputs);
		m_pGameStateStack.top()->Update(elapsedTimeInSec);
	}
}

void CGameFramework::RenderScene()
{
	m_pRenderer->SetDisplay();
	if (m_pGameStateStack.top() != NULL)
	{
		m_pGameStateStack.top()->RenderScene();
	}
}

CGameState* CGameFramework::GetPrevState()
{
	if (m_pGameStateStack.size() <= 1) {
		std::cout << " 스테이트가 1개이하 입니다.\n";
		return NULL;
	}
	else {
		CGameState* frontState = m_pGameStateStack.top();
		m_pGameStateStack.pop();
		CGameState* backState = m_pGameStateStack.top();
		m_pGameStateStack.push(frontState);

		return backState;
	}
}

void CGameFramework::SetIsEnableToEnterSystemMenu(bool enable)
{
	m_bIsEnableToEnterSystemMenu = enable;
}

bool CGameFramework::GetIsEnableToEnterSystemMenu() const
{
	return m_bIsEnableToEnterSystemMenu;
}
