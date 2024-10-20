#pragma once
#include <functional>
#include <vector>
#include <string>

enum BehaviorTreeState {
	BT_FAILURE,
	BT_RUNNING,
	BT_SUCCESS
};

class CNode;

// 블랙 보드(각 오브젝트에 공유할 데이터 저장)
class CBlackBoard {
private:
	CBlackBoard() { }
	~CBlackBoard() { }

public:
	// 싱글톤 패턴 구현
	static CBlackBoard* GetInstance() {
		static CBlackBoard instance;
		return &instance;
	}

private:
	glm::vec3 m_vHeroPosition;
	float m_fElapsedTimeInSec;

public:
	void SetHeroPosition(glm::vec3 pos) { m_vHeroPosition = pos; }
	glm::vec3 GetHeroPosition() const { return m_vHeroPosition; }

	void SetElapsedTimeInSec(float time) { m_fElapsedTimeInSec = time; }
	float GetElapsedTimeInSec() const { return m_fElapsedTimeInSec; }
};

class CBehaviorTree {
public:
	CBehaviorTree(std::string name, CNode* root);
	~CBehaviorTree() { }

private:
	CNode* m_pRoot;
	std::string m_name;

public:
	void Run();
};

class CNode {
public:
	CNode() { }
	~CNode() { }

private:

public:
	virtual BehaviorTreeState Run() = 0;

	virtual void AddChild(CNode* node) {
		// 
	}
};

// 셀렉터 노드(어떤 자식 노드이 성공할 경우 성공 리턴)
class CSelectorNode : public CNode {
public:
	CSelectorNode(std::string name);
	~CSelectorNode() { }

private:
	std::vector<CNode*> m_childrenVector;
	int m_iPrevRunningPos = 0;
	std::string m_name;

public:
	virtual BehaviorTreeState Run();
	virtual void AddChild(CNode* node);
};

// 시퀀스 노드(모든 자식 노드가 성공할 경우 성공 리턴)
class CSequenceNode : public CNode {
public:
	CSequenceNode(std::string name);
	~CSequenceNode() { }

private:
	std::vector<CNode*> m_childrenVector;
	int m_iPrevRunningPos = 0;
	std::string m_name;

public:
	virtual BehaviorTreeState Run();
	virtual void AddChild(CNode* node);
};

// 리프 노드
class CLeafNode : public CNode {
public:
	CLeafNode() { m_backBoard = CBlackBoard::GetInstance(); }
	~CLeafNode() { }

private:
	std::string m_name;
	CBlackBoard* m_backBoard = NULL;

public:
	void SetName(std::string name) { m_name = name; }
	std::string GetName() const { return m_name; }
	CBlackBoard* BackBoard() { return m_backBoard; }

	virtual BehaviorTreeState Run() = 0;
};
