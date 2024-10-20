/*
* Renderer
*	
*	Singleton pattern으로 변경 필요 
* 
*	쉐이터 컴파일
*	화면 초기화 
*	Obj 모델 렌더링
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
	// 싱글톤 패턴 구현 함수
	static CRenderer* GetInstance();
	
private:
	// 쉐이더 프로그램 아이디
	GLuint m_ShaderProgramID;					// 기본(일반 오브젝트 렌더링용) 쉐이더 프로그램
	GLuint m_TextShaderProgramID;				// 텍스트 렌더링용 쉐이더 프로그램
	GLuint m_2dUIShaderProgramID;				// 2D 텍스쳐 렌더링용 쉐이더 프로그램
	GLuint m_BillboardShaderProgramID;			// 빌보드를 이용한 텍스쳐 렌더링용 쉐이더 프로그램
	GLuint m_BillboardSpriteShaderProgramID;	// 빌보드를 이용한 스프라이트 시트 렌더링용 쉐이더 프로그램
	GLuint m_SkyboxShaderProgramID;				// 큐브맵을 이용한 스카이박스 렌더링용 쉐이더 프로그램
	GLuint m_DepthShaderProgramID;				// 깊이 맵을 이용한 디렉션널 라이트 쉐도우 맵핑용 쉐이더 프로그램
	GLuint m_BoundingBoxShaderProgramID;		// 충돌 박스(충돌 범위) 렌더링용 쉐이더 프로그램
	GLuint m_BorderShaderProgramID;				// 테두리(외곽선) 렌더링용 쉐이더 프로그램
	GLuint m_TotalSceneShaderProgramID;			// 전체 씬 렌더링용 쉐이더 프로그램
	GLuint m_InstanceShaderProgramID;			// 인스턴스 렌더링용 쉐이더 프로그램
	GLuint m_BasicMovingParticleProgramID;		// 움직이는 파티클 렌더링용 쉐이더 프로그램
	GLuint m_BillboardTextShaderProgramID;		// 빌보드를 이용한 텍스트 렌더링용 쉐이더 프로그램
	GLuint m_ChangeStateEffectShaderProgramID;	// 스테이트 변경 효과 렌더링용 쉐이더 프로그램
	GLuint m_DamageZoneShaderProgramID;			// 데미지 존 쉐이더 프로그램
	GLuint m_WaterShaderProgramID;				// 물 효과 쉐이더 프로그램
	GLuint m_GravityParticleShaderProgramID;	// 중력이 적용되는 파티클 쉐이더 프로그램
	GLuint m_DisplayFXAAShaderProgramID;		// 화면에 FXAA 적용 후 렌더링용 쉐이더 프로그램
	GLuint m_BlurShaderProgramID;				// bloom용 블러 효과 쉐이더 프로그램


	// 모델과 텍스쳐 데이터 저장을 위한 맵
	std::map<int, Renderer::SModel*> m_ModelMap;
	std::map<int, Renderer::STexture*> m_TextureMap;

	// 애니메이션 모델 데이터 저장을 위한 맵
	std::map<std::string, CModelLoader*> m_ModelWithBoneLoaderMap;
	std::map<std::string, CAnimationLoader*> m_AnimationLoaderMap;

	// 폰트 저장을 위한 맵
	std::map<wchar_t, Renderer::SCharacter> m_CharacterMap;
	// 스카이박스(큐브 맵) 저장을 위한 벡터
	std::map<std::string, int> m_SkyboxMap;
	std::vector<GLuint> m_SkyboxVector;

	// 텍스트 출력을 위한 vao, vbo
	GLuint m_TextVAO, m_TextVBO;

	// UI 제작용 2D 사각형를 위한 vao;
	GLuint m_2DRectangleVAO;
	GLuint m_2DRectangleVBO;

	// 빌보드 제작용 평면 vao;
	GLuint m_PlaneModelVAO;

	// 스카이박스 제작용 큐브 vao;
	GLuint m_SkyboxVAO;

	// 그림자를 위한 프레임 버퍼, 깊이 맵
	GLuint m_DepthMapFBO;
	GLuint m_DepthMap;

	// 씬 렌더링용 프레임 버퍼, 텍스쳐
	GLuint m_SceneFBO;
	GLuint m_SceneTexture;
	GLuint m_RBO;

	// Bloom 텍스쳐
	GLuint m_BloomTexture;

	// 렌더링 결과 저장 프레임 버퍼, 텍스쳐
	GLuint m_SaveRenderingResultFBO;
	GLuint m_SaveRenderingResultTexture;

	// 화면 렌더링용 멀티 샘플링 프레임 버퍼, 텍스쳐
	GLuint m_DisplayFXAAFBO;
	GLuint m_DisplayFXAATexture;

	// 화면 전체 텍스쳐 렌더링용 vao
	GLuint m_DisplayVAO = 0;

	// 화면 해상도
	int m_iResoultionX, m_iResoultionY;

	// 충돌 박스를 위한 모델 인덱스
	int m_iBoundingBoxModelIndex;
	int m_iCapsuleModelIndex;

	// 턴 기반 전투 관련
	// 스킬 덱 변경 파티클 vao
	GLuint m_ChangeSkillDeckParticleVAO;

	// 화면 후처리 비교용 선 위치
	int m_iDisplayPostEffectOnOffLine = 0;

	// 블러 생성용 fbo, 텍스쳐
	GLuint m_BlurFBO[2];
	GLuint m_BlurTexture[2];

public:
	// OBJ 모델 로드
	int LoadOBJModel(const char* fileName);

	// OBJ 모델 로드
	int LoadOBJModelWithIndex(const char* fileName);

	// BMP 텍스쳐 로드
	int LoadBMPTexture(const char* ambientMapFileName, 
		const char* diffuseMapFileName,
		const char* speculaMapFileName, 
		float shininess,
		const char* normalMapFileName);

	// PNG 텍스쳐 로드
	int LoadPNGTexture(const char* ambientMapFileName, 
		const char* diffuseMapFileName, 
		const char* speculaMapFileName, 
		float shininess,
		const char* normalMapFileName);

	// 애니메이션 모델 로드(이름에 해당하는 애니메이션 모델 데이터가 없을 경우 로드)
	void LoadModelWithBone(const char* fileName, std::string name);
	void LoadAnimation(const char* fileName, std::string modelName, std::string animationName);

	CModelLoader* GetModelWithBone(std::string modelName);
	CAnimationLoader* GetAnimation(std::string animationName);

	// 애니메이션 삭제
	void DeleteModelWithBone(std::string name);
	void DeleteAnimation(std::string animationName);

	// 스카이 박스(큐브 맵) 생성
	int LoadSkybox(std::string name, std::string* fileNames);

	// OBJ 모델 삭제 (빈 index가 들어올 경우 삭제 없이, 일반 index는 삭제 후 empty_value 리턴)
	int DeleteModelAndSetEmptyID(int index);

	// 텍스쳐 삭제 (빈 index가 들어올 경우 삭제 없이, 일반 index는 삭제 후 empty_value 리턴)
	int DeleteTextureAndSetEmptyID(int index);

	// 화면 설정
	void SetDisplay();

	// 삼각형으로 이루어진 모델 인덱스로 그리기
	void DrawElementsWithTriangles(int modelIndex, int textureIndex,
		glm::vec3 position,
		glm::vec3 rotation,
		glm::vec3 size,
		glm::quat quaternion,
		glm::vec4 color,
		bool isLightSource,
		bool outLine);

	// 깊이 맵을 위한 모델 인덱스로 렌더링
	void DrawElementsWithTrianglesForDepth(int modelIndex,
		glm::vec3 position,
		glm::vec3 rotation,
		glm::vec3 size,
		glm::quat quaternion);

	// 테두리(외각선)을 그리기 위한 모델 인덱스로 렌더링
	void DrawElementsWithTrianglesForBorder(int modelIndex,
		glm::vec3 position,
		glm::vec3 rotation,
		glm::vec3 size,
		glm::quat quaternion,
		glm::vec4 color);

	// 애니메이션 모델 렌더링
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

	// 깊이 맵을 위한 애니메이션 모델 렌더링
	void DrawAnimationModelForDapth(
		std::string modelName,
		std::vector<std::vector<SVertex>>& verticesVector,
		glm::vec3 position,
		glm::vec3 rotation,
		glm::vec3 size,
		glm::quat quaternion);

	// 테두리(외각선)을 그리기 위한 애니메이션 모델 렌더링
	void DrawAnimationModelForBorder(
		std::string modelName,
		std::vector<std::vector<SVertex>>& verticesVector,
		glm::vec3 position,
		glm::vec3 rotation,
		glm::vec3 size,
		glm::quat quaternion,
		glm::vec4 color);

	// 깊이 맵을 위한 애니메이션 모델 렌더링(버텍스 계산)
	void CaculateVerticesAfterAnimation(
		std::string modelName,
		std::vector<std::vector<SVertex>>& verticesVector,
		std::vector<glm::mat4> finalBonesMatrices);

	// 충돌 박스 렌더링
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
	
	// 텍스트를 2차원상의 화면에 그리기
	void RenderText(const wchar_t* text, float xRate, float yRate, float scale, glm::vec3 color);
	
	// 텍스트를 3차원상의 화면에 그리기(빌보드)
	void RenderText3D(const wchar_t* text, float scale, glm::vec3 position, glm::vec4 color);

	// 텍스쳐 이미지를 2차원상의 화면에 그리기
	void RenderTexture2D(GLuint textureID,
		glm::vec2 positionRate, glm::vec2 rotation, glm::vec2 size, glm::vec4 color);

	// 텍스쳐 이미지를 3차원상의 화면에 그리기(빌보드)
	void RenderTexture3D(GLuint textureID,
		glm::vec3 position, glm::vec2 size, glm::vec4 color);

	// 애니메이션 스프라이트 시트 이미지를 2차원상의 화면에 그리기
	// - 총 몇 개(spriteCount) 중 몇 번째(order)를 그릴 지 인자로 받음: order는 0번부터 시작 spriteCount -1로 끝남
	void RenderSpriteSheet2D(GLuint textureID, 
		int spriteCountX, int spriteCountY, int orderX, int orderY,
		glm::vec2 positionRate, glm::vec2 rotation, glm::vec2 size, glm::vec4 color);

	// 애니메이션 스프라이트 시트 이미지를 3차원상의 화면에 그리기(빌보드) - 프레임 인덱스 방식은 2D와 동일
	void RenderSpriteSheet3D(GLuint textureID, 
		int spriteCountX, int spriteCountY, int orderX, int orderY,
		glm::vec3 position, glm::vec2 size, glm::vec4 color);

	// 스카이 박스 렌더링(blendRate가 0.0f가 아닌 경우 두 텍스쳐를 혼합하여 출력)
	void DrawSkybox(int skybox1ID, int skybox2ID, float blendRate);
	
	// 인스턴스 렌더링
	void RenderInstance(int modelIndex, GLuint textureID, GLuint ssboModelMatrices, float time, int count);

	// 화면 렌더링
	void RenderDisplayFXAA();

	// 쉐이더 프로그램 아이디 전달
	GLuint GetShaderProgram(int type) const;

	// PNG 텍스쳐 파일 읽기
	GLuint ReadPNGTextureFile(const char* fileName);

	// 타입 별로 렌더링 함수를 부르기 전에 호출
	void UseShader(int type);

	void GetDepthMap(GLuint* depthMapFBO, GLuint* depthMap);
	GLuint GetDepthMapFBO();

	GLuint GetSceneFBO();
	GLuint GetSceneTexture();

	GLuint GetSaveRenderingResultFBO();
	GLuint GetSaveRenderingResultTexture() const;

	GLuint GetDisplayFXAAFBO();
	GLuint GetDisplayFXAATexture();

	// 전체 씬 렌더링
	void RenderTotalScene(bool blur, bool invert);

	// 화면 해상도 설정, 얻기
	void SetDisplayResolution(int x, int y);
	void GetDisplayResolution(int* x, int* y);

	// 텍스쳐 버퍼, 렌더링 버퍼 사이즈 재설정
	void SetTextureAndRenderBufferResize();

	Renderer::SModel* GetModelData(int index);

	// 턴 기반 전투 파티클관련
	// 스킬 덱 변경 효과
	void RenderChangeSkillDeckParticle(int textureID, float timeInSec);

	void RenderChangeStateEffect(float time, bool heroWin);

	// 중력을 적용 받는 파티클 렌더링
	void RenderGravityParticle(GLuint vao, int vertexCount, int textureID, 
		glm::vec3 position, glm::vec3 size, glm::vec4 color, float timeInSec, bool loop, bool gravity);

	void RenderDamageZone(int modelIndex,
		glm::vec3 position, glm::vec3 rotation, glm::vec3 size, float timeRate);

	void ContorlDisplayPostEffectOnOffLine(GameInputs* inputs, float elapsedTimeInSec);

	void ApplyBlurToBloomTexture();

private:
	// 폰트 로드
	void LoadFont(const char* fileName, int size);

	// 쉐이더 코드 로드, 컴파일
	GLuint LoadShaders(const char* fileNameVertex, const char* fileNameFragment);

	// 파일 읽기
	char* BringFileToBuf(const char* fileName);
	
	// obj 파일 읽기
	bool ReadOBJFile(const char* fileName,
		std::vector<glm::vec3>& outVertices,
		std::vector<glm::vec2>& outUVs,
		std::vector<glm::vec3>& outNormals);

	// 일반 버텍스 데이터를 인덱스로 변경
	void ChangeToIndexObj(
		std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals,
		std::vector<glm::vec3>& tangents, std::vector<glm::vec3>& bitangents,
		std::vector<glm::vec3>& outVertices, std::vector<glm::vec2>& outUVs,std::vector<glm::vec3>& outNormals,
		std::vector<glm::vec3>& outTangents, std::vector<glm::vec3>& outBitangents,
		std::vector<unsigned int>& indices);

	// 탄젠트와 바이탄젠트 벡터 계산
	void CalculateTangentAndBitangent(
		std::vector<glm::vec3>& vertices,
		std::vector<glm::vec2>& uvs,
		std::vector<glm::vec3>& normals,
		std::vector<glm::vec3>& tangents,
		std::vector<glm::vec3>& bitangents);

	// 정점 속성 설정
	GLuint SetVertexAttribute(
		std::vector<glm::vec3>& vertices,
		std::vector<glm::vec2>& uvs,
		std::vector<glm::vec3>& normals);

	// EBO를 사용하는 정점 속성 설정
	GLuint SetVertexAttributeWithElementBufferObject(
		std::vector<glm::vec3>& vertices,
		std::vector<glm::vec2>& uvs,
		std::vector<glm::vec3>& normals,
		std::vector<glm::vec3>& tangents,
		std::vector<glm::vec3>& bitangents,
		std::vector<unsigned int>& indices);

	// 2D 사각형 정점 속성 설정
	void SetVertexAttributeFor2DRectangle();

	// 스카이박스(큐브맵) 정점 속성 설정
	void SetVertexAttributeForSkybox();

	// 씬 렌더링용 fbo 생성
	void CreateSceneFBOAndTexture();

	// 화면에 텍스쳐 사각형 렌더링용 vao 설정
	void SetDisplayRect();

	// BMP 텍스쳐 파일 읽기
	GLuint ReadBMPTextureFile(const char* fileName);

	// 디렉셔널 라이트에 의한 그림자를 위한 깊이 맵 생성
	void CreateDepthMap();

	Renderer::SModel* GetModel(int index);
	Renderer::STexture* GetTexture(int index);

	// 턴 기반 전투 파티클관련
	// 스킬 덱 변경 파티클용 vao 생성
	void SetVAOForChangeSkillDeckParticle();

	// 렌더링 결과 저장용 fbo, 텍스쳐 생성
	void CreateSaveRenderingResultFBOAndTexture();

	// FXAA를 하는 디스플레이 출력 fbo, 텍스쳐 생성
	void CreateDisplayWithFXAAFBOAndTexture();

	// 블러 적용용 fbo, 텍스쳐 생성
	void CreateBlurFBOAndTexture();

private:
	// 쉐이더의 유니폼 로케이션 설정
	void SaveGettingUniformLocation();

	// 기본 쉐이더 유니폼 위치
	GLuint m_modelMatrixLocation;
	GLuint m_materialAmbientLocation;
	GLuint m_materialDiffuseLocation;
	GLuint m_materialSpecularLocation;
	GLuint m_materialShininessLocation;
	GLuint m_materialNormalLocation;
	GLuint m_colorLocation;
	GLuint m_isLightSourceLocation;

	// 깊이 쉐이더 유니폼 위치
	GLuint m_modelMatrixForDepthLocation;

	// 테두리 쉐이더 유니폼 위치
	GLuint m_modelMatrixForBorderLocation;
	GLuint m_colorForBorderLocation;

	// 텍스트 쉐이더 유니폼 위치
	GLuint m_projectionMatrixForTextLocation;
	GLuint m_textLocation;
	GLuint m_colorForTextLocation;

	// 텍스쳐 쉐이더 유니폼 위치
	GLuint m_projectionMatrixForTexture2DLocation;
	GLuint m_textureForTexture2DLocation;
	GLuint m_colorForTexture2DLocation;

	// 빌보드 텍스쳐 쉐이더 유니폼 위치	
	GLuint m_translateForTexture3DLocation;
	GLuint m_scaleForTexture3DLocation;
	GLuint m_textureForTexture3DLocation;
	GLuint m_colorForTexture3DLocation;

	// 빌보드 텍스트 쉐이더 유니폼 위치	
	GLuint m_translateForText3DLocation;
	GLuint m_scaleForText3DLocation;
	GLuint m_text3DLocation;
	GLuint m_colorForText3DLocation;

	// 빌보드 스프라이트 시트 쉐이더 유니폼 위치
	GLuint m_translateForBillboardSpriteSheetLocation;
	GLuint m_scaleForBillboardSpriteSheetLocation;
	GLuint m_textureForBillboardSpriteSheetLocation;
	GLuint m_frameForBillboardSpriteSheetLocation;
	GLuint m_colorForBillboardSpriteSheetLocation;

	// 스카이박스 쉐이더 유니폼 위치
	GLuint m_textureForSkyboxLocation;
	GLuint m_texture2ForSkyboxLocation;
	GLuint m_blendRateForSkyboxLocation;

	// 대미지 존 쉐이더 유니폼 위치
	GLuint m_modelMatrixLocationForDamageZone;
	GLuint m_timeRateLocationForDamageZone;

	// 물 효과 쉐이더 유니폼 위치
	GLuint m_modelMatrixLocationForWater;
	GLuint m_textureForWaterLocation;
	GLuint m_texture2ForWaterLocation;
	GLuint m_blendRateForWaterLocation;
	GLuint m_timeForWaterLocation;

	// 중력을 받는 파티클 쉐이더 유니폼 위치
	GLuint m_translateForGravityParticle;
	GLuint m_scaleForGravityParticle;
	GLuint m_textureForGravityParticle;
	GLuint m_colorForGravityParticle;
	GLuint m_timeForGravityParticle;
	GLuint m_loopForGravityParticle;
	GLuint m_applyGravityForGravityParticle;

	// 인스턴스 쉐이더 유니폼 위치
	GLuint m_textureForInstanceLocation;
	GLuint m_timeForInstanceLocation;
};