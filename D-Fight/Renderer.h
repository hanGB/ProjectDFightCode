/*
* Renderer
*	
*	Singleton pattern���� ���� �ʿ� 
* 
*	������ ������
*	ȭ�� �ʱ�ȭ 
*	Obj �� ������
*/

#pragma once
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include "Globals.h"
#include "ModelLoader.h"
#include "AnimationLoader.h"

namespace Renderer {
	struct SModel
	{
		GLuint VAO;
		GLuint iNumberOfVertices;
	};

	struct STexture
	{
		GLuint ambientMapTextureID;
		GLuint diffuseMapTextureID;
		GLuint speculaMapTextureID;
		float fShininess;
		GLuint normalMapTextureID;
	};

	struct SCharacter
	{
		GLuint textureID;
		glm::ivec2 vSize;
		glm::ivec2 vBearing;
		unsigned int iAdvance;
	};
}

class CRenderer
{
private:
	CRenderer();
	~CRenderer();

public:
	// �̱��� ���� ���� �Լ�
	static CRenderer* GetInstance();
	
private:
	// ���̴� ���α׷� ���̵�
	GLuint m_ShaderProgramID;					// �⺻(�Ϲ� ������Ʈ ��������) ���̴� ���α׷�
	GLuint m_TextShaderProgramID;				// �ؽ�Ʈ �������� ���̴� ���α׷�
	GLuint m_2dUIShaderProgramID;				// 2D �ؽ��� �������� ���̴� ���α׷�
	GLuint m_BillboardShaderProgramID;			// �����带 �̿��� �ؽ��� �������� ���̴� ���α׷�
	GLuint m_BillboardSpriteShaderProgramID;	// �����带 �̿��� ��������Ʈ ��Ʈ �������� ���̴� ���α׷�
	GLuint m_SkyboxShaderProgramID;				// ť����� �̿��� ��ī�̹ڽ� �������� ���̴� ���α׷�
	GLuint m_DepthShaderProgramID;				// ���� ���� �̿��� �𷺼ǳ� ����Ʈ ������ ���ο� ���̴� ���α׷�
	GLuint m_BoundingBoxShaderProgramID;		// �浹 �ڽ�(�浹 ����) �������� ���̴� ���α׷�
	GLuint m_BorderShaderProgramID;				// �׵θ�(�ܰ���) �������� ���̴� ���α׷�
	GLuint m_TotalSceneShaderProgramID;			// ��ü �� �������� ���̴� ���α׷�
	GLuint m_InstanceShaderProgramID;			// �ν��Ͻ� �������� ���̴� ���α׷�
	GLuint m_BasicMovingParticleProgramID;		// �����̴� ��ƼŬ �������� ���̴� ���α׷�
	GLuint m_BillboardTextShaderProgramID;		// �����带 �̿��� �ؽ�Ʈ �������� ���̴� ���α׷�
	GLuint m_ChangeStateEffectShaderProgramID;	// ������Ʈ ���� ȿ�� �������� ���̴� ���α׷�
	GLuint m_DamageZoneShaderProgramID;			// ������ �� ���̴� ���α׷�
	GLuint m_WaterShaderProgramID;				// �� ȿ�� ���̴� ���α׷�
	GLuint m_GravityParticleShaderProgramID;	// �߷��� ����Ǵ� ��ƼŬ ���̴� ���α׷�
	GLuint m_DisplayFXAAShaderProgramID;		// ȭ�鿡 FXAA ���� �� �������� ���̴� ���α׷�
	GLuint m_BlurShaderProgramID;				// bloom�� �� ȿ�� ���̴� ���α׷�


	// �𵨰� �ؽ��� ������ ������ ���� ��
	std::map<int, Renderer::SModel*> m_ModelMap;
	std::map<int, Renderer::STexture*> m_TextureMap;

	// �ִϸ��̼� �� ������ ������ ���� ��
	std::map<std::string, CModelLoader*> m_ModelWithBoneLoaderMap;
	std::map<std::string, CAnimationLoader*> m_AnimationLoaderMap;

	// ��Ʈ ������ ���� ��
	std::map<wchar_t, Renderer::SCharacter> m_CharacterMap;
	// ��ī�̹ڽ�(ť�� ��) ������ ���� ����
	std::map<std::string, int> m_SkyboxMap;
	std::vector<GLuint> m_SkyboxVector;

