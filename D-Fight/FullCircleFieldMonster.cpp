#include "stdafx.h"
#include "FullCircleFieldMonster.h"


CFullCircleFieldMonster::CFullCircleFieldMonster()
{
	BuildBehaviorTree();
}

CFullCircleFieldMonster::~CFullCircleFieldMonster()
{
}

void CFullCircleFieldMonster::BuildBehaviorTree()
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
	CFullCircleFieldMonsterMoveToWatchPosition* moveToWatchPosition
		= new CFullCircleFieldMonsterMoveToWatchPosition(this, "Move To Watch Position");
	CFullCircleFieldMonsterScoutWorld* scoutWorld = new CFullCircleFieldMonsterScoutWorld(this, "Scout World");

	CSequenceNode* ssequence = new CSequenceNode("Scout World Sequence");
	ssequence->AddChild(setWatchPositionToBack);
	ssequence->AddChild(moveToWatchPosition);
	ssequence->AddChild(scoutWorld);

	CSelectorNode* selector = new CSelectorNode("Selector");
	selector->AddChild(fsequence);
	selector->AddChild(ssequence);

	SetBehaviorTree(new CBehaviorTree("Field monster ai", selector));
}

BehaviorTreeState CFullCircleFieldMonsterMoveToWatchPosition::Run()
{
	glm::vec3 position = object->GetPosition();
	float angle = object->GetAngle();
	int areaNumber = object->GetAreaNumber();
	int distanceToCenter = areaNumber * DEFAULT_AREA_WIDTH + DEFAULT_AREA_WIDTH / 2;

	if (areaNumber == 1) {
		distanceToCenter += DEFAULT_AREA_WIDTH / 8;
	}

	glm::vec3 watchPoint = glm::vec3(glm::cos(glm::radians(angle)) * distanceToCenter,
		position.y, glm::sin(glm::radians(angle)) * distanceToCenter);

	float dis = glm::distance(watchPoint, position);

	glm::vec3 dir = glm::normalize(glm::vec3(watchPoint.x, 0.0f, watchPoint.z)
		- glm::vec3(position.x, 0.0f, position.z));

	float speed = 30.f;

	GameUpdateParameters param;
	memset(&param, 0, sizeof(param));
	param.force = glm::vec3(dir.x * speed * object->GetMass(), 0, dir.z * speed * object->GetMass());

	object->CGameObject::Update(BackBoard()->GetElapsedTimeInSec(), param);

	glm::vec3 targetPos = glm::vec3(watchPoint.x, 0.0f, watchPoint.z);
	glm::vec3 nowPos = glm::vec3(object->GetPosition().x, 0.0f, object->GetPosition().z);
	
	if (glm::distance(targetPos, nowPos) < 5.0f) return BT_SUCCESS;
	else return BT_FAILURE;
}

BehaviorTreeState CFullCircleFieldMonsterScoutWorld::Run()
{
	glm::vec3 position = object->GetPosition();

	float angle = object->GetAngle();
	if (object->GetIsClockwise()) {
		angle += 0.5f;
	}

	float start, end;
	object->GetScoutRange(&start, &end);

	if (angle > 360.0f)
		angle -= 360.0f;

	object->SetAngle(angle);

	return BT_SUCCESS;
}
