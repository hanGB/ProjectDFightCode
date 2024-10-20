/*
* Object
*	�� 3��(������)���� ȣ��Ǵ� Ŭ����
*
*	���� �� ������Ʈ�� ����(��ġ, ũŰ, Obj, �ൿ ��� ��)
*	Renderer�� ��� �Լ��� ���� ������
*/

#pragma once
#include "Renderer.h"
#include "Globals.h"
#include "Animator.h"
#include <vector>

enum GameObjectType {
	TYPE_HERO,
	TYPE_FLOOR,
	TYPE_FIXED,
	TYPE_MOVABLE,
	TYPE_ENEMY,
	TYPE_BULLET,
	TYPE_COLLISION_IGNORE_AREA,			// �浹�� �浹�� ��ü�� �� ������ ���� �浹���� �ʵ��� ��
	TYPE_DAMAGE_ZONE,					// ���� ������ ������� �޴� ���� ����
	TYPE_WEAPON,						// ����
	TYPE_CAMERA_COLLISION,				// ī�޶� ��ġ ������ �浹ü
	TYPE_RECOVER_SPOT,					// ȸ�� ����
	TYPE_BOSS,
	TYPE_BOSS_HEAD,
};

enum GameObjectState {
	STATE_GROUND,
	STATE_FALLING
};

enum GameObjectAnimationState {
	ANI_STATE_IDLE,
	ANI_STATE_WALK,
	ANI_STATE_ATTACK,
	ANI_STATE_HIT,
	ANI_STATE_AVOID,
	ANI_STATE_SKILL_1,
	ANI_STATE_SKILL_2,
	ANI_STATE_SKILL_3,
	ANI_STATE_IDLE_LONG,
	ANI_STATE_DEATH,
	ANI_STATE_RECOVER,
	ANI_STATE_JUMP,
	ANI_STATE_HOLD,

	ANI_STATE_SKILL_LEFT,
	ANI_STATE_SKILL_RIGHT,
	ANI_STATE_HIT_LEFT,
	ANI_STATE_HIT_RIGHT,

	ANI_STATE_PHASE_CHANGE,
};


class CGameObject
{
public:
	CGameObject();
	~CGameObject();

private:
	// �ε���
	int m_Index;

	// ��ġ, ȸ��, ũ��
	glm::vec3 m_vPosition = glm::vec3(0.0);
	glm::vec3 m_vRotation = glm::vec3(0.0);
	glm::vec3 m_vSize = glm::vec3(0.0);

	// ����(�浹 �ڽ� ����)
	glm::vec3 m_vVolume;

	// �ӵ�, ���ӵ�
	glm::vec3 m_vVelocity;
	glm::vec3 m_vAcceleration;

	// �ü� ����
	glm::vec3 m_vDirection;

	// �浹 ���
	bool m_bIsBoxCollision;

	// ������, ����
	float m_fRadius;
	float m_fHeight;

	// ����
	float m_fMass;

	// �ӵ� ����
	float m_fLimitVelocity;

	// ���� �ð�
	float m_fLifeTime;
	
	// ���� ���� ��Ÿ��(�ӽ�: ��� �������� ������ �� ����ϵ��� ����)
	float m_fCoolTime;

	// ��, �ؽ��� �ε���
	int m_iModelIndex;
	int m_iTextureIndex;

	// ������Ʈ Ÿ��
	GameObjectType m_type;
	// ������Ʈ ����
	GameObjectState m_state;

	// �浹 ����
	bool m_bEnableCollision;

	// ��� �浹 ����
	bool m_bIsIgnoredCollision;

	// ������ ����(�浹�ڽ� �� ���������� �ʴ� ��� üũ)
	bool m_bWhetherToRender;

	// �浹��� �� Ȯ��
	bool m_bCollisioned;

	// �׵θ� ǥ��
	bool m_bBorder;

	// ���ʹϾ� ȸ�� - ȸ���� �� �ٽ� ȸ���ϱ� ���� ���
	// ���ʹϾ� ���� �浹 ó���� ���� ����(���ʹϾ� �� ȸ�������� �浹 ó��))
	glm::quat m_Quaternion = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);

	// �θ�(���� ������Ʈ�� ������ ������Ʈ)
	CGameObject* m_pParent = NULL;

	// ��ü ���� ����
	glm::vec4 m_vColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	
	// �׵θ� ���� ����
	glm::vec4 m_vBorderColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	// �ø��� ���� ����
	bool m_bIsCullingFace = true;
	bool m_bIsLightSource = false;

	// �׸��ڸ� �������� ����
	bool m_bIsMadeShadow = true;

	// �ִϸ��̼� ���� ����
	bool m_bIsAnimate;
	GameObjectAnimationState m_animationState;
	CAnimator* m_pAnimator = NULL;
	std::string m_modelWithBoneName;
	std::vector<std::vector<SVertex>> m_calculateVerticesVector;

	std::map<GameObjectAnimationState, std::string> m_animationDataNameMap;

	// ��(����) �ؽ���
	int m_BloodTextureID;

	// �⺻ ����
	int m_DefaultSoundID;
	// �ǰ� ����
	int m_HitSoundID;