	// �ؽ�Ʈ ����� ���� vao, vbo
	GLuint m_TextVAO, m_TextVBO;

	// UI ���ۿ� 2D �簢���� ���� vao;
	GLuint m_2DRectangleVAO;
	GLuint m_2DRectangleVBO;

	// ������ ���ۿ� ��� vao;
	GLuint m_PlaneModelVAO;

	// ��ī�̹ڽ� ���ۿ� ť�� vao;
	GLuint m_SkyboxVAO;

	// �׸��ڸ� ���� ������ ����, ���� ��
	GLuint m_DepthMapFBO;
	GLuint m_DepthMap;

	// �� �������� ������ ����, �ؽ���
	GLuint m_SceneFBO;
	GLuint m_SceneTexture;
	GLuint m_RBO;

	// Bloom �ؽ���
	GLuint m_BloomTexture;

	// ������ ��� ���� ������ ����, �ؽ���
	GLuint m_SaveRenderingResultFBO;
	GLuint m_SaveRenderingResultTexture;

	// ȭ�� �������� ��Ƽ ���ø� ������ ����, �ؽ���
	GLuint m_DisplayFXAAFBO;
	GLuint m_DisplayFXAATexture;

	// ȭ�� ��ü �ؽ��� �������� vao
	GLuint m_DisplayVAO = 0;

	// ȭ�� �ػ�
	int m_iResoultionX, m_iResoultionY;

	// �浹 �ڽ��� ���� �� �ε���
	int m_iBoundingBoxModelIndex;
	int m_iCapsuleModelIndex;

	// �� ��� ���� ����
	// ��ų �� ���� ��ƼŬ vao
	GLuint m_ChangeSkillDeckParticleVAO;

	// ȭ�� ��ó�� �񱳿� �� ��ġ
	int m_iDisplayPostEffectOnOffLine = 0;

	// �� ������ fbo, �ؽ���
	GLuint m_BlurFBO[2];
	GLuint m_BlurTexture[2];

public:
	// OBJ �� �ε�
	int LoadOBJModel(const char* fileName);

	// OBJ �� �ε�
	int LoadOBJModelWithIndex(const char* fileName);

	// BMP �ؽ��� �ε�
	int LoadBMPTexture(const char* ambientMapFileName, 
		const char* diffuseMapFileName,
		const char* speculaMapFileName, 
		float shininess,
		const char* normalMapFileName);

	// PNG �ؽ��� �ε�
	int LoadPNGTexture(const char* ambientMapFileName, 
		const char* diffuseMapFileName, 
		const char* speculaMapFileName, 
		float shininess,
		const char* normalMapFileName);

	// �ִϸ��̼� �� �ε�(�̸��� �ش��ϴ� �ִϸ��̼� �� �����Ͱ� ���� ��� �ε�)
	void LoadModelWithBone(const char* fileName, std::string name);
	void LoadAnimation(const char* fileName, std::string modelName, std::string animationName);

	CModelLoader* GetModelWithBone(std::string modelName);
	CAnimationLoader* GetAnimation(std::string animationName);

	// �ִϸ��̼� ����
	void DeleteModelWithBone(std::string name);
	void DeleteAnimation(std::string animationName);

	// ��ī�� �ڽ�(ť�� ��) ����
	int LoadSkybox(std::string name, std::string* fileNames);

	// OBJ �� ���� (�� index�� ���� ��� ���� ����, �Ϲ� index�� ���� �� empty_value ����)
	int DeleteModelAndSetEmptyID(int index);

	// �ؽ��� ���� (�� index�� ���� ��� ���� ����, �Ϲ� index�� ���� �� empty_value ����)
	int DeleteTextureAndSetEmptyID(int index);

	// ȭ�� ����
	void SetDisplay();

	// �ﰢ������ �̷���� �� �ε����� �׸���
	void DrawElementsWithTriangles(int modelIndex, int textureIndex,
		glm::vec3 position,
		glm::vec3 rotation,
		glm::vec3 size,
		glm::quat quaternion,
		glm::vec4 color,
		bool isLightSource,
		bool outLine);

	// ���� ���� ���� �� �ε����� ������
	void DrawElementsWithTrianglesForDepth(int modelIndex,
		glm::vec3 position,
		glm::vec3 rotation,
		glm::vec3 size,
		glm::quat quaternion);

