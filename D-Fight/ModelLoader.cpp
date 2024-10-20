#include "stdafx.h"
#include "ModelLoader.h"

CModelLoader::CModelLoader(const char* fileName)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(fileName, aiProcess_Triangulate);

	// ���������� �ҷ��Դ��� Ȯ��
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "Assimp ���� �ε� ����: " << importer.GetErrorString() << std::endl;
		return;
	}

	ProcessNode(scene->mRootNode, scene);
}

CModelLoader::~CModelLoader()
{
}

std::vector<SAssimpAnimationModel> CModelLoader::GetModels() const
{
	return m_Models;
}

std::map<std::string, SBoneInfo> CModelLoader::GetBoneInfoMap() const
{
	return m_BoneInfoMap;
}

int CModelLoader::GetBoneCounter() const
{
	return m_iBoneCounter;
}

void CModelLoader::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_Models.push_back(ProcessMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		ProcessNode(node->mChildren[i], scene);
	}
}

SAssimpAnimationModel CModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<SVertex> vertices;
	std::vector<unsigned int> indices;

	// ���� ������ ����
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
		SVertex vertex;

		// ���� ���� �ʱ�ȭ
		for (int i = 0; i < MAX_BONE_INFLUENCE_SIZE; ++i) {
			vertex.boneIDs[i] = -1;
			vertex.weights[i] = 0.0f;
		}

		// position
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		// normal
		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		// uv
		if (mesh->mTextureCoords[0]) {
			vertex.uv.x = mesh->mTextureCoords[0][i].x;
			vertex.uv.y = mesh->mTextureCoords[0][i].y;
		}
		else {
			vertex.uv = glm::vec2(0.0f);
		}

		vertices.push_back(vertex);
	}

	// ź��Ʈ ���� ��� ���
	CalculateTangentAndBitangent(vertices);

	// �ε��� ������ ����
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j) {
			indices.push_back(face.mIndices[j]);
		}
	}

	ExtractBoneWeightForVertices(vertices, mesh, scene);

	SAssimpAnimationModel model;
	SetVertexAttributeWithElementBufferObject(&model.vao, &model.vbo, vertices, indices);
	model.iNumberOfVertices = mesh->mNumVertices;
	model.vertices = vertices;

	return model;
}

void CModelLoader::ExtractBoneWeightForVertices(
	std::vector<SVertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
	for (unsigned int bi = 0; bi < mesh->mNumBones; ++bi) {
		int boneID = -1;
		std::string boneName = mesh->mBones[bi]->mName.C_Str();

		if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end()) {
			SBoneInfo boneInfo;

			boneInfo.id = m_iBoneCounter;
			
			// assimp ����� glm ��ķ� ��ȯ(��켱 -> ���켱)
			glm::mat4 mat;
			aiMatrix4x4 aiMat = mesh->mBones[bi]->mOffsetMatrix;
			mat[0][0] = aiMat.a1; mat[1][0] = aiMat.a2; mat[2][0] = aiMat.a3; mat[3][0] = aiMat.a4;
			mat[0][1] = aiMat.b1; mat[1][1] = aiMat.b2; mat[2][1] = aiMat.b3; mat[3][1] = aiMat.b4;
			mat[0][2] = aiMat.c1; mat[1][2] = aiMat.c2; mat[2][2] = aiMat.c3; mat[3][2] = aiMat.c4;
			mat[0][3] = aiMat.d1; mat[1][3] = aiMat.d2; mat[2][3] = aiMat.d3; mat[3][3] = aiMat.d4;
			boneInfo.offset = mat;

			m_BoneInfoMap[boneName] = boneInfo;
			boneID = m_iBoneCounter;
			m_iBoneCounter++;
		}
		else {
			// �̹� ���� ��� ���̵� ����
			boneID = m_BoneInfoMap[boneName].id;
		}

		aiVertexWeight* weights = mesh->mBones[bi]->mWeights;
		for (unsigned int wi = 0; wi < mesh->mBones[bi]->mNumWeights; ++wi) {
			int vertexId = weights[wi].mVertexId;
			float weight = weights[wi].mWeight;

			for (int i = 0; i < MAX_BONE_INFLUENCE_SIZE; ++i) {
				if (vertices[vertexId].boneIDs[i] < 0) {
					vertices[vertexId].weights[i] = weight;
					vertices[vertexId].boneIDs[i] = boneID;
					break;
				}
			}
		}
	}
}

