#include "stdafx.h"
#include "PauseMenuState.h"

CPauseMenuState::CPauseMenuState()
{
	// CGameFramework에서 CGameState를 사용하기 때문에 CGameState의 자식 클래스인 현 클래스에서 CGameFramework를 가져올 것!
	m_pGameFramework = CGameFramework::GetInstance();
	m_pTitleMenuState = new CTitleMenuState;

	LoadData();
	AddObjectToManager();
}

CPauseMenuState::~CPauseMenuState()
{
}

void CPauseMenuState::ProcessInputs(float elapsedTimeInSec, GameInputs* inputs)
{
	if (inputs->IsPressedDownKeyOnce(&inputs->KeyESC, &inputs->processingEnd.KeyESC)) {
		m_pGameFramework->PopState(true);
	}

	ButtonSelectInputs(elapsedTimeInSec, inputs);

	if (m_ButtonVector[m_iSelectedButton]->SetIsUsed(inputs)) {
		UseButton(m_iSelectedButton);
	}
}

void CPauseMenuState::Update(float elapsedTimeInSec)
{
}

void CPauseMenuState::RenderScene()
{
	m_pGameFramework->GetPrevState()->SetBlurEffect(true);
	m_pGameFramework->GetPrevState()->Camera()->UseCamera();
	m_pGameFramework->GetPrevState()->SaveRenderingResultOnTexture();
	m_pGameFramework->GetPrevState()->SetBlurEffect(false);

	// 후처리된 화면 렌더링
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	// UI 렌더링
	UIRender();
}

void CPauseMenuState::Enter()
{
	ShowMouseCursor(true);
	m_bIsOpened = true;
}

void CPauseMenuState::Exit()
{
	ShowMouseCursor(false);
	int m_iSelectedButton = 0;
	for (auto button : m_ButtonVector) {
		button->SetIsSelected(false);
	}
	m_bIsOpened = false;
}

void CPauseMenuState::Pause()
{
	ShowMouseCursor(false);
	int m_iSelectedButton = 0;
	for (auto button : m_ButtonVector) {
		button->SetIsSelected(false);
	}
	m_bIsOpened = false;
}

void CPauseMenuState::Resume()
{
	ShowMouseCursor(true);
	m_bIsOpened = true;
}

bool CPauseMenuState::GetBattleMode() const
{
	return m_bBattleMode;
}

bool CPauseMenuState::GetIsOpened() const
{
	return m_bIsOpened;
}

void CPauseMenuState::NormalRender()
{
}

void CPauseMenuState::UIRender()
{
	Renderer()->UseShader(UI_SHADER);

	int x, y;
	Renderer()->GetDisplayResolution(&x, &y);

	Renderer()->RenderTexture2D(Renderer()->GetSaveRenderingResultTexture(),
		glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f),
		glm::vec2((float)x, (float)y), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	RenderButtonVector();
}

void CPauseMenuState::LoadData()
{
	// 흰색 사각형 텍스쳐 로드
	m_WhiteRectTextureID = (int)Renderer()->ReadPNGTextureFile("./resource/ui/whiteRect.png");
}

void CPauseMenuState::AddObjectToManager()
{
	m_ButtonVector.push_back(new CButton(L"게임으로 돌아가기"));
	m_ButtonVector.push_back(new CButton(L"주인공 위치 초기화"));
	m_ButtonVector.push_back(new CButton(L"전투 모드 변경"));
	m_ButtonVector.push_back(new CButton(L"화면 표시 모드 변경"));
	m_ButtonVector.push_back(new CButton(L"메인 메뉴로 이동"));
	m_ButtonVector.push_back(new CButton(L"게임 종료"));
}

void CPauseMenuState::ShowMouseCursor(bool show)
{
	if (show) {
		glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
	}
	else {
		glutSetCursor(GLUT_CURSOR_NONE);
		int x, y;
		Renderer()->GetDisplayResolution(&x, &y);
		glutWarpPointer(x / 2, y / 2);
	}	
}

void CPauseMenuState::RenderButtonVector()
{
	int x, y;
	Renderer()->GetDisplayResolution(&x, &y);

	for (int i = 0; i < m_ButtonVector.size(); ++i) {
		float gap = (i * 75.0f) / (float)y;
		float margin = 10.0f / (float)x;
		m_ButtonVector[i]->Render(m_WhiteRectTextureID, glm::vec2(0.15f, 0.8f - gap), 
			glm::vec2(350.0f, 45.0f), 0.75f, margin);
	}
}

void CPauseMenuState::ButtonSelectInputs(float elapsedTimeInSec, GameInputs* inputs)
{
	bool useMoveKey = false;
	if (inputs->KeyW) {
		if (inputs->IsPressedDownKeyOnce(&inputs->KeyW, &inputs->processingEnd.KeyW)) {
			m_fKeepPressKey = 0.0f;
			m_iSelectedButton--;
			useMoveKey = true;
		}
		else {
			if (IsKeyResultWaitedEnough(elapsedTimeInSec)) {
				m_iSelectedButton--;
				useMoveKey = true;
			}
		}
	}
	if (inputs->KeyS) {
		if (inputs->IsPressedDownKeyOnce(&inputs->KeyS, &inputs->processingEnd.KeyS)) {
			m_fKeepPressKey = 0.0f;
			m_iSelectedButton++;
			useMoveKey = true;
		}
		else {
			if (IsKeyResultWaitedEnough(elapsedTimeInSec)) {
				m_iSelectedButton++;
				useMoveKey = true;
			}
		}
	}
	if (useMoveKey) {
		m_iSelectedButton = glm::clamp(m_iSelectedButton, 0, (int)m_ButtonVector.size() - 1);

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
			float gap = (i * 75.0f) / (float)y;

			if (m_ButtonVector[i]->SelectedWithMouse(inputs->MouseXPos, y - inputs->MouseYPos,
				0.15f * x, (0.8f - gap) * y + 45.0f, 0.15f * x + 350.0f, (0.8f - gap) * y)) {
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

bool CPauseMenuState::IsKeyResultWaitedEnough(float time)
{
	m_fKeepPressKey += time * DEFAULT_MOVE_IN_MENU_SPEED;
	if (m_fKeepPressKey >= 1.0f) {
		m_fKeepPressKey = 0.0f;
		return true;
	}
	return false;
}

void CPauseMenuState::UseButton(int menu)
{
	switch (menu) {
	case 0:
		m_pGameFramework->PopState(true);
		break;
	case 1:
		m_pGameFramework->GetPrevState()->InitLocationForBugFix();
		break;
	case 2:
		m_bBattleMode = (m_bBattleMode + 1) % 2;
		break;
	case 3:
		glutFullScreenToggle();
		break;
	case 4:
		m_pGameFramework->GetPrevState()->SetIsAlreadyStartedAndStillPlaying(false);
		m_pGameFramework->GetPrevState()->Exit();
		m_pGameFramework->ChangeState(m_pTitleMenuState, true);
		break;
	case 5:
		glutExit();
		break;
	}
}
