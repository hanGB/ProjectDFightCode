#pragma once

#include "GameState.h"
#include "GameFramework.h"
#include "TimeOfDaySkybox.h"
#include "FountainWaterDropParticle.h"
#include "Button.h"

class CTitleMenuState : public CGameState {
public:
	CTitleMenuState();
	~CTitleMenuState();

private:
	CGameFramework* m_pGameFramework = NULL;
	// 시간에 따라 변하는 스카이박스
	CTimeOfDaySkybox* m_pTimeOfDaySkybox = NULL;

	// 모델, 텍스쳐, 사운드 인덱스
	// 바닥
	int m_floorModelID = EMPTY_VALUE;
	int m_floorTextureID = EMPTY_VALUE;
	int m_floorLineModelID = EMPTY_VALUE;
	int m_floorOutLineModelID = EMPTY_VALUE;
	int m_floorOutLineTextureID = EMPTY_VALUE;

	// 잔디
	int m_grassModelID = EMPTY_VALUE;
	int m_grassTextureID = EMPTY_VALUE;
	GLuint m_GrassSSBOModelMatrices;
	float m_fTimeForGrass = 0;

	// 분수대
	int m_fountainModelID = EMPTY_VALUE;
	int m_fountainTextureID = EMPTY_VALUE;
	
	// 물
	int m_waterModelID = EMPTY_VALUE;

	// 로고
	int m_LogoTextureID = EMPTY_VALUE;

	// BGM
	int m_titleBGMID = EMPTY_VALUE;
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

	void MakeGrassInstance();

	void RenderWaterObject(int modelID);

	void ButtonSelectInputs(float elapsedTimeInSec, GameInputs* inputs);

	void UseButton(int menu);

	void StartState();
	void EndState();
};