/*
* Game Framework
* 
*	Game State�� ����(������Ʈ, ������, ���� ��)
*	Renderer�� ����
*/

#pragma once
#include <stack>
#include "Globals.h"
#include "GameState.h"
#include "Renderer.h"

class CGameFramework
{
private:
	CGameFramework();
	~CGameFramework();

public:
	// �̱��� ���� ���� �Լ�
	static CGameFramework* GetInstance();

private:
	// Game State�� �ӽ� �����ϱ� ���� ����
	std::stack<CGameState*> m_pGameStateStack;

	// ������
	CRenderer* m_pRenderer		= NULL;

	int m_iWindowSizeX;
	int m_iWindowSizeY;

	// �������� ����ð�(�и��� ����(1000���� 1��)
	int m_fPrevTimeInMillisecond = 0;

	// FPS �����
	char m_cFpsText[30];
	float m_fSecond = 1;

	// �ý��� �޴�(�Ͻ����� �޴�) ���� Ȱ��ȭ
	bool m_bIsEnableToEnterSystemMenu;

public:
	// ���� ����(��� �ð� ����)
	int Loop(GameInputs* inputs);

	// ���� ������Ʈ�� �����ϰ� ���� state�� ����
	void ChangeState(CGameState* gameState, bool keep);

	// ���� ������Ʈ�� ���߰� ���� state�� ����
	void PushState(CGameState* gameState);

	// ���� state�� ������ ���� ������Ʈ�� ���ư�
	void PopState(bool keep);

	// ���� state�� ���߰� ���� ������Ʈ�� ���ư�(���� ������Ʈ�� ���� ������Ʈ�� ��ġ ����)
	void ConvertWithPrevState();

	// ���� state ����
	void PopPrevState(bool keep);

	// ������ ������ ����
	void SetWindowSize(int windowSizeX, int windowSizeY);

	// ���� state ������Ʈ �� ������
	void RenderScene();

	// ���� state ���
	CGameState* GetPrevState();

	void SetIsEnableToEnterSystemMenu(bool enable);
	bool GetIsEnableToEnterSystemMenu() const;

private:
	void Update(float elapsedTimeInSec, GameInputs* inputs);

};

