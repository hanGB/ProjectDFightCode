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
	//std::cout << "�ൿƮ��: " << m_name << " ����\n";
	m_pRoot->Run();
	//std::cout << "�ൿƮ��: " << m_name << " ��\n";
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
	//std::cout << "- ������ ���: " << m_name << " ����\n";

	for (int i = m_iPrevRunningPos; i != m_childrenVector.size(); ++i) 
	{
		BehaviorTreeState state = m_childrenVector.at(i)->Run();
		if (state == BT_RUNNING)
		{
			m_iPrevRunningPos = i;
			//std::cout << "- ������ ���: " << m_name << " ����\n";
			return BT_RUNNING;
		}
		else if (state == BT_SUCCESS)
		{
			m_iPrevRunningPos = 0;
			//std::cout << "- ������ ���: " << m_name << " ����\n";
			return BT_SUCCESS;
		}
	}
	m_iPrevRunningPos = 0;
	//std::cout << "- ������ ���: " << m_name << " ����\n";
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
	//std::cout << "- ������ ���: " << m_name << " ����\n";

	for (int i = m_iPrevRunningPos; i != m_childrenVector.size(); ++i)
	{
		BehaviorTreeState state = m_childrenVector.at(i)->Run();
		if (state == BT_RUNNING)
		{
			m_iPrevRunningPos = i;
			//std::cout << "- ������ ���: " << m_name << " ����\n";
			return BT_RUNNING;
		}
		else if (state == BT_FAILURE)
		{
			m_iPrevRunningPos = 0;
			//std::cout << "- ������ ���: " << m_name << " ����\n";
			return BT_FAILURE;
		}
	}
	m_iPrevRunningPos = 0;
	//std::cout << "- ������ ���: " << m_name << " ����\n";
	return BT_SUCCESS;
}
