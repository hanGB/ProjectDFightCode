#include "stdafx.h"
#include "BossMonster.h"

CBossHead::CBossHead()
{
}

CBossHead::~CBossHead()
{
}

void CBossHead::Update(float elapsedTimeInSec, GameUpdateParameters parameters)
{
	UpdateDamageCoolTime(elapsedTimeInSec);

	if (GetTurnBattleObject()->UpdateDownLevelForHybridBattle(elapsedTimeInSec)) {
		if (GetAnimationState() != ANI_STATE_IDLE) {
			SetAnimationState(ANI_STATE_IDLE);
		}
		// 다운 되어 있을 경우 리턴
		return;
	}

	// 상태이상 처리
	TurnBattle::SBattleVariable bv = GetTurnBattleObject()->GetBattleVariable();
	if (bv.condition != NORMAL_CONDITION) {
		bv.conditionDuration -= elapsedTimeInSec;
		if (bv.conditionDuration <= 0.0f) {
			bv.condition = NORMAL_CONDITION;
		}
		GetTurnBattleObject()->SetBattleVariable(bv);
	}
}

void CBossHead::SetIsRightHead(bool right)
{
	m_bIsRightHead = right;
}

bool CBossHead::GetIsRightHead() const
{
	return m_bIsRightHead;
}



CBossMonster::CBossMonster()
{
	BuildBehaviorTree();
}

CBossMonster::~CBossMonster()
{
}

void CBossMonster::Update(float elapsedTimeInSec, GameUpdateParameters parameters)
{
	if (GetTurnBattleObject()->GetAnimationState() == ANI_STATE_DEATH) {
		MatchAnimationWithTurnBattleObject();
		return;
	}

	// 턴 게이지가 hp바에 반영되도록 설정
	GetTurnBattleObject()->SetGettingTurnGauge(GetTurnGauge());

	UpdateDamageCoolTime(elapsedTimeInSec);

	CGameObject::Update(elapsedTimeInSec, parameters);

	if (GetTurnBattleObject()->UpdateDownLevelForHybridBattle(elapsedTimeInSec)) {
		if (GetAnimationState() != ANI_STATE_IDLE) {
			SetAnimationState(ANI_STATE_IDLE);
		}
		// 다운 되어 있을 경우 리턴
		return;
	}

	if (!GetIsAttacking()) {
		if (!GetIsChangedPhase()
			&& GetAnimationState() != ANI_STATE_HIT
			&& GetAnimationState() != ANI_STATE_HIT_LEFT
			&& GetAnimationState() != ANI_STATE_HIT_RIGHT) {
			if (!GetAnimator()->GetIsLoop()) {
				if (GetAnimator()->IsOneCycleEnd()) {
					glm::vec3 velocity = GetVelocity(); velocity.y = 0;
					if (glm::length(velocity) > 0.0f) {
						SetAnimationState(ANI_STATE_WALK);
					}
					else {
						SetAnimationState(ANI_STATE_IDLE);
					}
				}
			}
		}
	}

	// 상태이상 처리
	TurnBattle::SBattleVariable bv = GetTurnBattleObject()->GetBattleVariable();
	if (bv.condition != NORMAL_CONDITION) {
		bv.conditionDuration -= elapsedTimeInSec;
		if (bv.conditionDuration <= 0.0f) {
			bv.condition = NORMAL_CONDITION;
		}
		GetTurnBattleObject()->SetBattleVariable(bv);
	}

	UpdateSuperArmorRecoverTime(elapsedTimeInSec);
	GetBehaviorTree()->Run();

	if (IsGetTurn()) {
		GetTurnBattleObject()->InitTurn();
		SetTurnGauge(0.0f);
	}

	if (m_pLeftHead != NULL || m_pRightHead != NULL) {
		SetHeadLocation();
	}
}

void CBossMonster::SetHead(CBossHead* left, CBossHead* right)
{
	m_pLeftHead = left;
	m_pRightHead = right;

	m_pLeftHead->SetParent(this);
	m_pRightHead->SetParent(this);
}

void CBossMonster::SetHeadNull(bool right)
{
	if (right) m_pRightHead = NULL;
	else m_pLeftHead = NULL;
}

CBossHead* CBossMonster::GetLeftHead()
{
	return m_pLeftHead;
}

CBossHead* CBossMonster::GetRightHead()
{
	return m_pRightHead;
}

void CBossMonster::SetIsChangedPhase(bool change)
{
	m_bIsChangedPhase = change;
}

bool CBossMonster::GetIsChangedPhase() const
{
	return m_bIsChangedPhase;
}

short CBossMonster::GetPhase() const
{
	return m_sPhase;
}

void CBossMonster::SetPhase(short s)
{
	m_sPhase = s;
}

void CBossMonster::SetHeadLocation()
{
	glm::quat modelQuat
		= glm::quat(glm::vec3(glm::radians(GetRotation().x), glm::radians(GetRotation().y), glm::radians(GetRotation().z)));
	glm::mat4 rotationMat = glm::toMat4(modelQuat);

	glm::vec3 leftPos = rotationMat * glm::vec4(-2.0f, 1.5f, -2.5f, 1.0f);
	glm::vec3 rightPos = rotationMat * glm::vec4(2.0f, 1.5f, -2.5f, 1.0f);

	if (m_pLeftHead != NULL) {
		m_pLeftHead->SetPosition(glm::vec3(leftPos.x, leftPos.y, leftPos.z));
		m_pLeftHead->SetRotation(GetRotation());
		m_pLeftHead->StickToParent();
	}

	if (m_pRightHead != NULL) {
		m_pRightHead->SetPosition(glm::vec3(rightPos.x, rightPos.y, rightPos.z));
		m_pRightHead->SetRotation(GetRotation());
		m_pRightHead->StickToParent();
	}
}
