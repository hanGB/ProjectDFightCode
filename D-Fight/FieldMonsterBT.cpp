#include "stdafx.h"
#include "FieldMonster.h"
#include <random>
 
// 행동 트리 빌드

void CFieldMonster::BuildBehaviorTree()
{
	CFieldMonsterFindHero* findHero = new CFieldMonsterFindHero(this, "Find Hero");
	CFieldMonsterFallowHero* fallowoHero = new CFieldMonsterFallowHero(this, "Fallow Hero");
	CFieldMonsterCheckIsInScoutRange* checkIsInScoutRange
		= new CFieldMonsterCheckIsInScoutRange(this, "Check Is In Scout Range");

	CSequenceNode* fsequence = new CSequenceNode("Find and Fallow Hero Sequence");
	fsequence->AddChild(checkIsInScoutRange);
	fsequence->AddChild(findHero);
	fsequence->AddChild(fallowoHero);

	CFieldMonsterSetWatchPositionToBack* setWatchPositionToBack
		= new CFieldMonsterSetWatchPositionToBack(this, "Set Watch Position To Back");
	CFieldMonsterMoveToWatchPosition* moveToWatchPosition
		= new CFieldMonsterMoveToWatchPosition(this, "Move To Watch Position");
	CFieldMonsterScoutWorld* scoutWorld = new CFieldMonsterScoutWorld(this, "Scout World");

	CSequenceNode* ssequence = new CSequenceNode("Scout World Sequence");
	ssequence->AddChild(setWatchPositionToBack);
	ssequence->AddChild(moveToWatchPosition);
	ssequence->AddChild(scoutWorld);


	CSelectorNode* selector = new CSelectorNode("Selector");
	selector->AddChild(fsequence);
	selector->AddChild(ssequence);

	m_pBehaviorTree = new CBehaviorTree("Field monster ai", selector);
}

// CFieldMonsterFindHero -------------------------------------

BehaviorTreeState CFieldMonsterFindHero::Run()
{
	// 필요 정보
	glm::vec3 heroPos = BackBoard()->GetHeroPosition();
	glm::vec3 position = object->GetPosition();
	
	position.y = 0;
	heroPos.y = 0;

	float dis = 10.0f;
	if (object->GetAreaNumber() == 1) {
		dis = 20.0f;
	}

	if (glm::distance(position, heroPos) < dis) {
		object->SetIsDeviatedTrack(true);
		return BT_SUCCESS;
	}

	return BT_FAILURE;
}

// CFieldMonsterFallowHero -------------------------------------

BehaviorTreeState CFieldMonsterFallowHero::Run()
{
	glm::vec3 position = object->GetPosition();
	glm::vec3 heroPos = BackBoard()->GetHeroPosition();

	glm::vec3 dir = glm::normalize(glm::vec3(heroPos.x, 0.0f, heroPos.z)
		- glm::vec3(position.x, 0.0f, position.z));

	GameUpdateParameters param;
	memset(&param, 0, sizeof(param));
	param.force = glm::vec3(dir.x * object->GetSpeed() * object->GetMass(), 0,
		dir.z * object->GetSpeed() * object->GetMass());

	object->CGameObject::Update(BackBoard()->GetElapsedTimeInSec(), param);

	return BT_SUCCESS;
}

// CFieldMonsterMoveToWatchPosition -------------------------------------

BehaviorTreeState CFieldMonsterMoveToWatchPosition::Run()
{
	glm::vec3 position = object->GetPosition();
	float angle = object->GetAngle();
	int areaNumber = object->GetAreaNumber();
	int distanceToCenter = areaNumber * DEFAULT_AREA_WIDTH + DEFAULT_AREA_WIDTH / 2;

	glm::vec3 watchPoint = glm::vec3(glm::cos(glm::radians(angle)) * distanceToCenter,
		position.y, glm::sin(glm::radians(angle)) * distanceToCenter);

	float dis = glm::distance(watchPoint, position);

	glm::vec3 dir = glm::normalize(glm::vec3(watchPoint.x, 0.0f, watchPoint.z)
		- glm::vec3(position.x, 0.0f, position.z));
	
	float speed = 25.f;

	GameUpdateParameters param;
	memset(&param, 0, sizeof(param));
	param.force = glm::vec3(dir.x * speed * object->GetMass(), 0, dir.z * speed * object->GetMass());

	object->CGameObject::Update(BackBoard()->GetElapsedTimeInSec(), param);

	return BT_SUCCESS;
}

