#include "stdafx.h"
#include "GameObject.h"
#include <string>

CGameObject::CGameObject()
{
	m_fLifeTime = 10000000.f;
	m_fLimitVelocity = 6.0f;
	m_type = TYPE_FIXED;
	m_state = STATE_GROUND;
	m_bEnableCollision = false;
	m_bWhetherToRender = true;
	m_vVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
	m_vAcceleration = glm::vec3(0.0f, 0.0f, 0.0f);
	m_bCollisioned = false;
	m_bBorder = false;
	m_vDirection = glm::vec3(0.0f, 0.0f, -1.0f);
	m_fCoolTime = 0.0f;
	m_bIsIgnoredCollision = false;
	m_bIsAnimate = false;
	m_animationState = ANI_STATE_IDLE;
}

CGameObject::~CGameObject()
{
	
}

void CGameObject::CalculateVerticesAfterAnimation(CRenderer* renderer)
{
	if (m_bWhetherToRender || m_bIsMadeShadow || m_bBorder) {
		if (m_bIsAnimate) {
			renderer->CaculateVerticesAfterAnimation(
				m_modelWithBoneName,
				m_calculateVerticesVector,
				m_pAnimator->GetFinalBoneMatrices());
		}
	}
}

void CGameObject::SetModelIndex(int index)
{
	m_iModelIndex = index;
}

void CGameObject::SetTextureIndex(int index)
{
	m_iTextureIndex = index;
}

int CGameObject::GetModelIndex() const
{
	return m_iModelIndex;
}

int CGameObject::GetTextureIndex() const
{
	return m_iTextureIndex;
}

void CGameObject::Update(float elapsedTimeInSec, GameUpdateParameters parameters)
{
	if (m_pAnimator) {
		m_pAnimator->UpdateAnimation(elapsedTimeInSec);
	}

	// 쿨타임 처리
	if (m_fCoolTime > 0.f)
		m_fCoolTime -= elapsedTimeInSec;

	// 힘 처리
	glm::vec3 force = parameters.force;

	glm::vec3 acc = glm::vec3(0.0f, 0.0f, 0.0f);

	glm::vec3 prevVelocity = m_vVelocity;

	if (m_state == STATE_GROUND)
	{
		// 힘 계산
		acc = glm::vec3(force.x / m_fMass, force.y / m_fMass, force.z / m_fMass);

		// 가속도 추가
		acc = glm::vec3(acc.x + m_vAcceleration.x, acc.y + m_vAcceleration.y, acc.z + m_vAcceleration.z);

		glm::vec3 vel = glm::vec3(0.0f, 0.0f, 0.0f);

		// 수평 계산
		float absVelX = std::abs(m_vVelocity.x);
		if (absVelX > 0.0f)
		{
			// 마찰력 적용
			acc.x += (m_vVelocity.x / absVelX) * 0.7f * -GRAVITY_VALUE;

			// 속도 계산
			vel.x = m_vVelocity.x + acc.x * elapsedTimeInSec;
			if (m_vVelocity.x * vel.x < 0.f) m_vVelocity.x = 0.f;
			else m_vVelocity.x = vel.x;

			// 최종 속도 제한
			if (absVelX > m_fLimitVelocity) m_vVelocity.x = (m_vVelocity.x / absVelX) * m_fLimitVelocity;
		}	
		else
		{
			m_vVelocity.x += acc.x * elapsedTimeInSec;
		}

		float absVelZ = std::abs(m_vVelocity.z);
		if (absVelZ > 0.0f)
		{
			// 마찰력 적용
			acc.z += (m_vVelocity.z / absVelZ) * 0.7f * -GRAVITY_VALUE;

			// 속도 계산
			vel.z = m_vVelocity.z + acc.z * elapsedTimeInSec;
			if (m_vVelocity.z * vel.z < 0.f) m_vVelocity.z = 0.f;
			else m_vVelocity.z = vel.z;

			// 최종 속도 제한
			if (absVelZ > m_fLimitVelocity) m_vVelocity.z = (m_vVelocity.z / absVelZ) * m_fLimitVelocity;
		}
		else
		{
			m_vVelocity.z += acc.z * elapsedTimeInSec;
		}

		// 수직 계산
		m_vVelocity.y += acc.y * elapsedTimeInSec;
	}
	else if (m_state == STATE_FALLING)
	{
		// 힘 계산(떨어지는 도중(공중)에서는 힘을 받지 않음)
		acc = glm::vec3(0.0f, 0.0f, 0.0f);

		// 가속도 추가
		acc = glm::vec3(acc.x + m_vAcceleration.x, acc.y + m_vAcceleration.y, acc.z + m_vAcceleration.z);

		glm::vec3 vel = glm::vec3(0.0f, 0.0f, 0.0f);

		// 수평 계산
		float absVelX = std::abs(m_vVelocity.x);
		if (absVelX > 0.0f)
		{
			// 속도 계산
			vel.x = m_vVelocity.x + acc.x * elapsedTimeInSec;

			// 공기 저항
			vel.x += (m_vVelocity.x / absVelX) * 0.01f * -vel.x;

			if (m_vVelocity.x * vel.x < 0.f) m_vVelocity.x = 0.f;
			else m_vVelocity.x = vel.x;

			// 최종 속도 제한
			if (absVelX > m_fLimitVelocity) m_vVelocity.x = (m_vVelocity.x / absVelX) * m_fLimitVelocity;
		}
		else
		{
			m_vVelocity.x += acc.x * elapsedTimeInSec;
		}

		float absVelZ = std::abs(m_vVelocity.z);
		if (absVelZ > 0.0f)
		{
			// 속도 계산
			vel.z = m_vVelocity.z + acc.z * elapsedTimeInSec;

			// 공기 저항
			vel.z += (m_vVelocity.z / absVelZ) * 0.01f * -vel.z;

			if (m_vVelocity.z * vel.z < 0.f) m_vVelocity.z = 0.f;
			else m_vVelocity.z = vel.z;

			// 최종 속도 제한
			if (absVelZ > m_fLimitVelocity) m_vVelocity.z = (m_vVelocity.z / absVelZ) * m_fLimitVelocity;
		}
		else
		{
			m_vVelocity.z += acc.z * elapsedTimeInSec;
		}

		// 중력 적용
		m_vVelocity.y += -GRAVITY_VALUE * elapsedTimeInSec;
	}

	float elapsedTimeInSec2 = elapsedTimeInSec * elapsedTimeInSec;

	// 최종 위치 계산
	m_vPosition = glm::vec3(
		m_vPosition.x + m_vVelocity.x * elapsedTimeInSec + 0.5f * acc.x * elapsedTimeInSec2,
		m_vPosition.y + m_vVelocity.y * elapsedTimeInSec + 0.5f * acc.y * elapsedTimeInSec2,
		m_vPosition.z + m_vVelocity.z * elapsedTimeInSec + 0.5f * acc.z * elapsedTimeInSec2
	);

	// 보는 방향 계산
	if (force != glm::vec3(0.0f, 0.0f, 0.0f))
	{
		float angle = glm::atan(force.z, -force.x) * 180.0f / PI_VALUE;
		angle += 90.0f;
		SetRotation(glm::vec3(m_vRotation.x, angle, m_vRotation.z));
	}
}

