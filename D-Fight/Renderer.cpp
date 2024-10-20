#include "stdafx.h"
#include "Renderer.h"
#include <random>

// PNG 등의 확장자로 된 텍스쳐 로드 라이브러리
#define STB_IMAGE_IMPLEMENTATION
#include "Dependencies/stb_image.h"

CRenderer::CRenderer()
{
	glClearDepth(1.f);

	// 쉐이더 컴파일
	m_ShaderProgramID = 
		LoadShaders("./shader/VertexShader.glsl", "./shader/FragmentShader.glsl");
	m_TextShaderProgramID =
		LoadShaders("./shader/VertexShaderText.glsl", "./shader/FragmentShaderText.glsl");
	m_2dUIShaderProgramID = 
		LoadShaders("./shader/VertexShader2DUI.glsl", "./shader/FragmentShader2DUI.glsl");
	m_BillboardShaderProgramID = 
		LoadShaders("./shader/VertexShaderBillboard.glsl", "./shader/FragmentShaderBillboard.glsl");
	m_BillboardSpriteShaderProgramID = 
		LoadShaders("./shader/VertexShaderBillboard.glsl", "./shader/FragmentShaderBillboardSprite.glsl");
	m_SkyboxShaderProgramID = 
		LoadShaders("./shader/VertexShaderSkybox.glsl", "./shader/FragmentShaderSkybox.glsl");
	m_DepthShaderProgramID = 
		LoadShaders("./shader/VertexShaderDepth.glsl", "./shader/FragmentShaderDepth.glsl");
	m_BoundingBoxShaderProgramID =
		LoadShaders("./shader/VertexShaderBoundingBox.glsl", "./shader/FragmentShaderBoundingBox.glsl");
	m_BorderShaderProgramID =
		LoadShaders("./shader/VertexShaderBorder.glsl", "./shader/FragmentShaderBorder.glsl");
	m_TotalSceneShaderProgramID = 
		LoadShaders("./shader/VertexShaderTotalScene.glsl", "./shader/FragmentShaderTotalScene.glsl");
	m_InstanceShaderProgramID =
		LoadShaders("./shader/VertexShaderInstance.glsl", "./shader/FragmentShaderInstance.glsl");
	m_BillboardTextShaderProgramID =
		LoadShaders("./shader/VertexShaderBillboardText.glsl", "./shader/FragmentShaderBillboardText.glsl");
	m_DisplayFXAAShaderProgramID =
		LoadShaders("./shader/VertexShaderDisplayFXAA.glsl", "./shader/FragmentShaderDisplayFXAA.glsl");

	m_BasicMovingParticleProgramID = 
		LoadShaders("./shader/VertexShaderBasicMovingParticle.glsl", "./shader/FragmentShaderBasicMovingParticle.glsl");
	m_ChangeStateEffectShaderProgramID 
		= LoadShaders("./shader/VertexShaderChangeStateEffect.glsl", "./shader/FragmentShaderChangeStateEffect.glsl");
	m_DamageZoneShaderProgramID
		= LoadShaders("./shader/VertexShaderForDamageZone.glsl", "./shader/FragmentShaderForDamageZone.glsl");
	m_WaterShaderProgramID
		= LoadShaders("./shader/VertexShaderForWater.glsl", "./shader/FragmentShaderForWater.glsl");
	m_GravityParticleShaderProgramID
		= LoadShaders(
			"./shader/VertexShaderMovingWithGravityParticle.glsl", 
			"./shader/FragmentShaderMovingWithGravityParticle.glsl");
	m_BlurShaderProgramID = LoadShaders("./shader/VertexShaderBlur.glsl", "./shader/FragmentShaderBlur.glsl");

	// 2D UI 렌더링을 위한 사각형 정점 설정
	SetVertexAttributeFor2DRectangle();
	// 스카이박스 렌더링을 위한 사각형 정점 설정
	SetVertexAttributeForSkybox();
	// 화면에 텍스쳐 렌더링용 사각형 정점 설정
	SetDisplayRect();

	LoadFont("./resource/font/Eulyoo1945-SemiBold.ttf", DEFAULT_FONT_SIZE);

	// 빌보드용 평면 불러오기
	int planeID = LoadOBJModel("./resource/obj/plane.obj");
	m_PlaneModelVAO = GetModel(planeID)->VAO;
	m_ModelMap.erase(planeID);

	m_iBoundingBoxModelIndex = LoadOBJModel("./resource/obj/box.obj");
	m_iCapsuleModelIndex = LoadOBJModel("./resource/obj/capsule.obj");

	// 깊이 맵 생성
	CreateDepthMap();

	// 디스플레이 프레임버퍼 생성
	CreateSceneFBOAndTexture();
	CreateSaveRenderingResultFBOAndTexture();
	CreateDisplayWithFXAAFBOAndTexture();
	CreateBlurFBOAndTexture();

	// 유니폼 로케이션 저장
	SaveGettingUniformLocation();

	// 파티클 vao
	SetVAOForChangeSkillDeckParticle();

	// 기본 기능 설정
	// 투명도
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// 컬링
	glEnable(GL_CULL_FACE);
	// 깊이 테스트
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);
	// 스텐실 테스트
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
}

CRenderer::~CRenderer()
{
	// 쉐이더 삭제
	glDeleteProgram(m_ShaderProgramID);
	glDeleteShader(m_TextShaderProgramID);
	glDeleteShader(m_2dUIShaderProgramID);
	glDeleteShader(m_BillboardShaderProgramID);
	glDeleteShader(m_BillboardSpriteShaderProgramID);
	glDeleteShader(m_SkyboxShaderProgramID);
	glDeleteShader(m_DepthShaderProgramID);
	glDeleteShader(m_BoundingBoxShaderProgramID);
	glDeleteShader(m_BorderShaderProgramID);
	glDeleteShader(m_TotalSceneShaderProgramID);
	glDeleteShader(m_ChangeStateEffectShaderProgramID);
}

CRenderer* CRenderer::GetInstance()
{
	static CRenderer instance;
	return &instance;
}

int CRenderer::LoadOBJModel(const char* fileName)
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	bool result = ReadOBJFile(fileName, vertices, uvs, normals);
	if (!result)
		return EMPTY_VALUE;

	Renderer::SModel* model = new Renderer::SModel;
	model->iNumberOfVertices = (GLuint)vertices.size();
	model->VAO = SetVertexAttribute(vertices, uvs, normals);

	int index = 0;
	while (1) 
	{
		if (m_ModelMap.find(index) == m_ModelMap.end())
			break;

		++index;
	}
	m_ModelMap.insert(std::pair<int, Renderer::SModel*>(index, model));

	return index;
}

int CRenderer::LoadOBJModelWithIndex(const char* fileName)
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;

	std::vector<glm::vec3> indexVertices;
	std::vector<glm::vec2> indexUVs;
	std::vector<glm::vec3> indexNormals;
	std::vector<glm::vec3> indexTangents;
	std::vector<glm::vec3> indexBitangents;
	std::vector<unsigned int> indices;

	bool result = ReadOBJFile(fileName, 
		vertices, uvs, normals);
	if (!result)
		return EMPTY_VALUE;

	CalculateTangentAndBitangent(vertices, uvs, normals,
		tangents, bitangents);

	ChangeToIndexObj(vertices, uvs, normals, 
		tangents, bitangents,
		indexVertices, indexUVs, indexNormals,
		indexTangents, indexBitangents,
		indices);

	Renderer::SModel* model = new Renderer::SModel;
	model->iNumberOfVertices = (GLuint)indices.size();
	model->VAO = SetVertexAttributeWithElementBufferObject(
		indexVertices, indexUVs, indexNormals, 
		indexTangents, indexBitangents,
		indices);

	int index = 0;
	while (1)
	{
		if (m_ModelMap.find(index) == m_ModelMap.end())
			break;

		++index;
	}
	m_ModelMap.insert(std::pair<int, Renderer::SModel*>(index, model));

	return index;
}

int CRenderer::LoadBMPTexture(const char* ambientMapFileName, const char* diffuseMapFileName, const char* speculaMapFileName, float shininess, const char* normalMapFileName)
{
	Renderer::STexture* texture = new Renderer::STexture;

	texture->ambientMapTextureID = ReadBMPTextureFile(ambientMapFileName);
	texture->diffuseMapTextureID = ReadBMPTextureFile(diffuseMapFileName);
	texture->speculaMapTextureID = ReadBMPTextureFile(speculaMapFileName);
	texture->fShininess = shininess;
	texture->normalMapTextureID = ReadBMPTextureFile(normalMapFileName);

	int index = 0;
	while (1)
	{
		if (m_TextureMap.find(index) == m_TextureMap.end())
			break;

		++index;
	}
	m_TextureMap.insert(std::pair<int, Renderer::STexture*>(index, texture));

	return index;
}

int CRenderer::LoadPNGTexture(const char* ambientMapFileName, const char* diffuseMapFileName, const char* speculaMapFileName, float shininess, const char* normalMapFileName)
{
	Renderer::STexture* texture = new Renderer::STexture;
	texture->ambientMapTextureID = ReadPNGTextureFile(ambientMapFileName);
	texture->diffuseMapTextureID = ReadPNGTextureFile(diffuseMapFileName);
	texture->speculaMapTextureID = ReadPNGTextureFile(speculaMapFileName);
	texture->fShininess = shininess;
	texture->normalMapTextureID = ReadPNGTextureFile(normalMapFileName);

	int index = 0;
	while (1)
	{
		if (m_TextureMap.find(index) == m_TextureMap.end())
			break;

		++index;
	}
	m_TextureMap.insert(std::pair<int, Renderer::STexture*>(index, texture));

	return index;
}

void CRenderer::LoadModelWithBone(const char* fileName, std::string name)
{
	// 같은 이름의 모델을 찾았을 경우 리턴 
	if (m_ModelWithBoneLoaderMap.find(name) != m_ModelWithBoneLoaderMap.end()) return;

	// 모델 로드
	CModelLoader* modelLoader = new CModelLoader(fileName);
	m_ModelWithBoneLoaderMap.insert(std::pair<std::string, CModelLoader*>(name, modelLoader));
}

void CRenderer::LoadAnimation(const char* fileName, std::string modelName, std::string animationName)
{
	// 같은 이름의 모델을 찾았을 경우 리턴 
	if (m_AnimationLoaderMap.find(animationName) != m_AnimationLoaderMap.end()) return;

	// 해당 애니메이션을 플레이할 모델이 없을 경우 리턴
	auto it = m_ModelWithBoneLoaderMap.find(modelName);
	if (it == m_ModelWithBoneLoaderMap.end()) {
		std::cout << modelName << "에 해당하는 모델이 없어 애니메이션 로드가 불가능합니다.\n";
		return;
	}

	// 애니메이션 로드
	CAnimationLoader* animation = new CAnimationLoader(fileName, (*it).second);
	m_AnimationLoaderMap.insert(std::pair<std::string, CAnimationLoader*>(animationName, animation));
}

CModelLoader* CRenderer::GetModelWithBone(std::string modelName)
{
	auto it = m_ModelWithBoneLoaderMap.find(modelName);

	if (it == m_ModelWithBoneLoaderMap.end()) {
		std::cout << modelName << "라는 애니메이션이 없습니다.\n";
		return nullptr;
	}
	return (*it).second;
}

CAnimationLoader* CRenderer::GetAnimation(std::string animationName)
{
	auto it = m_AnimationLoaderMap.find(animationName);

	if (it == m_AnimationLoaderMap.end()) {
		std::cout << animationName << "라는 애니메이션이 없습니다.\n";
		return nullptr;
	}
	return (*it).second;
}

