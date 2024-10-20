#pragma once

#include "AnimationLoader.h"
#include "Globals.h"

class CAnimator {
public:
	CAnimator();
	CAnimator(CAnimationLoader* animation);
	~CAnimator();

private:
	std::vector<glm::mat4> m_FinalBoneMatrices;
	CAnimationLoader* m_pCurrentAnimation = NULL;
	
	float m_fCurrentTime;
	bool m_bIsLoop = true;
	float m_fSpeed = 1.0f;
	bool m_bIsStop = false;

public:
	void SetAnimation(CAnimationLoader* animation);
	void UpdateAnimation(float elapsedTimeInSec);
	std::vector<glm::mat4> GetFinalBoneMatrices();
	
	void SetIsLoop(bool loop);
	void SetSpeed(float speed);
	void SetIsStop(bool stop);

	bool GetIsLoop() const;
	float GetSpeed() const;
	bool GetIsStop() const;

	bool IsOneCycleEnd();

	void SetAnimationCurrentTime(float time);
	float GetAnimationCurrentTime() const;

private:
	void CalculateBoneTrasform(const SAssimpNodeData* node, glm::mat4 parentTransform);

};