void CGameObject::Render(CRenderer* renderer)
{
	if (m_bIsCullingFace) {
		glEnable(GL_CULL_FACE);
	}
	else {
		glDisable(GL_CULL_FACE);
	}

	if (!m_bIsAnimate) {
		renderer->DrawElementsWithTriangles(m_iModelIndex, m_iTextureIndex,
			m_vPosition, m_vRotation, m_vSize, m_Quaternion, m_vColor, m_bIsLightSource, m_bBorder);
	}
	else {
		renderer->DrawAnimationModel(m_modelWithBoneName, m_iTextureIndex,
			m_calculateVerticesVector,
			m_vPosition, m_vRotation, m_vSize, m_Quaternion, m_vColor, m_bIsLightSource, m_bBorder);
	}

	if (!m_bIsCullingFace) {
		glEnable(GL_CULL_FACE);
	}
}

void CGameObject::DepthRender(CRenderer* renderer)
{
	if (m_type == TYPE_FLOOR) {
		glDisable(GL_CULL_FACE);
	}

	if (!m_bIsAnimate) {
		renderer->DrawElementsWithTrianglesForDepth(m_iModelIndex,
			m_vPosition, m_vRotation, m_vSize, m_Quaternion);
	}
	else {
		renderer->DrawAnimationModelForDapth(m_modelWithBoneName,
			m_calculateVerticesVector,
			m_vPosition, m_vRotation, m_vSize, m_Quaternion);
	}

	if (m_type == TYPE_FLOOR) {
		glEnable(GL_CULL_FACE);
	}
}