	// �׵θ�(�ܰ���)�� �׸��� ���� �� �ε����� ������
	void DrawElementsWithTrianglesForBorder(int modelIndex,
		glm::vec3 position,
		glm::vec3 rotation,
		glm::vec3 size,
		glm::quat quaternion,
		glm::vec4 color);

	// �ִϸ��̼� �� ������
	void DrawAnimationModel(
		std::string modelName,
		int textureIndex,
		std::vector<std::vector<SVertex>>& verticesVector,
		glm::vec3 position,
		glm::vec3 rotation,
		glm::vec3 size,
		glm::quat quaternion, 
		glm::vec4 color,
		bool isLightSource,
		bool outLine);

	// ���� ���� ���� �ִϸ��̼� �� ������
	void DrawAnimationModelForDapth(
		std::string modelName,
		std::vector<std::vector<SVertex>>& verticesVector,
		glm::vec3 position,
		glm::vec3 rotation,
		glm::vec3 size,
		glm::quat quaternion);

	// �׵θ�(�ܰ���)�� �׸��� ���� �ִϸ��̼� �� ������
	void DrawAnimationModelForBorder(
		std::string modelName,
		std::vector<std::vector<SVertex>>& verticesVector,
		glm::vec3 position,
		glm::vec3 rotation,
		glm::vec3 size,
		glm::quat quaternion,
		glm::vec4 color);

	// ���� ���� ���� �ִϸ��̼� �� ������(���ؽ� ���)
	void CaculateVerticesAfterAnimation(
		std::string modelName,
		std::vector<std::vector<SVertex>>& verticesVector,
		std::vector<glm::mat4> finalBonesMatrices);

	// �浹 �ڽ� ������
	void DrawBoundingBox(
		glm::vec3 position,
		glm::vec3 rotation,
		glm::vec3 size,
		glm::vec3 volume);
	
	void DrawCapsule(
		glm::vec3 position,
		float radius,
		float height);

	void DrawWaterObject(int modelIndex,
		glm::vec3 position,
		glm::vec3 rotation,
		glm::vec3 size,
		int skybox1ID, int skybox2ID, float blendRate,
		float time);
	
	// �ؽ�Ʈ�� 2�������� ȭ�鿡 �׸���
	void RenderText(const wchar_t* text, float xRate, float yRate, float scale, glm::vec3 color);
	
	// �ؽ�Ʈ�� 3�������� ȭ�鿡 �׸���(������)
	void RenderText3D(const wchar_t* text, float scale, glm::vec3 position, glm::vec4 color);

	// �ؽ��� �̹����� 2�������� ȭ�鿡 �׸���
	void RenderTexture2D(GLuint textureID,
		glm::vec2 positionRate, glm::vec2 rotation, glm::vec2 size, glm::vec4 color);

	// �ؽ��� �̹����� 3�������� ȭ�鿡 �׸���(������)
	void RenderTexture3D(GLuint textureID,
		glm::vec3 position, glm::vec2 size, glm::vec4 color);

	// �ִϸ��̼� ��������Ʈ ��Ʈ �̹����� 2�������� ȭ�鿡 �׸���
	// - �� �� ��(spriteCount) �� �� ��°(order)�� �׸� �� ���ڷ� ����: order�� 0������ ���� spriteCount -1�� ����
	void RenderSpriteSheet2D(GLuint textureID, 
		int spriteCountX, int spriteCountY, int orderX, int orderY,
		glm::vec2 positionRate, glm::vec2 rotation, glm::vec2 size, glm::vec4 color);

	// �ִϸ��̼� ��������Ʈ ��Ʈ �̹����� 3�������� ȭ�鿡 �׸���(������) - ������ �ε��� ����� 2D�� ����
	void RenderSpriteSheet3D(GLuint textureID, 
		int spriteCountX, int spriteCountY, int orderX, int orderY,
		glm::vec3 position, glm::vec2 size, glm::vec4 color);

	// ��ī�� �ڽ� ������(blendRate�� 0.0f�� �ƴ� ��� �� �ؽ��ĸ� ȥ���Ͽ� ���)
	void DrawSkybox(int skybox1ID, int skybox2ID, float blendRate);
	
	// �ν��Ͻ� ������
	void RenderInstance(int modelIndex, GLuint textureID, GLuint ssboModelMatrices, float time, int count);

	// ȭ�� ������
	void RenderDisplayFXAA();

