#include "stdafx.h"
#include "AnimationLoader.h"

CBone::CBone(const std::string& name, int id, const aiNodeAnim* channel)
{
	m_name = name;
	m_id = id;
	m_mLocalTransform = glm::mat4(1.0f);

	for (unsigned int i = 0; i < channel->mNumPositionKeys; ++i) {
		SKeyVector3Data data;
		data.timeStamp = (float)channel->mPositionKeys[i].mTime;
		data.v3Data.x = channel->mPositionKeys[i].mValue.x;
		data.v3Data.y = channel->mPositionKeys[i].mValue.y;
		data.v3Data.z = channel->mPositionKeys[i].mValue.z;

		m_Positions.push_back(data);
	}
	for (unsigned int i = 0; i < channel->mNumRotationKeys; ++i) {
		SKeyQuatData data;
		data.timeStamp = (float)channel->mRotationKeys[i].mTime;
		data.qData.x = channel->mRotationKeys[i].mValue.x;
		data.qData.y = channel->mRotationKeys[i].mValue.y;
		data.qData.z = channel->mRotationKeys[i].mValue.z;
		data.qData.w = channel->mRotationKeys[i].mValue.w;

		m_Rotations.push_back(data);
	}
	for (unsigned int i = 0; i < channel->mNumScalingKeys; ++i) {
		SKeyVector3Data data;
		data.timeStamp = (float)channel->mScalingKeys[i].mTime;
		data.v3Data.x = channel->mScalingKeys[i].mValue.x;
		data.v3Data.y = channel->mScalingKeys[i].mValue.y;
		data.v3Data.z = channel->mScalingKeys[i].mValue.z;

		m_Scales.push_back(data);
	}
}

CBone::~CBone()
{
}

void CBone::Update(float animationTime)
{
	m_mLocalTransform = 
		InterpolatePosition(animationTime) 
		* InterpolateRotation(animationTime) 
		* InterpolateScale(animationTime);
}

glm::mat4 CBone::GetLocalTransform() const
{
	return m_mLocalTransform;
}

std::string CBone::GetName() const
{
	return m_name;
}

int CBone::GetId() const
{
	return m_id;
}

int CBone::GetPositionIndex(float animationTime)
{
	return GetKeyVector3DataIndexWithTime(m_Positions, animationTime);
}

int CBone::GetRotationIndex(float animationTime)
{
	return GetKeyQuatDataIndexWithTime(m_Rotations, animationTime);
}

int CBone::GetScaleIndex(float animationTime)
{
	return GetKeyVector3DataIndexWithTime(m_Scales, animationTime);
}

float CBone::GetSacleFactor(float animationTime, float lastTimeStamp, float nextTimeStamp)
{
	float midWayLength = animationTime - lastTimeStamp;
	float framesDiff = nextTimeStamp - lastTimeStamp;

	return midWayLength / framesDiff;
}

glm::mat4 CBone::InterpolatePosition(float animationTime)
{
	if (m_Positions.size() == 1) return glm::translate(glm::mat4(1.0f), m_Positions[0].v3Data);

	int index = GetPositionIndex(animationTime);
	float scaleFactor = GetSacleFactor(animationTime,
		m_Positions[index].timeStamp, 
		m_Positions[index + 1].timeStamp);
	
	glm::vec3 final = glm::mix(m_Positions[index].v3Data, m_Positions[index + 1].v3Data, scaleFactor);

	return glm::translate(glm::mat4(1.0f), final);
}

glm::mat4 CBone::InterpolateRotation(float animationTime)
{
	if (m_Rotations.size() == 1) return glm::toMat4(glm::normalize(m_Rotations[0].qData));

	int index = GetRotationIndex(animationTime);
	float scaleFactor = GetSacleFactor(animationTime,
		m_Rotations[index].timeStamp,
		m_Rotations[index + 1].timeStamp);

	glm::quat final = glm::slerp(m_Rotations[index].qData, m_Rotations[index + 1].qData, scaleFactor);

	return glm::toMat4(final);
}

