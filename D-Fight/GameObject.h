/*
* Object
*	제 3번(최하위)으로 호출되는 클래스
*
*	게임 내 오브젝트의 정보(위치, 크키, Obj, 행동 방식 등)
*	Renderer의 멤버 함수를 통해 렌더링
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
	TYPE_COLLISION_IGNORE_AREA,			// 충돌시 충돌한 물체를 그 프레임 동안 충돌하지 않도록 함
	TYPE_DAMAGE_ZONE,					// 적의 공격의 대미지를 받는 영역 설정
	TYPE_WEAPON,						// 무기
	TYPE_CAMERA_COLLISION,				// 카메라 위치 설정용 충돌체
	TYPE_RECOVER_SPOT,					// 회복 스폿
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
	// 인덱스
	int m_Index;

	// 위치, 회전, 크기
	glm::vec3 m_vPosition = glm::vec3(0.0);
	glm::vec3 m_vRotation = glm::vec3(0.0);
	glm::vec3 m_vSize = glm::vec3(0.0);

	// 부피(충돌 박스 계산용)
	glm::vec3 m_vVolume;

	// 속도, 가속도
	glm::vec3 m_vVelocity;
	glm::vec3 m_vAcceleration;

	// 시선 방향
	glm::vec3 m_vDirection;

	// 충돌 방식
	bool m_bIsBoxCollision;

	// 반지름, 높이
	float m_fRadius;
	float m_fHeight;

	// 무게
	float m_fMass;

	// 속도 제한
	float m_fLimitVelocity;

	// 생존 시간
	float m_fLifeTime;
	
	// 공격 등의 쿨타임(임시: 모션 프레임이 끝났을 때 사용하도록 변경)
	float m_fCoolTime;

	// 모델, 텍스쳐 인덱스
	int m_iModelIndex;
	int m_iTextureIndex;

	// 오브젝트 타입
	GameObjectType m_type;
	// 오브젝트 상태
	GameObjectState m_state;

	// 충돌 가능
	bool m_bEnableCollision;

	// 잠시 충돌 무시
	bool m_bIsIgnoredCollision;

	// 렌더링 여부(충돌박스 등 렌더링하지 않는 경우 체크)
	bool m_bWhetherToRender;

	// 충돌됬는 지 확인
	bool m_bCollisioned;

	// 테두리 표시
	bool m_bBorder;

	// 쿼터니언 회전 - 회전된 걸 다시 회전하기 위해 사용
	// 쿼터니언에 대한 충돌 처리는 하지 않음(쿼터니언 전 회전만으로 충돌 처리))
	glm::quat m_Quaternion = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);

	// 부모(현재 오브젝트를 생성한 오브젝트)
	CGameObject* m_pParent = NULL;

	// 전체 색상 설정
	glm::vec4 m_vColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	
	// 테두리 색상 설정
	glm::vec4 m_vBorderColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	// 컬링을 할지 설정
	bool m_bIsCullingFace = true;
	bool m_bIsLightSource = false;

	// 그림자를 생성할지 설정
	bool m_bIsMadeShadow = true;

	// 애니메이션 관련 변수
	bool m_bIsAnimate;
	GameObjectAnimationState m_animationState;
	CAnimator* m_pAnimator = NULL;
	std::string m_modelWithBoneName;
	std::vector<std::vector<SVertex>> m_calculateVerticesVector;

	std::map<GameObjectAnimationState, std::string> m_animationDataNameMap;

	// 피(파편) 텍스쳐
	int m_BloodTextureID;

	// 기본 사운드
	int m_DefaultSoundID;
	// 피격 사운드
	int m_HitSoundID;

public:
	virtual void Update(float elapsedTimeInSec, GameUpdateParameters parameters);
	virtual void Render(CRenderer* renderer);
	void DepthRender(CRenderer* renderer);
	void BoundingBoxRenderer(CRenderer* renderer);
	void BorderRender(CRenderer* renderer);

	void CalculateVerticesAfterAnimation(CRenderer* renderer);

	// 모델, 텍스쳐 인덱스 setter, getter
	void SetModelIndex(int index);
	void SetTextureIndex(int index);

	int GetModelIndex() const;
	int GetTextureIndex() const;

	void SetIndex(int index);
	int GetIndex() const;

	// 위치, 회전, 크기, 부피, 속도, 가속도, 질량 setter, getter
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

	// 속도 제한
	void SetLimitVelocity(float velocity);

	// 이동 방향
	glm::vec3 GetDirection() const;
	void SetDirection(glm::vec3 direction);

	// 타입, 상태 setter, getter
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

	// 렌더링 여부 확인
	void SetWhetherToRender(bool rendering);
	bool GetWhetherToRender() const;

	// 컬링 여부 확인
	void SetIsCullingFace(bool culling);
	bool GetIsCullingFace() const;

	void SetCollisioned(bool collisioned);
	bool GetCollisioned() const;

	void SetBorder(bool border);
	bool GetBorder() const;

	void SetQuaternion(glm::quat quaternion);
	glm::quat GetQuaternion() const;

	// 부모 설정
	void SetParent(CGameObject* parent);
	CGameObject* GetParent();

	// 부모 위치를 원점으로 하여 이동
	void StickToParent();

	// 색상 설정
	void SetColor(glm::vec4 color);
	glm::vec4 GetColor() const;

	// 광원 여부 설정
	void SetIsLightSource(bool lightSource);
	bool GetIsLightSource() const;

	// 그림자 생성 여부
	void SetIsMadeShadow(bool shadow);
	bool GetIsMadeShadow() const;

	// 애니메이션 여부 설정
	void SetIsAnimate(bool animate);
	bool GetIsAnimate() const;

	// 애니메이션 설정
	void SetAnimation(std::string animationName);
	void SetModelWithBone(std::string name);
	void SetAnimationDataNameMap(std::map<GameObjectAnimationState, std::string> map);

	std::string GetModelWithBone();
	std::map<GameObjectAnimationState, std::string> GetAnimationDataNameMap();

	// 애니메이션 상태 setter, getter
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

