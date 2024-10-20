#pragma once
#include "GameState.h"
#include "GameFramework.h"
#include "HeroObject.h"

class CTestState : public CGameState {
public:
	CTestState();
	~CTestState();

private:
	CGameFramework* m_pGameFramework = NULL;
	CSpotLight* m_pSpotLight = NULL;

	// 테스트 용 박스
	int m_boxModelID = EMPTY_VALUE;
	int m_boxTextureID = EMPTY_VALUE;

	// 모델, 텍스쳐, 사운드 인덱스
	int m_floorModelID = EMPTY_VALUE;
	int m_floorTextureID = EMPTY_VALUE;

	int m_enemyModelID = EMPTY_VALUE;
	int m_enemyTextureID = EMPTY_VALUE;

	int m_enemy2ModelID = EMPTY_VALUE;
	int m_enemy2TextureID = EMPTY_VALUE;

	int m_cityBGMID = EMPTY_VALUE;

	int m_grassModelID = EMPTY_VALUE;
	int m_grassTextureID = EMPTY_VALUE;

	GLuint m_GrassSSBOModelMatrices;

	// UI 2D 텍스쳐 테스트용 인덱스
	int m_uiTextureID = EMPTY_VALUE;

	// 스프라이트 시트 테스트용 인덱스, 변수
	int m_spriteSheetID = EMPTY_VALUE;
	int m_iFrame = 0;
	float m_fFrameTime = 0;

	// 빌보드 테스트용 인덱스 
	int m_billboardTextureID = EMPTY_VALUE;
	int m_moonTextureID = EMPTY_VALUE;

	// 스카이박스 인덱스
	int m_skyboxTextureID = EMPTY_VALUE;
	int m_skyboxDarkTextureID = EMPTY_VALUE;

	// 주인공 아이디
	CHeroObject* m_pHeroObject = NULL;
	int m_HeroModelID = EMPTY_VALUE;
	int m_HeroTextureID = EMPTY_VALUE;

	// BGM
	irrklang::ISound* m_pBGM = NULL;

	// 손전등 조작을 위한 변수
	bool m_bFlashLightOn = false;

	// 카메라 변경
	bool m_bTestCameraOn = true;

	int m_switchSoundID = EMPTY_VALUE;

	// 마우스 고정 설정 변수
	bool m_bMouseFixed = true;
	int m_iPrevMousePosX = WINDOW_SIZE_X / 2;
	int m_iPrevMousePosY = WINDOW_SIZE_Y / 2;

	// FPS 계수기
	wchar_t m_wFpsText[10];
	float m_fSecond = 1;

	// 바운딩 박스 그리기
	bool m_bShowBoundingBox = false;

	// 시간
	float m_fHour = 6;

	// 잔디 흔들림을 위한 시간 변수
	float m_fTimeForGrass = 0;

public:
	// 플레이어의 키 입력 처리
	virtual void ProcessInputs(float elapsedTimeInSec, GameInputs* inputs);

	virtual void Update(float elapsedTimeInSec);
	virtual void RenderScene();

	// 스테이트 진입시
	virtual void Enter();
	// 스테이트 종료시
	virtual void Exit();
	// 스테이트 임시 이동
	virtual void Pause();
	// 스테이트 복귀
	virtual void Resume();

private:
	// 일반 렌더러(계산된 그림자 이용)
	virtual void NormalRender();

	// UI 렌더링
	virtual void UIRender();

	// 필요 모델, 텍스쳐, 사운드 로드
	virtual void LoadData();

	// 처음 State가 만들어졌을때 오브젝트 매니저에 오브젝트 추가
	virtual void AddObjectToManager();

	void MakeGrassInstance();
};