// CFieldMonsterScoutWorld -------------------------------------

BehaviorTreeState CFieldMonsterScoutWorld::Run()
{
	glm::vec3 position = object->GetPosition();

	float angle = object->GetAngle();
	if (object->GetIsClockwise()) {
		angle = angle + BackBoard()->GetElapsedTimeInSec() * object->GetAngleSpeed();
	}
	else {
		angle = angle - BackBoard()->GetElapsedTimeInSec() * object->GetAngleSpeed();
	}

	float start, end;
	object->GetScoutRange(&start, &end);


	if (angle < start) {
		angle = start + 1.0f;
		object->SetIsClockwise(true);
	}
	else if (angle > end) {
		angle = end - 1.0f;
		object->SetIsClockwise(false);
	}

	object->SetAngle(angle);

	return BT_SUCCESS;
}

// CFieldMonsterSetWatchPositionToBack -------------------------------------

BehaviorTreeState CFieldMonsterSetWatchPositionToBack::Run()
{
	if (!object->GetIsDeviatedTrack()) {
		return BT_SUCCESS;
	}
	else {
		// 필요 정보
		glm::vec3 position = object->GetPosition();
		int areaNumber = object->GetAreaNumber();
		int distanceToCenter = areaNumber * DEFAULT_AREA_WIDTH + DEFAULT_AREA_WIDTH / 2;

		if (areaNumber == 1) {
			distanceToCenter += DEFAULT_AREA_WIDTH / 8;
		}

		// 최소 거리 계산 변수
		float minDistance = 999999.0f;  // 나올 수 없는 최대값
		float angle = 0;

		float start, end;
		object->GetScoutRange(&start, &end);

		float interval = 10.0f;

		// 원형일 경우(가장 안쪽 트랙일 경우)
		if ((end - start) >= 360.0f) {
			interval = 5.0f;
		}

		for (float i = start; i < end; i += interval) {
			glm::vec3 watchPoint = glm::vec3(glm::cos(glm::radians(i)) * distanceToCenter,
				position.y, glm::sin(glm::radians(i)) * distanceToCenter);

			float distance = glm::distance(glm::vec3(position.x, 0.0f, position.z),
				glm::vec3(watchPoint.x, 0.0f, watchPoint.z));

			if (minDistance > distance) {
				minDistance = distance;
				angle = i;
			}
		}

		object->SetAngle(angle);

		object->SetIsDeviatedTrack(false);
		return BT_FAILURE;
	}
}

// CFieldMonsterCheckIsInScoutRange -------------------------------------

BehaviorTreeState CFieldMonsterCheckIsInScoutRange::Run()
{
	if (m_NofallowHeroTime <= 0.0f) {
		glm::vec3 position = object->GetPosition();
		int areaNumber = object->GetAreaNumber();
		int distanceToCenter = areaNumber * DEFAULT_AREA_WIDTH + DEFAULT_AREA_WIDTH / 2;

		if (areaNumber == 1) {
			distanceToCenter += DEFAULT_AREA_WIDTH / 8;
		}

		// 돌아갈 포인트의 최대 거리를 넘을 경우 추적 포기
		float maxDistance = 10.0f;
		float angle = 0;

		float start, end;
		object->GetScoutRange(&start, &end);

		bool isNear = false;
		for (float i = start; i < end; i += 5.0f) {
			glm::vec3 watchPoint = glm::vec3(glm::cos(glm::radians(i)) * distanceToCenter,
				position.y, glm::sin(glm::radians(i)) * distanceToCenter);

			float distance = glm::distance(glm::vec3(position.x, 0.0f, position.z),
				glm::vec3(watchPoint.x, 0.0f, watchPoint.z));

			if (distance < maxDistance) {
				isNear = true;
				break;
			}
		}
		if (!isNear) {
			object->SetIsDeviatedTrack(true);
			m_NofallowHeroTime = 2.0f;
			return BT_FAILURE;
		}
		else {
			return BT_SUCCESS;
		}
	}
	else {
		m_NofallowHeroTime -= BackBoard()->GetElapsedTimeInSec();
		return BT_FAILURE;
	}
}
