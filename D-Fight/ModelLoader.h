#pragma once
#include "Globals.h"
#include <vector>
#include <map>

struct SVertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;

	glm::vec3 tangent;
	glm::vec3 bitangent;

	int boneIDs[MAX_BONE_INFLUENCE_SIZE];
	float weights[MAX_BONE_INFLUENCE_SIZE];
};

struct SAssimpAnimationModel {
	GLuint vao;
	GLuint vbo;

	GLuint iNumberOfVertices;

	std::vector<SVertex> vertices;
};


struct SBoneInfo
{
	int id;
	glm::mat4 offset;
};

class CModelLoader {
public:
	CModelLoader(const char* fileName);
	~CModelLoader();

private:
	std::vector<SAssimpAnimationModel> m_Models;
	std::map<std::string, SBoneInfo> m_BoneInfoMap;

	int m_iBoneCounter = 0;

public:
	std::vector<SAssimpAnimationModel> GetModels() const;

	std::map<std::string, SBoneInfo> GetBoneInfoMap() const;
	int GetBoneCounter() const;

private:
	void ProcessNode(aiNode* node, const aiScene* scene);
	SAssimpAnimationModel ProcessMesh(aiMesh* mesh, const aiScene* scene);

	void ExtractBoneWeightForVertices(std::vector<SVertex>& vertices, aiMesh* mesh, const aiScene* scene);

	void CalculateTangentAndBitangent(std::vector<SVertex>& vertices);

	// VAO ¼³Á¤
	void SetVertexAttributeWithElementBufferObject(GLuint* vao, GLuint* vbo,
		std::vector<SVertex>& vertices, std::vector<unsigned int>& indices);
};
