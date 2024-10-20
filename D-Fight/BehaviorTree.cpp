#include "BehaviorTree.h"
#include "stdafx.h"
#include "BehaviorTree.h"

CBehaviorTree::CBehaviorTree(std::string name, CNode* root)
{
	m_pRoot = std::move(root);
	m_name = name;
}

void CBehaviorTree::Run()
{
	//std::cout << "행동트리: " << m_name << " 시작\n";
	m_pRoot->Run();
	//std::cout << "행동트리: " << m_name << " 끝\n";
}

CSelectorNode::CSelectorNode(std::string name)
{
	m_name = name;
}


void CSelectorNode::AddChild(CNode* node)
{
	m_childrenVector.push_back(std::move(node));
}

BehaviorTreeState CSelectorNode::Run()
{
	//std::cout << "- 셀렉터 노드: " << m_name << " 실행\n";

	for (int i = m_iPrevRunningPos; i != m_childrenVector.size(); ++i) 
	{
		BehaviorTreeState state = m_childrenVector.at(i)->Run();
		if (state == BT_RUNNING)
		{
			m_iPrevRunningPos = i;
			//std::cout << "- 셀렉터 노드: " << m_name << " 러닝\n";
			return BT_RUNNING;
		}
		else if (state == BT_SUCCESS)
		{
			m_iPrevRunningPos = 0;
			//std::cout << "- 셀렉터 노드: " << m_name << " 성공\n";
			return BT_SUCCESS;
		}
	}
	m_iPrevRunningPos = 0;
	//std::cout << "- 셀렉터 노드: " << m_name << " 실패\n";
	return BT_FAILURE;
}


CSequenceNode::CSequenceNode(std::string name)
{
	m_name = name;
}


void CSequenceNode::AddChild(CNode* node)
{
	m_childrenVector.push_back(std::move(node));
}

BehaviorTreeState CSequenceNode::Run()
{
	//std::cout << "- 시퀀스 노드: " << m_name << " 실행\n";

	for (int i = m_iPrevRunningPos; i != m_childrenVector.size(); ++i)
	{
		BehaviorTreeState state = m_childrenVector.at(i)->Run();
		if (state == BT_RUNNING)
		{
			m_iPrevRunningPos = i;
			//std::cout << "- 시퀀스 노드: " << m_name << " 러닝\n";
			return BT_RUNNING;
		}
		else if (state == BT_FAILURE)
		{
			m_iPrevRunningPos = 0;
			//std::cout << "- 시퀀스 노드: " << m_name << " 실패\n";
			return BT_FAILURE;
		}
	}
	m_iPrevRunningPos = 0;
	//std::cout << "- 시퀀스 노드: " << m_name << " 성공\n";
	return BT_SUCCESS;
}
