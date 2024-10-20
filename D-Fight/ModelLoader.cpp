#include "stdafx.h"
#include "ModelLoader.h"

CModelLoader::CModelLoader(const char* fileName)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(fileName, aiProcess_Triangulate);

	// 정상적으로 불러왔는지 확인
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "Assimp 파일 로드 실패: " << importer.GetErrorString() << std::endl;
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

	// 정점 데이터 저장
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
		SVertex vertex;

		// 뼈대 정보 초기화
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

	// 탄젠트 공간 노멀 계산
	CalculateTangentAndBitangent(vertices);

	// 인덱싱 데이터 저장
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
			
			// assimp 행렬을 glm 행렬로 변환(행우선 -> 열우선)
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
			// 이미 있을 경우 아이디만 저장
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

	// vao와 vbo 객체 생성
	glGenVertexArrays(1, vao);
	glGenBuffers(1, vbo);
	glGenBuffers(1, &ebo);

	// 사용할 vao 바인딩
	glBindVertexArray(*vao);

	// 버텍스 데이터 저장을 위한 vbo 바인딩
	glBindBuffer(GL_ARRAY_BUFFER, *vbo);
	// 버텍스 데이터 입력
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SVertex), NULL, GL_DYNAMIC_DRAW);

	// 현재 바인딩되어있는 VBO를 쉐이더 프로그램과 연결: 0번째 attribute
	glVertexAttribPointer(
		0,					// 0번째 속성, 쉐이더의 레이아웃과 맞추어야 됨
		3,					// 크기
		GL_FLOAT,			// 타입
		GL_FALSE,			// 정규화 여부
		sizeof(SVertex),	// 다음과의 간격
		(void*)0);			// 배열 버퍼의 오프셋

	glEnableVertexAttribArray(0);

	// 현재 바인딩되어있는 VBO를 쉐이더 프로그램과 연결: 1번째 attribute
	glVertexAttribPointer(
		1,					// 1번째 속성, 쉐이더의 레이아웃과 맞추어야 됨
		2,					// 크기
		GL_FLOAT,			// 타입
		GL_FALSE,			// 정규화 여부
		sizeof(SVertex),	// 다음과의 간격
		(void*)offsetof(SVertex, uv));			// 배열 버퍼의 오프셋

	glEnableVertexAttribArray(1);

	// 현재 바인딩되어있는 VBO를 쉐이더 프로그램과 연결: 2번째 attribute
	glVertexAttribPointer(
		2,					// 2번째 속성, 쉐이더의 레이아웃과 맞추어야 됨
		3,					// 크기
		GL_FLOAT,			// 타입
		GL_FALSE,			// 정규화 여부
		sizeof(SVertex),	// 다음과의 간격
		(void*)offsetof(SVertex, normal));			// 배열 버퍼의 오프셋

	glEnableVertexAttribArray(2);

	// 현재 바인딩되어있는 VBO를 쉐이더 프로그램과 연결: 3번째 attribute
	glVertexAttribPointer(
		3,					// 3번째 속성, 쉐이더의 레이아웃과 맞추어야 됨
		3,					// 크기
		GL_FLOAT,			// 타입
		GL_FALSE,			// 정규화 여부
		sizeof(SVertex),	// 다음과의 간격
		(void*)offsetof(SVertex, tangent));			// 배열 버퍼의 오프셋

	glEnableVertexAttribArray(3);

	// 현재 바인딩되어있는 VBO를 쉐이더 프로그램과 연결: 4번째 attribute
	glVertexAttribPointer(
		4,					// 4번째 속성, 쉐이더의 레이아웃과 맞추어야 됨
		3,					// 크기
		GL_FLOAT,			// 타입
		GL_FALSE,			// 정규화 여부
		sizeof(SVertex),	// 다음과의 간격
		(void*)offsetof(SVertex, bitangent));			// 배열 버퍼의 오프셋

	glEnableVertexAttribArray(4);

	// 인덱스 바인드 및 데이터 입력
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &ebo);
}
