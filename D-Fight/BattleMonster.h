#pragma once
#pragma once
#include "GameObject.h"
#include "BehaviorTree.h"
#include "ObjectManager.h"
#include "DamageZone.h"
#include "TurnBattleObject.h"
#include "HeroOBject.h"
#include <string>

struct SBackupAnimationData {
	GameObjectAnimationState state;
	bool isLoop;
	bool isStop;
	float speed;
	float time;
};

class CBattleMonster : public CGameObject {
public:
	CBattleMonster();
	~CBattleMonster();

private:
	CBehaviorTree* m_pBehaviorTree = NULL;
	CTurnBattleObject* m_pTurnBattleObject = NULL;
	CHeroObject* m_pHero = NULL;

	short m_sAttackPattern = 0;
	bool m_bIsAttacking = false;

	CObjectManager* m_pObjectManager = NULL;

	bool m_bIsDead = false;

	float m_fTurnGauge = 0.0f;

	int m_iSelectSkill;

	int m_DustTextureID;		// ���� �� ���� ��ƼŬ�� �ؽ���
	int m_BallTextureID;		// ���Ÿ� �߻�ü ��ƼŬ�� �ؽ���

	short m_AttackPatternList[MAX_MONSTER_ATTACK_PATTERN];

	bool m_bIsSuperArmor = true;
	float m_fSuperArmorRecoverTime = DEFAULT_SUPER_ARMOR_RECOVER_TIME;
	short m_sSuperArmorGauge = DEFAULT_SUPER_ARMOR_SIZE;

	float m_fSpeed = 20.0f;

	SBackupAnimationData m_backupAnimationData;

	float m_fDamageCoolTime = 0.0f;

public:
	virtual void Update(float elapsedTimeInSec, GameUpdateParameters parameters);
	virtual void Render(CRenderer* renderer);

	bool GiveDamageToHp(short damage);

	void SetAttackPattern(short pattern);
	short GetAttackPattern() const;

	void SetObjectManager(CObjectManager* manager);
	CObjectManager* GetObjectManager();

	void SetIsAttacking(bool attack);
	bool GetIsAttacking() const;

	void SetTurnBattleObject(CTurnBattleObject* object);
	CTurnBattleObject* GetTurnBattleObject();

	void InitWithTurnBattleObject();

	void SetIsDead(bool dead);
	bool GetIsDead() const;

	float GetTurnGauge() const;
	void SetTurnGauge(float gauge);

	bool IsGetTurn();
	void AddTurnGauge(float value);

	void UseSkillCameraSetting(CCamera* camera);

	void SetSelectedSkill(int skillIndex);
	int GetSelectedSkill() const;

	void SetDustTextureID(int id);
	int GetDustTextureID() const; 
	
	void SetBallTextureID(int id);
	int GetBallTextureID() const;

	void MatchAnimationWithTurnBattleObject();

	void SetAttackPatternList(short* pattern);
	short* GetAttackPatternList();

	void SetHero(CHeroObject* hero);
	CHeroObject* GetHero();

	void SetBehaviorTree(CBehaviorTree* behaviorTree);
	CBehaviorTree* GetBehaviorTree() const;

	void SetIsSuperArmor(bool armor);
	bool GetIsSuperArmor() const;

	void UpdateSuperArmorGauge(short value);
	void SetSuperArmorGauge(short value);

	void UpdateSuperArmorRecoverTime(float elapsedTimeInSec);

	void SetSpeed(float speed);
	float GetSpeed() const;

	void BackupAnimation();
	void RestoreAnimation();

	bool IsCanAppliedDamage();
	void UpdateDamageCoolTime(float elapsedTimeInSec);

private:
	virtual void BuildBehaviorTree();
};

// ���ΰ� ���� ����
class CBattleMonsterFallowHero : public CLeafNode {
public:
	CBattleMonsterFallowHero(CBattleMonster* object, std::string name) : object(object) {
		SetName(name);
	}
	~CBattleMonsterFallowHero() { }

private:
	CBattleMonster* object;

public:
	virtual BehaviorTreeState Run();
};

// ���ΰ� ���� ����
class CSetAttackPattern : public CLeafNode {
public:
	CSetAttackPattern(CBattleMonster* object, std::string name) : object(object) {
		SetName(name);
	}
	~CSetAttackPattern() { }

private:
	CBattleMonster* object;
	float m_fWaitTime = 1.0f;

	// ���� ���� ��
	short m_sContinuousAttackTime = 0;

public:
	virtual BehaviorTreeState Run();
};

// ���� ����
// ���� ����1 : ���ΰ� �������� ������ �� �����Ͽ� ��� �������� ����� ���� 
class CPlayAttackPattern1 : public CLeafNode {
public:
	CPlayAttackPattern1(CBattleMonster* object, std::string name) : object(object) {
		SetName(name);
	}
	~CPlayAttackPattern1() { }

private:
	CBattleMonster* object = NULL;
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
class CPlayAttackPattern2 : public CLeafNode {
public:
	CPlayAttackPattern2(CBattleMonster* object, std::string name) : object(object) {
		SetName(name);
	}
	~CPlayAttackPattern2() { }

private:
	CBattleMonster* object = NULL;
	glm::vec3 m_vHeroDirection = glm::vec3(0.0f);
	bool m_bIsSpawnedDamageZone = false;
	float m_fWaitTime = 1.0f;

	CDamageZone* m_pDamageZone = NULL;

public:
	virtual BehaviorTreeState Run();
};

// ���� ����3 : �ٷ� ���� ����
class CPlayAttackPattern3 : public CLeafNode {
public:
	CPlayAttackPattern3(CBattleMonster* object, std::string name) : object(object) {
		SetName(name);
	}
	~CPlayAttackPattern3() { }

private:
	CBattleMonster* object = NULL;
	bool m_bIsSpawnedDamageZone = false;
	float m_fWaitTime = 0.5f;

	CDamageZone* m_pDamageZone = NULL;

public:
	virtual BehaviorTreeState Run();
};

// ���� ����4 : ����(���ΰ� ��ġ�� ���� ����)
class CPlayAttackPattern4 : public CLeafNode {
public:
	CPlayAttackPattern4(CBattleMonster* object, std::string name) : object(object) {
		SetName(name);
	}
	~CPlayAttackPattern4() { }

private:
	CBattleMonster* object = NULL;
	bool m_bIsSpawnedDamageZone = false;
	float m_fWaitTime = 0.5f;

	CDamageZone* m_pDamageZone = NULL;

public:
	virtual BehaviorTreeState Run();
};

// ���� ����5 : Ȧ��(���ΰ��� ���� ��� ����)
class CPlayAttackPattern5 : public CLeafNode {
public:
	CPlayAttackPattern5(CBattleMonster* object, std::string name) : object(object) {
		SetName(name);
	}
	~CPlayAttackPattern5() { }

private:
	CBattleMonster* object = NULL;
	glm::vec3 m_vHeroDirection = glm::vec3(0.0f);
	glm::vec3 m_vPrevPosition = glm::vec3(0.0f);
	bool m_bIsSpawnedDamageZone = false;
	float m_fWaitTime = 1.0f;

	bool m_bIsHoldEnd = false;

	CDamageZone* m_pDamageZone = NULL;

public:
	virtual BehaviorTreeState Run();
};
