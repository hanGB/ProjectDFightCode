#pragma once
#include "GameState.h"
#include "GameFramework.h"
#include "HeroObject.h"

class CTestState : public CGameState {
public:
	CTestState();
	~CTestState();

private:
	CGameFramework* m_pGameFramework = NULL;
	CSpotLight* m_pSpotLight = NULL;

	// �׽�Ʈ �� �ڽ�
	int m_boxModelID = EMPTY_VALUE;
	int m_boxTextureID = EMPTY_VALUE;

	// ��, �ؽ���, ���� �ε���
	int m_floorModelID = EMPTY_VALUE;
	int m_floorTextureID = EMPTY_VALUE;

	int m_enemyModelID = EMPTY_VALUE;
	int m_enemyTextureID = EMPTY_VALUE;

	int m_enemy2ModelID = EMPTY_VALUE;
	int m_enemy2TextureID = EMPTY_VALUE;

	int m_cityBGMID = EMPTY_VALUE;

	int m_grassModelID = EMPTY_VALUE;
	int m_grassTextureID = EMPTY_VALUE;

	GLuint m_GrassSSBOModelMatrices;

	// UI 2D �ؽ��� �׽�Ʈ�� �ε���
	int m_uiTextureID = EMPTY_VALUE;

	// ��������Ʈ ��Ʈ �׽�Ʈ�� �ε���, ����
	int m_spriteSheetID = EMPTY_VALUE;
	int m_iFrame = 0;
	float m_fFrameTime = 0;

	// ������ �׽�Ʈ�� �ε��� 
	int m_billboardTextureID = EMPTY_VALUE;
	int m_moonTextureID = EMPTY_VALUE;

	// ��ī�̹ڽ� �ε���
	int m_skyboxTextureID = EMPTY_VALUE;
	int m_skyboxDarkTextureID = EMPTY_VALUE;

	// ���ΰ� ���̵�
	CHeroObject* m_pHeroObject = NULL;
	int m_HeroModelID = EMPTY_VALUE;
	int m_HeroTextureID = EMPTY_VALUE;

	// BGM
	irrklang::ISound* m_pBGM = NULL;

	// ������ ������ ���� ����
	bool m_bFlashLightOn = false;

	// ī�޶� ����
	bool m_bTestCameraOn = true;

	int m_switchSoundID = EMPTY_VALUE;

	// ���콺 ���� ���� ����
	bool m_bMouseFixed = true;
	int m_iPrevMousePosX = WINDOW_SIZE_X / 2;
	int m_iPrevMousePosY = WINDOW_SIZE_Y / 2;

	// FPS �����
	wchar_t m_wFpsText[10];
	float m_fSecond = 1;

	// �ٿ�� �ڽ� �׸���
	bool m_bShowBoundingBox = false;

	// �ð�
	float m_fHour = 6;

	// �ܵ� ��鸲�� ���� �ð� ����
	float m_fTimeForGrass = 0;

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

private:
	// �Ϲ� ������(���� �׸��� �̿�)
	virtual void NormalRender();

	// UI ������
	virtual void UIRender();

	// �ʿ� ��, �ؽ���, ���� �ε�
	virtual void LoadData();

	// ó�� State�� ����������� ������Ʈ �Ŵ����� ������Ʈ �߰�
	virtual void AddObjectToManager();

	void MakeGrassInstance();
};