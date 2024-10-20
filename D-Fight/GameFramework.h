/*
* Game Framework
* 
*	Game State를 관리(업데이트, 렌더링, 변경 등)
*	Renderer를 소유
*/

#pragma once
#include <stack>
#include "Globals.h"
#include "GameState.h"
#include "Renderer.h"

class CGameFramework
{
private:
	CGameFramework();
	~CGameFramework();

public:
	// 싱글톤 패턴 구현 함수
	static CGameFramework* GetInstance();

private:
	// Game State을 임시 저장하기 위한 스택
	std::stack<CGameState*> m_pGameStateStack;

	// 렌더러
	CRenderer* m_pRenderer		= NULL;

	int m_iWindowSizeX;
	int m_iWindowSizeY;

	// 이전까지 경과시간(밀리초 단위(1000분의 1초)
	int m_fPrevTimeInMillisecond = 0;

	// FPS 계수기
	char m_cFpsText[30];
	float m_fSecond = 1;

	// 시스템 메뉴(일시정지 메뉴) 진입 활성화
	bool m_bIsEnableToEnterSystemMenu;

public:
	// 게임 루프(대기 시간 전달)
	int Loop(GameInputs* inputs);

	// 지금 스테이트를 종료하고 받은 state로 변경
	void ChangeState(CGameState* gameState, bool keep);

	// 지금 스테이트를 멈추고 받은 state로 변경
	void PushState(CGameState* gameState);

	// 현재 state를 끝내고 이전 스테이트로 돌아감
	void PopState(bool keep);

	// 현재 state를 멈추고 이전 스테이트로 돌아감(현재 스테이트와 이전 스테이트의 위치 변경)
	void ConvertWithPrevState();

	// 이전 state 제거
	void PopPrevState(bool keep);

	// 윈도우 사이즈 설정
	void SetWindowSize(int windowSizeX, int windowSizeY);

	// 현재 state 업데이트 및 렌더링
	void RenderScene();

	// 이전 state 얻기
	CGameState* GetPrevState();

	void SetIsEnableToEnterSystemMenu(bool enable);
	bool GetIsEnableToEnterSystemMenu() const;

private:
	void Update(float elapsedTimeInSec, GameInputs* inputs);

};

