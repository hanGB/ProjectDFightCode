#include "stdafx.h"
#include "Animator.h"

CAnimator::CAnimator()
{
	m_FinalBoneMatrices.reserve(MAX_BONE_SIZE);

	for (int i = 0; i < MAX_BONE_SIZE; ++i) {
		m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
	}
}

CAnimator::CAnimator(CAnimationLoader* animation)
{
	m_fCurrentTime = 0.0f;
	m_pCurrentAnimation = animation;

	m_FinalBoneMatrices.reserve(MAX_BONE_SIZE);

	for (int i = 0; i < MAX_BONE_SIZE; ++i) {
		m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
	}
}

CAnimator::~CAnimator()
{
}

void CAnimator::SetAnimation(CAnimationLoader* animation)
{
	m_pCurrentAnimation = animation;
	m_fCurrentTime = 0.0f;

	// 기본 설정으로 초기화
	SetIsLoop(true);
	SetSpeed(1.0f);
}

void CAnimator::UpdateAnimation(float elapsedTimeInSec)
{
	if (!m_pCurrentAnimation) return;

	if (!m_bIsStop) {
		m_fCurrentTime += m_pCurrentAnimation->GetTicksPerSecond() * elapsedTimeInSec * m_fSpeed;
	}
	if (!m_bIsLoop) {
		if (IsOneCycleEnd()) {
			return;
		}
	}
	m_fCurrentTime = glm::mod(m_fCurrentTime, m_pCurrentAnimation->GetDuration());
	CalculateBoneTrasform(&m_pCurrentAnimation->GetRootNode(), glm::mat4(1.0f));
}

std::vector<glm::mat4> CAnimator::GetFinalBoneMatrices()
{
	return m_FinalBoneMatrices;
}

void CAnimator::SetIsLoop(bool loop)
{
	m_bIsLoop = loop;
}

void CAnimator::SetSpeed(float speed)
{
	m_fSpeed = speed;
}

void CAnimator::SetIsStop(bool stop)
{
	m_bIsStop = stop;
}

bool CAnimator::GetIsLoop() const
{
	return m_bIsLoop;
}

float CAnimator::GetSpeed() const
{
	return m_fSpeed;
}

bool CAnimator::GetIsStop() const
{
	return m_bIsStop;
}

bool CAnimator::IsOneCycleEnd()
{
	return m_fCurrentTime >= m_pCurrentAnimation->GetDuration();
}

void CAnimator::SetAnimationCurrentTime(float time)
{
	m_fCurrentTime = time;
}

float CAnimator::GetAnimationCurrentTime() const
{
	return m_fCurrentTime;
}

void CAnimator::CalculateBoneTrasform(const SAssimpNodeData* node, glm::mat4 parentTransform)
{
	std::string nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;

	CBone* bone = m_pCurrentAnimation->FindBone(nodeName);

	if (bone) {
		bone->Update(m_fCurrentTime);
		nodeTransform = bone->GetLocalTransform();
	}

	glm::mat4 transform = parentTransform * nodeTransform;

	std::map<std::string, SBoneInfo> boneInfoMap = m_pCurrentAnimation->GetBoneInfoMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
		int id = boneInfoMap[nodeName].id;
		glm::mat4 offset = boneInfoMap[nodeName].offset;
		m_FinalBoneMatrices[id] = transform * offset;
	}

	for (int i = 0; i < node->children.size(); ++i) {
		CalculateBoneTrasform(&node->children[i], transform);
	}
}
