#pragma once
#include "GameObject.h"
#include "TurnBattleObject.h"
#include "ObjectManager.h"
#include "Camera.h"

class CHeroObject : public CGameObject {
public:
	CHeroObject();
	~CHeroObject();

private:
	CTurnBattleObject* m_pTurnBattleObject = NULL;

	float m_fCameraDistance = 10.0f;

	float m_fMinDistance = 3.0f;
	float m_fAngleChangeStartDistance = 5.0f;
	float m_fMaxDistance = 10.0f;

	// ���׹̳�(sp) ����
	float m_fMaxStemina = 0.0f;
	float m_fStemina = 0.0f;
	short m_sLevel = 1;

	// ���� ���� ���� ����
	float m_fJumpTime = 0.5f;
	bool m_bIsJumping = false;

	float m_fIdleTime = 0.0f;
	
	CGameObject* m_pWeapon = NULL;

	// ȿ���� ����
	int m_walkSoundID = EMPTY_VALUE;
	float m_walkSoundCoolTime = 0.0f;
	int m_attackSoundID = EMPTY_VALUE;
	int m_hitSoundID = EMPTY_VALUE;
	int m_dashSoundID = EMPTY_VALUE;

	float m_fCameraHorizontalAngle = PI_VALUE;
	float m_fCameraVerticalAngle = 0.0f;
	float m_fCameraSpeed = 0.35f;
	float m_fCameraHeight = 2.0f;

	CGameObject* m_pCameraCollisionObject = NULL;

	glm::vec3 m_vPrevDirection;
	glm::vec3 m_vPrevRight;
	glm::vec3 m_vPrevUp;

	// �ܻ� ȿ�� ��Ÿ��
	float m_fSpectrumCoolTime = DEFAULT_SPECTRUM_COOL_TIME;

	bool m_bIsRecovering = false;
	float m_fRecoverCoolTime = 0.0f;

	bool m_bIsHeldByEnemy = false;

public:
	virtual void Update(float elapsedTimeInSec, GameUpdateParameters parameters);
	void ProcessInputs(float elapsedTimeInSec, GameInputs* inputs);

	void SpectrumUpdater(float elapsedTimeInSec, CObjectManager* objectManager);

	// ���� �� ī�޶�
	void SetCameraPosWithHeroPos(CCamera* camera);
	void SetScrollWheel(CCamera* camera, float elapsedTimeInSec, bool wheelUp);
	
	// 3��Ī �� �� ī�޶�
	void ControlBackViewCamera(CCamera* camera, float elapsedTimeInSec, GameInputs* inputs, bool mouse);

	void CalculateBackViewCamera(CCamera* camera);

	// �� ��忡���� ī�޶� ����
	void ControlBackViewCameraInTurnMode(CCamera* camera, 
		float elapsedTimeInSec, GameInputs* inputs, float horizontalAngle);

	// ���� �ٶ󺸴� ī�޶�
	void ControlLookAtCameraInTurnMode(CCamera* camera,
		float elapsedTimeInSec, GameInputs* inputs, float horizontalAngle, glm::vec3 lookPosition);

	// �� ��忡�� ���� ī�޶� ����
	void SetFrontViewCameraInTurnMode(CCamera* camera, float horizontalAngle);

	void SetTurnBattleObject(CTurnBattleObject* object);
	CTurnBattleObject* GetTurnBattleObject();

	// ���׹̳� �ʱ�ȭ
	void InitStemina();

	// ���׹̳� ���� ���
	void GetStemina(float* stemina, float* maxStemina);

	bool UseStemina(float amount);

	void SetWeaponObject(CGameObject* object);

	void GetCameraAngle(float* horizontalAngle, float* verticalAngle);
	void SetCameraAngle(float horizontalAngle, float verticalAngle);

	// ���� ��ġ ����
	void SetWeaponPosition();
	CGameObject* GetWeaponObject();

	void SetCameraCollisionObject(CGameObject* object);
	CGameObject* GetCameraCollisionObject();

	void InitCameraCollisionObject();

	void MatchAnimationWithTurnBattleObject();

	void SetRecoverAnimation(float time);
	void SetRecoverCoolTime();

	void SetIsHeldByEnemy(bool hold);

private:
	// ���׹̳� ȸ��
	void RecoverStemina(float elapsedTimeInSec);
};