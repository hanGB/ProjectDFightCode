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

	// 정찰 위치 및 범위
	float m_fAngle = 0.0f;
	float m_fScoutRangeStart = 0.0f;
	float m_fScoutRangeEnd = 360.0f;

	float m_fAngleSpeed = 10.0f;

	// 정찰 방향
	bool m_bIsClockwise = true;

	// 정찰 경로 이탈 여부
	bool m_bIsDeviatedTrack = true;

	// 전투 관련
	short m_sHp = 100;
	short m_sMaxHp = 100;

	bool m_bIsRigidity = false;
	// 경직 시간
	float m_fRigidityTime = 1.0f;

	bool m_bIsDead = false;
	float m_fRespawnTime = DEFAULT_RESPAWN_TIME;

	float m_fSpeed = 25.0f;
	
	float m_fDamageCoolTime = 0.0f;

	float m_fPlayDefaultSoundInterval = DEFAULT_SOUND_PLAY_INTERVAL;

public:
	virtual void Update(float elapsedTimeInSec, GameUpdateParameters parameters);
	virtual void Render(CRenderer* renderer);

	// 위에 hp 바 렌더링
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

	// 경직 설정
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

// 주인공 추적 포기 관련
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

// 주인공 추적 관련
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

// 원형 월드 순찰 관련
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
