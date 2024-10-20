#pragma once
#include "GameObject.h"
#include "BehaviorTree.h"
#include <string>

class CTestObject : public CGameObject {
public:
	CTestObject();
	~CTestObject();

private:
	CBehaviorTree* m_pBehaviorTree = NULL;
	float m_fAngle = 0;

public:
	virtual void Update(float elapsedTimeInSec, GameUpdateParameters parameters);
	float GetAngle() const {
		return m_fAngle;
	}
	void SetAngle(float angle) {
		m_fAngle = angle;
	}

private:
	void BuildBehaviorTree();
};

class CFindHero : public CLeafNode {
public:
	CFindHero(CTestObject* object, std::string name) : object(object) {
		SetName(name);
	}
	~CFindHero() { }

private:
	CTestObject* object;

public:
	virtual BehaviorTreeState Run() {
		//std::cout << "- 리프 노드: " << GetName() << " 실행\n";

		glm::vec3 position = object->GetPosition();
		glm::vec3 heroPos = BackBoard()->GetHeroPosition();

		if (heroPos.x - 10 < position.x && position.x < heroPos.x + 10)
			if (heroPos.y - 10 < position.y && position.y < heroPos.y + 10)
				if (heroPos.z - 10 < position.z && position.z < heroPos.z + 10)
					return BT_SUCCESS;

		return BT_FAILURE;
	}
};

class FallowHero : public CLeafNode {
public:
	FallowHero(CTestObject* object, std::string name) : object(object) {
		SetName(name);
	}
	~FallowHero() { }

private:
	CTestObject* object;

public:
	virtual BehaviorTreeState Run() {
		//std::cout << "- 리프 노드: " << GetName() << " 실행\n";

		glm::vec3 position = object->GetPosition();
		glm::vec3 heroPos = BackBoard()->GetHeroPosition();

		if (heroPos.x - 0.5 < position.x && position.x < heroPos.x + 0.5)
			if (heroPos.y - 0.5 < position.y && position.y < heroPos.y + 0.5)
				if (heroPos.z - 0.5 < position.z && position.z < heroPos.z + 0.5)
					return BT_SUCCESS;

		glm::vec3 dir = glm::normalize(heroPos - position);

		object->SetPosition(position + BackBoard()->GetElapsedTimeInSec() * dir);
		return BT_SUCCESS;
	}
};

class BackToWatchPosition : public CLeafNode {
public:
	BackToWatchPosition(CTestObject* object, std::string name) : object(object) {
		SetName(name);
	}
	~BackToWatchPosition() { }

private:
	CTestObject* object;

public:
	virtual BehaviorTreeState Run() {
		//std::cout << "- 리프 노드: " << GetName() << " 실행\n";

		glm::vec3 position = object->GetPosition();
		float angle = object->GetAngle();
		glm::vec3 watchPoint = glm::vec3(glm::cos(glm::radians(angle)) * 10,
			position.y, glm::sin(glm::radians(angle)) * 10);

		float dis = glm::distance(watchPoint, position);

		if (dis < 1.0f)
			return BT_SUCCESS;

		glm::vec3 dir = glm::normalize(watchPoint - position);

		object->SetPosition(position + BackBoard()->GetElapsedTimeInSec() * dir);
		return BT_FAILURE;
	}
};

class ScoutWorld : public CLeafNode {
public:
	ScoutWorld(CTestObject* object, std::string name) : object(object) {
		SetName(name);
	}
	~ScoutWorld() { }

private:
	CTestObject* object;
	float m_speed = 40;

public:
	virtual BehaviorTreeState Run() {
		//std::cout << "- 리프 노드: " << GetName() << " 실행\n";

		glm::vec3 position = object->GetPosition();

		float angle = object->GetAngle();

		angle = angle + BackBoard()->GetElapsedTimeInSec() * m_speed;
		if (angle > 360)
			angle = 0;

		object->SetAngle(angle);

		return BT_SUCCESS;
	}
};
