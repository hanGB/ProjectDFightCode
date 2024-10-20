/*
* Game State
*	제 1번으로 호출되는 클래스
*
*	게임 상태를 관리
*		조작 방법이 달라지는 경우 게임 상태 변경(메인 화면 -> 게임 플레이 화면 등)
*	플레이어 입력 처리
*	Player(Object) 생성
* 
*	ObjectManager를 소유
*	Camera 소유
*/

#pragma once
#include "Globals.h"
#include "Renderer.h"
#include "ObjectManager.h"
#include "SoundPlayer.h"
#include "SpotLight.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "BehaviorTree.h"
#include "Camera.h"

class CGameState
{
public:
	CGameState();
	~CGameState();

private:
	CRenderer* m_pRenderer = NULL;
	CSoundPlayer* m_pSoundPlayer = NULL;
	CBlackBoard* m_pBackBoard = NULL;
	CObjectManager* m_pObjectManager = NULL;
	CCamera* m_pCamera = NULL;

	// 조명
	CDirectionalLight* m_pDirectionalLight = NULL;
	CPointLight* m_pPointLight = NULL;

	// 블러 효과 사용 여부
	bool m_bBlurEffect = false;
	bool m_bColorInvertEffect = false;

	// 일시정지한다고 초기화 되지 않도록 하는 변수
	bool m_bIsAlreadyStartedAndStillPlaying = false;

public:
	// 플레이어의 키 입력 처리
	virtual void ProcessInputs(float elapsedTimeInSec, GameInputs* inputs) = 0;

	virtual void Update(float elapsedTimeInSec);
	virtual void RenderScene();

	// 버그를 고치지 위한 위치 초기화
	virtual void InitLocationForBugFix();

	// 스테이트 진입시
	virtual void Enter() = 0;
	// 스테이트 종료시
	virtual void Exit() = 0;
	// 스테이트 임시 이동
	virtual void Pause() = 0;
	// 스테이트 복귀
	virtual void Resume() = 0;

	// 각종 클래스를 자식 클래스에서 이용하기 위한 함수
	CRenderer* Renderer() const { return m_pRenderer; } 
	CSoundPlayer* SoundPlayer() const { return m_pSoundPlayer; }
	CBlackBoard* BackBoard() const { return m_pBackBoard; }
	CObjectManager* ObjectManager() const { return m_pObjectManager; }
	CCamera* Camera() const { return m_pCamera; }

	CDirectionalLight* DirectionalLight() { return m_pDirectionalLight; }
	CPointLight* PointLight() { return m_pPointLight; }

	void LoadDirectionalLight(CDirectionalLight* DirectionalLight) { m_pDirectionalLight = DirectionalLight; }
	void LoadPointLight(CPointLight* PointLight) { m_pPointLight = PointLight; }

	// 일반 렌더러(계산된 그림자 이용)
	virtual void NormalRender();

	// 기본 렌더링 파이프라인
	void DefualtRenderPipeline();

	// 후처리 렌더러
	void PostProcessingRender();

	// 디렉셔널 라이트 그림자 계산용 렌더링
	void DepthRender();

	// 렌더링 결과 텍스쳐에 저장
	void SaveRenderingResultOnTexture();

	// UI 렌더러
	virtual void UIRender() = 0;

	void SetBlurEffect(bool blur) { m_bBlurEffect = blur; }
	void SetColorInvertEffect(bool invert) { m_bColorInvertEffect = invert; }

	// 오브젝트 메니저 변경
	void ChangeObjectManager(CObjectManager* objectmanager);

	void SetIsAlreadyStartedAndStillPlaying(bool alreadyStarted);
	bool GetIsAlreadyStartedAndStillPlaying() const;

private:
	// 필요 모델, 텍스쳐, 사운드 로드
	virtual void LoadData() = 0;

	// 처음 State가 만들어졌을때 오브젝트 매니저에 오브젝트 추가
	virtual void AddObjectToManager() = 0;

};

