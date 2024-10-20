#include "stdafx.h"
#include "TestObject.h"

CTestObject::CTestObject()
{
	CTestObject::BuildBehaviorTree();
}

CTestObject::~CTestObject()
{
	delete m_pBehaviorTree;
}

void CTestObject::Update(float elapsedTimeInSec, GameUpdateParameters parameters)
{
	m_pBehaviorTree->Run();
	CGameObject::Update(elapsedTimeInSec, parameters);
}

void CTestObject::BuildBehaviorTree()
{
	CFindHero* findHero = new CFindHero(this, "Find Hero");
	FallowHero* fallowoHero = new FallowHero(this, "Fallow Hero");
	CSequenceNode* fsequence = new CSequenceNode("Find and Fallow Hero Sequence");

	fsequence->AddChild(findHero);
	fsequence->AddChild(fallowoHero);

	BackToWatchPosition* backToWatchPosition = new BackToWatchPosition(this, "Back To Watch Position");
	ScoutWorld* scoutWorld = new ScoutWorld(this, "Scout World");
	CSequenceNode* ssequence = new CSequenceNode("Scout World Sequence");

	ssequence->AddChild(backToWatchPosition);
	ssequence->AddChild(scoutWorld);

	CSelectorNode* selector = new CSelectorNode("Selector");

	selector->AddChild(fsequence);
	selector->AddChild(ssequence);

	m_pBehaviorTree = new CBehaviorTree("test object ai", selector);
}
