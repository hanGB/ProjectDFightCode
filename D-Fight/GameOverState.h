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

	// �ΰ�
	int m_LogoTextureID = EMPTY_VALUE;

	// BGM
	int m_gameoverBGMID = EMPTY_VALUE;
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

	void ButtonSelectInputs(float elapsedTimeInSec, GameInputs* inputs);

	void UseButton(int menu);
};

