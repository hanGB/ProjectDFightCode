#include "stdafx.h"
#include "FieldObjectManager.h"
#include "RecoverSpot.h"
#include "BloodEffectParticle.h"
#include "SoundPlayer.h"

CFieldObjectManager::CFieldObjectManager()
{
}

CFieldObjectManager::~CFieldObjectManager()
{
}

bool CFieldObjectManager::GetIsEnteredBattle() const
{
	return m_bIsEnteredBattle;
}

void CFieldObjectManager::SetIsEnteredBattle(bool enter)
{
	m_bIsEnteredBattle = enter;
}

bool CFieldObjectManager::GetIsHeroWin() const
{
	return m_bIsHeroWin;
}

int CFieldObjectManager::GetAreaNumber() const
{
	return m_iAreaNumber;
}

void CFieldObjectManager::ProcessCollisionWithOtherData(CGameObject* objectA, CGameObject* objectB)
{
	CObjectManager::ProcessCollisionWithOtherData(objectA, objectB);

	// 적과 무기와의 충돌
	if (objectB->GetType() == TYPE_WEAPON && objectA->GetType() == TYPE_ENEMY) {
		CFieldMonster* monster = dynamic_cast<CFieldMonster*>(objectA);

		if (monster->IsCanAppliedDamage()) {
			// 무기 무게로 데미지 계산
			bool isDead = monster->GiveDamageToHp((short)objectB->GetMass());
			objectB->SetEnableCollision(false);

			int particle = AddParticle<CBloodEffectParticle>(objectA->GetPosition(), glm::vec3(1.0f, 1.0f, 1.0f));
			GetParticle<CBloodEffectParticle>(particle)->SetColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
			GetParticle<CBloodEffectParticle>(particle)->SetDirection(objectB->GetPosition() - objectA->GetPosition());
			GetParticle<CBloodEffectParticle>(particle)->SetTextureID(objectA->GetBloodTextureID());
			GetParticle<CBloodEffectParticle>(particle)->SetLifeTime(3.0f);
			GetParticle<CBloodEffectParticle>(particle)->CreateVAO();

			CSoundPlayer::GetInstance()->PlaySound(monster->GetHitSoundID(), 0.5f);

			if (isDead) {
				m_iAreaNumber = monster->GetAreaNumber();
				m_bIsEnteredBattle = true;
				m_bIsHeroWin = true;
				monster->SetIsDead(true);
			}
		}
	}
	else if (objectB->GetType() == TYPE_ENEMY && objectA->GetType() == TYPE_WEAPON) {
		CFieldMonster* monster = dynamic_cast<CFieldMonster*>(objectB);

		if (monster->IsCanAppliedDamage()) {
			// 무기 무게로 데미지 계산
			bool isDead = monster->GiveDamageToHp((short)objectA->GetMass());
			objectA->SetEnableCollision(false);

			int particle = AddParticle<CBloodEffectParticle>(objectB->GetPosition(), glm::vec3(1.0f, 1.0f, 1.0f));
			GetParticle<CBloodEffectParticle>(particle)->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			GetParticle<CBloodEffectParticle>(particle)->SetDirection(objectA->GetPosition() - objectB->GetPosition());
			GetParticle<CBloodEffectParticle>(particle)->SetTextureID(objectB->GetBloodTextureID());
			GetParticle<CBloodEffectParticle>(particle)->SetLifeTime(3.0f);
			GetParticle<CBloodEffectParticle>(particle)->CreateVAO();

			CSoundPlayer::GetInstance()->PlaySound(monster->GetHitSoundID(), 0.5f);

			if (isDead) {
				m_iAreaNumber = monster->GetAreaNumber();
				m_bIsEnteredBattle = true;
				m_bIsHeroWin = true;
				monster->SetIsDead(true);
			}
		}
	}
	// 주인공과 적과의 충돌
	else if (objectB->GetType() == TYPE_HERO && objectA->GetType() == TYPE_ENEMY) {
		CFieldMonster* monster = dynamic_cast<CFieldMonster*>(objectA);
		if (!monster->GetIsRigidity()) {
			objectB->SetAnimationState(ANI_STATE_HIT);
			objectB->GetAnimator()->SetIsLoop(false);
			m_iAreaNumber = monster->GetAreaNumber();
			m_bIsEnteredBattle = true;
			m_bIsHeroWin = false;
			monster->SetIsDead(true);

			CSoundPlayer::GetInstance()->PlaySound(objectB->GetHitSoundID(), 0.75f);
		}
	}
	else if (objectB->GetType() == TYPE_ENEMY && objectA->GetType() == TYPE_HERO) {
		CFieldMonster* monster = dynamic_cast<CFieldMonster*>(objectB);
		if (!monster->GetIsRigidity()) {
			objectA->SetAnimationState(ANI_STATE_HIT);
			objectA->GetAnimator()->SetIsLoop(false);
			m_iAreaNumber = monster->GetAreaNumber();
			m_bIsEnteredBattle = true;
			m_bIsHeroWin = false;
			monster->SetIsDead(true);

			CSoundPlayer::GetInstance()->PlaySound(objectA->GetHitSoundID(), 0.75f);
		}
	}
	// 주인공과 회복 스폿과의 충돌
	else if (objectA->GetType() == TYPE_HERO && objectB->GetType() == TYPE_RECOVER_SPOT) {
		CRecoverSpot* spot = dynamic_cast<CRecoverSpot*>(objectB);
		spot->SetIsOverlapedHero(true);
	}
	else if (objectA->GetType() == TYPE_RECOVER_SPOT && objectB->GetType() == TYPE_HERO) {
		CRecoverSpot* spot = dynamic_cast<CRecoverSpot*>(objectA);
		spot->SetIsOverlapedHero(true);
	}
}
