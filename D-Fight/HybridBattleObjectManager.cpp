#include "stdafx.h"
#include "HybridBattleObjectManager.h"
#include "BloodEffectParticle.h"
#include "BossMonster.h"
#include "SoundPlayer.h"

CHybridBattleObjectManager::CHybridBattleObjectManager()
{
	float* wallSize = new float[1];
	wallSize[0] = 96.0f;

	CObjectManager::SetWallSizes(wallSize, 1);
}

CHybridBattleObjectManager::~CHybridBattleObjectManager()
{
	
}

bool CHybridBattleObjectManager::GetIsEnteredBattle() const
{
	return m_bIsEnteredBattle;
}

void CHybridBattleObjectManager::SetIsEnteredBattle(bool enter)
{
	m_bIsEnteredBattle = enter;
}

bool CHybridBattleObjectManager::GetIsHeroWin() const
{
	return m_bIsHeroWin;
}

void CHybridBattleObjectManager::ProcessCollisionWithOtherData(CGameObject* objectA, CGameObject* objectB)
{
	CObjectManager::ProcessCollisionWithOtherData(objectA, objectB);

	if (objectA->GetType() == TYPE_DAMAGE_ZONE && objectB->GetType() == TYPE_HERO) {
		if (objectB->GetAnimationState() != ANI_STATE_HIT) {
			if (objectB->GetAnimationState() != ANI_STATE_ATTACK) {
				dynamic_cast<CHeroObject*>(objectB)->GetWeaponObject()->SetEnableCollision(false);
			}
			objectB->SetAnimationState(ANI_STATE_HIT);
			objectB->GetAnimator()->SetIsLoop(false);
		}
		ProcessingDamageZoneDamage(objectA, objectB);
	}
	else if (objectB->GetType() == TYPE_DAMAGE_ZONE && objectA->GetType() == TYPE_HERO) {
		if (objectA->GetAnimationState() != ANI_STATE_HIT) {
			if (objectA->GetAnimationState() != ANI_STATE_ATTACK) {
				dynamic_cast<CHeroObject*>(objectA)->GetWeaponObject()->SetEnableCollision(false);
			}
			objectA->SetAnimationState(ANI_STATE_HIT);
			objectA->GetAnimator()->SetIsLoop(false);
		}
		ProcessingDamageZoneDamage(objectB, objectA);
	}
	else if (objectB->GetType() == TYPE_WEAPON && objectA->GetType() == TYPE_ENEMY) {
		ProcessingWeaponDamage(objectB, objectA);
	}
	else if (objectB->GetType() == TYPE_ENEMY && objectA->GetType() == TYPE_WEAPON) {
		ProcessingWeaponDamage(objectA, objectB);
	}
	else if (objectB->GetType() == TYPE_WEAPON && objectA->GetType() == TYPE_BOSS) {
		ProcessingWeaponDamageWithBoss(objectB, objectA);
	}
	else if (objectB->GetType() == TYPE_BOSS && objectA->GetType() == TYPE_WEAPON) {
		ProcessingWeaponDamageWithBoss(objectA, objectB);
	}
	else if (objectB->GetType() == TYPE_WEAPON && objectA->GetType() == TYPE_BOSS_HEAD) {
		ProcessingWeaponDamageWithBossHead(objectB, objectA);
	}
	else if (objectB->GetType() == TYPE_BOSS_HEAD && objectA->GetType() == TYPE_WEAPON) {
		ProcessingWeaponDamageWithBossHead(objectA, objectB);
	}
}

void CHybridBattleObjectManager::ProcessingWeaponDamage(CGameObject* weapon, CGameObject* monster)
{
	CBattleMonster* battleMon = dynamic_cast<CBattleMonster*>(monster);
	CHeroObject* hero = dynamic_cast<CHeroObject*>(weapon->GetParent());
	
	int particle = AddParticle<CBloodEffectParticle>(monster->GetPosition(), glm::vec3(1.0f, 1.0f, 1.0f));
	GetParticle<CBloodEffectParticle>(particle)->SetColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	GetParticle<CBloodEffectParticle>(particle)->SetDirection(weapon->GetPosition() - monster->GetPosition());
	GetParticle<CBloodEffectParticle>(particle)->SetTextureID(monster->GetBloodTextureID());
	GetParticle<CBloodEffectParticle>(particle)->SetLifeTime(3.0f);
	GetParticle<CBloodEffectParticle>(particle)->CreateVAO();

	CSoundPlayer::GetInstance()->PlaySound(battleMon->GetHitSoundID(), 0.5f);


	if (!battleMon->GetIsAttacking() || !battleMon->GetIsSuperArmor()) {
		GameObjectAnimationState state = monster->GetAnimationState();
		if (state != ANI_STATE_HIT) {
			monster->SetAnimationState(ANI_STATE_HIT);
			monster->GetAnimator()->SetIsLoop(false);
		}
	}

	// 턴 배틀 오브젝트에 공격 대미지 부여
	battleMon->UpdateSuperArmorGauge(1);
	battleMon->GetTurnBattleObject()->GiveAttackDamage(hero->GetTurnBattleObject(), true);
	weapon->SetEnableCollision(false);
	hero->GetTurnBattleObject()->IncreaseGettingTurnGauge(DEFAULT_TURN_GETTING_GAUGE_INCREASE_RATE);
}

