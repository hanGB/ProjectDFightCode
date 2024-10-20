/*
* Game State
*	�� 1������ ȣ��Ǵ� Ŭ����
*
*	���� ���¸� ����
*		���� ����� �޶����� ��� ���� ���� ����(���� ȭ�� -> ���� �÷��� ȭ�� ��)
*	�÷��̾� �Է� ó��
*	Player(Object) ����
* 
*	ObjectManager�� ����
*	Camera ����
*/

#pragma once
#include "Globals.h"
#include "Renderer.h"
#include "ObjectManager.h"
#include "SoundPlayer.h"
#include "SpotLight.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "BehaviorTree.h"
#include "Camera.h"

class CGameState
{
public:
	CGameState();
	~CGameState();

private:
	CRenderer* m_pRenderer = NULL;
	CSoundPlayer* m_pSoundPlayer = NULL;
	CBlackBoard* m_pBackBoard = NULL;
	CObjectManager* m_pObjectManager = NULL;
	CCamera* m_pCamera = NULL;

	// ����
	CDirectionalLight* m_pDirectionalLight = NULL;
	CPointLight* m_pPointLight = NULL;

	// �� ȿ�� ��� ����
	bool m_bBlurEffect = false;
	bool m_bColorInvertEffect = false;

	// �Ͻ������Ѵٰ� �ʱ�ȭ ���� �ʵ��� �ϴ� ����
	bool m_bIsAlreadyStartedAndStillPlaying = false;

public:
	// �÷��̾��� Ű �Է� ó��
	virtual void ProcessInputs(float elapsedTimeInSec, GameInputs* inputs) = 0;

	virtual void Update(float elapsedTimeInSec);
	virtual void RenderScene();

	// ���׸� ��ġ�� ���� ��ġ �ʱ�ȭ
	virtual void InitLocationForBugFix();

	// ������Ʈ ���Խ�
	virtual void Enter() = 0;
	// ������Ʈ �����
	virtual void Exit() = 0;
	// ������Ʈ �ӽ� �̵�
	virtual void Pause() = 0;
	// ������Ʈ ����
	virtual void Resume() = 0;

	// ���� Ŭ������ �ڽ� Ŭ�������� �̿��ϱ� ���� �Լ�
	CRenderer* Renderer() const { return m_pRenderer; } 
	CSoundPlayer* SoundPlayer() const { return m_pSoundPlayer; }
	CBlackBoard* BackBoard() const { return m_pBackBoard; }
	CObjectManager* ObjectManager() const { return m_pObjectManager; }
	CCamera* Camera() const { return m_pCamera; }

	CDirectionalLight* DirectionalLight() { return m_pDirectionalLight; }
	CPointLight* PointLight() { return m_pPointLight; }

	void LoadDirectionalLight(CDirectionalLight* DirectionalLight) { m_pDirectionalLight = DirectionalLight; }
	void LoadPointLight(CPointLight* PointLight) { m_pPointLight = PointLight; }

	// �Ϲ� ������(���� �׸��� �̿�)
	virtual void NormalRender();

	// �⺻ ������ ����������
	void DefualtRenderPipeline();

	// ��ó�� ������
	void PostProcessingRender();

	// �𷺼ų� ����Ʈ �׸��� ���� ������
	void DepthRender();

	// ������ ��� �ؽ��Ŀ� ����
	void SaveRenderingResultOnTexture();

	// UI ������
	virtual void UIRender() = 0;

	void SetBlurEffect(bool blur) { m_bBlurEffect = blur; }
	void SetColorInvertEffect(bool invert) { m_bColorInvertEffect = invert; }

	// ������Ʈ �޴��� ����
	void ChangeObjectManager(CObjectManager* objectmanager);

	void SetIsAlreadyStartedAndStillPlaying(bool alreadyStarted);
	bool GetIsAlreadyStartedAndStillPlaying() const;

private:
	// �ʿ� ��, �ؽ���, ���� �ε�
	virtual void LoadData() = 0;

	// ó�� State�� ����������� ������Ʈ �Ŵ����� ������Ʈ �߰�
	virtual void AddObjectToManager() = 0;

};

