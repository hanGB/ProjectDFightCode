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

	// �ð��� ���� ���ϴ� ��ī�̹ڽ�
	CTimeOfDaySkybox* m_pTimeOfDaySkybox = NULL;

	// �� ���� ������Ʈ
	CTurnBattleState* m_pTurnBattleState = NULL;

	CRainDropParticle* m_pRainDropParticle = NULL;

	// �� ���� ��Ƽ ����
	std::vector<CBattleMonster*> m_MonsterVector;

	// ��, �ؽ���, ���� �ε���
	int m_fieldBGMID = EMPTY_VALUE;

	int m_grassModelID = EMPTY_VALUE;
	int m_grassTextureID = EMPTY_VALUE;

	int m_waterModelID = EMPTY_VALUE;

	int m_weaponModelID = EMPTY_VALUE;

	int m_waterDropTexureID = EMPTY_VALUE;

	int m_dustTexureID = EMPTY_VALUE;
	int m_ballTexureID = EMPTY_VALUE;

	GLuint m_GrassSSBOModelMatrices;

	// ���ΰ� ���̵�
	CHeroObject* m_pHeroObject = NULL;

	// ���� UI ���� �ؽ���
	int m_WhiteRectTextureID = EMPTY_VALUE;
	int m_UIHpSpBarTextureID = EMPTY_VALUE;
	int m_UIMouseButtonInformationTextureID = EMPTY_VALUE;
	int m_UILongButtonInformationTextureID = EMPTY_VALUE;
	int m_UISmallKeyButtonTextureID = EMPTY_VALUE;
	int m_UITurnGaugeTextureID = EMPTY_VALUE;
	int m_UIBurnConditionTextureID = EMPTY_VALUE;
	int m_UIAnxietyConditionTextureID = EMPTY_VALUE;

	// ������ ������ ���� ����
	bool m_bFlashLightOn = false;

	// ī�޶� ����
	bool m_bTestCameraOn = false;

	int m_switchSoundID = EMPTY_VALUE;

	// ���콺 ���� ���� ����
	bool m_bMouseFixed = true;
	int m_iPrevMousePosX = WINDOW_SIZE_X / 2;
	int m_iPrevMousePosY = WINDOW_SIZE_Y / 2;

	// �ٿ�� �ڽ� �׸���
	bool m_bShowBoundingBox = false;

	// �ܵ� ��鸲�� ���� �ð� ����
	float m_fTimeForGrass = 0.0f;

	// ���� �� ���ΰ��� �ٷ� �浹���� �ʵ��� ����
	bool m_bIsStartHeroCollision = false;

	int m_iAreaNumber = 4;
	bool m_bIsStartedWithHeroWin = true;

	bool m_bIsTurnMode = false;

	float m_fTurnModeCameraHorizontalAngle = PI_VALUE;
	float m_fTargetHorizontalAngle = PI_VALUE;

	// ���� ���
	float m_fStartWaitTime = 0.0f;
	bool m_bIsBattleStartWait = false;

	int m_HeroIndex = -1;
	int m_WeaponIndex = -1;
	
	CGameObject* m_pTempAutomata = NULL;
	float m_fAutomataSummonTime = -1.0f;
	bool m_bIsSummonedTempAutomata = false;
	bool m_bIsSummonedAutomataOn = false;

	bool m_bIsEnemyUseSkill = false;

	// �� ��� ���� �� ���ΰ� �ִϸ��̼� �ʱ�ȭ ����
	bool m_bInitHeroAnimationAfterEndTurnMode = false;
	// �� �ൿ ���� �� �� �ִϸ��̼� �ʱ�ȭ ����
	bool m_bInitEnemyAnimationAfterEndTurnMode = false;

	std::vector<CGameObject*> m_vLampLightVector;

	bool m_bIsRenderParticle = true;

public:
	// �÷��̾��� Ű �Է� ó��
	virtual void ProcessInputs(float elapsedTimeInSec, GameInputs* inputs);

	virtual void Update(float elapsedTimeInSec);
	virtual void RenderScene();

	// ���׸� ��ġ�� ���� ��ġ �ʱ�ȭ
	virtual void InitLocationForBugFix();

	// �ʵ�� ������ ���带 ������ �����ϴ� �κ�
	void SetHeroObject(CHeroObject* heroObject, CGameObject* weaponObject);
	void SetTurnBattleState(CTurnBattleState* turnBattleState);
	void SetTimeOfDaySkybox(CTimeOfDaySkybox* timeOfDaySkybox);

	// ������Ʈ ���Խ�	
	virtual void Enter();
	// ������Ʈ �����
	virtual void Exit();
	// ������Ʈ �ӽ� �̵�
	virtual void Pause();
	// ������Ʈ ����
	virtual void Resume();

	// ���� ���� �� ����
	void SetAreaNumber(int areaNumber);
	void SetTimeForGrass(float time);
	void SetIsStartedWithHeroWin(bool win);

	float GetFlashLightOn() const;
	void SetFlashLightOn(bool on);

	CHeroObject* GetHeroObject();

private:
	// �Ϲ� ������(���� �׸��� �̿�)
	virtual void NormalRender();

	// UI ������
	virtual void UIRender();

	// �ʿ� ��, �ؽ���, ���� �ε�
	virtual void LoadData();

	// ó�� State�� ����������� ������Ʈ �Ŵ����� ������Ʈ �߰�
	virtual void AddObjectToManager();

	void InitHybridBattle();
	void AddMonsterToMonsterVector();
	void AddMonsterToMonsterVectorBoss();
	void RemoveMonsterAndDamageZone();

	// ���� �ʵ� ����
	void MakeBattleField(int areaNumber);

	// UI
	void LoadUIData();

	void RenderHPSPBarUI(int x, int y);
	void RenderNameAndTurnUI(int x, int y);
	void RenderKeyInformationUI(int x, int y);

	// �ð��� ���� ���� ���� ������Ʈ
	void UpdateDirectionalLightToTime(float elapsedTimeInSec);
	// �ð��� ���� ���� ���� ������Ʈ
	void SetDirectionalLightToTime();

	void SetTargetWithMouse(float elapsedTimeInSec, GameInputs* inputs);

	// ������ ���� �ʱ�ȭ(��ü ��, ���ΰ� ���̱�)
	void InitVisiblity();

	// ������ ���� ����
	void SetVisiblity(bool hero, bool visible);

	// ���丶Ÿ ��ȯ ����Ʈ
	void ProcessingSummonAutomata(float elapsedTimeInSec);
	void UpdateAutomataSummonEffect(float elapsedTimeInSec);
};