glm::mat4 CBone::InterpolateScale(float animationTime)
{
	if (m_Scales.size() == 1) return glm::scale(glm::mat4(1.0f), m_Scales[0].v3Data);

	int index = GetScaleIndex(animationTime);
	float scaleFactor = GetSacleFactor(animationTime,
		m_Scales[index].timeStamp,
		m_Scales[index + 1].timeStamp);

	glm::vec3 final = glm::mix(m_Scales[index].v3Data, m_Scales[index + 1].v3Data, scaleFactor);

	return glm::scale(glm::mat4(1.0f), final);
}

int CBone::GetKeyVector3DataIndexWithTime(std::vector<SKeyVector3Data>& data, float time)
{
	for (int i = 0; i < data.size() - 1; ++i) {
		if (time < data[i + 1].timeStamp) {
			return i;
		}
	}

	//std::cout << " ERROR CBone에서 최대 시간을 넘는 시간이 넘어왔거나 정보가 없음" << "(" << time << ")" << ".\n";
	return 0;
}

int CBone::GetKeyQuatDataIndexWithTime(std::vector<SKeyQuatData>& data, float time)
{
	for (int i = 0; i < data.size() - 1; ++i) {
		if (time < data[i + 1].timeStamp) {
			return i;
		}
	}

	//std::cout << " ERROR CBone에서 최대 시간을 넘는 시간이 넘어왔거나 정보가 없음" << "(" << time << ")" << ".\n";
	return 0;
}

CAnimationLoader::CAnimationLoader(const char* fileName, CModelLoader* model)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(fileName, aiProcess_Triangulate);

	// 정상적으로 불러왔는지 확인
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "Assimp 파일 로드 실패: " << importer.GetErrorString() << std::endl;
		return;
	}

	aiAnimation* animation = scene->mAnimations[0];
	m_fDuration = (float)animation->mDuration;
	m_iTicksPerSecond = (int)animation->mTicksPerSecond;
	ReadNodeChildrenData(m_RootNode, scene->mRootNode);
	ReadMissBones(animation, *model);
}

CAnimationLoader::~CAnimationLoader()
{

}

CBone* CAnimationLoader::FindBone(const std::string name)
{
	auto it = std::find_if(m_Bones.begin(), m_Bones.end(), [&](const CBone& bone) {
		return bone.GetName() == name;
	});
	if (it == m_Bones.end()) return NULL;
	else return &(*it);
}

int CAnimationLoader::GetTicksPerSecond() const
{
	return m_iTicksPerSecond;
}

float CAnimationLoader::GetDuration() const
{
	return m_fDuration;
}

SAssimpNodeData& CAnimationLoader::GetRootNode() 
{
	return m_RootNode;
}

std::map<std::string, SBoneInfo> CAnimationLoader::GetBoneInfoMap()
{
	return m_BoneInfoMap;
}

void CAnimationLoader::ReadNodeChildrenData(SAssimpNodeData& data, const aiNode* node)
{
	data.name = node->mName.data;

	glm::mat4 mat;
	aiMatrix4x4 aiMat = node->mTransformation;
	mat[0][0] = aiMat.a1; mat[1][0] = aiMat.a2; mat[2][0] = aiMat.a3; mat[3][0] = aiMat.a4;
	mat[0][1] = aiMat.b1; mat[1][1] = aiMat.b2; mat[2][1] = aiMat.b3; mat[3][1] = aiMat.b4;
	mat[0][2] = aiMat.c1; mat[1][2] = aiMat.c2; mat[2][2] = aiMat.c3; mat[3][2] = aiMat.c4;
	mat[0][3] = aiMat.d1; mat[1][3] = aiMat.d2; mat[2][3] = aiMat.d3; mat[3][3] = aiMat.d4;
	data.transformation = mat;

	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		SAssimpNodeData child;
		ReadNodeChildrenData(child, node->mChildren[i]);
		data.children.push_back(child);
	}
}

void CAnimationLoader::ReadMissBones(const aiAnimation* animation, CModelLoader& model)
{
	int size = animation->mNumChannels;

	std::map<std::string, SBoneInfo>  boneInfoMap = model.GetBoneInfoMap();
	int boneCount = model.GetBoneCounter();

	for (int i = 0; i < size; ++i) {
		aiNodeAnim* channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;

		if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
			boneInfoMap[boneName].id = boneCount;
			boneCount++;
		}
		m_Bones.push_back(CBone(channel->mNodeName.data, 
			boneInfoMap[channel->mNodeName.data].id, channel));
	}
	m_BoneInfoMap = boneInfoMap;
}
