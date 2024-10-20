#include "stdafx.h"
#include "GameState.h"

CGameState::CGameState()
{
	m_pRenderer = CRenderer::GetInstance();
	m_pSoundPlayer = CSoundPlayer::GetInstance();
	m_pBackBoard = CBlackBoard::GetInstance();
	m_pObjectManager = new CObjectManager();
	m_pCamera = new CCamera();
}

CGameState::~CGameState()
{
	delete m_pObjectManager;
	delete m_pCamera;
	delete m_pDirectionalLight;
	delete m_pPointLight;
}

void CGameState::Update(float elapsedTimeInSec)
{
	m_pBackBoard->SetElapsedTimeInSec(elapsedTimeInSec);
	m_pObjectManager->Update(elapsedTimeInSec);
}

void CGameState::RenderScene()
{
	DefualtRenderPipeline();

	// ÈÄÃ³¸®¸¦ À§ÇÑ ·»´õ¸µ
	PostProcessingRender();

	// UI ·»´õ¸µ
	UIRender();
}

void CGameState::InitLocationForBugFix()
{

}

void CGameState::DepthRender()
{
	m_pRenderer->UseShader(DEPTH_SHADER);
	m_pDirectionalLight->SetLightSpaceMatrix(DEPTH_SHADER);
	m_pObjectManager->DepthRender();
}

void CGameState::SaveRenderingResultOnTexture()
{
	DefualtRenderPipeline();

	// ÈÄÃ³¸®¸¦ À§ÇÑ ·»´õ¸µ
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, m_pRenderer->GetSaveRenderingResultFBO());
	glClear(GL_COLOR_BUFFER_BIT);
	m_pRenderer->UseShader(TOTAL_SCENE_SHADER);
	m_pRenderer->RenderTotalScene(m_bBlurEffect, m_bColorInvertEffect);
	glEnable(GL_DEPTH_TEST);
}

void CGameState::ChangeObjectManager(CObjectManager* objectmanager)
{
	delete m_pObjectManager;
	m_pObjectManager = objectmanager;
}

void CGameState::SetIsAlreadyStartedAndStillPlaying(bool alreadyStarted)
{
	m_bIsAlreadyStartedAndStillPlaying = alreadyStarted;
}

bool CGameState::GetIsAlreadyStartedAndStillPlaying() const
{
	return m_bIsAlreadyStartedAndStillPlaying;
}

void CGameState::NormalRender()
{
	// ±âº» ·»´õ¸µ
	m_pRenderer->UseShader(DEFAULT_SHADER);
	m_pDirectionalLight->UseLight(DEFAULT_SHADER);
	m_pDirectionalLight->SetLightSpaceMatrix(DEFAULT_SHADER);
	m_pPointLight->UseLight(DEFAULT_SHADER);
	m_pObjectManager->Render();
}

void CGameState::DefualtRenderPipeline()
{
	// µð·º¼Å³Î ¶óÀÌÆ® ±íÀÌ ·»´õ¸µ
	glCullFace(GL_FRONT); // ÇÇÅÍ ÆÐ´×(Peter Panning) Çö»ó ¼öÁ¤

	glViewport(0, 0, SHADOW_RESOLUTION, SHADOW_RESOLUTION);
	glBindFramebuffer(GL_FRAMEBUFFER, m_pRenderer->GetDepthMapFBO());
	glClear(GL_DEPTH_BUFFER_BIT);
	DepthRender();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glCullFace(GL_BACK);

	// ÀÏ¹Ý ·»´õ¸µ
	int w, h;
	m_pRenderer->GetDisplayResolution(&w, &h);
	glViewport(0, 0, w, h);
	glBindFramebuffer(GL_FRAMEBUFFER, m_pRenderer->GetSceneFBO());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	m_pRenderer->UseShader(DEFAULT_SHADER);
	NormalRender();
}

void CGameState::PostProcessingRender()
{
	m_pRenderer->ApplyBlurToBloomTexture();

	// scene + bloom ·»´õ¸µ
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, m_pRenderer->GetDisplayFXAAFBO());
	glClear(GL_COLOR_BUFFER_BIT);
	m_pRenderer->UseShader(TOTAL_SCENE_SHADER);
	m_pRenderer->RenderTotalScene(m_bBlurEffect, m_bColorInvertEffect);

	// ¾ÈÆ¼ ¿¤¸®¾î½ÌµÈ È­¸é ·»´õ¸µ
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	m_pRenderer->UseShader(DISPLAY_FXAA_SHADER);
	m_pRenderer->RenderDisplayFXAA();
	glEnable(GL_DEPTH_TEST);
}
