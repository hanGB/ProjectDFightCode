#pragma once
#include "GameState.h"
#include "GameFramework.h"
#include "HeroObject.h"
#include "TurnBattleState.h"
#include "HybridBattleObjectManager.h"
#include "BattleMonster.h"
#include "TimeOfDaySkybox.h"
#include "FountainWaterDropParticle.h"
#include "RainDropParticle.h"

class CHybridBattleState : public CGameState {
public:
	CHybridBattleState();
	~CHybridBattleState();

private:
	CGameFramework* m_pGameFramework = NULL;
	CSpotLight* m_pSpotLight = NULL;

	// 시간에 따라 변하는 스카이박스
	CTimeOfDaySkybox* m_pTimeOfDaySkybox = NULL;

	// 턴 전투 스테이트
	CTurnBattleState* m_pTurnBattleState = NULL;

	CRainDropParticle* m_pRainDropParticle = NULL;

	// 적 몬스터 파티 정보
	std::vector<CBattleMonster*> m_MonsterVector;

	// 모델, 텍스쳐, 사운드 인덱스
	int m_fieldBGMID = EMPTY_VALUE;

	int m_grassModelID = EMPTY_VALUE;
	int m_grassTextureID = EMPTY_VALUE;

	int m_waterModelID = EMPTY_VALUE;

	int m_weaponModelID = EMPTY_VALUE;

	int m_waterDropTexureID = EMPTY_VALUE;

	int m_dustTexureID = EMPTY_VALUE;
	int m_ballTexureID = EMPTY_VALUE;

	GLuint m_GrassSSBOModelMatrices;

	// 주인공 아이디
	CHeroObject* m_pHeroObject = NULL;

	// 전투 UI 관련 텍스쳐
	int m_WhiteRectTextureID = EMPTY_VALUE;
	int m_UIHpSpBarTextureID = EMPTY_VALUE;
	int m_UIMouseButtonInformationTextureID = EMPTY_VALUE;
	int m_UILongButtonInformationTextureID = EMPTY_VALUE;
	int m_UISmallKeyButtonTextureID = EMPTY_VALUE;
	int m_UITurnGaugeTextureID = EMPTY_VALUE;
	int m_UIBurnConditionTextureID = EMPTY_VALUE;
	int m_UIAnxietyConditionTextureID = EMPTY_VALUE;

	// 손전등 조작을 위한 변수
	bool m_bFlashLightOn = false;

	// 카메라 변경
	bool m_bTestCameraOn = false;

	int m_switchSoundID = EMPTY_VALUE;

	// 마우스 고정 설정 변수
	bool m_bMouseFixed = true;
	int m_iPrevMousePosX = WINDOW_SIZE_X / 2;
	int m_iPrevMousePosY = WINDOW_SIZE_Y / 2;

	// 바운딩 박스 그리기
	bool m_bShowBoundingBox = false;

	// 잔디 흔들림을 위한 시간 변수
	float m_fTimeForGrass = 0.0f;

	// 진입 후 주인공이 바로 충돌하지 않도록 설정
	bool m_bIsStartHeroCollision = false;

	int m_iAreaNumber = 4;
	bool m_bIsStartedWithHeroWin = true;

	bool m_bIsTurnMode = false;

	float m_fTurnModeCameraHorizontalAngle = PI_VALUE;
	float m_fTargetHorizontalAngle = PI_VALUE;

	// 시작 대기
	float m_fStartWaitTime = 0.0f;
	bool m_bIsBattleStartWait = false;

	int m_HeroIndex = -1;
	int m_WeaponIndex = -1;
	
	CGameObject* m_pTempAutomata = NULL;
	float m_fAutomataSummonTime = -1.0f;
	bool m_bIsSummonedTempAutomata = false;
	bool m_bIsSummonedAutomataOn = false;

	bool m_bIsEnemyUseSkill = false;

	// 턴 모드 종료 후 주인공 애니메이션 초기화 변수
	bool m_bInitHeroAnimationAfterEndTurnMode = false;
	// 적 행동 종료 후 적 애니메이션 초기화 변수
	bool m_bInitEnemyAnimationAfterEndTurnMode = false;

	std::vector<CGameObject*> m_vLampLightVector;

	bool m_bIsRenderParticle = true;

public:
	// 플레이어의 키 입력 처리
	virtual void ProcessInputs(float elapsedTimeInSec, GameInputs* inputs);

	virtual void Update(float elapsedTimeInSec);
	virtual void RenderScene();

	// 버그를 고치지 위한 위치 초기화
	virtual void InitLocationForBugFix();

	// 필드와 동일한 월드를 갖도록 설정하는 부분
	void SetHeroObject(CHeroObject* heroObject, CGameObject* weaponObject);
	void SetTurnBattleState(CTurnBattleState* turnBattleState);
	void SetTimeOfDaySkybox(CTimeOfDaySkybox* timeOfDaySkybox);

	// 스테이트 진입시	
	virtual void Enter();
	// 스테이트 종료시
	virtual void Exit();
	// 스테이트 임시 이동
	virtual void Pause();
	// 스테이트 복귀
	virtual void Resume();

	// 전투 진입 전 설정
	void SetAreaNumber(int areaNumber);
	void SetTimeForGrass(float time);
	void SetIsStartedWithHeroWin(bool win);

	float GetFlashLightOn() const;
	void SetFlashLightOn(bool on);

	CHeroObject* GetHeroObject();

private:
	// 일반 렌더러(계산된 그림자 이용)
	virtual void NormalRender();

	// UI 렌더링
	virtual void UIRender();

	// 필요 모델, 텍스쳐, 사운드 로드
	virtual void LoadData();

	// 처음 State가 만들어졌을때 오브젝트 매니저에 오브젝트 추가
	virtual void AddObjectToManager();

	void InitHybridBattle();
	void AddMonsterToMonsterVector();
	void AddMonsterToMonsterVectorBoss();
	void RemoveMonsterAndDamageZone();

	// 전투 필드 제작
	void MakeBattleField(int areaNumber);

	// UI
	void LoadUIData();

	void RenderHPSPBarUI(int x, int y);
	void RenderNameAndTurnUI(int x, int y);
	void RenderKeyInformationUI(int x, int y);

	// 시간에 따른 전역 광원 업데이트
	void UpdateDirectionalLightToTime(float elapsedTimeInSec);
	// 시간에 맞춘 전역 광원 업데이트
	void SetDirectionalLightToTime();

	void SetTargetWithMouse(float elapsedTimeInSec, GameInputs* inputs);

	// 렌더링 여부 초기화(전체 적, 주인공 보이기)
	void InitVisiblity();

	// 렌더링 여부 설정
	void SetVisiblity(bool hero, bool visible);

	// 오토마타 소환 이펙트
	void ProcessingSummonAutomata(float elapsedTimeInSec);
	void UpdateAutomataSummonEffect(float elapsedTimeInSec);
};