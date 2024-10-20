#pragma once
#include "GameState.h"
#include "GameFramework.h"
#include "HeroObject.h"
#include "TurnBattleState.h"
#include "HybridBattleState.h"
#include "FieldObjectManager.h"
#include "FountainWaterDropParticle.h"
#include "TimeOfDaySkybox.h"
#include "RecoverSpot.h"
#include "PauseMenuState.h"
#include "BloodEffectParticle.h"
#include <stb_image.h>

class CFieldState : public CGameState {
public:
	CFieldState();
	~CFieldState();

private:
	CGameFramework* m_pGameFramework = NULL;
	CSpotLight* m_pSpotLight = NULL;

	// 턴 전투 스테이트
	CTurnBattleState* m_pTurnBattleState = NULL;
	// 하이브리드 전투 스테이트
	CHybridBattleState* m_pHybridBattleState = NULL;
	// 일시정지 메뉴 스테이트
	CPauseMenuState* m_pPauseMenuState = NULL;

	// 시간에 따라 변하는 스카이박스
	CTimeOfDaySkybox* m_pTimeOfDaySkybox = NULL;

	// 모델, 텍스쳐, 사운드 인덱스
	// 바닥
	int m_areafloorModelID[4];

	int m_areaBorderModelID = EMPTY_VALUE;
	int m_areaBorderTextureID = EMPTY_VALUE;

	int m_floorLineModelID = EMPTY_VALUE;
	int m_floorOutLineModelID = EMPTY_VALUE;
	int m_floorOutLineTextureID = EMPTY_VALUE;

	int m_fieldBGMID = EMPTY_VALUE;

	int m_grassModelID = EMPTY_VALUE;
	int m_grassTextureID = EMPTY_VALUE;

	// 분수대
	int m_fountainModelID = EMPTY_VALUE;
	int m_fountainTextureID = EMPTY_VALUE;

	// 물
	int m_waterModelID = EMPTY_VALUE;

	// 가로등
	int m_lampModelID = EMPTY_VALUE;
	int m_lampTexturelID = EMPTY_VALUE;
	int m_lightModelID = EMPTY_VALUE;

	GLuint m_GrassSSBOModelMatrices;

	// 주인공 아이디
	CHeroObject* m_pHeroObject = NULL;
	int m_HeroModelID = EMPTY_VALUE;
	int m_HeroTextureID = EMPTY_VALUE;

	// 액션 전투 UI 관련 텍스쳐
	int m_WhiteRectTextureID = EMPTY_VALUE;
	int m_UIHpSpBarTextureID = EMPTY_VALUE;
	int m_UIMouseButtonInformationTextureID = EMPTY_VALUE;
	int m_UILongButtonInformationTextureID = EMPTY_VALUE;

	// BGM
	irrklang::ISound* m_pBGM = NULL;

	// 손전등 조작을 위한 변수
	bool m_bFlashLightOn = false;

	// 카메라 변경
	bool m_bTestCameraOn = false;

	int m_switchSoundID = EMPTY_VALUE;

	// 바운딩 박스 그리기
	bool m_bShowBoundingBox = false;

	// 잔디 흔들림을 위한 시간 변수
	float m_fTimeForGrass = 0;

	// 진입 후 주인공이 바로 충돌하지 않도록 설정
	bool m_bIsStartHeroCollision = false;

	// 전투 진입전 주인공 위치
	glm::vec3 m_vPrevHeroPosition;

	std::vector<CGameObject*> m_vLampLightVector;

	// 회복 스폿
	CRecoverSpot* m_pRecoverSpot = NULL;

	// 그림자 시아 종료
	bool m_bIsShadowSight = true;

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

	void SetPauseMenuState(CPauseMenuState* state);

private:
	// 일반 렌더러(계산된 그림자 이용)
	virtual void NormalRender();

	// UI 렌더링
	virtual void UIRender();

	// 필요 모델, 텍스쳐, 사운드 로드
	virtual void LoadData();

	// 처음 State가 만들어졌을때 오브젝트 매니저에 오브젝트 추가
	virtual void AddObjectToManager();

	// 버그를 고치지 위한 위치 초기화
	virtual void InitLocationForBugFix();

	void MakeGrassInstance();

	void RenderWaterObject(int modelID);

	// UI
	void LoadUIData();

	void RenderHPSPBarUI(int x, int y);
	void RenderNameAndLevelTextUI(int x, int y);
	void RenderKeyInformationUI(int x, int y);

	// 시간에 따른 전역 광원 업데이트
	void UpdateDirectionalLightToTime(float elapsedTimeInSec);
	// 시간에 맞춘 전역 광원 업데이트
	void SetDirectionalLightToTime();

	// 가로등 조명 온오프
	void TurnOnLampLight(bool on);

	// 전투 진입전 필요 설정
	void SetNeedSettingBeforeEnterBattle();

	// 주인공이 있는 지역 위치
	int ActiveAreaNumber();
};