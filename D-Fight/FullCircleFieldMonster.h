#pragma once
#include "FieldMonster.h"

class CFullCircleFieldMonster : public CFieldMonster {
public:
	CFullCircleFieldMonster();
	~CFullCircleFieldMonster();

private:

public:
	
private:
	virtual void BuildBehaviorTree();
};

// 원형 월드 순찰 관련
class CFullCircleFieldMonsterMoveToWatchPosition : public CLeafNode {
public:
	CFullCircleFieldMonsterMoveToWatchPosition(CFieldMonster* object, std::string name) : object(object) {
		SetName(name);
	}
	~CFullCircleFieldMonsterMoveToWatchPosition() { }

private:
	CFieldMonster* object;

public:
	virtual BehaviorTreeState Run();
};

class CFullCircleFieldMonsterScoutWorld : public CLeafNode {
public:
	CFullCircleFieldMonsterScoutWorld(CFieldMonster* object, std::string name) : object(object) {
		SetName(name);
	}
	~CFullCircleFieldMonsterScoutWorld() { }

private:
	CFieldMonster* object;

public:
	virtual BehaviorTreeState Run();
};