void CModelLoader::CalculateTangentAndBitangent(std::vector<SVertex>& vertices)
{
	for (int i = 0; i < vertices.size(); i += 3) {
		glm::vec3 deltaPos1 = vertices[i + 1].position - vertices[i].position;
		glm::vec3 deltaPos2 = vertices[i + 2].position - vertices[i].position;
		glm::vec2 deltaUV1 = vertices[i + 1].uv - vertices[i].uv;
		glm::vec2 deltaUV2 = vertices[i + 2].uv - vertices[i].uv;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
		glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

		for (int n = 0; n < 3; ++n) {
			vertices[i + n].tangent = tangent;
		}
		for (int n = 0; n < 3; ++n) {
			vertices[i + n].bitangent = bitangent;
		}
	}

	for (int i = 0; i < vertices.size(); ++i) {
		vertices[i].tangent = glm::normalize(vertices[i].tangent - vertices[i].normal * glm::dot(vertices[i].normal, vertices[i].tangent));
		if (glm::dot(glm::cross(vertices[i].normal, vertices[i].tangent), vertices[i].bitangent) < 0.0f) {
			vertices[i].tangent *= -1.0f;
		}
	}
}

void CModelLoader::SetVertexAttributeWithElementBufferObject(GLuint* vao, GLuint* vbo,
	std::vector<SVertex>& vertices, std::vector<unsigned int>& indices)
{
	GLuint ebo;

	// vao�� vbo ��ü ����
	glGenVertexArrays(1, vao);
	glGenBuffers(1, vbo);
	glGenBuffers(1, &ebo);

	// ����� vao ���ε�
	glBindVertexArray(*vao);

	// ���ؽ� ������ ������ ���� vbo ���ε�
	glBindBuffer(GL_ARRAY_BUFFER, *vbo);
	// ���ؽ� ������ �Է�
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SVertex), NULL, GL_DYNAMIC_DRAW);

	// ���� ���ε��Ǿ��ִ� VBO�� ���̴� ���α׷��� ����: 0��° attribute
	glVertexAttribPointer(
		0,					// 0��° �Ӽ�, ���̴��� ���̾ƿ��� ���߾�� ��
		3,					// ũ��
		GL_FLOAT,			// Ÿ��
		GL_FALSE,			// ����ȭ ����
		sizeof(SVertex),	// �������� ����
		(void*)0);			// �迭 ������ ������

	glEnableVertexAttribArray(0);

	// ���� ���ε��Ǿ��ִ� VBO�� ���̴� ���α׷��� ����: 1��° attribute
	glVertexAttribPointer(
		1,					// 1��° �Ӽ�, ���̴��� ���̾ƿ��� ���߾�� ��
		2,					// ũ��
		GL_FLOAT,			// Ÿ��
		GL_FALSE,			// ����ȭ ����
		sizeof(SVertex),	// �������� ����
		(void*)offsetof(SVertex, uv));			// �迭 ������ ������

	glEnableVertexAttribArray(1);

	// ���� ���ε��Ǿ��ִ� VBO�� ���̴� ���α׷��� ����: 2��° attribute
	glVertexAttribPointer(
		2,					// 2��° �Ӽ�, ���̴��� ���̾ƿ��� ���߾�� ��
		3,					// ũ��
		GL_FLOAT,			// Ÿ��
		GL_FALSE,			// ����ȭ ����
		sizeof(SVertex),	// �������� ����
		(void*)offsetof(SVertex, normal));			// �迭 ������ ������

	glEnableVertexAttribArray(2);

	// ���� ���ε��Ǿ��ִ� VBO�� ���̴� ���α׷��� ����: 3��° attribute
	glVertexAttribPointer(
		3,					// 3��° �Ӽ�, ���̴��� ���̾ƿ��� ���߾�� ��
		3,					// ũ��
		GL_FLOAT,			// Ÿ��
		GL_FALSE,			// ����ȭ ����
		sizeof(SVertex),	// �������� ����
		(void*)offsetof(SVertex, tangent));			// �迭 ������ ������

	glEnableVertexAttribArray(3);

	// ���� ���ε��Ǿ��ִ� VBO�� ���̴� ���α׷��� ����: 4��° attribute
	glVertexAttribPointer(
		4,					// 4��° �Ӽ�, ���̴��� ���̾ƿ��� ���߾�� ��
		3,					// ũ��
		GL_FLOAT,			// Ÿ��
		GL_FALSE,			// ����ȭ ����
		sizeof(SVertex),	// �������� ����
		(void*)offsetof(SVertex, bitangent));			// �迭 ������ ������

	glEnableVertexAttribArray(4);

	// �ε��� ���ε� �� ������ �Է�
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &ebo);
}
