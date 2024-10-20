#pragma once
#include "ObjectManager.h"
#include "FieldMonster.h"

class CFieldObjectManager : public CObjectManager {
public:
	CFieldObjectManager();
	~CFieldObjectManager();

private:
	bool m_bIsEnteredBattle = false;
	bool m_bIsHeroWin = false;
	int m_iAreaNumber = 1;			// 테스트로 1로 변경

public:
	bool GetIsEnteredBattle() const;
	void SetIsEnteredBattle(bool enter);

	bool GetIsHeroWin() const;

	int GetAreaNumber() const;

private:
	// 볼륨, 위치 등의 데이터가 필요없는 충돌 처리
	virtual void ProcessCollisionWithOtherData(CGameObject* objectA, CGameObject* objectB);
};