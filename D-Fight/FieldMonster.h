#pragma once
#pragma once
#include "GameObject.h"
#include "BehaviorTree.h"
#include "ObjectManager.h"
#include "DamageZone.h"
#include <string>

class CFieldMonster : public CGameObject {
public:
	CFieldMonster();
	~CFieldMonster();

private:
	CBehaviorTree* m_pBehaviorTree = NULL;

	int m_iAreaNumber = 4;

	// ���� ��ġ �� ����
	float m_fAngle = 0.0f;
	float m_fScoutRangeStart = 0.0f;
	float m_fScoutRangeEnd = 360.0f;

	float m_fAngleSpeed = 10.0f;

	// ���� ����
	bool m_bIsClockwise = true;

	// ���� ��� ��Ż ����
	bool m_bIsDeviatedTrack = true;

	// ���� ����
	short m_sHp = 100;
	short m_sMaxHp = 100;

	bool m_bIsRigidity = false;
	// ���� �ð�
	float m_fRigidityTime = 1.0f;

	bool m_bIsDead = false;
	float m_fRespawnTime = DEFAULT_RESPAWN_TIME;

	float m_fSpeed = 25.0f;
	
	float m_fDamageCoolTime = 0.0f;

	float m_fPlayDefaultSoundInterval = DEFAULT_SOUND_PLAY_INTERVAL;

public:
	virtual void Update(float elapsedTimeInSec, GameUpdateParameters parameters);
	virtual void Render(CRenderer* renderer);

	// ���� hp �� ������
	void RenderHPBarOnTop(CRenderer* renderer);

	float GetAngle() const;
	void SetAngle(float angle);

	float GetAngleSpeed() const;
	void SetAngleSpeed(float speed);

	int GetAreaNumber() const;
	void SetAreaNumbe(int number);

	bool GiveDamageToHp(short damage);

	bool GetIsDeviatedTrack() const;
	void SetIsDeviatedTrack(bool deviate);

	bool GetIsClockwise() const;
	void SetIsClockwise(bool clockwise);

	void GetScoutRange(float* start, float* end);
	void SetScoutRange(float start, float end);

	// ���� ����
	void SetRigidity(bool rigidity);
	bool GetIsRigidity() const;

	void SetIsDead(bool dead);

	void SetBehaviorTree(CBehaviorTree* behaviorTree);
	CBehaviorTree* GetBehaviorTree() const;

	void SetSpeed(float speed);
	float GetSpeed() const;

	bool IsCanAppliedDamage();

private:
	virtual void BuildBehaviorTree();
};

// ���ΰ� ���� ���� ����
class CFieldMonsterCheckIsInScoutRange : public CLeafNode {
public:
	CFieldMonsterCheckIsInScoutRange(CFieldMonster* object, std::string name) : object(object) {
		SetName(name);
	}
	~CFieldMonsterCheckIsInScoutRange() { }

private:
	CFieldMonster* object;
	float m_NofallowHeroTime = 0.0f;

public:
	virtual BehaviorTreeState Run();
};

// ���ΰ� ���� ����
class CFieldMonsterFindHero : public CLeafNode {
public:
	CFieldMonsterFindHero(CFieldMonster* object, std::string name) : object(object) {
		SetName(name);
	}
	~CFieldMonsterFindHero() { }

private:
	CFieldMonster* object;

public:
	virtual BehaviorTreeState Run();
};

class CFieldMonsterFallowHero : public CLeafNode {
public:
	CFieldMonsterFallowHero(CFieldMonster* object, std::string name) : object(object) {
		SetName(name);
	}
	~CFieldMonsterFallowHero() { }

private:
	CFieldMonster* object;

public:
	virtual BehaviorTreeState Run();
};

class CFieldMonsterSetWatchPositionToBack : public CLeafNode {
public:
	CFieldMonsterSetWatchPositionToBack(CFieldMonster* object, std::string name) : object(object) {
		SetName(name);
	}
	~CFieldMonsterSetWatchPositionToBack() { }

private:
	CFieldMonster* object;

public:
	virtual BehaviorTreeState Run();
};

// ���� ���� ���� ����
class CFieldMonsterMoveToWatchPosition : public CLeafNode {
public:
	CFieldMonsterMoveToWatchPosition(CFieldMonster* object, std::string name) : object(object) {
		SetName(name);
	}
	~CFieldMonsterMoveToWatchPosition() { }

private:
	CFieldMonster* object;

public:
	virtual BehaviorTreeState Run();
};

class CFieldMonsterScoutWorld : public CLeafNode {
public:
	CFieldMonsterScoutWorld(CFieldMonster* object, std::string name) : object(object) {
		SetName(name);
	}
	~CFieldMonsterScoutWorld() { }

private:
	CFieldMonster* object;

public:
	virtual BehaviorTreeState Run();
};
