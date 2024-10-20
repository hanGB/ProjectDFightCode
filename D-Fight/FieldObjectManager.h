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
	int m_iAreaNumber = 1;			// �׽�Ʈ�� 1�� ����

public:
	bool GetIsEnteredBattle() const;
	void SetIsEnteredBattle(bool enter);

	bool GetIsHeroWin() const;

	int GetAreaNumber() const;

private:
	// ����, ��ġ ���� �����Ͱ� �ʿ���� �浹 ó��
	virtual void ProcessCollisionWithOtherData(CGameObject* objectA, CGameObject* objectB);
};