public:
	virtual void Update(float elapsedTimeInSec, GameUpdateParameters parameters);
	virtual void Render(CRenderer* renderer);
	void DepthRender(CRenderer* renderer);
	void BoundingBoxRenderer(CRenderer* renderer);
	void BorderRender(CRenderer* renderer);

	void CalculateVerticesAfterAnimation(CRenderer* renderer);

	// ��, �ؽ��� �ε��� setter, getter
	void SetModelIndex(int index);
	void SetTextureIndex(int index);

	int GetModelIndex() const;
	int GetTextureIndex() const;

	void SetIndex(int index);
	int GetIndex() const;

	// ��ġ, ȸ��, ũ��, ����, �ӵ�, ���ӵ�, ���� setter, getter
	void SetPosition(glm::vec3 position);
	void SetRotation(glm::vec3 rotation);
	void SetSize(glm::vec3 size);
	void SetVolume(glm::vec3 volume);
	void SetVelocity(glm::vec3 velocity);
	void SetAcceleration(glm::vec3 acceleration);
	void SetMass(float mass);

	glm::vec3 GetPosition() const;
	glm::vec3 GetRotation() const;
	glm::vec3 GetSize() const;
	glm::vec3 GetVolume() const;
	glm::vec3 GetVelocity() const;
	glm::vec3 GetAcceleration() const;
	float GetMass() const;

	void SetRadiusAndHeight(float radius, float height);
	void GetRadiusAndHeight(float* radius, float* height);

	void SetIsBoxCollision(bool boxCollision);
	bool GetIsBoxCollision() const;

	// �ӵ� ����
	void SetLimitVelocity(float velocity);

	// �̵� ����
	glm::vec3 GetDirection() const;
	void SetDirection(glm::vec3 direction);

	// Ÿ��, ���� setter, getter
	void SetType(GameObjectType type);
	void SetState(GameObjectState state);
	GameObjectType GetType();
	GameObjectState GetState();

	void SetLifeTime(float time);
	float GetLifeTime() const;

	void SetCoolTime(float time);
	float GetCoolTime() const;

	bool GetEnableCollision() const;
	void SetEnableCollision(bool enable);

	bool GetIsIgnoredCollision() const;
	void SetIsIgnoredCollision(bool ignored);

	// ������ ���� Ȯ��
	void SetWhetherToRender(bool rendering);
	bool GetWhetherToRender() const;

	// �ø� ���� Ȯ��
	void SetIsCullingFace(bool culling);
	bool GetIsCullingFace() const;

	void SetCollisioned(bool collisioned);
	bool GetCollisioned() const;

	void SetBorder(bool border);
	bool GetBorder() const;

	void SetQuaternion(glm::quat quaternion);
	glm::quat GetQuaternion() const;

	// �θ� ����
	void SetParent(CGameObject* parent);
	CGameObject* GetParent();

	// �θ� ��ġ�� �������� �Ͽ� �̵�
	void StickToParent();

	// ���� ����
	void SetColor(glm::vec4 color);
	glm::vec4 GetColor() const;

	// ���� ���� ����
	void SetIsLightSource(bool lightSource);
	bool GetIsLightSource() const;

	// �׸��� ���� ����
	void SetIsMadeShadow(bool shadow);
	bool GetIsMadeShadow() const;

	// �ִϸ��̼� ���� ����
	void SetIsAnimate(bool animate);
	bool GetIsAnimate() const;

	// �ִϸ��̼� ����
	void SetAnimation(std::string animationName);
	void SetModelWithBone(std::string name);
	void SetAnimationDataNameMap(std::map<GameObjectAnimationState, std::string> map);

	std::string GetModelWithBone();
	std::map<GameObjectAnimationState, std::string> GetAnimationDataNameMap();

	// �ִϸ��̼� ���� setter, getter
	void SetAnimationState(GameObjectAnimationState state);
	GameObjectAnimationState GetAnimationState() const;
	CAnimator* GetAnimator();

	glm::vec4 GetBorderColor() const;
	void SetBorderColor(glm::vec4 color);

	void SetBloodTextureID(int id);
	int GetBloodTextureID() const;

	void SetDefaultSoundID(int id);
	int GetDefaultSoundID() const;

	void SetHitSoundID(int id);
	int GetHitSoundID() const;

private:

};

