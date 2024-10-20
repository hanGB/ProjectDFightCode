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

	// �� ���� ������Ʈ
	CTurnBattleState* m_pTurnBattleState = NULL;
	// ���̺긮�� ���� ������Ʈ
	CHybridBattleState* m_pHybridBattleState = NULL;
	// �Ͻ����� �޴� ������Ʈ
	CPauseMenuState* m_pPauseMenuState = NULL;

	// �ð��� ���� ���ϴ� ��ī�̹ڽ�
	CTimeOfDaySkybox* m_pTimeOfDaySkybox = NULL;

	// ��, �ؽ���, ���� �ε���
	// �ٴ�
	int m_areafloorModelID[4];

	int m_areaBorderModelID = EMPTY_VALUE;
	int m_areaBorderTextureID = EMPTY_VALUE;

	int m_floorLineModelID = EMPTY_VALUE;
	int m_floorOutLineModelID = EMPTY_VALUE;
	int m_floorOutLineTextureID = EMPTY_VALUE;

	int m_fieldBGMID = EMPTY_VALUE;

	int m_grassModelID = EMPTY_VALUE;
	int m_grassTextureID = EMPTY_VALUE;

	// �м���
	int m_fountainModelID = EMPTY_VALUE;
	int m_fountainTextureID = EMPTY_VALUE;

	// ��
	int m_waterModelID = EMPTY_VALUE;

	// ���ε�
	int m_lampModelID = EMPTY_VALUE;
	int m_lampTexturelID = EMPTY_VALUE;
	int m_lightModelID = EMPTY_VALUE;

	GLuint m_GrassSSBOModelMatrices;

	// ���ΰ� ���̵�
	CHeroObject* m_pHeroObject = NULL;
	int m_HeroModelID = EMPTY_VALUE;
	int m_HeroTextureID = EMPTY_VALUE;

	// �׼� ���� UI ���� �ؽ���
	int m_WhiteRectTextureID = EMPTY_VALUE;
	int m_UIHpSpBarTextureID = EMPTY_VALUE;
	int m_UIMouseButtonInformationTextureID = EMPTY_VALUE;
	int m_UILongButtonInformationTextureID = EMPTY_VALUE;

	// BGM
	irrklang::ISound* m_pBGM = NULL;

	// ������ ������ ���� ����
	bool m_bFlashLightOn = false;

	// ī�޶� ����
	bool m_bTestCameraOn = false;

	int m_switchSoundID = EMPTY_VALUE;

	// �ٿ�� �ڽ� �׸���
	bool m_bShowBoundingBox = false;

	// �ܵ� ��鸲�� ���� �ð� ����
	float m_fTimeForGrass = 0;

	// ���� �� ���ΰ��� �ٷ� �浹���� �ʵ��� ����
	bool m_bIsStartHeroCollision = false;

	// ���� ������ ���ΰ� ��ġ
	glm::vec3 m_vPrevHeroPosition;

	std::vector<CGameObject*> m_vLampLightVector;

	// ȸ�� ����
	CRecoverSpot* m_pRecoverSpot = NULL;

	// �׸��� �þ� ����
	bool m_bIsShadowSight = true;

public:
	// �÷��̾��� Ű �Է� ó��
	virtual void ProcessInputs(float elapsedTimeInSec, GameInputs* inputs);

	virtual void Update(float elapsedTimeInSec);
	virtual void RenderScene();

	// ������Ʈ ���Խ�
	virtual void Enter();
	// ������Ʈ �����
	virtual void Exit();
	// ������Ʈ �ӽ� �̵�
	virtual void Pause();
	// ������Ʈ ����
	virtual void Resume();

	void SetPauseMenuState(CPauseMenuState* state);

private:
	// �Ϲ� ������(���� �׸��� �̿�)
	virtual void NormalRender();

	// UI ������
	virtual void UIRender();

	// �ʿ� ��, �ؽ���, ���� �ε�
	virtual void LoadData();

	// ó�� State�� ����������� ������Ʈ �Ŵ����� ������Ʈ �߰�
	virtual void AddObjectToManager();

	// ���׸� ��ġ�� ���� ��ġ �ʱ�ȭ
	virtual void InitLocationForBugFix();

	void MakeGrassInstance();

	void RenderWaterObject(int modelID);

	// UI
	void LoadUIData();

	void RenderHPSPBarUI(int x, int y);
	void RenderNameAndLevelTextUI(int x, int y);
	void RenderKeyInformationUI(int x, int y);

	// �ð��� ���� ���� ���� ������Ʈ
	void UpdateDirectionalLightToTime(float elapsedTimeInSec);
	// �ð��� ���� ���� ���� ������Ʈ
	void SetDirectionalLightToTime();

	// ���ε� ���� �¿���
	void TurnOnLampLight(bool on);

	// ���� ������ �ʿ� ����
	void SetNeedSettingBeforeEnterBattle();

	// ���ΰ��� �ִ� ���� ��ġ
	int ActiveAreaNumber();
};