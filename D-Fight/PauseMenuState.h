#pragma once

#include "GameState.h"
#include "GameFramework.h"
#include "Button.h"
#include "Globals.h"
#include "TitleMenuState.h"
#include <vector>

class CPauseMenuState : public CGameState {
public:
	CPauseMenuState();
	~CPauseMenuState();

private:
	CGameFramework* m_pGameFramework = NULL;

	int m_WhiteRectTextureID = EMPTY_VALUE;

	std::vector<CButton*> m_ButtonVector;

	int m_iSelectedButton = 0;
	float m_fKeepPressKey = 0.0f;

	int m_bBattleMode = BATTLE_MODE_HYBRID;

	bool m_bIsOpened = false;

	CTitleMenuState* m_pTitleMenuState = NULL;

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

	bool GetBattleMode() const;

	bool GetIsOpened() const;

	void UseButton(int menu);

private:
	// �Ϲ� ������(���� �׸��� �̿�)
	virtual void NormalRender();

	// UI ������
	virtual void UIRender();

	// �ʿ� ��, �ؽ���, ���� �ε�
	virtual void LoadData();

	// ó�� State�� ����������� ������Ʈ �Ŵ����� ������Ʈ �߰�
	virtual void AddObjectToManager();

	void ShowMouseCursor(bool show);

	void RenderButtonVector();

	void ButtonSelectInputs(float elapsedTimeInSec, GameInputs* inputs);

	bool IsKeyResultWaitedEnough(float time);
};