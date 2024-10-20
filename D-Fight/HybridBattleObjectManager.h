#pragma once
#include "ObjectManager.h"
#include "BattleMonster.h"
#include "HeroObject.h"
#include "PointLight.h"

class CHybridBattleObjectManager : public CObjectManager {
public:
	CHybridBattleObjectManager();
	~CHybridBattleObjectManager();

private:
	bool m_bIsEnteredBattle = false;
	bool m_bIsHeroWin = false;

public:
	bool GetIsEnteredBattle() const;
	void SetIsEnteredBattle(bool enter);

	bool GetIsHeroWin() const;

private:
	// ����, ��ġ ���� �����Ͱ� �ʿ���� �浹 ó��
	virtual void ProcessCollisionWithOtherData(CGameObject* objectA, CGameObject* objectB);

	void ProcessingWeaponDamage(CGameObject* weapon, CGameObject* monster);
	void ProcessingWeaponDamageWithBoss(CGameObject* weapon, CGameObject* boss);
	void ProcessingWeaponDamageWithBossHead(CGameObject* weapon, CGameObject* head);
	void ProcessingDamageZoneDamage(CGameObject* damageZone, CGameObject* hero);

};