	// ���̴� ���α׷� ���̵� ����
	GLuint GetShaderProgram(int type) const;

	// PNG �ؽ��� ���� �б�
	GLuint ReadPNGTextureFile(const char* fileName);

	// Ÿ�� ���� ������ �Լ��� �θ��� ���� ȣ��
	void UseShader(int type);

	void GetDepthMap(GLuint* depthMapFBO, GLuint* depthMap);
	GLuint GetDepthMapFBO();

	GLuint GetSceneFBO();
	GLuint GetSceneTexture();

	GLuint GetSaveRenderingResultFBO();
	GLuint GetSaveRenderingResultTexture() const;

	GLuint GetDisplayFXAAFBO();
	GLuint GetDisplayFXAATexture();

	// ��ü �� ������
	void RenderTotalScene(bool blur, bool invert);

	// ȭ�� �ػ� ����, ���
	void SetDisplayResolution(int x, int y);
	void GetDisplayResolution(int* x, int* y);

	// �ؽ��� ����, ������ ���� ������ �缳��
	void SetTextureAndRenderBufferResize();

	Renderer::SModel* GetModelData(int index);

	// �� ��� ���� ��ƼŬ����
	// ��ų �� ���� ȿ��
	void RenderChangeSkillDeckParticle(int textureID, float timeInSec);

	void RenderChangeStateEffect(float time, bool heroWin);

	// �߷��� ���� �޴� ��ƼŬ ������
	void RenderGravityParticle(GLuint vao, int vertexCount, int textureID, 
		glm::vec3 position, glm::vec3 size, glm::vec4 color, float timeInSec, bool loop, bool gravity);

	void RenderDamageZone(int modelIndex,
		glm::vec3 position, glm::vec3 rotation, glm::vec3 size, float timeRate);

	void ContorlDisplayPostEffectOnOffLine(GameInputs* inputs, float elapsedTimeInSec);

	void ApplyBlurToBloomTexture();

private:
	// ��Ʈ �ε�
	void LoadFont(const char* fileName, int size);

	// ���̴� �ڵ� �ε�, ������
	GLuint LoadShaders(const char* fileNameVertex, const char* fileNameFragment);

	// ���� �б�
	char* BringFileToBuf(const char* fileName);
	
	// obj ���� �б�
	bool ReadOBJFile(const char* fileName,
		std::vector<glm::vec3>& outVertices,
		std::vector<glm::vec2>& outUVs,
		std::vector<glm::vec3>& outNormals);

	// �Ϲ� ���ؽ� �����͸� �ε����� ����
	void ChangeToIndexObj(
		std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals,
		std::vector<glm::vec3>& tangents, std::vector<glm::vec3>& bitangents,
		std::vector<glm::vec3>& outVertices, std::vector<glm::vec2>& outUVs,std::vector<glm::vec3>& outNormals,
		std::vector<glm::vec3>& outTangents, std::vector<glm::vec3>& outBitangents,
		std::vector<unsigned int>& indices);

	// ź��Ʈ�� ����ź��Ʈ ���� ���
	void CalculateTangentAndBitangent(
		std::vector<glm::vec3>& vertices,
		std::vector<glm::vec2>& uvs,
		std::vector<glm::vec3>& normals,
		std::vector<glm::vec3>& tangents,
		std::vector<glm::vec3>& bitangents);

	// ���� �Ӽ� ����
	GLuint SetVertexAttribute(
		std::vector<glm::vec3>& vertices,
		std::vector<glm::vec2>& uvs,
		std::vector<glm::vec3>& normals);

	// EBO�� ����ϴ� ���� �Ӽ� ����
	GLuint SetVertexAttributeWithElementBufferObject(
		std::vector<glm::vec3>& vertices,
		std::vector<glm::vec2>& uvs,
		std::vector<glm::vec3>& normals,
		std::vector<glm::vec3>& tangents,
		std::vector<glm::vec3>& bitangents,
		std::vector<unsigned int>& indices);

	// 2D �簢�� ���� �Ӽ� ����
	void SetVertexAttributeFor2DRectangle();

	// ��ī�̹ڽ�(ť���) ���� �Ӽ� ����
	void SetVertexAttributeForSkybox();

	// �� �������� fbo ����
	void CreateSceneFBOAndTexture();

