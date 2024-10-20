#include "stdafx.h"
#include "GameOverState.h"

CGameOverState::CGameOverState()
{
	// CGameFramework에서 CGameState를 사용하기 때문에 CGameState의 자식 클래스인 현 클래스에서 CGameFramework를 가져올 것!
	m_pGameFramework = CGameFramework::GetInstance();

	m_pTimeOfDaySkybox = new CTimeOfDaySkybox();
	m_pTimeOfDaySkybox->SetIsRainy(true);

	Camera()->SetTestCameraPosition(glm::vec3(-10.0f, -1.0f, 45.0f));
	Camera()->TestCameraSetting();

	glm::vec3 pos = glm::vec3(100.0f, 0.0f, 0.0f);
	glm::vec3 dir = glm::normalize(pos);

	LoadDirectionalLight(new CDirectionalLight(pos,
		glm::vec3(-dir.x, -dir.y, -dir.z),
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));
	DirectionalLight()->SetSkyLightColor(NIGHT_LIGHT_COLOR);

	LoadData();
	AddObjectToManager();
}

CGameOverState::~CGameOverState()
{
}


void CGameOverState::NormalRender()
{
	// 스카이 박스 렌더링
	m_pTimeOfDaySkybox->RenderSkybox(DirectionalLight());
}

void CGameOverState::UIRender()
{
	Renderer()->UseShader(UI_SHADER);

	// 타이틀 로고
	Renderer()->RenderTexture2D(m_LogoTextureID,
		glm::vec2(0.125f, 0.6f), glm::vec2(0.0f, 0.0f),
		glm::vec2(1200.f, 300), glm::vec4(1.0f));

	int x, y;
	Renderer()->GetDisplayResolution(&x, &y);
	for (int i = 0; i < m_ButtonVector.size(); ++i) {
		float gap = (i * 75.0f + 250.0f) / (float)y;
		float margin = 10.0f / (float)x;
		m_ButtonVector[i]->Render(m_WhiteRectTextureID, glm::vec2(0.125f, 0.6f - gap),
			glm::vec2(250.0f, 35.0f), 0.65f, margin);
	}
}

void CGameOverState::LoadData()
{
	m_WhiteRectTextureID = (int)Renderer()->ReadPNGTextureFile("./resource/ui/whiteRect.png");

	m_gameoverBGMID = SoundPlayer()->LoadSound("./resource/sound/BGM/deathBGM.mp3");

	// bgm 설정
	m_pBGM = SoundPlayer()->SetBackGroundMusic(m_gameoverBGMID, 0.5f);

	// 타이틀 로고
	m_LogoTextureID = (int)Renderer()->ReadPNGTextureFile("./resource/ui/Gameover.png");

}

void CGameOverState::AddObjectToManager()
{
	m_ButtonVector.push_back(new CButton(L"Back to Field"));
	m_ButtonVector.push_back(new CButton(L"Exit"));
}

void CGameOverState::ProcessInputs(float elapsedTimeInSec, GameInputs* inputs)
{
	ButtonSelectInputs(elapsedTimeInSec, inputs);

	if (m_ButtonVector[m_iSelectedButton]->SetIsUsed(inputs)) {
		UseButton(m_iSelectedButton);
	}
}

void CGameOverState::Update(float elapsedTimeInSec)
{
	m_pTimeOfDaySkybox->Update(elapsedTimeInSec * 2, DirectionalLight());
}

void CGameOverState::RenderScene()
{
	Camera()->UseCamera();

	CGameState::RenderScene();
}

void CGameOverState::Enter()
{
	glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
	SoundPlayer()->PlayAndStopBackGroundMusic(m_pBGM, true);
	m_pGameFramework->SetIsEnableToEnterSystemMenu(false);
	m_pGameFramework->PopPrevState(true);
}

void CGameOverState::Exit()
{
	m_pGameFramework->SetIsEnableToEnterSystemMenu(true);
	SoundPlayer()->PlayAndStopBackGroundMusic(m_pBGM, false);
	glutSetCursor(GLUT_CURSOR_NONE);
	int x, y;
	Renderer()->GetDisplayResolution(&x, &y);
	glutWarpPointer(x / 2, y / 2);
}

void CGameOverState::Pause()
{
	m_pGameFramework->SetIsEnableToEnterSystemMenu(true);
	SoundPlayer()->PlayAndStopBackGroundMusic(m_pBGM, false);
	glutSetCursor(GLUT_CURSOR_NONE);
	int x, y;
	Renderer()->GetDisplayResolution(&x, &y);
	glutWarpPointer(x / 2, y / 2);
}

void CGameOverState::Resume()
{
	glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
	SoundPlayer()->PlayAndStopBackGroundMusic(m_pBGM, true);
	m_pGameFramework->SetIsEnableToEnterSystemMenu(false);
	m_pGameFramework->PopPrevState(true);
}

void CGameOverState::ButtonSelectInputs(float elapsedTimeInSec, GameInputs* inputs)
{
	bool useMoveKey = false;
	if (inputs->KeyW) {
		useMoveKey = true;
		m_iSelectedButton = 0;
	}
	if (inputs->KeyS) {
		useMoveKey = true;
		m_iSelectedButton = 1;
	}

	if (useMoveKey) {
		for (auto button : m_ButtonVector) {
			button->SetIsSelected(false);
		}
		m_ButtonVector[m_iSelectedButton]->SelectedWithKeyBoard();
	}
	else {
		int x, y;
		Renderer()->GetDisplayResolution(&x, &y);
		int num = -1;
		for (int i = 0; i < m_ButtonVector.size(); ++i) {
			float gap = (i * 75.0f + 250.0f) / (float)y;
			if (m_ButtonVector[i]->SelectedWithMouse(inputs->MouseXPos, y - inputs->MouseYPos,
				0.125f * x, (0.6f - gap) * y + 35.0f, 0.15f * x + 300.0f, (0.6f - gap) * y)) {
				num = i;
				m_iSelectedButton = i;
				break;
			}
		}
		if (num != -1) {
			for (int i = 0; i < m_ButtonVector.size(); ++i) {
				if (i != num) {
					m_ButtonVector[i]->SetIsSelected(false);
					m_ButtonVector[i]->SetIsMousePressDown(false);
				}
			}
		}
	}
}

void CGameOverState::UseButton(int menu)
{
	switch (menu) {
	case 0:
		m_pGameFramework->PopState(true);
		break;
	case 1:
		glutExit();
		break;
	}
}