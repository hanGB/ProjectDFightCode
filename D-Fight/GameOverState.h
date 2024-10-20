#pragma once

#include "GameState.h"
#include "GameFramework.h"
#include "TimeOfDaySkybox.h"
#include "Button.h"

class CGameOverState : public CGameState {
public:
	CGameOverState();
	~CGameOverState();

private:
	CGameFramework* m_pGameFramework = NULL;

	CTimeOfDaySkybox* m_pTimeOfDaySkybox = NULL;

	// 로고
	int m_LogoTextureID = EMPTY_VALUE;

	// BGM
	int m_gameoverBGMID = EMPTY_VALUE;
	irrklang::ISound* m_pBGM = NULL;

	int m_WhiteRectTextureID = EMPTY_VALUE;

	std::vector<CButton*> m_ButtonVector;

	int m_iSelectedButton = 0;

public:
	// 플레이어의 키 입력 처리
	virtual void ProcessInputs(float elapsedTimeInSec, GameInputs* inputs);

	virtual void Update(float elapsedTimeInSec);
	virtual void RenderScene();

	// 스테이트 진입시
	virtual void Enter();
	// 스테이트 종료시
	virtual void Exit();
	// 스테이트 임시 이동
	virtual void Pause();
	// 스테이트 복귀
	virtual void Resume();

private:
	// 일반 렌더러(계산된 그림자 이용)
	virtual void NormalRender();

	// UI 렌더링
	virtual void UIRender();

	// 필요 모델, 텍스쳐, 사운드 로드
	virtual void LoadData();

	// 처음 State가 만들어졌을때 오브젝트 매니저에 오브젝트 추가
	virtual void AddObjectToManager();

	void ButtonSelectInputs(float elapsedTimeInSec, GameInputs* inputs);

	void UseButton(int menu);
};

