#pragma once
#include "BattleMonster.h"

// 행동트리를 업데이트 하지 않는(움직이지 않는) CBattleMonster;
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

// 주인공 추적 관련
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

// 주인공 공격 관련
class CSetBossAttackPattern : public CLeafNode {
public:
	CSetBossAttackPattern(CBossMonster* object, std::string name) : object(object) {
		SetName(name);
	}
	~CSetBossAttackPattern() { }

private:
	CBossMonster* object;
	float m_fWaitTime = 1.0f;

	// 연속 공격 수
	short m_sContinuousAttackTime = 0;

public:
	virtual BehaviorTreeState Run();
};

// 페이즈 변경 관련
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

// 공격 패턴
// 공격 패턴1 : 주인공 방향으로 점프한 뒤 착륙하여 모든 방향으로 충격파 전달 
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

// 공격 패턴2 : 주인공 방향으로 돌진하여 지나간 자리에 대미지를 줌
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

// 공격 패턴3 : 머리 3개로 앞을 물기
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

// 공격 패턴4 : 2단 저격(주인공 위치에 원격 공격)
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

// 공격 패턴5 : 정면 빔 공격
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

// 공격 패턴6 : 바로 앞을 공격
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

// 공격 패턴7 : 저격(주인공 위치에 원격 공격)
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
