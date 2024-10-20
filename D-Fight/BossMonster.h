#pragma once
#include "BattleMonster.h"

// �ൿƮ���� ������Ʈ ���� �ʴ�(�������� �ʴ�) CBattleMonster;
class CBossHead : public CBattleMonster {
public:
	CBossHead();
	~CBossHead();

private:
	bool m_bIsRightHead = false;

public:
	virtual void Update(float elapsedTimeInSec, GameUpdateParameters parameters);

	void SetIsRightHead(bool right);
	bool GetIsRightHead() const;

};


class CBossMonster : public CBattleMonster {
public:
	CBossMonster();
	~CBossMonster();

private:
	CBossHead* m_pLeftHead = NULL;
	CBossHead* m_pRightHead = NULL;

	bool m_bIsChangedPhase = false;

	short m_sPhase = 1;

public:
	virtual void Update(float elapsedTimeInSec, GameUpdateParameters parameters);

	void SetHead(CBossHead* left, CBossHead* right);
	void SetHeadNull(bool right);
	CBossHead* GetLeftHead();
	CBossHead* GetRightHead();

	void SetIsChangedPhase(bool change);
	bool GetIsChangedPhase() const;

	short GetPhase() const;
	void SetPhase(short s);

private:
	void SetHeadLocation();

	virtual void BuildBehaviorTree();
};

// ���ΰ� ���� ����
class CBossMonsterFallowHero : public CLeafNode {
public:
	CBossMonsterFallowHero(CBossMonster* object, std::string name) : object(object) {
		SetName(name);
	}
	~CBossMonsterFallowHero() { }

private:
	CBossMonster* object;

public:
	virtual BehaviorTreeState Run();
};

// ���ΰ� ���� ����
class CSetBossAttackPattern : public CLeafNode {
public:
	CSetBossAttackPattern(CBossMonster* object, std::string name) : object(object) {
		SetName(name);
	}
	~CSetBossAttackPattern() { }

private:
	CBossMonster* object;
	float m_fWaitTime = 1.0f;

	// ���� ���� ��
	short m_sContinuousAttackTime = 0;

public:
	virtual BehaviorTreeState Run();
};

// ������ ���� ����
class CBossChangePhase : public CLeafNode {
public:
	CBossChangePhase(CBossMonster* object, std::string name) : object(object) {
		SetName(name);
	}
	~CBossChangePhase() { }

private:
	CBossMonster* object;

public:
	virtual BehaviorTreeState Run();
};

// ���� ����
// ���� ����1 : ���ΰ� �������� ������ �� �����Ͽ� ��� �������� ����� ���� 
class CPlayBossAttackPattern1 : public CLeafNode {
public:
	CPlayBossAttackPattern1(CBossMonster* object, std::string name) : object(object) {
		SetName(name);
	}
	~CPlayBossAttackPattern1() { }

private:
	CBossMonster* object = NULL;
	bool m_bIsAddForce = false;
	bool m_bIsSpawnedDamageZone = false;
	bool m_bIsOnAir = false;
	float m_fSize = 1.0f;
	float m_fWaitTime = 0.0f;

	CDamageZone* m_pDamageZone = NULL;

public:
	virtual BehaviorTreeState Run();
};

// ���� ����2 : ���ΰ� �������� �����Ͽ� ������ �ڸ��� ������� ��
class CPlayBossAttackPattern2 : public CLeafNode {
public:
	CPlayBossAttackPattern2(CBossMonster* object, std::string name) : object(object) {
		SetName(name);
	}
	~CPlayBossAttackPattern2() { }

private:
	CBossMonster* object = NULL;
	glm::vec3 m_vHeroDirection = glm::vec3(0.0f);
	bool m_bIsSpawnedDamageZone = false;
	float m_fWaitTime = 1.0f;

	CDamageZone* m_pDamageZone = NULL;

public:
	virtual BehaviorTreeState Run();
};

// ���� ����3 : �Ӹ� 3���� ���� ����
class CPlayBossAttackPattern3 : public CLeafNode {
public:
	CPlayBossAttackPattern3(CBossMonster* object, std::string name) : object(object) {
		SetName(name);
	}
	~CPlayBossAttackPattern3() { }

private:
	CBossMonster* object = NULL;
	bool m_bIsSpawnedDamageZone = false;
	float m_fWaitTime = 0.5f;

	CDamageZone* m_pDamageZone1 = NULL;
	CDamageZone* m_pDamageZone2 = NULL;
	CDamageZone* m_pDamageZone3 = NULL;

public:
	virtual BehaviorTreeState Run();
};

// ���� ����4 : 2�� ����(���ΰ� ��ġ�� ���� ����)
class CPlayBossAttackPattern4 : public CLeafNode {
public:
	CPlayBossAttackPattern4(CBossMonster* object, std::string name) : object(object) {
		SetName(name);
	}
	~CPlayBossAttackPattern4() { }

private:
	CBossMonster* object = NULL;
	bool m_bIsRightEnd = false;
	bool m_bIsLeftEnd = false;
	bool m_bIsSpawnedDamageZone = false;

	CDamageZone* m_pDamageZone = NULL;

public:
	virtual BehaviorTreeState Run();
};

// ���� ����5 : ���� �� ����
class CPlayBossAttackPattern5 : public CLeafNode {
public:
	CPlayBossAttackPattern5(CBossMonster* object, std::string name) : object(object) {
		SetName(name);
	}
	~CPlayBossAttackPattern5() { }

private:
	CBossMonster* object = NULL;
	bool m_bIsSpawnedDamageZone = false;

	CDamageZone* m_pDamageZone = NULL;

public:
	virtual BehaviorTreeState Run();
};

// ���� ����6 : �ٷ� ���� ����
class CPlayBossAttackPattern6 : public CLeafNode {
public:
	CPlayBossAttackPattern6(CBossMonster* object, std::string name) : object(object) {
		SetName(name);
	}
	~CPlayBossAttackPattern6() { }

private:
	CBossMonster* object = NULL;
	bool m_bIsSpawnedDamageZone = false;
	float m_fWaitTime = 0.5f;

	CDamageZone* m_pDamageZone = NULL;

public:
	virtual BehaviorTreeState Run();
};

// ���� ����7 : ����(���ΰ� ��ġ�� ���� ����)
class CPlayBossAttackPattern7 : public CLeafNode {
public:
	CPlayBossAttackPattern7(CBossMonster* object, std::string name) : object(object) {
		SetName(name);
	}
	~CPlayBossAttackPattern7() { }

private:
	CBossMonster* object = NULL;
	bool m_bIsSpawnedDamageZone = false;
	float m_fWaitTime = 0.5f;

	CDamageZone* m_pDamageZone = NULL;

public:
	virtual BehaviorTreeState Run();
};