void CRenderer::DeleteModelWithBone(std::string name)
{
	auto it = m_ModelWithBoneLoaderMap.find(name);

	if (it == m_ModelWithBoneLoaderMap.end()) return;

	delete (*it).second;
	m_ModelWithBoneLoaderMap.erase(name);
}

void CRenderer::DeleteAnimation(std::string name)
{
	auto it = m_AnimationLoaderMap.find(name);

	if (it == m_AnimationLoaderMap.end()) return;

	delete (*it).second;
	m_AnimationLoaderMap.erase(name);
}

int CRenderer::LoadSkybox(std::string name, std::string* fileNames)
{
	auto skybox = m_SkyboxMap.find(name);

	if (skybox != m_SkyboxMap.end()) {
		return skybox->second;
	}

	// 텍스쳐 생성
	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0 + SKYBOX_TEXTURE_NUMBER);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	unsigned char* data;
	stbi_set_flip_vertically_on_load(false);
	for (int i = 0; i < 6; ++i)
	{
		data = stbi_load(fileNames[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			GLenum format;
			if (nrChannels == 1)
				format = GL_RED;
			else if (nrChannels == 3)
				format = GL_RGB;
			else if (nrChannels == 4)
				format = GL_RGBA;

			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << fileNames[i] << "를 불러올 수 없습니다.\n";
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	int index = (int)m_SkyboxVector.size();
	m_SkyboxVector.push_back(textureID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	m_SkyboxMap.insert(std::pair<std::string, int>(name, index));

	return index;
}

int CRenderer::DeleteModelAndSetEmptyID(int index)
{
	if (index == EMPTY_VALUE)
		return EMPTY_VALUE;

	auto it = m_ModelMap.find(index);
	glDeleteVertexArrays(1, &(*it).second->VAO);
	(*it).second = NULL;
	m_ModelMap.erase(index);

	return EMPTY_VALUE;
}

int CRenderer::DeleteTextureAndSetEmptyID(int index)
{
	if (index == EMPTY_VALUE)
		return EMPTY_VALUE;

	auto it = m_TextureMap.find(index);
	GLuint tempTexture[3] = {
		(*it).second->ambientMapTextureID,
		(*it).second->diffuseMapTextureID,
		(*it).second->speculaMapTextureID
	};
	glDeleteTextures(3, tempTexture);

	(*it).second = NULL;
	m_TextureMap.erase(index);

	return EMPTY_VALUE;
}

void CRenderer::SetDisplay()
{
	// 화면 색상 초기화
	glClearColor(0.f, 0.0f, 0.4f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void CRenderer::DrawElementsWithTriangles(int modelIndex, int textureIndex,
	glm::vec3 position, glm::vec3 rotation, glm::vec3 size, glm::quat quaternion, glm::vec4 color,
	bool isLightSource, bool outLine)
{
	if (outLine)
	{
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
	}
	else
	{
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
	}

	Renderer::SModel* model = GetModel(modelIndex);
	Renderer::STexture* texture = GetTexture(textureIndex);

	// 이동 변환 행렬
	glm::mat4 trasformMat = glm::mat4(1.0f);
	// 크기 변환 행렬
	glm::mat4 scaleMat = glm::mat4(1.0f);

	// 이동 변환
	trasformMat = glm::translate(trasformMat, position);

	// 각 축에 대한 쿼터니언
	glm::quat modelQuat
		= glm::quat(glm::vec3(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z)));
	glm::mat4 rotationMat = glm::toMat4(modelQuat);

	// 크기 변환
	scaleMat = glm::scale(scaleMat, size);

	// 모델 변환 행렬 설정
	glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE,
		glm::value_ptr(trasformMat * rotationMat * glm::mat4(quaternion) * scaleMat));

	// 재질 설정
	glUniform1f(m_materialShininessLocation, texture->fShininess);

	// 색상 설정
	glUniform4f(m_colorLocation, color.r, color.g, color.b, color.a);

	glUniform1i(m_isLightSourceLocation, isLightSource);

	// 텍스쳐 위치 설정
	glUniform1i(m_materialAmbientLocation, 0);
	glUniform1i(m_materialDiffuseLocation, 1);
	glUniform1i(m_materialSpecularLocation, 2);
	glUniform1i(m_materialNormalLocation, 3);

	// 사용할 VAO 불러오기
	glBindVertexArray(model->VAO);

	// 사용할 텍스쳐 불러오기
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->ambientMapTextureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture->diffuseMapTextureID);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture->speculaMapTextureID);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, texture->normalMapTextureID);

	// 삼각형 그리기
	glDrawElements(GL_TRIANGLES, model->iNumberOfVertices, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
}

void CRenderer::DrawElementsWithTrianglesForDepth(int modelIndex, 
	glm::vec3 position, glm::vec3 rotation, glm::vec3 size, glm::quat quaternion)
{
	Renderer::SModel* model = GetModel(modelIndex);

	// 이동 변환 행렬
	glm::mat4 trasformMat = glm::mat4(1.0f);
	// 크기 변환 행렬
	glm::mat4 scaleMat = glm::mat4(1.0f);

	// 이동 변환
	trasformMat = glm::translate(trasformMat, position);

	// 각 축에 대한 쿼터니언
	glm::quat modelQuat
		= glm::quat(glm::vec3(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z)));
	glm::mat4 rotationMat = glm::toMat4(modelQuat);

	// 크기 변환
	scaleMat = glm::scale(scaleMat, size);

	// 모델 변환 행렬 설정
	glUniformMatrix4fv(m_modelMatrixForDepthLocation, 1, GL_FALSE,
		glm::value_ptr(trasformMat * rotationMat * glm::mat4(quaternion) *  scaleMat));

	// 사용할 VAO 불러오기
	glBindVertexArray(model->VAO);

	// 삼각형 그리기
	glDrawElements(GL_TRIANGLES, model->iNumberOfVertices, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void CRenderer::DrawElementsWithTrianglesForBorder(int modelIndex, 
	glm::vec3 position, glm::vec3 rotation, glm::vec3 size, glm::quat quaternion,
	glm::vec4 color)
{
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);

	Renderer::SModel* model = GetModel(modelIndex);

	// 이동 변환 행렬
	glm::mat4 trasformMat = glm::mat4(1.0f);
	// 크기 변환 행렬
	glm::mat4 scaleMat = glm::mat4(1.0f);
	// 실제 보다 조금 큰 크기로 변환하는 행렬
	glm::mat4 scaleLittleMat = glm::mat4(1.0f);

	// 이동 변환
	trasformMat = glm::translate(trasformMat, position);

	// 각 축에 대한 쿼터니언
	glm::quat modelQuat
		= glm::quat(glm::vec3(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z)));
	glm::mat4 rotationMat = glm::toMat4(modelQuat);

	// 크기 변환
	scaleMat = glm::scale(scaleMat, size);
	// 크기 변환
	scaleLittleMat = glm::scale(scaleLittleMat, glm::vec3(1.05f, 1.05f, 1.05f));

	// 모델 변환 행렬 설정
	glUniformMatrix4fv(m_modelMatrixForBorderLocation, 1, GL_FALSE,
		glm::value_ptr(trasformMat * rotationMat *glm::mat4(quaternion) * scaleMat * scaleLittleMat));

	// 색상 설정
	glUniform4f(m_colorForBorderLocation, color.r, color.g, color.b, color.a);

	// 사용할 VAO 불러오기
	glBindVertexArray(model->VAO);

	// 삼각형 그리기
	glDrawElements(GL_TRIANGLES, model->iNumberOfVertices, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
}

void CRenderer::DrawAnimationModel(std::string modelName, int textureIndex, std::vector<std::vector<SVertex>>& verticesVector, glm::vec3 position, glm::vec3 rotation, glm::vec3 size, glm::quat quaternion, glm::vec4 color, bool isLightSource, bool outLine)
{
	if (outLine)
	{
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
	}
	else
	{
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
	}

	Renderer::STexture* texture = GetTexture(textureIndex);

	// 이동 변환 행렬
	glm::mat4 trasformMat = glm::mat4(1.0f);
	// 크기 변환 행렬
	glm::mat4 scaleMat = glm::mat4(1.0f);

	// 이동 변환
	trasformMat = glm::translate(trasformMat, position);

	// 각 축에 대한 쿼터니언
	glm::quat modelQuat
		= glm::quat(glm::vec3(glm::radians(rotation.x), glm::radians(rotation.y - 180.0f), glm::radians(rotation.z)));
	glm::mat4 rotationMat = glm::toMat4(modelQuat);

	// 크기 변환
	scaleMat = glm::scale(scaleMat, size);

	// 모델 변환 행렬 설정
	glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE,
		glm::value_ptr(trasformMat * rotationMat * glm::mat4(quaternion) * scaleMat));


	// 색상 설정
	glUniform4f(m_colorLocation, color.r, color.g, color.b, color.a);

	glUniform1i(m_isLightSourceLocation, isLightSource);

	// 텍스쳐 위치 설정
	glUniform1i(m_materialAmbientLocation, 0);
	glUniform1i(m_materialDiffuseLocation, 1);
	glUniform1i(m_materialSpecularLocation, 2);
	glUniform1i(m_materialNormalLocation, 3);

	// 사용할 텍스쳐 불러오기
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->ambientMapTextureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture->diffuseMapTextureID);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture->speculaMapTextureID);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, texture->normalMapTextureID);

	auto it = m_ModelWithBoneLoaderMap.find(modelName);
	std::vector<SAssimpAnimationModel> models = (*it).second->GetModels();

	int verticesIndex = 0;
	for (SAssimpAnimationModel model : models) {
		// 사용할 VAO 불러오기
		glBindVertexArray(model.vao);
		glBindBuffer(GL_ARRAY_BUFFER, model.vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0,
			verticesVector[verticesIndex].size() * sizeof(SVertex), &verticesVector[verticesIndex][0]);

		// 삼각형 그리기
		glDrawElements(GL_TRIANGLES, model.iNumberOfVertices, GL_UNSIGNED_INT, 0);

		verticesIndex++;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
}

void CRenderer::DrawAnimationModelForDapth(std::string modelName, 
	std::vector<std::vector<SVertex>>& verticesVector,
	glm::vec3 position, glm::vec3 rotation, glm::vec3 size, glm::quat quaternion)
{
	// 이동 변환 행렬
	glm::mat4 trasformMat = glm::mat4(1.0f);
	// 크기 변환 행렬
	glm::mat4 scaleMat = glm::mat4(1.0f);

	// 이동 변환
	trasformMat = glm::translate(trasformMat, position);

	// 각 축에 대한 쿼터니언
	glm::quat modelQuat
		= glm::quat(glm::vec3(glm::radians(rotation.x), glm::radians(rotation.y - 180.0f), glm::radians(rotation.z)));
	glm::mat4 rotationMat = glm::toMat4(modelQuat);

	// 크기 변환
	scaleMat = glm::scale(scaleMat, size);

	// 모델 변환 행렬 설정
	glUniformMatrix4fv(m_modelMatrixForDepthLocation, 1, GL_FALSE,
		glm::value_ptr(trasformMat * rotationMat * glm::mat4(quaternion) * scaleMat));

	auto it = m_ModelWithBoneLoaderMap.find(modelName);
	if (it == m_ModelWithBoneLoaderMap.end()) {
		std::cout << modelName << "에 해당하는 모델이 없습니다.\n";
	}

	std::vector<SAssimpAnimationModel> models = (*it).second->GetModels();

	int verticesIndex = 0;
	for (SAssimpAnimationModel model : models) {
		// 사용할 VAO 불러오기
		glBindVertexArray(model.vao);
		glBindBuffer(GL_ARRAY_BUFFER, model.vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0,
			verticesVector[verticesIndex].size() * sizeof(SVertex), &verticesVector[verticesIndex][0]);

		// 삼각형 그리기
		glDrawElements(GL_TRIANGLES, model.iNumberOfVertices, GL_UNSIGNED_INT, 0);

		verticesIndex++;
	}

	glBindVertexArray(0);
}