void CGameObject::BorderRender(CRenderer* renderer)
{
	if (!m_bIsAnimate) {
		renderer->DrawElementsWithTrianglesForBorder(m_iModelIndex,
			m_vPosition, m_vRotation, m_vSize, m_Quaternion, m_vBorderColor);
	}
	else {
		renderer->DrawAnimationModelForBorder(m_modelWithBoneName,
			m_calculateVerticesVector,
			m_vPosition, m_vRotation, m_vSize, m_Quaternion, m_vBorderColor);
	}
}

void CGameObject::BoundingBoxRenderer(CRenderer* renderer)
{
	if (m_bIsBoxCollision) {
		renderer->DrawBoundingBox(m_vPosition, m_vRotation, m_vSize, m_vVolume);
	}
	else {
		renderer->DrawCapsule(m_vPosition, m_fRadius, m_fHeight);
	}
}

void CGameObject::SetIndex(int index)
{
	m_Index = index;
}

int CGameObject::GetIndex() const
{
	return m_Index;
}

void CGameObject::SetPosition(glm::vec3 position)
{
	m_vPosition = position;
}

void CGameObject::SetRotation(glm::vec3 rotation)
{
	m_vRotation = rotation;
}

void CGameObject::SetSize(glm::vec3 size)
{
	m_vSize = size;
}

void CGameObject::SetVolume(glm::vec3 volume)
{
	m_bEnableCollision = true;
	m_bIsBoxCollision = true;
	m_vVolume = volume;
}

void CGameObject::SetRadiusAndHeight(float radius, float height)
{
	m_vVolume = glm::vec3(radius, height, radius);

	m_bEnableCollision = true;
	m_bIsBoxCollision = false;
	m_fRadius = radius;
	m_fHeight = height;
}

void CGameObject::GetRadiusAndHeight(float* radius, float* height)
{
	*radius = m_fRadius;
	*height = m_fHeight;
}

void CGameObject::SetMass(float mass)
{
	m_fMass = mass;
}

void CGameObject::SetVelocity(glm::vec3 velocity)
{
	m_vVelocity = velocity;
}

void CGameObject::SetAcceleration(glm::vec3 acceleration)
{
	m_vAcceleration = acceleration;
}

void CGameObject::SetType(GameObjectType type)
{
	m_type = type;
}

void CGameObject::SetState(GameObjectState state)
{
	m_state = state;
}

GameObjectType CGameObject::GetType()
{
	return m_type;
}

GameObjectState CGameObject::GetState()
{
	return m_state;
}

void CGameObject::SetBorder(bool border)
{
	m_bBorder = border;
}

bool CGameObject::GetBorder() const
{
	return m_bBorder;
}

void CGameObject::SetCollisioned(bool collisioned)
{
	m_bCollisioned = collisioned;
}

void CGameObject::SetAnimationDataNameMap(std::map<GameObjectAnimationState, std::string> map)
{
	m_animationDataNameMap = map;
}

std::string CGameObject::GetModelWithBone()
{
	return m_modelWithBoneName;
}

std::map<GameObjectAnimationState, std::string> CGameObject::GetAnimationDataNameMap()
{
	return m_animationDataNameMap;
}

bool CGameObject::GetCollisioned() const
{
	return m_bCollisioned;
}

void CGameObject::SetLifeTime(float time)
{
	m_fLifeTime = time;
}

float CGameObject::GetLifeTime() const
{
	return m_fLifeTime;
}

void CGameObject::SetCoolTime(float time)
{
	m_fCoolTime = time;
}

float CGameObject::GetCoolTime() const
{
	return m_fCoolTime;
}

bool CGameObject::GetEnableCollision() const
{
	return m_bEnableCollision;
}

void CGameObject::SetEnableCollision(bool enable)
{
	m_bEnableCollision = enable;
}

bool CGameObject::GetIsIgnoredCollision() const
{
	return m_bIsIgnoredCollision;
}

void CGameObject::SetIsIgnoredCollision(bool ignored)
{
	m_bIsIgnoredCollision = ignored;
}

void CGameObject::SetColor(glm::vec4 color)
{
	m_vColor = color;
}

glm::vec4 CGameObject::GetColor() const
{
	return m_vColor;
}

void CGameObject::SetIsLightSource(bool lightSource)
{
	m_bIsLightSource = lightSource;
}

bool CGameObject::GetIsLightSource() const
{
	return m_bIsLightSource;
}

void CGameObject::SetIsMadeShadow(bool shadow)
{
	m_bIsMadeShadow = shadow;
}

bool CGameObject::GetIsMadeShadow() const
{
	return m_bIsMadeShadow;
}