	// ȭ�鿡 �ؽ��� �簢�� �������� vao ����
	void SetDisplayRect();

	// BMP �ؽ��� ���� �б�
	GLuint ReadBMPTextureFile(const char* fileName);

	// �𷺼ų� ����Ʈ�� ���� �׸��ڸ� ���� ���� �� ����
	void CreateDepthMap();

	Renderer::SModel* GetModel(int index);
	Renderer::STexture* GetTexture(int index);

	// �� ��� ���� ��ƼŬ����
	// ��ų �� ���� ��ƼŬ�� vao ����
	void SetVAOForChangeSkillDeckParticle();

	// ������ ��� ����� fbo, �ؽ��� ����
	void CreateSaveRenderingResultFBOAndTexture();

	// FXAA�� �ϴ� ���÷��� ��� fbo, �ؽ��� ����
	void CreateDisplayWithFXAAFBOAndTexture();

	// �� ����� fbo, �ؽ��� ����
	void CreateBlurFBOAndTexture();

private:
	// ���̴��� ������ �����̼� ����
	void SaveGettingUniformLocation();

	// �⺻ ���̴� ������ ��ġ
	GLuint m_modelMatrixLocation;
	GLuint m_materialAmbientLocation;
	GLuint m_materialDiffuseLocation;
	GLuint m_materialSpecularLocation;
	GLuint m_materialShininessLocation;
	GLuint m_materialNormalLocation;
	GLuint m_colorLocation;
	GLuint m_isLightSourceLocation;

	// ���� ���̴� ������ ��ġ
	GLuint m_modelMatrixForDepthLocation;

	// �׵θ� ���̴� ������ ��ġ
	GLuint m_modelMatrixForBorderLocation;
	GLuint m_colorForBorderLocation;

	// �ؽ�Ʈ ���̴� ������ ��ġ
	GLuint m_projectionMatrixForTextLocation;
	GLuint m_textLocation;
	GLuint m_colorForTextLocation;

	// �ؽ��� ���̴� ������ ��ġ
	GLuint m_projectionMatrixForTexture2DLocation;
	GLuint m_textureForTexture2DLocation;
	GLuint m_colorForTexture2DLocation;

	// ������ �ؽ��� ���̴� ������ ��ġ	
	GLuint m_translateForTexture3DLocation;
	GLuint m_scaleForTexture3DLocation;
	GLuint m_textureForTexture3DLocation;
	GLuint m_colorForTexture3DLocation;

	// ������ �ؽ�Ʈ ���̴� ������ ��ġ	
	GLuint m_translateForText3DLocation;
	GLuint m_scaleForText3DLocation;
	GLuint m_text3DLocation;
	GLuint m_colorForText3DLocation;

	// ������ ��������Ʈ ��Ʈ ���̴� ������ ��ġ
	GLuint m_translateForBillboardSpriteSheetLocation;
	GLuint m_scaleForBillboardSpriteSheetLocation;
	GLuint m_textureForBillboardSpriteSheetLocation;
	GLuint m_frameForBillboardSpriteSheetLocation;
	GLuint m_colorForBillboardSpriteSheetLocation;

	// ��ī�̹ڽ� ���̴� ������ ��ġ
	GLuint m_textureForSkyboxLocation;
	GLuint m_texture2ForSkyboxLocation;
	GLuint m_blendRateForSkyboxLocation;

	// ����� �� ���̴� ������ ��ġ
	GLuint m_modelMatrixLocationForDamageZone;
	GLuint m_timeRateLocationForDamageZone;

	// �� ȿ�� ���̴� ������ ��ġ
	GLuint m_modelMatrixLocationForWater;
	GLuint m_textureForWaterLocation;
	GLuint m_texture2ForWaterLocation;
	GLuint m_blendRateForWaterLocation;
	GLuint m_timeForWaterLocation;

	// �߷��� �޴� ��ƼŬ ���̴� ������ ��ġ
	GLuint m_translateForGravityParticle;
	GLuint m_scaleForGravityParticle;
	GLuint m_textureForGravityParticle;
	GLuint m_colorForGravityParticle;
	GLuint m_timeForGravityParticle;
	GLuint m_loopForGravityParticle;
	GLuint m_applyGravityForGravityParticle;

	// �ν��Ͻ� ���̴� ������ ��ġ
	GLuint m_textureForInstanceLocation;
	GLuint m_timeForInstanceLocation;
};