void CHybridBattleObjectManager::ProcessingWeaponDamageWithBoss(CGameObject* weapon, CGameObject* boss)
{
	CBossMonster* bossMonster = dynamic_cast<CBossMonster*>(boss);
	CHeroObject* hero = dynamic_cast<CHeroObject*>(weapon->GetParent());

	if (bossMonster->GetIsChangedPhase()) {
		return;
	}

	int particle = AddParticle<CBloodEffectParticle>(bossMonster->GetPosition(), glm::vec3(1.0f, 1.0f, 1.0f));
	GetParticle<CBloodEffectParticle>(particle)->SetColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	GetParticle<CBloodEffectParticle>(particle)->SetDirection(weapon->GetPosition() - bossMonster->GetPosition());
	GetParticle<CBloodEffectParticle>(particle)->SetTextureID(bossMonster->GetBloodTextureID());
	GetParticle<CBloodEffectParticle>(particle)->SetLifeTime(3.0f);
	GetParticle<CBloodEffectParticle>(particle)->CreateVAO();

	CSoundPlayer::GetInstance()->PlaySound(bossMonster->GetHitSoundID(), 0.5f);

	if (!bossMonster->GetIsAttacking() || !bossMonster->GetIsSuperArmor()) {
		GameObjectAnimationState state = bossMonster->GetAnimationState();
		if (state != ANI_STATE_HIT && state != ANI_STATE_HIT_RIGHT && state != ANI_STATE_HIT_LEFT) {
			bossMonster->SetAnimationState(ANI_STATE_HIT);
			bossMonster->GetAnimator()->SetIsLoop(false);
		}
	}

	bossMonster->UpdateSuperArmorGauge(1);

	// 턴 배틀 오브젝트에 공격 대미지 부여
	bossMonster->GetTurnBattleObject()->GiveAttackDamage(hero->GetTurnBattleObject(), true);
	weapon->SetEnableCollision(false);
	hero->GetTurnBattleObject()->IncreaseGettingTurnGauge(DEFAULT_TURN_GETTING_GAUGE_INCREASE_RATE);
}

void CHybridBattleObjectManager::ProcessingWeaponDamageWithBossHead(CGameObject* weapon, CGameObject* head)
{
	CBossHead* bossHead= dynamic_cast<CBossHead*>(head);
	CHeroObject* hero = dynamic_cast<CHeroObject*>(weapon->GetParent());

	int particle = AddParticle<CBloodEffectParticle>(bossHead->GetPosition(), glm::vec3(1.0f, 1.0f, 1.0f));
	GetParticle<CBloodEffectParticle>(particle)->SetColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	GetParticle<CBloodEffectParticle>(particle)->SetDirection(weapon->GetPosition() - bossHead->GetPosition());
	GetParticle<CBloodEffectParticle>(particle)->SetTextureID(bossHead->GetBloodTextureID());
	GetParticle<CBloodEffectParticle>(particle)->SetLifeTime(3.0f);
	GetParticle<CBloodEffectParticle>(particle)->CreateVAO();

	CBossMonster* boss = dynamic_cast<CBossMonster*>(bossHead->GetParent());

	CSoundPlayer::GetInstance()->PlaySound(boss->GetHitSoundID(), 0.5f);


	if (!boss->GetIsAttacking() || !boss->GetIsSuperArmor()) {
		GameObjectAnimationState state = boss->GetAnimationState();
		if (state != ANI_STATE_HIT && state != ANI_STATE_HIT_RIGHT && state != ANI_STATE_HIT_LEFT) {
			if (bossHead->GetIsRightHead()) boss->SetAnimationState(ANI_STATE_HIT_RIGHT);
			else boss->SetAnimationState(ANI_STATE_HIT_LEFT);
			boss->GetAnimator()->SetIsLoop(false);
		}
	}

	boss->UpdateSuperArmorGauge(1);

	// 턴 배틀 오브젝트에 공격 대미지 부여
	bossHead->GetTurnBattleObject()->GiveAttackDamage(hero->GetTurnBattleObject(), true);
	weapon->SetEnableCollision(false);
	hero->GetTurnBattleObject()->IncreaseGettingTurnGauge(DEFAULT_TURN_GETTING_GAUGE_INCREASE_RATE);
}

void CHybridBattleObjectManager::ProcessingDamageZoneDamage(CGameObject* damageZone, CGameObject* hero)
{
	CBattleMonster* battleMon = dynamic_cast<CBattleMonster*>(damageZone->GetParent());
	CHeroObject* heroObject = dynamic_cast<CHeroObject*>(hero);

	// 턴 배틀 오브젝트에 공격 대미지 부여
	heroObject->GetTurnBattleObject()->GiveAttackDamage(battleMon->GetTurnBattleObject(), false);
	battleMon->AddTurnGauge(DEFAULT_TURN_GETTING_GAUGE_INCREASE_RATE_FOR_MONSTER);
	damageZone->SetEnableCollision(false);
	dynamic_cast<CDamageZone*>(damageZone)->SetIsGivenDamage(true);

	int particle = AddParticle<CBloodEffectParticle>(hero->GetPosition(), glm::vec3(1.0f, 1.0f, 1.0f));
	GetParticle<CBloodEffectParticle>(particle)->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	GetParticle<CBloodEffectParticle>(particle)->SetDirection(damageZone->GetPosition() - hero->GetPosition());
	GetParticle<CBloodEffectParticle>(particle)->SetTextureID(hero->GetBloodTextureID());
	GetParticle<CBloodEffectParticle>(particle)->SetLifeTime(3.0f);
	GetParticle<CBloodEffectParticle>(particle)->CreateVAO();
	CSoundPlayer::GetInstance()->PlaySound(hero->GetHitSoundID(), 0.75f);
}