void CRenderer::DrawAnimationModelForBorder(std::string modelName, 
	std::vector<std::vector<SVertex>>& verticesVector,
	glm::vec3 position, glm::vec3 rotation, glm::vec3 size, glm::quat quaternion,
	glm::vec4 color)
{
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);

	// 이동 변환 행렬
	glm::mat4 trasformMat = glm::mat4(1.0f);
	// 크기 변환 행렬
	glm::mat4 scaleMat = glm::mat4(1.0f);

	// 실제 보다 조금 큰 크기로 변환하는 행렬
	glm::mat4 scaleLittleMat = glm::mat4(1.0f);

	// 이동 변환
	trasformMat = glm::translate(trasformMat, position);

	// 각 축에 대한 쿼터니언
	glm::quat modelQuat
		= glm::quat(glm::vec3(glm::radians(rotation.x), glm::radians(rotation.y - 180.0f), glm::radians(rotation.z)));
	glm::mat4 rotationMat = glm::toMat4(modelQuat);

	// 크기 변환
	scaleMat = glm::scale(scaleMat, size);
	// 크기 변환
	scaleLittleMat = glm::scale(scaleLittleMat, glm::vec3(1.05f, 1.05f, 1.05f));

	// 모델 변환 행렬 설정
	glUniformMatrix4fv(m_modelMatrixForBorderLocation, 1, GL_FALSE,
		glm::value_ptr(trasformMat * rotationMat * glm::mat4(quaternion) * scaleMat * scaleLittleMat));

	glUniform4f(m_colorForBorderLocation, color.r, color.g, color.b, color.a);

	auto it = m_ModelWithBoneLoaderMap.find(modelName);
	std::vector<SAssimpAnimationModel> models = (*it).second->GetModels();

	int verticesIndex = 0;
	for (SAssimpAnimationModel model : models) {
		// 사용할 VAO 불러오기
		glBindVertexArray(model.vao);
		glBindBuffer(GL_ARRAY_BUFFER, model.vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 
			verticesVector[verticesIndex].size() * sizeof(SVertex), &verticesVector[verticesIndex][0]);

		// 삼각형 그리기
		glDrawElements(GL_TRIANGLES, model.iNumberOfVertices, GL_UNSIGNED_INT, 0);

		verticesIndex++;
	}

	glBindVertexArray(0);

	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
}

void CRenderer::CaculateVerticesAfterAnimation(std::string modelName, 
	std::vector<std::vector<SVertex>>& verticesVector, std::vector<glm::mat4> finalBonesMatrices)
{
	auto it = m_ModelWithBoneLoaderMap.find(modelName);
	if (it == m_ModelWithBoneLoaderMap.end()) {
		std::cout << modelName << "에 해당하는 모델이 없습니다.\n";
	}

	std::vector<SAssimpAnimationModel> models = (*it).second->GetModels();

	int modelIndex = 0;
	for (SAssimpAnimationModel model : models) {
		for (int i = 0; i < model.vertices.size(); ++i) {
			verticesVector[modelIndex][i].position = model.vertices[i].position;
			verticesVector[modelIndex][i].normal = model.vertices[i].normal;
		}

		for (int i = 0; i < verticesVector[modelIndex].size(); ++i) {
			glm::mat4 boneMatrix = glm::mat4(0.0f);
			for (int j = 0; j < MAX_BONE_INFLUENCE_SIZE; ++j) {
				if (verticesVector[modelIndex][i].boneIDs[j] == -1) continue;
				if (verticesVector[modelIndex][i].boneIDs[j] > MAX_BONE_SIZE) {
					boneMatrix = glm::mat4(1.0f);
					break;
				}
				boneMatrix += finalBonesMatrices[verticesVector[modelIndex][i].boneIDs[j]]
					* verticesVector[modelIndex][i].weights[j];
			}
			verticesVector[modelIndex][i].position = boneMatrix * glm::vec4(verticesVector[modelIndex][i].position, 1.0f);
			verticesVector[modelIndex][i].normal = boneMatrix * glm::vec4(verticesVector[modelIndex][i].normal, 0.0f);
		}

		modelIndex++;
	}
}

void CRenderer::DrawBoundingBox(glm::vec3 position, glm::vec3 rotation, glm::vec3 size, glm::vec3 volume)
{
	// 선으로 그리기 활성화
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// 깊이 테스트 비활설화
	glDisable(GL_DEPTH_TEST);

	Renderer::SModel* model = GetModel(m_iBoundingBoxModelIndex);

	// 모델 변환, 재질 로케이션
	GLuint modelMatrixLocation = glGetUniformLocation(m_BoundingBoxShaderProgramID, "u_mModelTransform");

	// 이동 변환 행렬
	glm::mat4 trasformMat = glm::mat4(1.0f);
	// 크기 변환 행렬
	glm::mat4 scaleMat = glm::mat4(1.0f);
	// 볼륨 크기
	glm::mat4 volumeMat = glm::mat4(1.0f);

	// 이동 변환
	trasformMat = glm::translate(trasformMat, position);

	// 각 축에 대한 쿼터니언
	glm::quat modelQuat
		= glm::quat(glm::vec3(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z)));
	glm::mat4 rotationMat = glm::toMat4(modelQuat);

	// 크기 변환
	scaleMat = glm::scale(scaleMat, size);

	// 크기 변환
	volumeMat = glm::scale(volumeMat, volume);

	// 모델 변환 행렬 설정
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE,
		glm::value_ptr(trasformMat * rotationMat * scaleMat * volumeMat));

	// 사용할 VAO 불러오기(스카이 박스 모델 정보 재사용)
	glBindVertexArray(model->VAO);

	glDrawArrays(GL_TRIANGLES, 0, model->iNumberOfVertices);

	glBindVertexArray(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
}

