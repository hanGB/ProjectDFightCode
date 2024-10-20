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
	// 볼륨, 위치 등의 데이터가 필요없는 충돌 처리
	virtual void ProcessCollisionWithOtherData(CGameObject* objectA, CGameObject* objectB);

	void ProcessingWeaponDamage(CGameObject* weapon, CGameObject* monster);
	void ProcessingWeaponDamageWithBoss(CGameObject* weapon, CGameObject* boss);
	void ProcessingWeaponDamageWithBossHead(CGameObject* weapon, CGameObject* head);
	void ProcessingDamageZoneDamage(CGameObject* damageZone, CGameObject* hero);

};