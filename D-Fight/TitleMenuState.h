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
	// �ð��� ���� ���ϴ� ��ī�̹ڽ�
	CTimeOfDaySkybox* m_pTimeOfDaySkybox = NULL;

	// ��, �ؽ���, ���� �ε���
	// �ٴ�
	int m_floorModelID = EMPTY_VALUE;
	int m_floorTextureID = EMPTY_VALUE;
	int m_floorLineModelID = EMPTY_VALUE;
	int m_floorOutLineModelID = EMPTY_VALUE;
	int m_floorOutLineTextureID = EMPTY_VALUE;

	// �ܵ�
	int m_grassModelID = EMPTY_VALUE;
	int m_grassTextureID = EMPTY_VALUE;
	GLuint m_GrassSSBOModelMatrices;
	float m_fTimeForGrass = 0;

	// �м���
	int m_fountainModelID = EMPTY_VALUE;
	int m_fountainTextureID = EMPTY_VALUE;
	
	// ��
	int m_waterModelID = EMPTY_VALUE;

	// �ΰ�
	int m_LogoTextureID = EMPTY_VALUE;

	// BGM
	int m_titleBGMID = EMPTY_VALUE;
	irrklang::ISound* m_pBGM = NULL;

	int m_WhiteRectTextureID = EMPTY_VALUE;

	std::vector<CButton*> m_ButtonVector;

	int m_iSelectedButton = 0;

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

	void RenderWaterObject(int modelID);

	void ButtonSelectInputs(float elapsedTimeInSec, GameInputs* inputs);

	void UseButton(int menu);

	void StartState();
	void EndState();
};