void CRenderer::DrawWaterObject(int modelIndex, glm::vec3 position, glm::vec3 rotation, glm::vec3 size,
	int skybox1ID, int skybox2ID, float blendRate,
	float time)
{
	Renderer::SModel* model = GetModel(modelIndex);

	// 이동 변환 행렬
	glm::mat4 trasformMat = glm::mat4(1.0f);
	// 크기 변환 행렬
	glm::mat4 scaleMat = glm::mat4(1.0f);

	// 이동 변환
	trasformMat = glm::translate(trasformMat, position);

	// 각 축에 대한 쿼터니언
	glm::quat modelQuat
		= glm::quat(glm::vec3(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z)));
	glm::mat4 rotationMat = glm::toMat4(modelQuat);

	// 크기 변환
	scaleMat = glm::scale(scaleMat, size);

	// 모델 변환 행렬 설정
	glUniformMatrix4fv(m_modelMatrixLocationForWater, 1, GL_FALSE,
		glm::value_ptr(trasformMat * rotationMat * scaleMat));

	glUniform1i(m_textureForWaterLocation, SKYBOX_TEXTURE_NUMBER);
	glActiveTexture(GL_TEXTURE0 + SKYBOX_TEXTURE_NUMBER);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_SkyboxVector.at(skybox1ID));

	if (blendRate != 0.0f) {
		glUniform1i(m_texture2ForWaterLocation, SKYBOX_TEXTURE_NUMBER + 1);
		glActiveTexture(GL_TEXTURE0 + SKYBOX_TEXTURE_NUMBER + 1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_SkyboxVector.at(skybox2ID));
	}

	glUniform1f(m_blendRateForWaterLocation, blendRate);
	glUniform1f(m_timeForWaterLocation, time);

	// 사용할 VAO 불러오기
	glBindVertexArray(model->VAO);

	// 삼각형 그리기
	glDrawElements(GL_TRIANGLES, model->iNumberOfVertices, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CRenderer::DrawCapsule(glm::vec3 position, float radius, float height)
{
	// 선으로 그리기 활성화
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// 깊이 테스트 비활설화
	glDisable(GL_DEPTH_TEST);

	Renderer::SModel* model = GetModel(m_iCapsuleModelIndex);

	// 모델 변환, 재질 로케이션
	GLuint modelMatrixLocation = glGetUniformLocation(m_BoundingBoxShaderProgramID, "u_mModelTransform");

	glm::vec3 volume = glm::vec3(radius, height, radius);

	// 이동 변환 행렬
	glm::mat4 trasformMat = glm::mat4(1.0f);
	// 크기 변환 행렬
	glm::mat4 volumeMat = glm::mat4(1.0f);

	// 이동 변환
	trasformMat = glm::translate(trasformMat, position);

	// 크기 변환
	volumeMat = glm::scale(volumeMat, volume);

	// 모델 변환 행렬 설정
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE,
		glm::value_ptr(trasformMat * volumeMat));

	// 사용할 VAO 불러오기(스카이 박스 모델 정보 재사용)
	glBindVertexArray(model->VAO);

	glDrawArrays(GL_TRIANGLES, 0, model->iNumberOfVertices);

	glBindVertexArray(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
}

void CRenderer::RenderText(const wchar_t* text, float xRate, float yRate, float scale, glm::vec3 color)
{
	glDisable(GL_DEPTH_TEST);

	float x = xRate * m_iResoultionX;
	float y = yRate * m_iResoultionY;

	// 화면 크기에 맞추어 직교 투영
	glm::mat4 projection = glm::ortho(0.0f, (float)m_iResoultionX, 0.0f, (float)m_iResoultionY);
	glUniformMatrix4fv(m_projectionMatrixForTextLocation, 1, GL_FALSE, glm::value_ptr(projection));
	glUniform3f(m_colorForTextLocation, color.x, color.y, color.z);

	// 사용할 텍스쳐, 정점 배열 등록
	glUniform1i(m_textLocation, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(m_TextVAO);

	for (int i = 0; i < wcslen(text); ++i)
	{
		Renderer::SCharacter ch = m_CharacterMap[text[i]];

		// 위치 계산
		float xpos = x + ch.vBearing.x * scale;
		float ypos = y - (ch.vSize.y - ch.vBearing.y) * scale;

		// 크기 계산
		float w = ch.vSize.x * scale;
		float h = ch.vSize.y * scale;

		// 각 글자에 VBO 업데이트
		float vertices[6][4] = {
			{xpos,		ypos + h,	0.f, 0.f},
			{xpos,		ypos,		0.f, 1.f},
			{xpos + w,	ypos,		1.f, 1.f},

			{xpos,		ypos + h,	0.f, 0.f},
			{xpos + w,	ypos,		1.f, 1.f},
			{xpos + w,	ypos + h,	1.f, 0.f}
		};

		glBindTexture(GL_TEXTURE_2D, ch.textureID);

		glBindBuffer(GL_ARRAY_BUFFER, m_TextVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		// 다음 글자를 위해 현재 커서 위치 계산(6만큼 오른쪽로 비트 시프트 하여 픽셀에서의 값을 얻음)
		x += (ch.iAdvance >> 6) * scale;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0); 

	glEnable(GL_DEPTH_TEST);
}

void CRenderer::RenderText3D(const wchar_t* text, float scale, glm::vec3 position, glm::vec4 color)
{
	// 변환 설정
	glUniform3f(m_translateForText3DLocation, position.x, position.y, position.z);
	glUniform2f(m_scaleForText3DLocation, scale, scale);

	// 색상 설정
	glUniform4f(m_colorForText3DLocation, color.r, color.g, color.b, color.a);

	// 사용할 텍스쳐, 정점 배열 등록
	glUniform1i(m_text3DLocation, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(m_TextVAO);
	
	float x = 0.0f, y = 0.0;

	float fontSizeReduceSize = 150.f;

	for (int i = 0; i < wcslen(text); ++i)
	{
		Renderer::SCharacter ch = m_CharacterMap[text[i]];

		// 위치 계산
		float xpos = (x + ch.vBearing.x) / fontSizeReduceSize;
		float ypos = (y - (ch.vSize.y - ch.vBearing.y)) / fontSizeReduceSize;

		// 크기 계산
		float w = (float)ch.vSize.x / fontSizeReduceSize;
		float h = (float)ch.vSize.y / fontSizeReduceSize;

		// 각 글자에 VBO 업데이트
		float vertices[6][4] = {
			{xpos,		ypos + h,	0.f, 0.f},
			{xpos,		ypos,		0.f, 1.f},
			{xpos + w,	ypos,		1.f, 1.f},

			{xpos,		ypos + h,	0.f, 0.f},
			{xpos + w,	ypos,		1.f, 1.f},
			{xpos + w,	ypos + h,	1.f, 0.f}
		};

		glBindTexture(GL_TEXTURE_2D, ch.textureID);

		glBindBuffer(GL_ARRAY_BUFFER, m_TextVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		// 다음 글자를 위해 현재 커서 위치 계산(6만큼 오른쪽로 비트 시프트 하여 픽셀에서의 값을 얻음)
		x += (ch.iAdvance >> 6);
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CRenderer::RenderTexture2D(GLuint textureID,
	glm::vec2 positionRate, glm::vec2 rotation, glm::vec2 size, glm::vec4 color)
{
	glDisable(GL_DEPTH_TEST);

	// 화면 크기에 맞추어 직교 투영
	glm::mat4 projection = glm::ortho(0.0f, (float)m_iResoultionX, 0.0f, (float)m_iResoultionY);
	glUniformMatrix4fv(m_projectionMatrixForTexture2DLocation, 1, GL_FALSE, glm::value_ptr(projection));
	glUniform4f(m_colorForTexture2DLocation, color.x, color.y, color.z, color.w);

	// VAO 바인드
	glBindVertexArray(m_2DRectangleVAO);

	// 텍스쳐 활성화 및 바인드
	glUniform1i(m_textureForTexture2DLocation, 0);
	glActiveTexture(GL_TEXTURE0);

	// 위치 계산
	float xpos = positionRate.x * m_iResoultionX;
	float ypos = positionRate.y * m_iResoultionY;

	// 크기 계산
	float w = size.x;
	float h = size.y;

	float vertices[] = {
		xpos,		ypos + h,	0.f, 1.f,
		xpos,		ypos,		0.f, 0.f,
		xpos + w,	ypos,		1.f, 0.f,

		xpos,		ypos + h,	0.f, 1.f,
		xpos + w,	ypos,		1.f, 0.f,
		xpos + w,	ypos + h,	1.f, 1.f
	};

	glBindTexture(GL_TEXTURE_2D, textureID);

	glBindBuffer(GL_ARRAY_BUFFER, m_2DRectangleVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnable(GL_DEPTH_TEST);
}

void CRenderer::RenderTexture3D(GLuint textureID, glm::vec3 position, glm::vec2 size, glm::vec4 color)
{
	// 변환 설정
	glUniform3f(m_translateForTexture3DLocation, position.x, position.y, position.z);
	glUniform2f(m_scaleForTexture3DLocation, size.x, size.y);
	
	glUniform4f(m_colorForTexture3DLocation, color.x, color.y, color.z, color.w);

	// 텍스쳐 위치 설정
	glUniform1i(m_textureForTexture3DLocation, 0);

	// 사용할 VAO 불러오기
	glBindVertexArray(m_PlaneModelVAO);

	// 사용할 텍스쳐 불러오기
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// 삼각형 그리기
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CRenderer::RenderSpriteSheet2D(GLuint textureID, int spriteCountX, int spriteCountY, int orderX, int orderY,
	glm::vec2 positionRate, glm::vec2 rotation, glm::vec2 size, glm::vec4 color)
{
	glDisable(GL_DEPTH_TEST);

	// 화면 크기에 맞추어 직교 투영
	glm::mat4 projection = glm::ortho(0.0f, (float)m_iResoultionX, 0.0f, (float)m_iResoultionY);
	glUniformMatrix4fv(m_projectionMatrixForTexture2DLocation, 1, GL_FALSE, glm::value_ptr(projection));
	glUniform4f(m_colorForTexture2DLocation, color.x, color.y, color.z, color.w);

	// VAO 바인드
	glBindVertexArray(m_2DRectangleVAO);

	// 텍스쳐 활성화 및 바인드
	glUniform1i(m_textureForTexture2DLocation, 0);
	glActiveTexture(GL_TEXTURE0);

	// 위치 계산
	float xpos = positionRate.x * m_iResoultionX;
	float ypos = positionRate.y * m_iResoultionY;

	// 크기 계산
	float w = size.x;
	float h = size.y;

	// 각 이미지 크기 결정
	float distanceX = 1.0f / (float)spriteCountX;

	float left = distanceX * orderX;
	float right = distanceX * (orderX + 1);

	float distanceY = 1.0f / (float)spriteCountY;

	float bottom = distanceY * orderY;
	float top = distanceY * (orderY + 1);

	float vertices[] = {
		xpos,		ypos + h,	left, top,
		xpos,		ypos,		left, bottom,
		xpos + w,	ypos,		right, bottom,
		
		xpos,		ypos + h,	left, top,
		xpos + w,	ypos,		right, bottom,
		xpos + w,	ypos + h,	right, top

	};

	glBindTexture(GL_TEXTURE_2D, textureID);

	glBindBuffer(GL_ARRAY_BUFFER, m_2DRectangleVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnable(GL_DEPTH_TEST);
}

void CRenderer::RenderSpriteSheet3D(GLuint textureID, 
	int spriteCountX, int spriteCountY, int orderX, int orderY,
	glm::vec3 position, glm::vec2 size, glm::vec4 color)
{
	// 변환 설정
	glUniform3f(m_translateForBillboardSpriteSheetLocation, position.x, position.y, position.z);
	glUniform2f(m_scaleForBillboardSpriteSheetLocation, size.x, size.y);

	glUniform4f(m_colorForBillboardSpriteSheetLocation, color.x, color.y, color.z, color.w);

	glUniform4f(m_frameForBillboardSpriteSheetLocation, 
		(float)spriteCountX, (float)spriteCountY, (float)orderX, (float)orderY);

	// 텍스쳐 위치 설정
	glUniform1i(m_textureForBillboardSpriteSheetLocation, 0);

	// 사용할 VAO 불러오기
	glBindVertexArray(m_PlaneModelVAO);

	// 사용할 텍스쳐 불러오기
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// 삼각형 그리기
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CRenderer::DrawSkybox(int skybox1ID, int skybox2ID, float blendRate)
{
	glDepthFunc(GL_LEQUAL);

	glBindVertexArray(m_SkyboxVAO);

	glUniform1i(m_textureForSkyboxLocation, SKYBOX_TEXTURE_NUMBER);
	glActiveTexture(GL_TEXTURE0 + SKYBOX_TEXTURE_NUMBER);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_SkyboxVector.at(skybox1ID));
	
	if (blendRate != 0.0f) {
		glUniform1i(m_texture2ForSkyboxLocation, SKYBOX_TEXTURE_NUMBER + 1);
		glActiveTexture(GL_TEXTURE0 + SKYBOX_TEXTURE_NUMBER + 1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_SkyboxVector.at(skybox2ID));
	}

	glUniform1f(m_blendRateForSkyboxLocation, blendRate);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthFunc(GL_LESS);
}

void CRenderer::RenderInstance(int modelIndex, GLuint textureID, GLuint ssboModelMatrices, float time, int count)
{
	glDisable(GL_CULL_FACE);

	Renderer::SModel* model = GetModel(modelIndex);

	glUniform1i(m_textureForInstanceLocation, 0);
	glUniform1f(m_timeForInstanceLocation, time);

	glBindVertexArray(model->VAO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboModelMatrices);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glDrawElementsInstanced(GL_TRIANGLES,
		model->iNumberOfVertices, GL_UNSIGNED_INT, 0, count);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnable(GL_CULL_FACE);
}

void CRenderer::RenderDisplayFXAA()
{
	GLuint resolutionLocation = glGetUniformLocation(m_DisplayFXAAShaderProgramID, "u_vResolution");
	glUniform2f(resolutionLocation, (GLfloat)m_iResoultionX, (GLfloat)m_iResoultionY);

	GLuint textureLocation = glGetUniformLocation(m_DisplayFXAAShaderProgramID, "u_texture");
	glUniform1i(textureLocation, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_DisplayFXAATexture);

	glBindVertexArray(m_DisplayVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint CRenderer::GetShaderProgram(int type) const
{
	if (type == TEXT_SHADER)
	{
		return m_TextShaderProgramID;
	}
	else if (type == UI_SHADER)
	{
		return m_2dUIShaderProgramID;
	}
	else if (type == BILLBOARD_SHADER)
	{
		return m_BillboardShaderProgramID;
	}
	else if (type == BILLBOARD_SPRITE_SHADER)
	{
		return m_BillboardSpriteShaderProgramID;
	}
	else if (type == SKYBOX_SHADER)
	{
		return m_SkyboxShaderProgramID;
	}
	else if (type == DEPTH_SHADER)
	{
		return m_DepthShaderProgramID;
	}
	else if (type == BOUNDING_BOX_SHADER)
	{
		return m_BoundingBoxShaderProgramID;
	}
	else if (type == BORDER_SHADER)
	{
		return m_BorderShaderProgramID;
	}
	else if (type == TOTAL_SCENE_SHADER)
	{
		return m_TotalSceneShaderProgramID;
	}
	else if (type == INSTANCE_SHADER)
	{
		return m_InstanceShaderProgramID;
	}
	else if (type == BILLBOARD_TEXT_SHADER)
	{
		return m_BillboardTextShaderProgramID;
	}
	else if (type == DAMAGE_ZONE_SHADER)
	{
		return m_DamageZoneShaderProgramID;
	}
	else if (type == WATER_SHADER)
	{
		return m_WaterShaderProgramID;
	}
	else if (type == GRAVITY_PARTICLE_SHADER)
	{
		return m_GravityParticleShaderProgramID;
	}
	else if (type == DISPLAY_FXAA_SHADER)
	{
		return m_DisplayFXAAShaderProgramID;
	}
	else if (type == Blur_SHADER)
	{
		return m_BlurShaderProgramID;
	}
	else
	{
		return m_ShaderProgramID;
	}
}

void CRenderer::LoadFont(const char* fileName, int size)
{
	// FreeType 라이브러리
	FT_Library FTLibrary;
	// 폰트 생성 변수
	FT_Face FTFace;

	// 폰트 라이브러리 초기화
	if (FT_Init_FreeType(&FTLibrary)) 
		std::cout << "FreeTypeError: Free Type Library 초기화 실패\n";

	// 폰트 생성
	if (FT_New_Face(FTLibrary, fileName, 0, &FTFace)) 
		std::cout << "FreeTypeError: " << fileName << "로 폰트 생성 실패\n";

	// 폰트 크기 설정(너비가 0일 경우 높이에 따라 너비가 계산됨)
	FT_Set_Pixel_Sizes(FTFace, 0, size);

	// 바이트 정렬 제한 비활성화(메모리에서 비트맵을 읽을 때)
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// 한글 유니코드 범위 0xAC00 ~ 0xD7AF
	for (wchar_t c = 0; c <= 0xD7AF; ++c)
	{
		// 알파벳 및 특수 기호 추가 완료되었을 경우 한글로 이동
		if (c == 128)
			c = 0xAC00;

		// 글자 로드
		if (FT_Load_Char(FTFace, c, FT_LOAD_RENDER))
		{
			std::cout << "FreeTypeError: " << c << "자 로드 실패\n";
			continue;
		}
		// 텍스쳐 생성 및 바인드
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D, 
			0, 
			GL_RED,
			FTFace->glyph->bitmap.width, 
			FTFace->glyph->bitmap.rows,
			0, 
			GL_RED, 
			GL_UNSIGNED_BYTE,
			FTFace->glyph->bitmap.buffer);

		// 텍스쳐 파라미터 설정
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// 다음번 사용을 위한 저장
		Renderer::SCharacter character = {
			texture, 
			glm::ivec2(FTFace->glyph->bitmap.width, FTFace->glyph->bitmap.rows),
			glm::ivec2(FTFace->glyph->bitmap_left, FTFace->glyph->bitmap_top),
			(unsigned int)FTFace->glyph->advance.x
		};
		m_CharacterMap.insert(std::pair<wchar_t, Renderer::SCharacter>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	// 사용 완료된 리소스 제거
	FT_Done_Face(FTFace);
	FT_Done_FreeType(FTLibrary);

	// 정점 배열 생성
	glGenVertexArrays(1, &m_TextVAO);
	glGenBuffers(1, &m_TextVBO);

	glBindVertexArray(m_TextVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_TextVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

GLuint CRenderer::LoadShaders(const char* fileNameVertex, const char* fileNameFragment)
{
	// 쉐이더 생성
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// 쉐이더 코드 파일 읽기
	char* vertexShaderCode = BringFileToBuf(fileNameVertex);
	char* fragmentShaderCode = BringFileToBuf(fileNameFragment);

	// 쉐이더 컴파일
	glShaderSource(VertexShaderID, 1, &vertexShaderCode, NULL);
	glCompileShader(VertexShaderID);
	glShaderSource(FragmentShaderID, 1, &fragmentShaderCode, NULL);
	glCompileShader(FragmentShaderID);

	// 쉐이더 오류 검사
	GLint result = GL_FALSE;
	int infoLogLength;

	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0)
	{
		std::vector<char> vertexShaderErrorMessage(infoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, infoLogLength, NULL, &vertexShaderErrorMessage[0]);
		std::cout << fileNameVertex << " Compile: \n";
		std::cout << &vertexShaderErrorMessage[0] << std::endl;
	}
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0)
	{
		std::vector<char> fragmentShaderErrorMessage(infoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, infoLogLength, NULL, &fragmentShaderErrorMessage[0]);
		std::cout << fileNameFragment << " Compile: \n";
		std::cout << &fragmentShaderErrorMessage[0] << std::endl;
	}

	// 프로그램에 링크
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// 프로그램 오류 검사
	glGetShaderiv(ProgramID, GL_LINK_STATUS, &result);
	glGetShaderiv(ProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0)
	{
		std::vector<char> ProgramErrorMessage(infoLogLength + 1);
		glGetShaderInfoLog(ProgramID, infoLogLength, NULL, &ProgramErrorMessage[0]);
		std::cout << "Shader Program: \n";
		std::cout << ProgramErrorMessage[0] << std::endl;
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	delete[] vertexShaderCode;
	delete[] fragmentShaderCode;

	return ProgramID;
}

void CRenderer::ContorlDisplayPostEffectOnOffLine(GameInputs* inputs, float elapsedTimeInSec)
{
	int speed = 500;

	if (inputs->KeyArrowLeft) {
		if (m_iDisplayPostEffectOnOffLine <= 0) {
			m_iDisplayPostEffectOnOffLine = 0;
		}
		else {
			m_iDisplayPostEffectOnOffLine -= (int)(elapsedTimeInSec * speed);
		}
	}
	if (inputs->KeyArrowRight) {
		if (m_iDisplayPostEffectOnOffLine >= m_iResoultionX) {
			m_iDisplayPostEffectOnOffLine = m_iResoultionX;
		}
		else {
			m_iDisplayPostEffectOnOffLine += (int)(elapsedTimeInSec * speed);
		}
	}
}

void CRenderer::ApplyBlurToBloomTexture()
{
	UseShader(Blur_SHADER);

	bool horizontal = true;
	bool first = true;
	int horizontalLocation =  glGetUniformLocation(GetShaderProgram(Blur_SHADER), "u_bHorizontal");

	for (int i = 0; i < 10; ++i) {
		glBindFramebuffer(GL_FRAMEBUFFER, m_BlurFBO[horizontal]);
		glUniform1i(horizontalLocation, horizontal);
		if (first) {
			glBindTexture(GL_TEXTURE_2D, m_BloomTexture);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, m_BlurTexture[horizontal]);
		}
		glBindVertexArray(m_DisplayVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);	

		horizontal = !horizontal;
		if (first) first = false;
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

char* CRenderer::BringFileToBuf(const char* fileName)
{
	char* buf;

	// 파일 열기
	std::ifstream in;
	in.open(fileName, std::ios::binary);

	if (in.is_open())
	{
		// 파일 크기 구하기
		in.seekg(0, std::ios::end);
		size_t length = (size_t)in.tellg();

		buf = new char[length + 1]; // 파일 크기: 내용 길이 + '\0'

		in.seekg(0, std::ios::beg);
		in.read(buf, length);

		in.close();
		
		// 맨 뒤에 null 문자 추가
		buf[length] = '\0';

		return buf;
	}
	else
	{
		std::cout << fileName << "is unable to open\n";
		return 0;
	}
}

bool CRenderer::ReadOBJFile(const char* fileName, 
	std::vector<glm::vec3>& outVertices, 
	std::vector<glm::vec2>& outUVs, 
	std::vector<glm::vec3>& outNormals)
{
	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> tempVertices;
	std::vector<glm::vec2> tempUVs;
	std::vector<glm::vec3> tmepNormals;

	FILE* file;
	fopen_s(&file, fileName, "r");
	if (file == NULL)
	{
		std::cout << fileName << " 을 열 수 없습니다.\n";
		fclose(file);
		return false;
	}

	while (1)
	{
		char lineHeader[OBJ_LINE_HEADER_SIZE];

		// 줄의 첫 단어 읽기
		int res = fscanf_s(file, "%s", lineHeader, OBJ_LINE_HEADER_SIZE);
		if (res == EOF)
			break;

		// 정점 좌표
		if (strcmp(lineHeader, "v") == 0)
		{
			glm::vec3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			tempVertices.push_back(vertex);
		}
		// UV 좌표
		else if (strcmp(lineHeader, "vt") == 0)
		{
			glm::vec2 uv;
			fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
			tempUVs.push_back(uv);
		}
		// 노멀 벡터
		else if (strcmp(lineHeader, "vn") == 0)
		{
			glm::vec3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			tmepNormals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
				&vertexIndex[0], &uvIndex[0], &normalIndex[0],
				&vertexIndex[1], &uvIndex[1], &normalIndex[1],
				&vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9)
			{
				std::cout << fileName << " 파일을 이 퍼서로 읽을 수 없음: (다른 옵션으로 가져와 시도)\n";
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}

	for (unsigned int i = 0; i < vertexIndices.size(); ++i)
	{
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		glm::vec3 vertex = tempVertices[vertexIndex - 1];
		glm::vec2 uv = tempUVs[uvIndex - 1];
		glm::vec3 normal = tmepNormals[normalIndex - 1];

		outVertices.push_back(vertex);
		outUVs.push_back(uv);
		outNormals.push_back(normal);
	}
	fclose(file);
	return true;
}

void CRenderer::ChangeToIndexObj(
	std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals, 
	std::vector<glm::vec3>& tangents, std::vector<glm::vec3>& bitangents,
	std::vector<glm::vec3>& outVertices, std::vector<glm::vec2>& outUVs, std::vector<glm::vec3>& outNormals, 
	std::vector<glm::vec3>& outTangents, std::vector<glm::vec3>& outBitangents, 
	std::vector<unsigned int>& indices)
{
	struct Data {
		glm::vec3 vertex;
		glm::vec2 uv;
		glm::vec3 normal;

		bool operator<(Data rhs) const {
			return memcmp(this, &rhs, sizeof(Data)) > 0;
		}
	};

	std::map<Data, unsigned int> dataMap;

	unsigned int index = 0;
	auto itUVs = uvs.begin();
	auto itNormals = normals.begin();
	auto itTangents = tangents.begin();
	auto itBitangents = bitangents.begin();

	unsigned int MaxIndex = INT_MAX;//65535;

	// 데이터 인덱스 화
	for (auto itVertices = vertices.begin(); itVertices != vertices.end(); ++itVertices)
	{
		Data data;
		data.vertex = *itVertices;
		data.uv = *itUVs;
		data.normal = *itNormals;
	
		auto it = dataMap.find(data);
		if (it == dataMap.end())
		{
			dataMap.insert(std::pair<Data, unsigned int>(data, index));
			indices.push_back(index);
			outVertices.push_back(data.vertex);
			outUVs.push_back(data.uv);
			outNormals.push_back(data.normal);
			outTangents.push_back(*itTangents);
			outBitangents.push_back(*itBitangents);
			index++;
		}
		else
		{
			indices.push_back(it->second);

			outTangents[it->second] += *itTangents;
			outBitangents[it->second] += *itBitangents;
		}

		itUVs++;
		itNormals++;
		itTangents++;
		itBitangents++;
	}
}

void CRenderer::CalculateTangentAndBitangent(
	std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals, 
	std::vector<glm::vec3>& tangents, std::vector<glm::vec3>& bitangents)
{
	for (int i = 0; i < vertices.size(); i += 3) {
		glm::vec3 deltaPos1 = vertices[i + 1] - vertices[i];
		glm::vec3 deltaPos2 = vertices[i + 2] - vertices[i];
		glm::vec2 deltaUV1 = uvs[i + 1] - uvs[i];
		glm::vec2 deltaUV2 = uvs[i + 2] - uvs[i];

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
		glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

		for (int n = 0; n < 3; ++n) {
			tangents.push_back(tangent);
		}
		for (int n = 0; n < 3; ++n) {
			bitangents.push_back(bitangent);
		}
	}

	for (int i = 0; i < vertices.size(); ++i) {
		tangents[i] = glm::normalize(tangents[i] - normals[i] * glm::dot(normals[i], tangents[i]));
		if (glm::dot(glm::cross(normals[i], tangents[i]), bitangents[i]) < 0.0f) {
			tangents[i] *= -1.0f;
		}
	}
}

GLuint CRenderer::SetVertexAttribute(
	std::vector<glm::vec3>& vertices, 
	std::vector<glm::vec2>& uvs, 
	std::vector<glm::vec3>& normals)
{
	GLuint VAO;
	GLuint VBOposition;
	GLuint VBOuv;
	GLuint VBOnormal;

	// vao와 vbo 객체 생성
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBOposition);
	glGenBuffers(1, &VBOuv);
	glGenBuffers(1, &VBOnormal);

	// 사용할 vao 바인딩
	glBindVertexArray(VAO);

	// positionData 저장을 위한 vbo 바인딩
	glBindBuffer(GL_ARRAY_BUFFER, VBOposition);

	// positionData 데이터 입력
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	// 현재 바인딩되어있는 VBO를 쉐이더 프로그램과 연결: 0번째 attribute
	glVertexAttribPointer(
		0,				// 0번째 속성, 쉐이더의 레이아웃과 맞추어야 됨
		3,				// 크기
		GL_FLOAT,		// 타입
		GL_FALSE,		// 정규화 여부
		0,				// 다음과의 간격
		(void*)0);		// 배열 버퍼의 오프셋

	glEnableVertexAttribArray(0);

	// uvData 저장을 위한 vbo 바인딩
	glBindBuffer(GL_ARRAY_BUFFER, VBOuv);

	// uvData 데이터 입력
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	// 현재 바인딩되어있는 VBO를 쉐이더 프로그램과 연결: 1번째 attribute
	glVertexAttribPointer(
		1,				// 1번째 속성, 쉐이더의 레이아웃과 맞추어야 됨
		2,				// 크기
		GL_FLOAT,		// 타입
		GL_FALSE,		// 정규화 여부
		0,				// 다음과의 간격
		(void*)0);		// 배열 버퍼의 오프셋

	glEnableVertexAttribArray(1);

	// normalData 저장을 위한 vbo 바인딩
	glBindBuffer(GL_ARRAY_BUFFER, VBOnormal);

	// normalData 데이터 입력
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	// 현재 바인딩되어있는 VBO를 쉐이더 프로그램과 연결: 2번째 attribute
	glVertexAttribPointer(
		2,				// 0번째 속성, 쉐이더의 레이아웃과 맞추어야 됨
		3,				// 크기
		GL_FLOAT,		// 타입
		GL_FALSE,		// 정규화 여부
		0,				// 다음과의 간격
		(void*)0);		// 배열 버퍼의 오프셋

	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	glDeleteBuffers(1, &VBOposition);
	glDeleteBuffers(1, &VBOuv);
	glDeleteBuffers(1, &VBOnormal);

	return VAO;
}


GLuint CRenderer::SetVertexAttributeWithElementBufferObject(
	std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals, 
	std::vector<glm::vec3>& tangents, std::vector<glm::vec3>& bitangents, 
	std::vector<unsigned int>& indices)
{
	GLuint VAO;
	GLuint VBOposition;
	GLuint VBOuv;
	GLuint VBOnormal;
	GLuint VBOtangent;
	GLuint VBObitangent;
	GLuint EBO;

	// vao와 vbo 객체 생성
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBOposition);
	glGenBuffers(1, &VBOuv);
	glGenBuffers(1, &VBOnormal);
	glGenBuffers(1, &VBOtangent);
	glGenBuffers(1, &VBObitangent);
	glGenBuffers(1, &EBO);

	// 사용할 vao 바인딩
	glBindVertexArray(VAO);

	// positionData 저장을 위한 vbo 바인딩
	glBindBuffer(GL_ARRAY_BUFFER, VBOposition);

	// positionData 데이터 입력
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	// 현재 바인딩되어있는 VBO를 쉐이더 프로그램과 연결: 0번째 attribute
	glVertexAttribPointer(
		0,				// 0번째 속성, 쉐이더의 레이아웃과 맞추어야 됨
		3,				// 크기
		GL_FLOAT,		// 타입
		GL_FALSE,		// 정규화 여부
		0,				// 다음과의 간격
		(void*)0);		// 배열 버퍼의 오프셋

	glEnableVertexAttribArray(0);

	// uvData 저장을 위한 vbo 바인딩
	glBindBuffer(GL_ARRAY_BUFFER, VBOuv);

	// uvData 데이터 입력
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	// 현재 바인딩되어있는 VBO를 쉐이더 프로그램과 연결: 1번째 attribute
	glVertexAttribPointer(
		1,				// 1번째 속성, 쉐이더의 레이아웃과 맞추어야 됨
		2,				// 크기
		GL_FLOAT,		// 타입
		GL_FALSE,		// 정규화 여부
		0,				// 다음과의 간격
		(void*)0);		// 배열 버퍼의 오프셋

	glEnableVertexAttribArray(1);

	// normalData 저장을 위한 vbo 바인딩
	glBindBuffer(GL_ARRAY_BUFFER, VBOnormal);

	// normalData 데이터 입력
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	// 현재 바인딩되어있는 VBO를 쉐이더 프로그램과 연결: 2번째 attribute
	glVertexAttribPointer(
		2,				// 2번째 속성, 쉐이더의 레이아웃과 맞추어야 됨
		3,				// 크기
		GL_FLOAT,		// 타입
		GL_FALSE,		// 정규화 여부
		0,				// 다음과의 간격
		(void*)0);		// 배열 버퍼의 오프셋

	glEnableVertexAttribArray(2);

	// tangentData 저장을 위한 vbo 바인딩
	glBindBuffer(GL_ARRAY_BUFFER, VBOtangent);

	// tangentData 데이터 입력
	glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), &tangents[0], GL_STATIC_DRAW);

	// 현재 바인딩되어있는 VBO를 쉐이더 프로그램과 연결: 3번째 attribute
	glVertexAttribPointer(
		3,				// 3번째 속성, 쉐이더의 레이아웃과 맞추어야 됨
		3,				// 크기
		GL_FLOAT,		// 타입
		GL_FALSE,		// 정규화 여부
		0,				// 다음과의 간격
		(void*)0);		// 배열 버퍼의 오프셋

	glEnableVertexAttribArray(3);

	// bitangentData 저장을 위한 vbo 바인딩
	glBindBuffer(GL_ARRAY_BUFFER, VBObitangent);

	// bitangentData 데이터 입력
	glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(glm::vec3), &bitangents[0], GL_STATIC_DRAW);

	// 현재 바인딩되어있는 VBO를 쉐이더 프로그램과 연결: 4번째 attribute
	glVertexAttribPointer(
		4,				// 4번째 속성, 쉐이더의 레이아웃과 맞추어야 됨
		3,				// 크기
		GL_FLOAT,		// 타입
		GL_FALSE,		// 정규화 여부
		0,				// 다음과의 간격
		(void*)0);		// 배열 버퍼의 오프셋

	glEnableVertexAttribArray(4);

	// 인덱스 바인드 및 데이터 입력
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &VBOposition);
	glDeleteBuffers(1, &VBOuv);
	glDeleteBuffers(1, &VBOnormal);
	glDeleteBuffers(1, &VBOtangent);
	glDeleteBuffers(1, &VBObitangent);
	glDeleteBuffers(1, &EBO);

	return VAO;
}

void CRenderer::SetVertexAttributeFor2DRectangle()
{
	// 정점 배열 생성
	glGenVertexArrays(1, &m_2DRectangleVAO);
	glGenBuffers(1, &m_2DRectangleVBO);

	glBindVertexArray(m_2DRectangleVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_2DRectangleVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void CRenderer::SetVertexAttributeForSkybox()
{
	float skyboxVertices[] = {
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	GLuint vbo;

	// 정점 배열 생성
	glGenVertexArrays(1, &m_SkyboxVAO);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 18, skyboxVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
}

void CRenderer::CreateSceneFBOAndTexture()
{
	// 프레임 버퍼 생성 및 바인드
	glGenFramebuffers(1, &m_SceneFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_SceneFBO);

	// 멀티 셈플링 텍스쳐 버퍼 생성 및 바인드
	glGenTextures(1, &m_SceneTexture);
	glGenTextures(1, &m_BloomTexture);


	for (int i = 0; i < 2; ++i) {
		GLuint texture = m_SceneTexture;
		if (i == 1) {
			texture = m_BloomTexture;
		}
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_SIZE_X, WINDOW_SIZE_Y, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture, 0);
	}

	// 렌더링 버퍼 생성 및 바인드
	glGenRenderbuffers(1, &m_RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_SIZE_X, WINDOW_SIZE_Y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);

	GLuint colorAttachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, colorAttachments);

	// 프레임 버퍼 상태 확인
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "프레임 버퍼 생성이 완료되지 않았습니다.\n";

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void CRenderer::SetDisplayRect()
{
	GLuint vbo;

	float vertices[] = {
		// 위치		   // UV
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	// 평면 vao 설정
	glGenVertexArrays(1, &m_DisplayVAO);
	glGenBuffers(1, &vbo);
	glBindVertexArray(m_DisplayVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

GLuint CRenderer::ReadBMPTextureFile(const char* fileName)
{
	// BMP 파일의 헤더 읽기
	unsigned char header[54];   // 각 BMP 파일는 54비트의 해더로 시작
	unsigned int dataPos;       // 실제 데이터 시작 위치
	unsigned int width, height;
	unsigned int imageSize;     // width * height * 3

	// 실제 RGB 데이터
	unsigned char* data;

	// 파일 열기
	FILE* file;
	fopen_s(&file, fileName, "rb");

	if (!file) {
		std::cout << fileName << " 파일 오픈 실패\n";
		return EMPTY_VALUE;
	}

	fread(header, 1, 54, file);

	if (header[0] != 'B' || header[1] != 'M') {
		std::cout << "BMP 파일이 아닙니다.\n";
		return EMPTY_VALUE;
	}

	dataPos = *(int*)&header[0x0A];
	imageSize = *(int*)&header[0x22];
	width = *(int*)&header[0x12];
	height = *(int*)&header[0x16];

	// 데이터 누락 검사
	if (imageSize == 0) imageSize = width * height * 3;
	if (dataPos == 0) dataPos = 54;

	data = new unsigned char[imageSize];

	fread(data, 1, imageSize, file);

	fclose(file);

	// OpenGL texture 생성
	GLuint textureID;
	glGenTextures(1, &textureID);

	// 텍스쳐 바인드
	glBindTexture(GL_TEXTURE_2D, textureID);

	// 텍스쳐 파라미터 설정
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// OpenGL에 이미지 넘김
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	delete[] data;

	return textureID;
}

GLuint CRenderer::ReadPNGTextureFile(const char* fileName)
{
	int width, height;
	int numberOfChannel;

	// 이미지 상하 반전(이미지가 반전되 나올경우 사용)
	stbi_set_flip_vertically_on_load(true);

	// png 이미지 파일 로드
	unsigned char* data;
	data = stbi_load(fileName, &width, &height, &numberOfChannel, 0);

	if (data == NULL)
	{
		std::cout << fileName << "을 불러올 수 없습니다.\n";
		return EMPTY_VALUE;
	}

	GLenum format;
	if (numberOfChannel == 1)
		format = GL_RED;
	else if (numberOfChannel == 3)
		format = GL_RGB;
	else if (numberOfChannel == 4)
		format = GL_RGBA;

	// OpenGL texture 생성
	GLuint textureID;
	glGenTextures(1, &textureID);

	// 텍스쳐 바인드
	glBindTexture(GL_TEXTURE_2D, textureID);

	// 텍스쳐 파라미터 설정
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// OpenGL에 이미지 넘김
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);

	stbi_set_flip_vertically_on_load(false);

	return textureID;
}

void CRenderer::UseShader(int type)
{
	if (type == TEXT_SHADER)
	{
		glUseProgram(m_TextShaderProgramID);
	}
	else if (type == UI_SHADER)
	{
		glUseProgram(m_2dUIShaderProgramID);
	}
	else if (type == BILLBOARD_SHADER)
	{
		glUseProgram(m_BillboardShaderProgramID);
	}
	else if (type == BILLBOARD_SPRITE_SHADER)
	{
		glUseProgram(m_BillboardSpriteShaderProgramID);
	}
	else if (type == SKYBOX_SHADER)
	{
		glUseProgram(m_SkyboxShaderProgramID);
	}
	else if (type == DEPTH_SHADER)
	{
		glUseProgram(m_DepthShaderProgramID);
	}
	else if (type == BOUNDING_BOX_SHADER)
	{
		glUseProgram(m_BoundingBoxShaderProgramID);
	}
	else if (type == BORDER_SHADER)
	{
		glUseProgram(m_BorderShaderProgramID);
	}
	else if (type == TOTAL_SCENE_SHADER)
	{
		glUseProgram(m_TotalSceneShaderProgramID);
	}
	else if (type == INSTANCE_SHADER)
	{
		glUseProgram(m_InstanceShaderProgramID);
	}
	else if (type == BILLBOARD_TEXT_SHADER) 
	{
		glUseProgram(m_BillboardTextShaderProgramID);
	}
	else if (type == DAMAGE_ZONE_SHADER)
	{
		glUseProgram(m_DamageZoneShaderProgramID);
	}
	else if (type == WATER_SHADER)
	{
		glUseProgram(m_WaterShaderProgramID);
	}
	else if (type == GRAVITY_PARTICLE_SHADER)
	{
		glUseProgram(m_GravityParticleShaderProgramID);
	}
	else if (type == DISPLAY_FXAA_SHADER)
	{
		glUseProgram(m_DisplayFXAAShaderProgramID);
	}
	else if (type == Blur_SHADER)
	{
		glUseProgram(m_BlurShaderProgramID);
	}
	else 
	{
		glUseProgram(m_ShaderProgramID);
	}
}

void CRenderer::CreateDepthMap()
{
	// 프레임 버퍼 생성
	glGenFramebuffers(1, &m_DepthMapFBO);

	// 텍스쳐 생성 및 바인드
	glGenTextures(1, &m_DepthMap);
	glBindTexture(GL_TEXTURE_2D, m_DepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
		SHADOW_RESOLUTION, SHADOW_RESOLUTION, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	// 텍스쳐 파라미터 설정
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// 프레임 버퍼 바인드
	glBindFramebuffer(GL_FRAMEBUFFER, m_DepthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CRenderer::GetDepthMap(GLuint* depthMapFBO, GLuint* depthMap)
{
	*depthMapFBO = m_DepthMapFBO;
	*depthMap = m_DepthMap;
}

GLuint CRenderer::GetDepthMapFBO()
{
	return m_DepthMapFBO;
}

GLuint CRenderer::GetSceneFBO()
{
	return m_SceneFBO;
}

GLuint CRenderer::GetSceneTexture()
{
	return m_SceneTexture;
}

GLuint CRenderer::GetSaveRenderingResultFBO()
{
	return m_SaveRenderingResultFBO;
}

GLuint CRenderer::GetSaveRenderingResultTexture() const
{
	return m_SaveRenderingResultTexture;
}

GLuint CRenderer::GetDisplayFXAAFBO()
{
	return m_DisplayFXAAFBO;
}

GLuint CRenderer::GetDisplayFXAATexture()
{
	return m_DisplayFXAATexture;
}

void CRenderer::RenderTotalScene(bool blur, bool invert)
{
	GLuint sceneLocation = glGetUniformLocation(m_TotalSceneShaderProgramID, "u_sceneTexture");
	glUniform1i(sceneLocation, 0);
	GLuint bloomLocation = glGetUniformLocation(m_TotalSceneShaderProgramID, "u_blurTexture");
	glUniform1i(bloomLocation, 1);

	GLuint blurLocation = glGetUniformLocation(m_TotalSceneShaderProgramID, "u_bBlur");
	glUniform1i(blurLocation, blur);
	GLuint invertLocation = glGetUniformLocation(m_TotalSceneShaderProgramID, "u_bColorInvert");
	glUniform1i(invertLocation, invert);

	GLuint lineLocation = glGetUniformLocation(m_TotalSceneShaderProgramID, "u_iEffectOnOffLine");
	glUniform1i(lineLocation, m_iDisplayPostEffectOnOffLine);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_SceneTexture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_BlurTexture[1]);


	glBindVertexArray(m_DisplayVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CRenderer::SetDisplayResolution(int x, int y)
{
	m_iResoultionX = x;
	m_iResoultionY = y;
}

void CRenderer::GetDisplayResolution(int* x, int* y)
{
	*x = m_iResoultionX;
	*y = m_iResoultionY;
}

void CRenderer::SetTextureAndRenderBufferResize()
{
	glBindTexture(GL_TEXTURE_2D, m_SceneTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_iResoultionX, m_iResoultionY, 0, GL_RGBA, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, m_BloomTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_iResoultionX, m_iResoultionY, 0, GL_RGBA, GL_FLOAT, NULL);

	glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_iResoultionX, m_iResoultionY);

	glBindTexture(GL_TEXTURE_2D, m_SaveRenderingResultTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_iResoultionX, m_iResoultionY, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, m_DisplayFXAATexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_iResoultionX, m_iResoultionY, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	
	for (int i = 0; i < 2; ++i) {
		glBindTexture(GL_TEXTURE_2D, m_BlurTexture[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_iResoultionX, m_iResoultionY, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Renderer::SModel* CRenderer::GetModelData(int index)
{
	return GetModel(index);
}

void CRenderer::RenderChangeSkillDeckParticle(int textureID, float timeInSec)
{
	glDisable(GL_DEPTH_TEST);

	glUseProgram(m_BasicMovingParticleProgramID);

	// 화면 크기에 맞추어 직교 투영
	glm::mat4 projection = glm::ortho(0.0f, (float)m_iResoultionX, 0.0f, (float)m_iResoultionY);

	GLuint projectionLocation = glGetUniformLocation(m_BasicMovingParticleProgramID, "u_mProjectionTransform");
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

	GLuint colorLocation = glGetUniformLocation(m_BasicMovingParticleProgramID, "u_vColor");
	glUniform4f(colorLocation, 1.0f, 1.0f, 1.0f, 1.0f);

	GLuint timeLocation = glGetUniformLocation(m_BasicMovingParticleProgramID, "u_fTime");
	glUniform1f(timeLocation, timeInSec);

	GLuint lifeTimeLocation = glGetUniformLocation(m_BasicMovingParticleProgramID, "u_fLifeTime");
	glUniform1f(lifeTimeLocation, 1.0f);

	// 텍스쳐 활성화 및 바인드
	GLuint textureLocation = glGetUniformLocation(m_BasicMovingParticleProgramID, "u_texture");
	glUniform1i(textureLocation, 0);
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glBindVertexArray(m_ChangeSkillDeckParticleVAO);

	glDrawArrays(GL_TRIANGLES, 0, 34 * 54 * 6);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnable(GL_DEPTH_TEST);
}

void CRenderer::RenderChangeStateEffect(float time, bool heroWin)
{
	glUseProgram(m_ChangeStateEffectShaderProgramID);

	GLuint textureLocation = glGetUniformLocation(m_ChangeStateEffectShaderProgramID, "u_sceneTexture");
	glUniform1i(textureLocation, 0);
	GLuint texturePreStateRenderingStateLocation
		= glGetUniformLocation(m_ChangeStateEffectShaderProgramID, "u_prevStateRenderingResultTexture");
	glUniform1i(texturePreStateRenderingStateLocation, 1);

	GLuint timeLocation = glGetUniformLocation(m_ChangeStateEffectShaderProgramID, "u_fTime");
	glUniform1f(timeLocation, time);

	GLuint heroWinLocation = glGetUniformLocation(m_ChangeStateEffectShaderProgramID, "u_bHeroWin");
	glUniform1i(heroWinLocation, heroWin);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_SceneTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_SaveRenderingResultTexture);

	glBindVertexArray(m_DisplayVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CRenderer::RenderGravityParticle(GLuint vao, int vertexCount, int textureID,
	glm::vec3 position, glm::vec3 size, glm::vec4 color, float timeInSec, bool loop, bool gravity)
{
	glDepthMask(GL_FALSE);

	glUniform3f(m_translateForGravityParticle, position.x, position.y, position.z);
	glUniform3f(m_scaleForGravityParticle, size.x, size.y, size.z);
	glUniform1i(m_textureForGravityParticle, 0);
	glUniform4f(m_colorForGravityParticle, color.r, color.g, color.b, color.a);
	glUniform1f(m_timeForGravityParticle, timeInSec);
	glUniform1i(m_loopForGravityParticle, loop);
	glUniform1i(m_applyGravityForGravityParticle, gravity);

	// 사용할 VAO 불러오기
	glBindVertexArray(vao);

	// 사용할 텍스쳐 불러오기
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// 삼각형 그리기
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDepthMask(GL_TRUE);
}

void CRenderer::RenderDamageZone(int modelIndex,
	glm::vec3 position, glm::vec3 rotation, glm::vec3 size, float timeRate)
{
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);

	Renderer::SModel* model = GetModel(modelIndex);

	// 이동 변환 행렬
	glm::mat4 traslateMat = glm::mat4(1.0f);
	// 크기 변환 행렬
	glm::mat4 scaleMat = glm::mat4(1.0f);

	// 이동 변환
	traslateMat = glm::translate(traslateMat, position);

	// 각 축에 대한 쿼터니언
	glm::quat modelQuat
		= glm::quat(glm::vec3(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z)));
	glm::mat4 rotationMat = glm::toMat4(modelQuat);

	// 크기 변환
	scaleMat = glm::scale(scaleMat, size);

	// 모델 변환 행렬 설정
	glUniformMatrix4fv(m_modelMatrixLocationForDamageZone, 1, GL_FALSE,
		glm::value_ptr(traslateMat * rotationMat * scaleMat));

	glUniform1f(m_timeRateLocationForDamageZone, timeRate);

	// 사용할 VAO 불러오기
	glBindVertexArray(model->VAO);

	// 삼각형 그리기
	glDrawElements(GL_TRIANGLES, model->iNumberOfVertices, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Renderer::SModel* CRenderer::GetModel(int index)
{
	auto it = m_ModelMap.find(index);

	return (*it).second;
}

Renderer::STexture* CRenderer::GetTexture(int index)
{
	auto it = m_TextureMap.find(index);

	return (*it).second;
}

void CRenderer::SetVAOForChangeSkillDeckParticle()
{
	// 위치 계산
	float xpos = 0.45f * WINDOW_SIZE_X;
	float ypos = 0.4f * WINDOW_SIZE_Y;

	// 크기 계산
	float w = 5;
	float h = 5;

	float* vertices = new float[4 * 6 * 34 * 54];
	float* velocity = new float[2 * 6 * 34 * 54];

	int index = 0;
	for (int x = 0; x < 34; ++x) {
		for (int y = 0; y < 54; ++y) {
			// 0
			// 위치, uv
			vertices[index] = xpos + x * h;
			index++;
			vertices[index] = ypos + h + y * w;
			index++;
			vertices[index] = 0.f;
			index++;
			vertices[index] = 1.f;
			index++;

			// 1
			vertices[index] = xpos + x * h;
			index++;
			vertices[index] = ypos + y * w;
			index++;
			vertices[index] = 0.f;
			index++;
			vertices[index] = 0.f;
			index++;

			// 2
			vertices[index] = xpos + w + x * h;
			index++;
			vertices[index] = ypos + y * w;
			index++;
			vertices[index] = 1.f;
			index++;
			vertices[index] = 0.f;
			index++;

			// 3
			vertices[index] = xpos + x * h;
			index++;
			vertices[index] = ypos + h + y * w;
			index++;
			vertices[index] = 0.f;
			index++;
			vertices[index] = 1.f;
			index++;

			// 4
			vertices[index] = xpos + w + x * h;
			index++;
			vertices[index] = ypos + y * w;
			index++;
			vertices[index] = 1.f;
			index++;
			vertices[index] = 0.f;
			index++;

			// 5
			vertices[index] = xpos + w + x * h;
			index++;
			vertices[index] = ypos + h + y * w;
			index++;
			vertices[index] = 1.f;
			index++;
			vertices[index] = 1.f;
			index++;
		}
	}

	glm::vec2 center = glm::vec2(xpos + 17 * 5.f, ypos + 27 * 5.f);

	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_int_distribution<int> uid(300, 1000);

	index = 0;
	for (int x = 0; x < 34; ++x) {
		for (int y = 0; y < 54; ++y) {
			glm::vec2 dir = (glm::normalize(center - glm::vec2(xpos + x * 5.f, ypos + y * 5.f)));

			int randomVel = uid(dre);
			glm::vec2 vel = glm::vec2(-dir.x * randomVel, -dir.y * randomVel);

			for (int i = 0; i < 6; ++i) {
				velocity[index] = vel.x;
				index++;
				velocity[index] = vel.y;
				index++;
			}
		}
	}

	// 정점 배열 생성
	GLuint vertexVBO;
	GLuint velocityVBO;

	glGenVertexArrays(1, &m_ChangeSkillDeckParticleVAO);
	glGenBuffers(1, &vertexVBO);
	glGenBuffers(1, &velocityVBO);

	glBindVertexArray(m_ChangeSkillDeckParticleVAO);

	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 34 * 54 * 6 * 4, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, velocityVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 34 * 54 * 6 * 2, velocity, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	delete[] vertices;
	delete[] velocity;
}

void CRenderer::CreateSaveRenderingResultFBOAndTexture()
{
	// 프레임 버퍼 생성 및 바인드
	glGenFramebuffers(1, &m_SaveRenderingResultFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_SaveRenderingResultFBO);

	// 텍스쳐 버퍼 생성 및 바인드
	glGenTextures(1, &m_SaveRenderingResultTexture);
	glBindTexture(GL_TEXTURE_2D, m_SaveRenderingResultTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_SIZE_X, WINDOW_SIZE_Y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_SaveRenderingResultTexture, 0);

	// 프레임 버퍼 상태 확인
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "렌더링 결과 저장 프레임 버퍼 생성이 완료되지 않았습니다.\n";

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CRenderer::CreateDisplayWithFXAAFBOAndTexture()
{
	// 프레임 버퍼 생성 및 바인드
	glGenFramebuffers(1, &m_DisplayFXAAFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_DisplayFXAAFBO);

	// 텍스쳐 버퍼 생성 및 바인드
	glGenTextures(1, &m_DisplayFXAATexture);
	glBindTexture(GL_TEXTURE_2D, m_DisplayFXAATexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_SIZE_X, WINDOW_SIZE_Y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_DisplayFXAATexture, 0);

	// 프레임 버퍼 상태 확인
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "렌더링 결과 저장 프레임 버퍼 생성이 완료되지 않았습니다.\n";

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CRenderer::CreateBlurFBOAndTexture()
{
	// 프레임 버퍼 생성 및 바인드
	glGenFramebuffers(2, m_BlurFBO);
	glGenTextures(2, m_BlurTexture);

	for (int i = 0; i < 2; ++i) {
		glBindFramebuffer(GL_FRAMEBUFFER, m_BlurFBO[i]);

		// 텍스쳐 버퍼 생성 및 바인드
		glBindTexture(GL_TEXTURE_2D, m_BlurTexture[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WINDOW_SIZE_X, WINDOW_SIZE_Y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BlurTexture[i], 0);

		// 프레임 버퍼 상태 확인
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "렌더링 결과 저장 프레임 버퍼 생성이 완료되지 않았습니다.\n";

	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CRenderer::SaveGettingUniformLocation()
{
	// 기본 쉐이더 유니폼 위치 설정
	glUseProgram(m_ShaderProgramID);
	m_modelMatrixLocation = glGetUniformLocation(m_ShaderProgramID, "u_mModelTransform");

	m_materialAmbientLocation = glGetUniformLocation(m_ShaderProgramID, "u_material.ambient");
	m_materialDiffuseLocation = glGetUniformLocation(m_ShaderProgramID, "u_material.diffuse");
	m_materialSpecularLocation = glGetUniformLocation(m_ShaderProgramID, "u_material.specular");

	m_materialShininessLocation = glGetUniformLocation(m_ShaderProgramID, "u_material.shininess");
	
	m_materialNormalLocation = glGetUniformLocation(m_ShaderProgramID, "u_material.normal");

	m_colorLocation = glGetUniformLocation(m_ShaderProgramID, "u_vColor");
	m_isLightSourceLocation = glGetUniformLocation(m_ShaderProgramID, "u_bIsLightSource");

	// 깊이 쉐이더 유니폼 위치 설정
	glUseProgram(m_DepthShaderProgramID);
	m_modelMatrixForDepthLocation = glGetUniformLocation(m_DepthShaderProgramID, "u_mModelTransform");

	// 테두리 쉐이더 유니폼 위치 설정
	glUseProgram(m_BorderShaderProgramID);
	m_modelMatrixForBorderLocation = glGetUniformLocation(m_BorderShaderProgramID, "u_mModelTransform");
	m_colorForBorderLocation = glGetUniformLocation(m_BorderShaderProgramID, "u_vColor");

	// 텍스트 쉐이더 유니폼 위치 설정
	glUseProgram(m_TextShaderProgramID);
	m_projectionMatrixForTextLocation = glGetUniformLocation(m_TextShaderProgramID, "u_mProjectionTransform");
	m_textLocation = glGetUniformLocation(m_TextShaderProgramID, "u_texture");
	m_colorForTextLocation = glGetUniformLocation(m_TextShaderProgramID, "u_vColor");

	// 2d 텍스쳐 쉐이더 유니폼 위치 설정	
	glUseProgram(m_2dUIShaderProgramID);
	m_projectionMatrixForTexture2DLocation = glGetUniformLocation(m_2dUIShaderProgramID, "u_mProjectionTransform");
	m_textureForTexture2DLocation = glGetUniformLocation(m_2dUIShaderProgramID, "u_texture");
	m_colorForTexture2DLocation = glGetUniformLocation(m_2dUIShaderProgramID, "u_vColor");

	// 빌보드 텍스쳐 쉐이더 유니폼 위치 설정
	glUseProgram(m_BillboardShaderProgramID);
	m_translateForTexture3DLocation = glGetUniformLocation(m_BillboardShaderProgramID, "u_vTranslateTransform");
	m_scaleForTexture3DLocation = glGetUniformLocation(m_BillboardShaderProgramID, "u_vScaleTransform");
	m_textureForTexture3DLocation = glGetUniformLocation(m_BillboardShaderProgramID, "u_texture");
	m_colorForTexture3DLocation = glGetUniformLocation(m_BillboardShaderProgramID, "u_vColor");

	// 빌보드 텍스트 쉐이더 유니폼 위치 설정
	glUseProgram(m_BillboardTextShaderProgramID);
	m_translateForText3DLocation = glGetUniformLocation(m_BillboardTextShaderProgramID, "u_vTranslateTransform");
	m_scaleForText3DLocation = glGetUniformLocation(m_BillboardTextShaderProgramID, "u_vScaleTransform");
	m_text3DLocation = glGetUniformLocation(m_BillboardTextShaderProgramID, "u_texture");
	m_colorForText3DLocation = glGetUniformLocation(m_BillboardTextShaderProgramID, "u_vColor");

	// 빌보드 스프라이트 시트 쉐이더 유니폼 위치 설정
	glUseProgram(m_BillboardSpriteShaderProgramID);
	m_translateForBillboardSpriteSheetLocation = glGetUniformLocation(m_BillboardSpriteShaderProgramID, "u_vTranslateTransform");
	m_scaleForBillboardSpriteSheetLocation = glGetUniformLocation(m_BillboardSpriteShaderProgramID, "u_vScaleTransform");
	m_textureForBillboardSpriteSheetLocation = glGetUniformLocation(m_BillboardSpriteShaderProgramID, "u_texture");
	m_frameForBillboardSpriteSheetLocation = glGetUniformLocation(m_BillboardSpriteShaderProgramID, "u_vFrame");
	m_colorForBillboardSpriteSheetLocation = glGetUniformLocation(m_BillboardSpriteShaderProgramID, "u_vColor");

	// 스카이박스 쉐이더 유니폼 위치 설정
	glUseProgram(m_SkyboxShaderProgramID);
	m_textureForSkyboxLocation = glGetUniformLocation(m_SkyboxShaderProgramID, "u_cubemap");
	m_texture2ForSkyboxLocation = glGetUniformLocation(m_SkyboxShaderProgramID, "u_cubemap2");
	m_blendRateForSkyboxLocation = glGetUniformLocation(m_SkyboxShaderProgramID, "u_fBlendRate");


	// 대미지 존 쉐이더 유니폼 위치 설정
	glUseProgram(m_DamageZoneShaderProgramID);
	m_modelMatrixLocationForDamageZone = glGetUniformLocation(m_DamageZoneShaderProgramID, "u_mModelTransform");
	m_timeRateLocationForDamageZone = glGetUniformLocation(m_DamageZoneShaderProgramID, "u_fTimeRate");

	// 물 효과 쉐이더 유니폼 위치 설정
	glUseProgram(m_WaterShaderProgramID);
	m_modelMatrixLocationForWater = glGetUniformLocation(m_WaterShaderProgramID, "u_mModelTransform");
	m_textureForWaterLocation = glGetUniformLocation(m_WaterShaderProgramID, "u_cubemap");
	m_texture2ForWaterLocation = glGetUniformLocation(m_WaterShaderProgramID, "u_cubemap2");
	m_blendRateForWaterLocation = glGetUniformLocation(m_WaterShaderProgramID, "u_fBlendRate");
	m_timeForWaterLocation = glGetUniformLocation(m_WaterShaderProgramID, "u_fTime");

	// 중력을 받는 쉐이더 유니폼 위치 설정
	glUseProgram(m_GravityParticleShaderProgramID);
	m_translateForGravityParticle = glGetUniformLocation(m_GravityParticleShaderProgramID, "u_vTranslateTransform");
	m_scaleForGravityParticle = glGetUniformLocation(m_GravityParticleShaderProgramID, "u_vScaleTransform");
	m_textureForGravityParticle = glGetUniformLocation(m_GravityParticleShaderProgramID, "u_texture");
	m_colorForGravityParticle = glGetUniformLocation(m_GravityParticleShaderProgramID, "u_vColor");
	m_timeForGravityParticle = glGetUniformLocation(m_GravityParticleShaderProgramID, "u_fTime");
	m_loopForGravityParticle = glGetUniformLocation(m_GravityParticleShaderProgramID, "u_bLoop");
	m_applyGravityForGravityParticle = glGetUniformLocation(m_GravityParticleShaderProgramID, "u_bApplyGravity");

	// 인스턴스 쉐이더 유니폼 위치
	glUseProgram(m_InstanceShaderProgramID);
	m_textureForInstanceLocation = glGetUniformLocation(m_InstanceShaderProgramID, "u_texture");
	m_timeForInstanceLocation  = glGetUniformLocation(m_InstanceShaderProgramID, "u_fTime");

	glUseProgram(0);
}
