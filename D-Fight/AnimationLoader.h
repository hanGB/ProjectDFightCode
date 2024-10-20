#pragma once
#include <vector>
#include "Globals.h"
#include "ModelLoader.h"

// 뼈대
// 시간에 따른 정점 정보
struct SKeyVector3Data {
	glm::vec3 v3Data;
	float timeStamp;
};

struct SKeyQuatData {
	glm::quat qData;
	float timeStamp;
};

class CBone {
public:
	CBone(const std::string& name, int id, const aiNodeAnim* channel);
	~CBone();

private:
	std::vector<SKeyVector3Data> m_Positions;
	std::vector<SKeyQuatData>	 m_Rotations;
	std::vector<SKeyVector3Data> m_Scales;

	glm::mat4 m_mLocalTransform;
	std::string m_name;
	int m_id;
	
public:
	void Update(float animationTime);

	glm::mat4 GetLocalTransform() const;
	std::string GetName() const;
	int GetId() const;

	int GetPositionIndex(float animationTime);
	int GetRotationIndex(float animationTime);
	int GetScaleIndex(float animationTime);

private:
	float GetSacleFactor(float animationTime, float lastTimeStamp, float nextTimeStamp);
	
	glm::mat4 InterpolatePosition(float animationTime);
	glm::mat4 InterpolateRotation(float animationTime);
	glm::mat4 InterpolateScale(float animationTime);

	int GetKeyVector3DataIndexWithTime(std::vector<SKeyVector3Data>& data, float time);
	int GetKeyQuatDataIndexWithTime(std::vector<SKeyQuatData>& data, float time);
};


// 애니메이션 로더
struct SAssimpNodeData{
	glm::mat4 transformation;
	std::string name;

	std::vector<SAssimpNodeData> children;
};

class CAnimationLoader {
public:
	CAnimationLoader(const char* fileName, CModelLoader* model);
	~CAnimationLoader();

private:
	float m_fDuration;
	int m_iTicksPerSecond;
	std::vector<CBone> m_Bones;
	SAssimpNodeData m_RootNode;
	std::map<std::string, SBoneInfo> m_BoneInfoMap;

public:
	CBone* FindBone(const std::string name);

	int GetTicksPerSecond() const;
	float GetDuration() const;
	SAssimpNodeData& GetRootNode();
	std::map<std::string, SBoneInfo> GetBoneInfoMap();

private:
	void ReadNodeChildrenData(SAssimpNodeData& data, const aiNode* node);
	void ReadMissBones(const aiAnimation* animation, CModelLoader& model);

};