void CGameObject::SetIsAnimate(bool animate)
{
	m_bIsAnimate = animate;
	if (m_bIsAnimate) {
		if (!m_pAnimator) {
			m_pAnimator = new CAnimator();
		}
	}
}

bool CGameObject::GetIsAnimate() const
{
	return m_bIsAnimate;
}

void CGameObject::SetAnimation(std::string animationName)
{
	if (m_pAnimator) {
		m_pAnimator->SetAnimation(CRenderer::GetInstance()->GetAnimation(animationName));
	}
}

void CGameObject::SetWhetherToRender(bool rendering)
{
	m_bWhetherToRender = rendering;

}
bool CGameObject::GetWhetherToRender() const
{
	return m_bWhetherToRender;
}

void CGameObject::SetIsCullingFace(bool culling)
{
	m_bIsCullingFace = culling;

}
bool CGameObject::GetIsCullingFace() const
{
	return m_bIsCullingFace;
}

void CGameObject::SetModelWithBone(std::string name)
{
	m_modelWithBoneName = name;

	m_calculateVerticesVector.clear();

	CModelLoader* modelData = CRenderer::GetInstance()->GetModelWithBone(name);
	auto models = modelData->GetModels();

	for (SAssimpAnimationModel model : models) {
		std::vector<SVertex> vertices;
		vertices.reserve(model.vertices.size());
		std::copy(model.vertices.begin(), model.vertices.end(), std::back_inserter(vertices));
		
		m_calculateVerticesVector.push_back(vertices);
	}
}

glm::vec3 CGameObject::GetPosition() const
{
	return m_vPosition;
}

glm::vec3 CGameObject::GetRotation() const
{
	return m_vRotation;
}

glm::vec3 CGameObject::GetSize() const
{
	return m_vSize;
}

glm::vec3 CGameObject::GetVelocity() const
{
	return m_vVelocity;
}

glm::vec3 CGameObject::GetAcceleration() const
{
	return m_vAcceleration;
}

glm::vec3 CGameObject::GetVolume() const
{
	return m_vVolume;
}

float CGameObject::GetMass() const
{
	return m_fMass;
}

void CGameObject::SetLimitVelocity(float velocity)
{
	m_fLimitVelocity = velocity;
}

glm::vec3 CGameObject::GetDirection() const
{
	return m_vDirection;
}

void CGameObject::SetIsBoxCollision(bool boxCollision)
{
	m_bIsBoxCollision = boxCollision;
}

bool CGameObject::GetIsBoxCollision() const
{
	return m_bIsBoxCollision;
}

void CGameObject::SetDirection(glm::vec3 direction)
{
	m_vDirection = direction;
}

void  CGameObject::SetParent(CGameObject* parent)
{
	m_pParent = parent;
}

CGameObject* CGameObject::GetParent()
{
	return m_pParent;
}

void CGameObject::SetQuaternion(glm::quat quaternion)
{
	m_Quaternion = quaternion;
}

glm::quat  CGameObject::GetQuaternion() const
{
	return m_Quaternion;
}

void CGameObject::StickToParent()
{
	if (m_pParent == NULL)
	{
		std::cout << "현재 오브젝트를 생성한 부모가 없습니다\n";
	}
	else
	{
		glm::vec3 parentPos = m_pParent->GetPosition();
		m_vPosition += parentPos;
	}
}

void CGameObject::SetAnimationState(GameObjectAnimationState state)
{
	m_animationState = state;
	if (m_bIsAnimate) {
		SetAnimation(m_animationDataNameMap.find(state)->second);
	}
}

GameObjectAnimationState CGameObject::GetAnimationState() const
{
	return m_animationState;
}

CAnimator* CGameObject::GetAnimator()
{
	return m_pAnimator;
}

void CGameObject::SetBloodTextureID(int id)
{
	m_BloodTextureID = id;
}

int CGameObject::GetBloodTextureID() const
{
	return m_BloodTextureID;
}

void CGameObject::SetDefaultSoundID(int id)
{
	m_DefaultSoundID = id;
}

int CGameObject::GetDefaultSoundID() const
{
	return m_DefaultSoundID;
}

void CGameObject::SetHitSoundID(int id)
{
	m_HitSoundID = id;
}

int CGameObject::GetHitSoundID() const
{
	return m_HitSoundID;
}

glm::vec4 CGameObject::GetBorderColor() const
{
	return m_vBorderColor;
}

void  CGameObject::SetBorderColor(glm::vec4 color)
{
	m_vBorderColor = color;
}