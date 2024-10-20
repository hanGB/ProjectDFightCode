#include "stdafx.h"
#include "TurnBattleState.h"
#include <random>

void CTurnBattleState::LoadUIData()
{
	// 흰색 사각형 텍스쳐 로드
	m_WhiteRectTextureID = (int)Renderer()->ReadPNGTextureFile("./resource/ui/whiteRect.png");

	m_UIEnemyNameBarTextureID = (int)Renderer()->ReadPNGTextureFile("./resource/ui/turnBattleAlpha/enemyNameBar.png");
	m_UIMainMenuBarTextureID = (int)Renderer()->ReadPNGTextureFile("./resource/ui/turnBattleAlpha/mainMenuBar.png");
	m_UIMainMenuBar2TextureID = (int)Renderer()->ReadPNGTextureFile("./resource/ui/turnBattleAlpha/mainMenuBar2.png");

	m_UIMouseButtonInformationTextureID =
		(int)Renderer()->ReadPNGTextureFile("./resource/ui/turnBattleAlpha/mouseButtonInformationBar.png");

	m_UIKeyInformationTextureID = (int)Renderer()->ReadPNGTextureFile("./resource/ui/turnBattleAlpha/keyInformationBar.png");

	m_UITurnNumberTextureID = (int)Renderer()->ReadPNGTextureFile("./resource/ui/turnBattleAlpha/turnNumber.png");

	m_UIHpSpBarTextureID = (int)Renderer()->ReadPNGTextureFile("./resource/ui/turnBattleAlpha/HpSpBar.png");
	m_UILongKeyButtonTextureID = (int)Renderer()->ReadPNGTextureFile("./resource/ui/turnBattleAlpha/longKeyButton.png");
	m_UISmallKeyButtonTextureID = (int)Renderer()->ReadPNGTextureFile("./resource/ui/turnBattleAlpha/smallKeyButton.png");

	m_UIListBarTextureID = (int)Renderer()->ReadPNGTextureFile("./resource/ui/turnBattleAlpha/listBar.png");
	m_UIListBarWithBoxTextureID = (int)Renderer()->ReadPNGTextureFile("./resource/ui/turnBattleAlpha/listBarWithBox.png");
	m_UIScrollBarTextureID = (int)Renderer()->ReadPNGTextureFile("./resource/ui/turnBattleAlpha/scrollBar.png");
	m_UIActBarTextureID = (int)Renderer()->ReadPNGTextureFile("./resource/ui/turnBattleAlpha/actBar.png");

	m_UICardForSkillDeckTextureID 
		= (int)Renderer()->ReadPNGTextureFile("./resource/ui/turnBattleAlpha/cardForSkillDeck.png");
	m_UICardForSkillDeckFrameTextureID
		= (int)Renderer()->ReadPNGTextureFile("./resource/ui/turnBattleAlpha/cardForSkillDeckFrame.png");

	m_UIActLogBarTextureID
		= (int)Renderer()->ReadPNGTextureFile("./resource/ui/turnBattleAlpha/actLogBar.png");

	m_UIBurnConditionTextureID
		= (int)Renderer()->ReadPNGTextureFile("./resource/ui/burn.png");
	m_UIAnxietyConditionTextureID
		= (int)Renderer()->ReadPNGTextureFile("./resource/ui/anxiety.png");

}

void CTurnBattleState::RenderEnemyNameBarUI(int x, int y)
{
	float middle, textLocationX, textLocationY;
	// 적 이름 표시 바
	wchar_t enemyLevelNameText[MAX_INFORMATION_SIZE];
	short level = m_EnemyMonsterVector.at(m_iTarget)->GetStatus().level;
	wchar_t nameText[MAX_NAME_SIZE];
	wcscpy_s(nameText, m_EnemyMonsterVector.at(m_iTarget)->GetName());
	glm::vec3 color;

	wsprintf(enemyLevelNameText, L"lv.%d   %s", level, nameText);
	Renderer()->UseShader(UI_SHADER);
	middle = 516.f / 2.f / (float)x;
	Renderer()->RenderTexture2D(m_UIEnemyNameBarTextureID,
		glm::vec2(0.5f - middle, 0.8f), glm::vec2(0.0f, 0.0f),
		glm::vec2(516.f, 85.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	Renderer()->UseShader(TEXT_SHADER);

	if (m_iTarget == 0) {
		color = DEFAULT_DISABLE_TEXT_COLOR;
	}
	else {
		color = DEFAULT_TEXT_COLOR;
	}
	textLocationX = 15.0f / (float)x;
	textLocationY = 20.f / (float)y;
	Renderer()->RenderText(L"A", 0.5f - middle + textLocationX, 0.8f + textLocationY,
		0.75f, color);

	if (m_iTarget == m_EnemyMonsterVector.size() - 1) {
		color = DEFAULT_DISABLE_TEXT_COLOR;
	}
	else {
		color = DEFAULT_TEXT_COLOR;
	}
	textLocationX = 470.f / (float)x;
	textLocationY = 20.f / (float)y;
	Renderer()->RenderText(L"D", 0.5f - middle + textLocationX, 0.8f + textLocationY,
		0.75f, color);

	textLocationX = 80.f / (float)x;
	textLocationY = 38.f / (float)y;
	Renderer()->RenderText(enemyLevelNameText, 0.5f - middle + textLocationX, 0.8f + textLocationY,
		0.65f, DEFAULT_TEXT_COLOR);
}

void CTurnBattleState::RenderMainMenuUI(int x, int y)
{
	float middle, textLocationX, textLocationY;
	float up = 0.0;

	// 메인 메뉴 표시 바
	Renderer()->UseShader(UI_SHADER);
	middle = 308.f / 2.f / (float)x;
	if (m_iAct == 0) {
		Renderer()->RenderTexture2D(m_UIMainMenuBarTextureID,
			glm::vec2(0.20f - middle, 0.125f), glm::vec2(0.0f, 0.0f),
			glm::vec2(308.f, 167.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}
	else {
		up = 157.f / (float)y;
		Renderer()->RenderTexture2D(m_UIMainMenuBar2TextureID,
			glm::vec2(0.20f - middle, 0.125f + up), glm::vec2(0.0f, 0.0f),
			glm::vec2(308.f, 112.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

		up = 100.0f / (float)y;
	}

	Renderer()->UseShader(TEXT_SHADER);
	wchar_t heroLevelNameText[20];
	short heroLevel = m_HeroVector.at(m_iOrder)->GetStatus().level;
	wchar_t heroNameText[10];
	wcscpy_s(heroNameText, m_HeroVector.at(m_iOrder)->GetName());
	wsprintf(heroLevelNameText, L"lv.%d     %s", heroLevel, heroNameText);

	textLocationX = -102.0f / (float)x;
	textLocationY = 202.0f / (float)y;
	Renderer()->RenderText(heroLevelNameText, 0.2f - middle + textLocationX, 0.125f + textLocationY + up,
		0.7f, glm::vec3(DEFAULT_BACKGROUND_COLOR));
	textLocationX = -100.0f / (float)x;
	textLocationY = 200.0f / (float)y;
	Renderer()->RenderText(heroLevelNameText, 0.2f - middle + textLocationX, 0.125f + textLocationY + up,
		0.7f, DEFAULT_TEXT_COLOR);

	// 사용 가능여부 체크
	int condition = m_HeroVector.at(m_iOrder)->GetBattleVariable().condition;

	glm::vec3 skillTextColor = DEFAULT_TEXT_COLOR;
	if (condition == ANXIETY_CONDITION) {
		skillTextColor = DEFAULT_DISABLE_TEXT_COLOR;
	}

	textLocationX = 15.0f / (float)x;
	textLocationY = 125.f / (float)y;
	Renderer()->RenderText(L"E", 0.2f - middle + textLocationX, 0.125f + textLocationY + up,
		0.8f, skillTextColor);

	textLocationX = 110.f / (float)x;
	textLocationY = 127.f / (float)y;
	Renderer()->RenderText(L"스킬 사용", 0.2f - middle + textLocationX, 0.125f + textLocationY + up,
		0.65f, skillTextColor);

	glm::vec3 summonColor = DEFAULT_DISABLE_TEXT_COLOR;
	if (m_iOrder == 0) {
		summonColor = DEFAULT_TEXT_COLOR;
	}

	textLocationX = 15.0f / (float)x;
	textLocationY = 67.f / (float)y;
	Renderer()->RenderText(L"F", 0.2f - middle + textLocationX, 0.125f + textLocationY + up,
		0.8f, summonColor);

	textLocationX = 80.f / (float)x;
	textLocationY = 72.f / (float)y;
	Renderer()->RenderText(L"오토마타 소환", 0.2f - middle + textLocationX, 0.125f + textLocationY + up,
		0.65f, summonColor);

	if (m_iAct == 0) {
		textLocationX = 15.0f / (float)x;
		textLocationY = 13.f / (float)y;
		Renderer()->RenderText(L"S", 0.2f - middle + textLocationX, 0.125f + textLocationY,
			0.8f, DEFAULT_TEXT_COLOR);

		textLocationX = 110.f / (float)x;
		textLocationY = 15.5f / (float)y;
		Renderer()->RenderText(L"메뉴 확장", 0.2f - middle + textLocationX, 0.125f + textLocationY,
			0.65f, DEFAULT_TEXT_COLOR);

		if (m_bIsShowEnemyInformation) {
			RenderEnemyInformationUI(x, y);
		}
	}
	else {
		float barLocationX;
		float barLocationY;

		for (int i = 1; i < 5; ++i) {
			float forward = 0.0f;

			if (m_iAct == i) {
				forward = 50.0f;
			}

			barLocationX = (forward) / float(x);
			barLocationY = (60.0f - i * 53.0f) / float(y);

			Renderer()->UseShader(UI_SHADER);
			Renderer()->RenderTexture2D(m_UIActBarTextureID,
				glm::vec2(0.20f - middle + barLocationX, 0.125f + up + barLocationY), glm::vec2(0.0f, 0.0f),
				glm::vec2(308.f, 50.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

			wchar_t actMenu[10];
			float gap = 0.0f;
			glm::vec3 actColor = DEFAULT_TEXT_COLOR;
			Renderer()->UseShader(TEXT_SHADER);
			if (i == 1) {
				wcscpy_s(actMenu, L"덱 변경");
				gap = 108.f;
				if (m_iOrder != 0) {
					actColor = DEFAULT_DISABLE_TEXT_COLOR;
				}
			}
			else if (i == 2) {
				wcscpy_s(actMenu, L"스틸");
				gap = 123.f;
				if (m_iOrder != 0) {
					actColor = DEFAULT_DISABLE_TEXT_COLOR;
				}
			}
			else if (i == 3) {
				wcscpy_s(actMenu, L"대기");
				gap = 123.f;
			}
			else if (i == 4) {
				wcscpy_s(actMenu, L"도주");
				gap = 123.f;
			}

			textLocationX = (gap + forward) / (float)x;
			textLocationY = (72.0f - i * 53.0f) / (float)y;
			Renderer()->RenderText(actMenu, 0.2f - middle + textLocationX, 0.125f + textLocationY + up,
				0.65f, actColor);
		}
	}
}

void CTurnBattleState::RenderKeyInformationUIForMainMenu(int x, int y)
{
	float middle, textLocationX, textLocationY;

	// 버튼 정보 표시 바
	Renderer()->UseShader(UI_SHADER);
	middle = 251.f / 2.f / (float)x;
	Renderer()->RenderTexture2D(m_UIMouseButtonInformationTextureID,
		glm::vec2(0.89f - middle, 0.3f), glm::vec2(0.0f, 0.0f),
		glm::vec2(251.f, 112.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	float barLocationX = -7.f / (float)x;
	float barLocationY = 109.f / (float)y;
	Renderer()->RenderTexture2D(m_UIKeyInformationTextureID,
		glm::vec2(0.89f - middle + barLocationX, 0.3f + barLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(250.f, 50.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	Renderer()->UseShader(TEXT_SHADER);

	// 사용 가능 여부 체크
	int condition = m_HeroVector.at(m_iOrder)->GetBattleVariable().condition;

	TurnBattle::SSkill skill;
	skill.turn = 1;
	skill.sp = 0;
	glm::vec3 textColor = DEFAULT_TEXT_COLOR;
	if (!m_HeroVector.at(m_iOrder)->CheckEnableSkill(&skill) || condition == BURN_CONDITION)
		textColor = DEFAULT_DISABLE_TEXT_COLOR;

	textLocationX = 75.f / (float)x;
	textLocationY = 70.f / (float)y;
	Renderer()->RenderText(L"물리 공격", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.625f, textColor);

	// 사용 가능 여부 체크
	skill.sp = 1;
	textColor = DEFAULT_TEXT_COLOR;
	if (!m_HeroVector.at(m_iOrder)->CheckEnableSkill(&skill) || condition == ANXIETY_CONDITION)
		textColor = DEFAULT_DISABLE_TEXT_COLOR;

	textLocationX = 75.f / (float)x;
	textLocationY = 17.5f / (float)y;
	Renderer()->RenderText(L"정신 공격", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.625f, textColor);

	wchar_t qText[MAX_NAME_SIZE] = L"적 정보 표시";
	if (m_bIsShowEnemyInformation) {
		wsprintf(qText, L"적 정보 닫기");
	}
	textLocationX = -2.5f / (float)x;
	textLocationY = 125.5f / (float)y;
	Renderer()->RenderText(L" Q", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.5f, DEFAULT_TEXT_COLOR);

	textLocationX = 60.f / (float)x;
	textLocationY = 122.5f / (float)y;
	Renderer()->RenderText(qText, 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.625f, DEFAULT_TEXT_COLOR);
}

void CTurnBattleState::RenderKeyInformationUIForActMenu(int x, int y)
{
	float middle, textLocationX, textLocationY;
	float barLocationX, barLocationY;

	// 버튼 정보 표시 바
	Renderer()->UseShader(UI_SHADER);
	middle = 251.f / 2.f / (float)x;
	Renderer()->RenderTexture2D(m_UIMouseButtonInformationTextureID,
		glm::vec2(0.89f - middle, 0.3f), glm::vec2(0.0f, 0.0f),
		glm::vec2(251.f, 112.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	barLocationX = -7.f / (float)x;
	barLocationY = 161.f / (float)y;
	Renderer()->RenderTexture2D(m_UIKeyInformationTextureID,
		glm::vec2(0.89f - middle + barLocationX, 0.3f + barLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(250.f, 50.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	barLocationX = -7.f / (float)x;
	barLocationY = 109.f / (float)y;
	Renderer()->RenderTexture2D(m_UIKeyInformationTextureID,
		glm::vec2(0.89f - middle + barLocationX, 0.3f + barLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(250.f, 50.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	Renderer()->UseShader(TEXT_SHADER);
	textLocationX = 110 / (float)x;
	textLocationY = 70.f / (float)y;
	Renderer()->RenderText(L"실행", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.625f, DEFAULT_TEXT_COLOR);

	textLocationX = 110.f / (float)x;
	textLocationY = 17.5f / (float)y;
	Renderer()->RenderText(L"취소", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.625f, DEFAULT_TEXT_COLOR);

	textLocationX = 0.f / (float)x;
	textLocationY = 177.f / (float)y;
	Renderer()->RenderText(L"W", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.5f, DEFAULT_TEXT_COLOR);

	if (m_iAct != 1) {
		textLocationX = 90.f / (float)x;
		textLocationY = 175.f / (float)y;
		Renderer()->RenderText(L"위 메뉴", 0.89f - middle + textLocationX, 0.3f + textLocationY,
			0.625f, DEFAULT_TEXT_COLOR);
	}
	else {
		textLocationX = 75.f / (float)x;
		textLocationY = 175.f / (float)y;
		Renderer()->RenderText(L"메뉴 축소", 0.89f - middle + textLocationX, 0.3f + textLocationY,
			0.625f, DEFAULT_TEXT_COLOR);
	}

	glm::vec3 textColor = DEFAULT_TEXT_COLOR;

	if (m_iAct == 4) {
		textColor = DEFAULT_DISABLE_TEXT_COLOR;
	}
	textLocationX = 0.f / (float)x;
	textLocationY = 124.5f / (float)y;
	Renderer()->RenderText(L" S", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.5f, textColor);

	textLocationX = 75.f / (float)x;
	textLocationY = 122.5f / (float)y;
	Renderer()->RenderText(L"아래 메뉴", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.625f, textColor);


	RenderSpaceAndShiftButtonUI(x, y);
}

void CTurnBattleState::RenderKeyInformationUIForSkillMenu(int x, int y)
{
	float middle, textLocationX, textLocationY;
	float barLocationX, barLocationY;

	short selectedSkillIndex = m_HeroVector.at(m_iOrder)->GetSkillVector().at(m_iSelectedSkill);
	short selectedSkillIndexTurn = selectedSkillIndex % 10 + 1;

	// 버튼 정보 표시 바
	Renderer()->UseShader(UI_SHADER);
	middle = 251.f / 2.f / (float)x;
	Renderer()->RenderTexture2D(m_UIMouseButtonInformationTextureID,
		glm::vec2(0.89f - middle, 0.3f), glm::vec2(0.0f, 0.0f),
		glm::vec2(251.f, 112.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	barLocationX = -7.f / (float)x;
	barLocationY = 161.f / (float)y;
	Renderer()->RenderTexture2D(m_UIKeyInformationTextureID,
		glm::vec2(0.89f - middle + barLocationX, 0.3f + barLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(250.f, 50.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	barLocationX = -7.f / (float)x;
	barLocationY = 109.f / (float)y;
	Renderer()->RenderTexture2D(m_UIKeyInformationTextureID,
		glm::vec2(0.89f - middle + barLocationX, 0.3f + barLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(250.f, 50.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	Renderer()->UseShader(TEXT_SHADER);
	glm::vec3 textColor = DEFAULT_TEXT_COLOR;

	// 스킬 사용 가능 여부 체크
	TurnBattle::SSkill* skill = m_SkillMap.find(selectedSkillIndex)->second;
	if (!m_HeroVector.at(m_iOrder)->CheckEnableSkill(skill))
		textColor = DEFAULT_DISABLE_TEXT_COLOR;

	textLocationX = 110 / (float)x;
	textLocationY = 70.f / (float)y;
	Renderer()->RenderText(L"선택", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.625f, textColor);

	textLocationX = 110.f / (float)x;
	textLocationY = 17.5f / (float)y;
	Renderer()->RenderText(L"취소", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.625f, DEFAULT_TEXT_COLOR);

	textLocationX = 2.5f / (float)x;
	textLocationY = 178.f / (float)y;
	Renderer()->RenderText(L"Q", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.5f, DEFAULT_TEXT_COLOR);

	wchar_t qText[MAX_NAME_SIZE] = L"정보 표시";
	if (m_bIsShowSkillInformation) {
		wsprintf(qText, L"정보 닫기");
	}
	textLocationX = 75.f / (float)x;
	textLocationY = 175.f / (float)y;
	Renderer()->RenderText(qText, 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.625f, DEFAULT_TEXT_COLOR);

	textColor = DEFAULT_TEXT_COLOR;
	if (!(selectedSkillIndexTurn == 1 || selectedSkillIndexTurn == 2 || selectedSkillIndexTurn == 3)) {
		textColor = DEFAULT_DISABLE_TEXT_COLOR;
	}

	// 파트, 메트는 따로 처리
	if (1000 <= selectedSkillIndex && selectedSkillIndex <= 1020) {
		if (selectedSkillIndexTurn <= 4) { 
			textColor = DEFAULT_TEXT_COLOR;
			if (selectedSkillIndexTurn >= 3) {
				selectedSkillIndexTurn = 3;
			}
			else {
				selectedSkillIndexTurn = 1;
			}
		}
	}
	
	textLocationX = 0.f / (float)x;
	textLocationY = 124.5f / (float)y;
	Renderer()->RenderText(L" R", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.5f, textColor);

	if (selectedSkillIndexTurn >= 3) {
		textLocationX = 60.f / (float)x;
		textLocationY = 122.5f / (float)y;
		Renderer()->RenderText(L"스킬 초기화", 0.89f - middle + textLocationX, 0.3f + textLocationY,
			0.625f, textColor);
	}
	else {
		textLocationX = 75.f / (float)x;
		textLocationY = 122.5f / (float)y;
		Renderer()->RenderText(L"스킬 강화", 0.89f - middle + textLocationX, 0.3f + textLocationY,
			0.625f, textColor);
	}


	RenderSpaceAndShiftButtonUI(x, y);
}

void CTurnBattleState::RenderKeyInformationUIForAutomataMenu(int x, int y)
{
	float middle, textLocationX, textLocationY;
	float barLocationX, barLocationY;
	short selectedAutomataIndex;

	if (m_HeroVector.at(m_iOrder)->GetAutomataVector().size() != 0) {
		selectedAutomataIndex = m_HeroVector.at(m_iOrder)->GetAutomataVector().at(m_iSelectedAutomata);
	}
	// 버튼 정보 표시 바
	Renderer()->UseShader(UI_SHADER);
	middle = 251.f / 2.f / (float)x;
	Renderer()->RenderTexture2D(m_UIMouseButtonInformationTextureID,
		glm::vec2(0.89f - middle, 0.3f), glm::vec2(0.0f, 0.0f),
		glm::vec2(251.f, 112.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	barLocationX = -7.f / (float)x;
	barLocationY = 161.f / (float)y;
	Renderer()->RenderTexture2D(m_UIKeyInformationTextureID,
		glm::vec2(0.89f - middle + barLocationX, 0.3f + barLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(250.f, 50.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	barLocationX = -7.f / (float)x;
	barLocationY = 109.f / (float)y;
	Renderer()->RenderTexture2D(m_UIKeyInformationTextureID,
		glm::vec2(0.89f - middle + barLocationX, 0.3f + barLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(250.f, 50.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	Renderer()->UseShader(TEXT_SHADER);
	textLocationX = 110 / (float)x;
	textLocationY = 70.f / (float)y;

	glm::vec3 textColor = DEFAULT_TEXT_COLOR;

	if (m_HeroVector.at(m_iOrder)->GetAutomataVector().size() != 0) {
		if (m_sSummonedAutomata[0] == selectedAutomataIndex || m_sSummonedAutomata[1] == selectedAutomataIndex ||
			m_HeroVector.at(0)->GetAutomataVector().size() == 0) {
			textColor = DEFAULT_DISABLE_TEXT_COLOR;
		}
	}
	else {
		textColor = DEFAULT_DISABLE_TEXT_COLOR;
	}
	Renderer()->RenderText(L"소환", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.625f, textColor);

	textLocationX = 110.f / (float)x;
	textLocationY = 17.5f / (float)y;
	Renderer()->RenderText(L"취소", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.625f, DEFAULT_TEXT_COLOR);

	textColor = DEFAULT_TEXT_COLOR;
	if (m_HeroVector.at(m_iOrder)->GetAutomataVector().size() == 0) {
		textColor = DEFAULT_DISABLE_TEXT_COLOR;
	}

	textLocationX = 2.5f / (float)x;
	textLocationY = 178.f / (float)y;
	Renderer()->RenderText(L"Q", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.5f, textColor);

	wchar_t qText[MAX_NAME_SIZE] = L"정보 표시";
	if (m_bIsShowAutomataInformation) {
		wsprintf(qText, L"정보 닫기");
	}
	textLocationX = 75.f / (float)x;
	textLocationY = 175.f / (float)y;
	Renderer()->RenderText(qText, 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.625f, textColor);

	textColor = DEFAULT_TEXT_COLOR;
	if (m_HeroVector.at(m_iOrder)->GetAutomataVector().size() != 0) {
		if (m_sSummonedAutomata[0] != selectedAutomataIndex && m_sSummonedAutomata[1] != selectedAutomataIndex) {
			textColor = DEFAULT_DISABLE_TEXT_COLOR;
		}
	}
	else {
		textColor = DEFAULT_DISABLE_TEXT_COLOR;
	}

	textLocationX = 0.f / (float)x;
	textLocationY = 124.5f / (float)y;
	Renderer()->RenderText(L" R", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.5f, textColor);

	textLocationX = 75.f / (float)x;
	textLocationY = 122.5f / (float)y;
	Renderer()->RenderText(L"소환 해제", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.625f, textColor);

	RenderSpaceAndShiftButtonUI(x, y);
}

void CTurnBattleState::RenderKeyInformationUIForDeckChangeMenu(int x, int y)
{
	float middle, textLocationX, textLocationY;
	float barLocationX, barLocationY;

	// 버튼 정보 표시 바
	Renderer()->UseShader(UI_SHADER);
	middle = 251.f / 2.f / (float)x;
	Renderer()->RenderTexture2D(m_UIMouseButtonInformationTextureID,
		glm::vec2(0.89f - middle, 0.3f), glm::vec2(0.0f, 0.0f),
		glm::vec2(251.f, 112.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	barLocationX = -7.f / (float)x;
	barLocationY = 213.f / (float)y;
	Renderer()->RenderTexture2D(m_UIKeyInformationTextureID,
		glm::vec2(0.89f - middle + barLocationX, 0.3f + barLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(250.f, 50.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	barLocationX = -7.f / (float)x;
	barLocationY = 161.f / (float)y;
	Renderer()->RenderTexture2D(m_UIKeyInformationTextureID,
		glm::vec2(0.89f - middle + barLocationX, 0.3f + barLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(250.f, 50.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	barLocationX = -7.f / (float)x;
	barLocationY = 109.f / (float)y;
	Renderer()->RenderTexture2D(m_UIKeyInformationTextureID,
		glm::vec2(0.89f - middle + barLocationX, 0.3f + barLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(250.f, 50.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	
	Renderer()->UseShader(TEXT_SHADER);
	textLocationX = 110.f / (float)x;
	textLocationY = 17.5f / (float)y;
	Renderer()->RenderText(L"취소", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.625f, DEFAULT_TEXT_COLOR);

	glm::vec3 textColor = DEFAULT_TEXT_COLOR;
	if ((m_HeroVector.at(m_iOrder)->GetDeckVector().size() == 0)) {
		textColor = DEFAULT_DISABLE_TEXT_COLOR;
	}

	textLocationX = 110 / (float)x;
	textLocationY = 70.f / (float)y;
	Renderer()->RenderText(L"변경", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.625f, textColor);

	textLocationX = 3.f / (float)x;
	textLocationY = 230.f / (float)y;
	Renderer()->RenderText(L"R", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.5f, textColor);

	textLocationX = 75.f / (float)x;
	textLocationY = 226.f / (float)y;
	Renderer()->RenderText(L"카드 제거", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.625f, textColor);


	textColor = DEFAULT_TEXT_COLOR;
	if (m_iSelectedSkillDeck == 0 || m_HeroVector.at(m_iOrder)->GetDeckVector().size() == 0) {
		textColor = DEFAULT_DISABLE_TEXT_COLOR;
	}

	textLocationX = 3.f / (float)x;
	textLocationY = 178.f / (float)y;
	Renderer()->RenderText(L"A", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.5f, textColor);

	textLocationX = 75.f / (float)x;
	textLocationY = 175.f / (float)y;
	Renderer()->RenderText(L"이전 카드", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.625f, textColor);

	textColor = DEFAULT_TEXT_COLOR;
	if (m_iSelectedSkillDeck == m_HeroVector.at(m_iOrder)->GetDeckVector().size() - 1) {
		textColor = DEFAULT_DISABLE_TEXT_COLOR;
	}

	textLocationX = 0.f / (float)x;
	textLocationY = 124.5f / (float)y;
	Renderer()->RenderText(L" D", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.5f, textColor);

	textLocationX = 75.f / (float)x;
	textLocationY = 122.5f / (float)y;
	Renderer()->RenderText(L"다음 카드", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.625f, textColor);

	RenderSpaceAndShiftButtonUI(x, y);
}

void CTurnBattleState::RenderKeyInformationUIForStillAndWait(int x, int y)
{
	float middle, textLocationX, textLocationY;
	float bartextLocationY;

	// 버튼 정보 표시 바
	Renderer()->UseShader(UI_SHADER);
	middle = 251.f / 2.f / (float)x;
	bartextLocationY = 50.0f / (float)y;
	Renderer()->RenderSpriteSheet2D(m_UIMouseButtonInformationTextureID, 1, 2, 0, 0,
		glm::vec2(0.89f - middle, 0.3f + bartextLocationY),
		glm::vec2(0.0f, 0.0f), glm::vec2(251.f, 112.f / 2.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	Renderer()->UseShader(TEXT_SHADER);
	textLocationX = 110.f / (float)x;
	textLocationY = 67.5f / (float)y;
	Renderer()->RenderText(L"취소", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.625f, DEFAULT_TEXT_COLOR);

	RenderShiftButtonUI(x, y);
}

void CTurnBattleState::RenderHPSPBarUI(int x, int y)
{
	float middle, textLocationX, textLocationY;

	float gap = -65.f;

	for (int i = 0; i < m_HeroVector.size(); ++i)
	{
		float forwardScale = -0.038f;
		if (i == m_iOrder && m_bHeroTeam) {
			forwardScale = -0.085f;
		}

		// HP, SP 바
		Renderer()->UseShader(UI_SHADER);
		middle = 498.f / 2.f / (float)x;

		// 흰색으로 채우기
		float colorLocationX = 2.0f / (float)x;
		float colorLocationY = (5.0f + i * gap) / (float)y;
		Renderer()->RenderTexture2D(m_WhiteRectTextureID,
			glm::vec2(0.81f - middle + colorLocationX + forwardScale, 0.19f + colorLocationY), glm::vec2(0.0f, 0.0f),
			glm::vec2(238.f, 30.f), DEFAULT_BACKGROUND_COLOR);
		colorLocationX = 257.0f / (float)x;
		colorLocationY = (5.0f + i * gap) / (float)y;
		Renderer()->RenderTexture2D(m_WhiteRectTextureID,
			glm::vec2(0.81f - middle + colorLocationX + forwardScale, 0.19f + colorLocationY), glm::vec2(0.0f, 0.0f),
			glm::vec2(238.f, 30.f), DEFAULT_BACKGROUND_COLOR);

		TurnBattle::SStatus status = m_HeroVector.at(i)->GetStatus();
		TurnBattle::SBattleVariable battleVariable = m_HeroVector.at(i)->GetBattleVariable();

		int hp = battleVariable.hp;
		int sp = battleVariable.sp;
		int maxHp = status.maxHp;
		int maxSp = status.maxSp;

		glm::vec4 hpColor;
		glm::vec4 spColor;

		if (i == 0) {
			hpColor = glm::vec4(1.0f, 0.0f, 0.5f, 1.0f);
			spColor = glm::vec4(0.0f, 0.5f, 1.0f, 1.0f);
		}
		else {
			hpColor = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
			spColor = glm::vec4(0.0f, 0.5f, 0.5f, 1.0f);
		}

		// 약한 빨간색으로 채우기
		colorLocationX = 7.0f / (float)x;
		colorLocationY = (5.0f + i * gap) / (float)y;
		Renderer()->RenderTexture2D(m_WhiteRectTextureID,
			glm::vec2(0.81f - middle + colorLocationX + forwardScale, 0.19f + colorLocationY),
			glm::vec2(0.0f, 0.0f), glm::vec2(230.f * (float)hp / (float)maxHp, 30.f), hpColor);

		// 약한 파란색으로 채우기
		colorLocationX = 260.0f / (float)x;
		colorLocationY = (5.0f + i * gap) / (float)y;
		Renderer()->RenderTexture2D(m_WhiteRectTextureID,
			glm::vec2(0.81f - middle + colorLocationX + forwardScale, 0.19f + colorLocationY),
			glm::vec2(0.0f, 0.0f), glm::vec2(230.f * (float)sp / (float)maxSp, 30.f), spColor);

		// 바 테두리
		colorLocationY = (i * gap) / (float)y;
		Renderer()->RenderTexture2D(m_UIHpSpBarTextureID,
			glm::vec2(0.81f - middle + forwardScale, 0.19f + colorLocationY), glm::vec2(0.0f, 0.0f),
			glm::vec2(498.f, 42.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

		// 상태 이상 표시
		short conditionType = battleVariable.condition;
		if (conditionType != NORMAL_CONDITION) {
			int conditionTextureID = EMPTY_VALUE;
			if (conditionType == BURN_CONDITION) {
				conditionTextureID = m_UIBurnConditionTextureID;
			}
			else if (conditionType == ANXIETY_CONDITION) {
				conditionTextureID = m_UIAnxietyConditionTextureID;
			}

			if (conditionTextureID != EMPTY_VALUE) {
				colorLocationX = -80.0f / (float)x;
				colorLocationY = (3 + i * gap) / (float)y;
				Renderer()->RenderTexture2D(m_WhiteRectTextureID,
					glm::vec2(0.81f - middle + colorLocationX + forwardScale, 0.19f + colorLocationY),
					glm::vec2(0.0f, 0.0f), glm::vec2(40, 40), DEFAULT_BACKGROUND_COLOR
					* glm::vec4(1.0f, 1.0f, 1.0f, battleVariable.conditionDuration / RECOVER_CONDITION_TIME));
				Renderer()->RenderTexture2D(conditionTextureID,
					glm::vec2(0.81f - middle + colorLocationX + forwardScale, 0.19f + colorLocationY),
					glm::vec2(0.0f, 0.0f), glm::vec2(40, 40),
					glm::vec4(1.0f, 1.0f, 1.0f, battleVariable.conditionDuration / RECOVER_CONDITION_TIME));
			}			
		}

		wchar_t hpText[10];
		wchar_t spText[10];
		if (hp < 10) {
			wsprintf(hpText, L"00%d", hp);
		}
		else if (hp < 100) {
			wsprintf(hpText, L"0%d", hp);
		}
		else {
			wsprintf(hpText, L"%d", hp);
		}

		if (sp < 10) {
			wsprintf(spText, L"00%d", sp);
		}
		else if (sp < 100) {
			wsprintf(spText, L"0%d", sp);
		}
		else {
			wsprintf(spText, L"%d", sp);
		}
		Renderer()->UseShader(TEXT_SHADER);

		// 소환된 오토마타 위치
		wchar_t location[3];
		int iLocation = i;

		wsprintf(location, L"%d", iLocation);
		textLocationX = -27.0f / (float)x;
		textLocationY = (10.0f + i * gap) / (float)y;
		Renderer()->RenderText(location, 0.81f - middle + textLocationX + forwardScale, 0.19f + textLocationY,
			0.7f, glm::vec3(DEFAULT_BACKGROUND_COLOR));

		wsprintf(location, L"%d", iLocation);
		textLocationX = -25.0f / (float)x;
		textLocationY = (8.0f + i * gap) / (float)y;
		Renderer()->RenderText(location, 0.81f - middle + textLocationX + forwardScale, 0.19f + textLocationY,
			0.7f, DEFAULT_TEXT_COLOR);


		textLocationX = 5.f / (float)x;
		textLocationY = (45.0f + i * gap) / (float)y;
		Renderer()->RenderText(L"HP", 0.81f - middle + textLocationX + forwardScale, 0.19f + textLocationY,
			0.55f, glm::vec3(DEFAULT_BACKGROUND_COLOR));

		textLocationX = 7.f / (float)x;
		textLocationY = (43.0f + i * gap) / (float)y;
		Renderer()->RenderText(L"HP", 0.81f - middle + textLocationX + forwardScale, 0.19f + textLocationY,
			0.55f, DEFAULT_TEXT_COLOR);

		textLocationX = 260.f / (float)x;
		textLocationY = (45.0f + i * gap) / (float)y;
		Renderer()->RenderText(L"SP", 0.81f - middle + textLocationX + forwardScale, 0.19f + textLocationY,
			0.55f, glm::vec3(DEFAULT_BACKGROUND_COLOR));

		textLocationX = 262.f / (float)x;
		textLocationY = (43.0f + i * gap) / (float)y;
		Renderer()->RenderText(L"SP", 0.81f - middle + textLocationX + forwardScale, 0.19f + textLocationY,
			0.55f, DEFAULT_TEXT_COLOR);

		textLocationX = 185.f / (float)x;
		textLocationY = (45.0f + i * gap) / (float)y;
		Renderer()->RenderText(hpText, 0.81f - middle + textLocationX + forwardScale, 0.19f + textLocationY,
			0.55f, glm::vec3(DEFAULT_BACKGROUND_COLOR));

		textLocationX = 187.f / (float)x;
		textLocationY = (43.0f + i * gap) / (float)y;
		Renderer()->RenderText(hpText, 0.81f - middle + textLocationX + forwardScale, 0.19f + textLocationY,
			0.55f, DEFAULT_TEXT_COLOR);

		textLocationX = 440.f / (float)x;
		textLocationY = (45.0f + i * gap) / (float)y;
		Renderer()->RenderText(spText, 0.81f - middle + textLocationX + forwardScale, 0.19f + textLocationY,
			0.55f, glm::vec3(DEFAULT_BACKGROUND_COLOR));

		textLocationX = 442.f / (float)x;
		textLocationY = (43.0f + i * gap) / (float)y;
		Renderer()->RenderText(spText, 0.81f - middle + textLocationX + forwardScale, 0.19f + textLocationY,
			0.55f, DEFAULT_TEXT_COLOR);
	}
}

void CTurnBattleState::RenderSkillListUI(int x, int y)
{
	Renderer()->UseShader(UI_SHADER);
	float middle, textLocationX, textLocationY;
	float imageLocationX, imageLocationY;

	middle = 308.f / 2.f / (float)x;

	Renderer()->UseShader(TEXT_SHADER);

	textLocationX = 63.0f / (float)x;
	textLocationY = 202.0f / (float)y;
	Renderer()->RenderText(L"SKILL LIST", 0.2f - middle + textLocationX, 0.125f + textLocationY,
		0.7f, glm::vec3(DEFAULT_BACKGROUND_COLOR));
	textLocationX = 65.0f / (float)x;
	textLocationY = 200.0f / (float)y;
	Renderer()->RenderText(L"SKILL LIST", 0.2f - middle + textLocationX, 0.125f + textLocationY,
		0.7f, DEFAULT_TEXT_COLOR);

	// 스크롤 바
	Renderer()->UseShader(UI_SHADER);
	imageLocationX = 4.0f / (float)x;
	imageLocationY = 5.0f / (float)y;
	Renderer()->RenderTexture2D(m_UIScrollBarTextureID,
		glm::vec2(0.20f - middle + imageLocationX, 0.125f + imageLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(30.f, 150.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	int skillListSize = (int)m_HeroVector.at(m_iOrder)->GetSkillVector().size();
	imageLocationX = 4.0f / (float)x;
	imageLocationY = (135.0f - 115.0f / skillListSize - (115.0f / skillListSize) * m_iSelectedSkill) / (float)y;
	Renderer()->RenderTexture2D(m_WhiteRectTextureID,
		glm::vec2(0.20f - middle + imageLocationX, 0.125f + imageLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(30.f, 120.0f / skillListSize), glm::vec4(DEFAULT_TEXT_COLOR, 1.0f));

	imageLocationX = 0.0f / (float)x;
	imageLocationY = 130.0f / (float)y;
	Renderer()->RenderTexture2D(m_UISmallKeyButtonTextureID,
		glm::vec2(0.20f - middle + imageLocationX, 0.125f + imageLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(38.f, 38.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	imageLocationX = 0.0f / (float)x;
	imageLocationY = -10.0f / (float)y;
	Renderer()->RenderTexture2D(m_UISmallKeyButtonTextureID,
		glm::vec2(0.20f - middle + imageLocationX, 0.125f + imageLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(38.f, 38.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	glm::vec3 color;
	if (m_iSelectedSkill == 0) {
		color = DEFAULT_DISABLE_TEXT_COLOR;
	}
	else {
		color = DEFAULT_TEXT_COLOR;
	}
	Renderer()->UseShader(TEXT_SHADER);
	textLocationX = 5.0f / (float)x;
	textLocationY = 138.0f / (float)y;
	Renderer()->RenderText(L"W", 0.2f - middle + textLocationX, 0.125f + textLocationY,
		0.5f, color);

	if (m_iSelectedSkill == m_HeroVector.at(m_iOrder)->GetSkillVector().size() - 1) {
		color = DEFAULT_DISABLE_TEXT_COLOR;
	}
	else {
		color = DEFAULT_TEXT_COLOR;
	}
	textLocationX = 5.0f / (float)x;
	textLocationY = -2.0f / (float)y;
	Renderer()->RenderText(L" S", 0.2f - middle + textLocationX, 0.125f + textLocationY,
		0.5f, color);

	int firstShowedSkill = m_iSelectedSkill - m_iShowedSkillNum;

	int skillSize = (int)m_HeroVector.at(m_iOrder)->GetSkillVector().size();

	int maxShowedSkill = 4;
	if (maxShowedSkill > skillSize)
		maxShowedSkill = skillSize;

	for (int i = 0; i < maxShowedSkill; ++i) {
		float forward = 0.0f;
		if (m_iShowedSkillNum == i) {
			forward = 50.0f;
		}
		Renderer()->UseShader(UI_SHADER);
		imageLocationX = (70.0f + forward) / (float)x;
		imageLocationY = (130.0f - i * 50.0f) / (float)y;
		Renderer()->RenderTexture2D(m_UIListBarTextureID,
			glm::vec2(0.20f - middle + imageLocationX, 0.125f + imageLocationY), glm::vec2(0.0f, 0.0f),
			glm::vec2(337.f, 50.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

		Renderer()->UseShader(TEXT_SHADER);

		// 스킬 데이터
		short index = m_HeroVector.at(m_iOrder)->GetSkillVector().at(firstShowedSkill++);
		TurnBattle::SSkill* skill = m_SkillMap.find(index)->second;

		glm::vec3 textColor = DEFAULT_TEXT_COLOR;

		if (!m_HeroVector.at(m_iOrder)->CheckEnableSkill(skill))
			textColor = DEFAULT_DISABLE_TEXT_COLOR;

		// 소모 턴 수
		wchar_t turn[5];
		int iTurn = skill->turn;
		wsprintf(turn, L"%d", iTurn);
		textLocationX = (80.0f + forward) / (float)x;
		textLocationY = (142.0f - i * 50.0f) / (float)y;
		Renderer()->RenderText(turn, 0.2f - middle + textLocationX, 0.125f + textLocationY,
			0.7f, textColor);
		// 턴(T)
		textLocationX = (100.0f + forward) / (float)x;
		textLocationY = (135.0f - i * 50.0f) / (float)y;
		Renderer()->RenderText(L"T", 0.2f - middle + textLocationX, 0.125f + textLocationY,
			0.4f, textColor);
		// 스킬 이름
		textLocationX = (123.0f + forward) / (float)x;
		textLocationY = (145.0f - i * 50.0f) / (float)y;
		Renderer()->RenderText(skill->name, 0.2f - middle + textLocationX, 0.125f + textLocationY,
			0.55f, textColor);

		// 소모 sp량
		wchar_t sp[10];
		int iSP = skill->sp;

		if (iSP < 10) {
			wsprintf(sp, L"    %d", iSP);
		}
		else if (iSP < 100) {
			wsprintf(sp, L"  %d", iSP);
		}
		else {
			wsprintf(sp, L"%d", iSP);
		}
		textLocationX = (332.0f + forward) / (float)x;
		textLocationY = (145.0f - i * 50.0f) / (float)y;
		Renderer()->RenderText(sp, 0.2f - middle + textLocationX, 0.125f + textLocationY,
			0.55f, textColor);

		// 소모 단위(SP)
		textLocationX = (375.0f + forward) / (float)x;
		textLocationY = (135.0f - i * 50.0f) / (float)y;
		Renderer()->RenderText(L"SP", 0.2f - middle + textLocationX, 0.125f + textLocationY,
			0.45f, textColor);
	}
}

void CTurnBattleState::RenderAutomataListUI(int x, int y)
{
	Renderer()->UseShader(UI_SHADER);
	float middle, textLocationX, textLocationY;
	float imageLocationX, imageLocationY;

	middle = 308.f / 2.f / (float)x;

	Renderer()->UseShader(TEXT_SHADER);

	textLocationX = 63.0f / (float)x;
	textLocationY = 202.0f / (float)y;
	Renderer()->RenderText(L"AUTOMATA LIST", 0.2f - middle + textLocationX, 0.125f + textLocationY,
		0.7f, glm::vec3(DEFAULT_BACKGROUND_COLOR));
	textLocationX = 65.0f / (float)x;
	textLocationY = 200.0f / (float)y;
	Renderer()->RenderText(L"AUTOMATA LIST", 0.2f - middle + textLocationX, 0.125f + textLocationY,
		0.7f, DEFAULT_TEXT_COLOR);

	// 스크롤 바
	Renderer()->UseShader(UI_SHADER);
	imageLocationX = 4.0f / (float)x;
	imageLocationY = 5.0f / (float)y;
	Renderer()->RenderTexture2D(m_UIScrollBarTextureID,
		glm::vec2(0.20f - middle + imageLocationX, 0.125f + imageLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(30.f, 150.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	int automataListSize = (int)m_HeroVector.at(m_iOrder)->GetAutomataVector().size();
	imageLocationX = 4.0f / (float)x;
	imageLocationY = (135.0f - 115.0f / automataListSize - (115.0f / automataListSize) * m_iSelectedAutomata) / (float)y;
	Renderer()->RenderTexture2D(m_WhiteRectTextureID,
		glm::vec2(0.20f - middle + imageLocationX, 0.125f + imageLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(30.f, 120.0f / automataListSize), glm::vec4(DEFAULT_TEXT_COLOR, 1.0f));

	imageLocationX = 0.0f / (float)x;
	imageLocationY = 130.0f / (float)y;
	Renderer()->RenderTexture2D(m_UISmallKeyButtonTextureID,
		glm::vec2(0.20f - middle + imageLocationX, 0.125f + imageLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(38.f, 38.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	imageLocationX = 0.0f / (float)x;
	imageLocationY = -10.0f / (float)y;
	Renderer()->RenderTexture2D(m_UISmallKeyButtonTextureID,
		glm::vec2(0.20f - middle + imageLocationX, 0.125f + imageLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(38.f, 38.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	glm::vec3 color;
	if (m_iSelectedAutomata == 0) {
		color = DEFAULT_DISABLE_TEXT_COLOR;
	}
	else {
		color = DEFAULT_TEXT_COLOR;
	}
	Renderer()->UseShader(TEXT_SHADER);
	textLocationX = 5.0f / (float)x;
	textLocationY = 138.0f / (float)y;
	Renderer()->RenderText(L"W", 0.2f - middle + textLocationX, 0.125f + textLocationY,
		0.5f, color);

	if (m_iSelectedAutomata == m_HeroVector.at(m_iOrder)->GetAutomataVector().size() - 1) {
		color = DEFAULT_DISABLE_TEXT_COLOR;
	}
	else {
		color = DEFAULT_TEXT_COLOR;
	}
	textLocationX = 5.0f / (float)x;
	textLocationY = -2.0f / (float)y;
	Renderer()->RenderText(L" S", 0.2f - middle + textLocationX, 0.125f + textLocationY,
		0.5f, color);

	int firstShowedAutomata = m_iSelectedAutomata - m_iShowedAutomataNum;

	int automataSize = (int)m_HeroVector.at(m_iOrder)->GetAutomataVector().size();

	if (automataSize == 0) {
		// 오토마타가 없을 경우 리턴
		return;
	}

	int maxShowedAutomata = 4;
	if (maxShowedAutomata > automataSize)
		maxShowedAutomata = automataSize;

	short heroSp = m_HeroVector.at(m_iOrder)->GetBattleVariable().sp;

	for (int i = 0; i < maxShowedAutomata; ++i) {
		float forward = 0.0f;
		if (m_iShowedAutomataNum == i) {
			forward = 50.0f;
		}
		Renderer()->UseShader(UI_SHADER);
		imageLocationX = (70.0f + forward) / (float)x;
		imageLocationY = (130.0f - i * 50.0f) / (float)y;
		Renderer()->RenderTexture2D(m_UIListBarWithBoxTextureID,
			glm::vec2(0.20f - middle + imageLocationX, 0.125f + imageLocationY), glm::vec2(0.0f, 0.0f),
			glm::vec2(337.f, 50.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

		Renderer()->UseShader(TEXT_SHADER);

		// 오토마타 데이터
		short index = m_HeroVector.at(m_iOrder)->GetAutomataVector().at(firstShowedAutomata++);
		TurnBattle::SAutomata* automata = m_AutomataMap.find(index)->second;

		// 소환 넘버
		if (m_sSummonedAutomata[0] == index || m_sSummonedAutomata[1] == index) {
			wchar_t location[3];
			int iLocation = 2;

			if (m_sSummonedAutomata[0] == index)
				iLocation = 1;

			wsprintf(location, L"%d", iLocation);
			textLocationX = (85.0f + forward) / (float)x;
			textLocationY = (142.0f - i * 50.0f) / (float)y;
			Renderer()->RenderText(location, 0.2f - middle + textLocationX, 0.125f + textLocationY,
				0.75f, DEFAULT_TEXT_COLOR);
		}

		// 소모 sp량
		wchar_t sp[10];
		int iSP = automata->status.level * 2;

		glm::vec3 textColor = DEFAULT_TEXT_COLOR;

		if (iSP > heroSp)
			textColor = DEFAULT_DISABLE_TEXT_COLOR;

		if (iSP < 10) {
			wsprintf(sp, L"    %d", iSP);
		}
		else if (iSP < 100) {
			wsprintf(sp, L"  %d", iSP);
		}
		else {
			wsprintf(sp, L"%d", iSP);
		}
		textLocationX = (332.0f + forward) / (float)x;
		textLocationY = (145.0f - i * 50.0f) / (float)y;
		Renderer()->RenderText(sp, 0.2f - middle + textLocationX, 0.125f + textLocationY,
			0.55f, textColor);

		// 소모 단위(SP)
		textLocationX = (375.0f + forward) / (float)x;
		textLocationY = (135.0f - i * 50.0f) / (float)y;
		Renderer()->RenderText(L"SP", 0.2f - middle + textLocationX, 0.125f + textLocationY,
			0.45f, textColor);

		// 오토마타 이름
		textLocationX = (123.0f + forward) / (float)x;
		textLocationY = (145.0f - i * 50.0f) / (float)y;
		Renderer()->RenderText(automata->name, 0.2f - middle + textLocationX, 0.125f + textLocationY,
			0.55f, textColor);
	}
}

void CTurnBattleState::RenderSpaceAndShiftButtonUI(int x, int y)
{
	float middle, textLocationX, textLocationY;
	float barLocationX, barLocationY;

	Renderer()->UseShader(UI_SHADER);
	middle = 251.f / 2.f / (float)x;

	// 버튼 이미지 
	barLocationX = -108.f / (float)x;
	barLocationY = 12.f / (float)y;
	Renderer()->RenderTexture2D(m_UILongKeyButtonTextureID,
		glm::vec2(0.89f - middle + barLocationX, 0.3f + barLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(73.f, 31.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	barLocationX = -108.f / (float)x;
	barLocationY = 67.f / (float)y;
	Renderer()->RenderTexture2D(m_UILongKeyButtonTextureID,
		glm::vec2(0.89f - middle + barLocationX, 0.3f + barLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(73.f, 31.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	Renderer()->UseShader(TEXT_SHADER);

	textLocationX = -101.f / (float)x;
	textLocationY = 76.f / (float)y;
	Renderer()->RenderText(L"Space", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.45f, DEFAULT_TEXT_COLOR);

	textLocationX = -95.f / (float)x;
	textLocationY = 19.f / (float)y;
	Renderer()->RenderText(L"Shift", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.45f, DEFAULT_TEXT_COLOR);

	textLocationX = -31.f / (float)x;
	textLocationY = 77.f / (float)y;
	Renderer()->RenderText(L"OR", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.4f, glm::vec3(DEFAULT_BACKGROUND_COLOR));
	textLocationX = -29.f / (float)x;
	textLocationY = 75.f / (float)y;
	Renderer()->RenderText(L"OR", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.4f, DEFAULT_TEXT_COLOR);

	textLocationX = -31.f / (float)x;
	textLocationY = 22.f / (float)y;
	Renderer()->RenderText(L"OR", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.4f, glm::vec3(DEFAULT_BACKGROUND_COLOR));
	textLocationX = -29.f / (float)x;
	textLocationY = 20.f / (float)y;
	Renderer()->RenderText(L"OR", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.4f, DEFAULT_TEXT_COLOR);
}

void CTurnBattleState::RenderShiftButtonUI(int x, int y)
{
	float middle, textLocationX, textLocationY;
	float barLocationX, barLocationY;

	Renderer()->UseShader(UI_SHADER);
	middle = 251.f / 2.f / (float)x;

	// 버튼 이미지 
	barLocationX = -108.f / (float)x;
	barLocationY = 62.f / (float)y;
	Renderer()->RenderTexture2D(m_UILongKeyButtonTextureID,
		glm::vec2(0.89f - middle + barLocationX, 0.3f + barLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(73.f, 31.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	Renderer()->UseShader(TEXT_SHADER);

	textLocationX = -95.f / (float)x;
	textLocationY = 69.f / (float)y;
	Renderer()->RenderText(L"Shift", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.45f, DEFAULT_TEXT_COLOR);

	textLocationX = -31.f / (float)x;
	textLocationY = 72.f / (float)y;
	Renderer()->RenderText(L"OR", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.4f, glm::vec3(DEFAULT_BACKGROUND_COLOR));
	textLocationX = -29.f / (float)x;
	textLocationY = 70.f / (float)y;
	Renderer()->RenderText(L"OR", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.4f, DEFAULT_TEXT_COLOR);
}

void CTurnBattleState::RenderTurns(int x, int y)
{
	Renderer()->UseShader(UI_SHADER);
	float middle, textLocationX, textLocationY;
	float imageLocationX, imageLocationY;

	// 현재 남은 턴 수 표시(키보드 메뉴 표시 바의 위치로 위치 결정)
	middle = 308.f / 2.f / (float)x;

	// 행동 메뉴안일시 위로 이동
	float up = 0.0f;
	if (m_iMenu == 0 && m_iAct != 0)
		up = 100.f / (float)y;

	imageLocationX = -120.0f / (float)x;
	imageLocationY = 20.0f / (float)y;
	Renderer()->RenderSpriteSheet2D(m_UITurnNumberTextureID, 10, 1,
		m_HeroVector.at(m_iOrder)->GetBattleVariable().turn, 0,
		glm::vec2(0.2f - middle + imageLocationX, 0.125f + imageLocationY + up),
		glm::vec2(0.0f, 0.0f), glm::vec2(100.0f, 150.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	Renderer()->UseShader(TEXT_SHADER);
	textLocationX = -102.0f / (float)x;
	textLocationY = 12.0f / (float)y;
	Renderer()->RenderText(L"Turns", 0.2f - middle + textLocationX, 0.125f + textLocationY + up,
		0.5f, glm::vec3(DEFAULT_BACKGROUND_COLOR));
	textLocationX = -100.0f / (float)x;
	textLocationY = 10.0f / (float)y;
	Renderer()->RenderText(L"Turns", 0.2f - middle + textLocationX, 0.125f + textLocationY + up,
		0.5f, DEFAULT_TEXT_COLOR);
}

void CTurnBattleState::RenderChangeSkillDeckUI(int x, int y)
{
	float middle, textLocationX, textLocationY;

	middle = 167.f / 2.0f / (float)x;

	Renderer()->UseShader(TEXT_SHADER);

	textLocationX = 32.0f / (float)x;
	textLocationY = 60.0f / (float)y;
	Renderer()->RenderText(L"CHANGE DECK", 0.12f - middle + textLocationX, 0.35f + textLocationY,
		0.75f, glm::vec3(DEFAULT_BACKGROUND_COLOR));
	textLocationX = 30.0f / (float)x;
	textLocationY = 62.0f / (float)y;
	Renderer()->RenderText(L"CHANGE DECK", 0.12f - middle + textLocationX, 0.35f + textLocationY,
		0.75f, DEFAULT_TEXT_COLOR);

	for (int i = 0; i < m_HeroVector.at(m_iOrder)->GetDeckVector().size(); ++i) {
		RenderSkillDeck(x, y, 0.0f, 0.0f, i);

		float up = 0.0f;
		if (i == m_iSelectedSkillDeck) {
			up = 60.0f;
		}
		Renderer()->UseShader(UI_SHADER);
		middle = 167.f / 2.0f / (float)x;
		float imageLocationX = (230.f * i) / (float)x;
		float imageLocationY = (150.0f + up) / (float)y;
		Renderer()->RenderTexture2D(m_UICardForSkillDeckFrameTextureID,
			glm::vec2(0.12f - middle + imageLocationX, 0.35f + imageLocationY), glm::vec2(0.0f, 0.0f),
			glm::vec2(167.f, 269.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		
	}
}

void CTurnBattleState::RenderSkillDeck(int x, int y, float locationX, float locationY, int index)
{
	float middle, textLocationX, textLocationY;
	float imageLocationX, imageLocationY;
	float up = 0;

	middle = 167.f / 2.0f / (float)x;

	Renderer()->UseShader(UI_SHADER);
	up = 0.0f;
	if (m_iSelectedSkillDeck == index) {
		up = 60.0f;
	}
	imageLocationX = (230.f * index) / (float)x;
	imageLocationY = (150.0f + up) / (float)y;
	Renderer()->RenderTexture2D(m_UICardForSkillDeckTextureID,
		glm::vec2(0.12f - middle + imageLocationX + locationX, 0.35f + imageLocationY + locationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(167.f, 269.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));


	Renderer()->UseShader(TEXT_SHADER);

	up = 0.0f;
	if (m_iSelectedSkillDeck == index) {
		up = 60.0f;
	}
	short deckIndex = m_HeroVector.at(0)->GetDeckVector().at(index);

	TurnBattle::SDeck* deck = m_DeckMap.find(deckIndex)->second;

	textLocationX = (25.0f + 230 * index) / (float)x;
	textLocationY = (370.0f + up) / (float)y;
	Renderer()->RenderText(deck->name, 0.12f - middle + textLocationX + locationX, 0.35f + textLocationY + locationY,
		0.6f, DEFAULT_TEXT_COLOR);

	textLocationX = (10.0f + 230 * index) / (float)x;
	textLocationY = (270.0f + up) / (float)y;
	Renderer()->RenderText(L"강점: ", 0.12f - middle + textLocationX + locationX, 0.35f + textLocationY + locationY,
		0.5f, DEFAULT_TEXT_COLOR);
	textLocationX = (65.0f + 230 * index) / (float)x;
	textLocationY = (270.0f + up) / (float)y;
	Renderer()->RenderText(deck->strengh, 0.12f - middle + textLocationX + locationX, 0.35f + textLocationY + locationY,
		0.5f, DEFAULT_TEXT_COLOR);

	textLocationX = (10.0f + 230 * index) / (float)x;
	textLocationY = (235.0f + up) / (float)y;
	Renderer()->RenderText(L"약점: ", 0.12f - middle + textLocationX + locationX, 0.35f + textLocationY + locationY,
		0.5f, DEFAULT_TEXT_COLOR);
	textLocationX = (65.0f + 230 * index) / (float)x;
	textLocationY = (235.0f + up) / (float)y;
	Renderer()->RenderText(deck->weak, 0.12f - middle + textLocationX + locationX, 0.35f + textLocationY + locationY,
		0.5f, DEFAULT_TEXT_COLOR);

	textLocationX = (10.0f + 230 * index) / (float)x;
	textLocationY = (170.0f + up) / (float)y;
	Renderer()->RenderText(deck->information, 0.12f - middle + textLocationX + locationX, 0.35f + textLocationY + locationY,
		0.45f, DEFAULT_TEXT_COLOR);
}

void CTurnBattleState::RenderStealMenuUI(int x, int y)
{
	float middle, imageLocationX, imageLocationY;
	float textLocationX, textLocationY;

	// 현재 남은 턴 수 표시(키보드 메뉴 표시 바의 위치로 위치 결정)
	middle = 308.f / 2.f / (float)x;

	Renderer()->UseShader(UI_SHADER);

	imageLocationX = 0.0f / (float)x;
	imageLocationY = 100.0f / (float)y;

	Renderer()->RenderSpriteSheet2D(m_UIMainMenuBar2TextureID, 1, 2, 0, 0,
		glm::vec2(0.2f - middle + imageLocationX, 0.125f + imageLocationY),
		glm::vec2(0.0f, 0.0f), glm::vec2(308.f, 115.f / 2.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	imageLocationX = 0.0f / (float)x;
	imageLocationY = 10.0f / (float)y;
	Renderer()->RenderSpriteSheet2D(m_UIMainMenuBar2TextureID, 1, 2, 0, 0,
		glm::vec2(0.2f - middle + imageLocationX, 0.125f + imageLocationY),
		glm::vec2(0.0f, 0.0f), glm::vec2(308.f, 115.f / 2.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	Renderer()->UseShader(TEXT_SHADER);
	wchar_t heroLevelText[20];
	short Level = m_HeroVector.at(m_iOrder)->GetStatus().level;
	wsprintf(heroLevelText, L"lv.%d", Level);

	textLocationX = -92.0f / (float)x;
	textLocationY = 182.0f / (float)y;
	Renderer()->RenderText(heroLevelText, 0.2f - middle + textLocationX, 0.125f + textLocationY,
		0.7f, glm::vec3(DEFAULT_BACKGROUND_COLOR));
	textLocationX = -90.0f / (float)x;
	textLocationY = 180.0f / (float)y;
	Renderer()->RenderText(heroLevelText, 0.2f - middle + textLocationX, 0.125f + textLocationY,
		0.7f, DEFAULT_TEXT_COLOR);


	textLocationX = 10.0f / (float)x;
	textLocationY = 182.0f / (float)y;
	Renderer()->RenderText(L"STEAL TARGET'S", 0.2f - middle + textLocationX, 0.125f + textLocationY,
		0.7f, glm::vec3(DEFAULT_BACKGROUND_COLOR));
	textLocationX = 12.0f / (float)x;
	textLocationY = 180.0f / (float)y;
	Renderer()->RenderText(L"STEAL TARGET'S", 0.2f - middle + textLocationX, 0.125f + textLocationY,
		0.7f, DEFAULT_TEXT_COLOR);

	short targetLevel = m_EnemyMonsterVector.at(m_iTarget)->GetStatus().level;
	short heroLevel = m_HeroVector.at(m_iOrder)->GetStatus().level;

	glm::vec3 textColor = DEFAULT_TEXT_COLOR;
	if (heroLevel < targetLevel)
		textColor = DEFAULT_DISABLE_TEXT_COLOR;

	textLocationX = 10.0f / (float)x;
	textLocationY = 115.f / (float)y;
	Renderer()->RenderText(L"W", 0.2f - middle + textLocationX, 0.125f + textLocationY,
		0.8f, textColor);

	textLocationX = 115.f / (float)x;
	textLocationY = 118.f / (float)y;
	Renderer()->RenderText(L"오토마타", 0.2f - middle + textLocationX, 0.125f + textLocationY,
		0.65f, textColor);

	textLocationX = 20.0f / (float)x;
	textLocationY = 24.f / (float)y;
	Renderer()->RenderText(L"S", 0.2f - middle + textLocationX, 0.125f + textLocationY,
		0.8f, textColor);

	textLocationX = 125.f / (float)x;
	textLocationY = 28.f / (float)y;
	Renderer()->RenderText(L"스킬 덱", 0.2f - middle + textLocationX, 0.125f + textLocationY,
		0.65f, textColor);
}

void CTurnBattleState::RenderWaitMenuUI(int x, int y)
{
	float middle, imageLocationX, imageLocationY;
	float textLocationX, textLocationY;

	// 현재 남은 턴 수 표시(키보드 메뉴 표시 바의 위치로 위치 결정)
	middle = 308.f / 2.f / (float)x;

	Renderer()->UseShader(UI_SHADER);

	imageLocationX = 0.0f / (float)x;
	imageLocationY = 100.0f / (float)y;
	Renderer()->RenderSpriteSheet2D(m_UIMainMenuBar2TextureID, 1, 2, 0, 0,
		glm::vec2(0.2f - middle + imageLocationX, 0.125f + imageLocationY),
		glm::vec2(0.0f, 0.0f), glm::vec2(308.f, 115.f / 2.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	imageLocationX = 0.0f / (float)x;
	imageLocationY = 10.0f / (float)y;
	Renderer()->RenderSpriteSheet2D(m_UIMainMenuBar2TextureID, 1, 2, 0, 0,
		glm::vec2(0.2f - middle + imageLocationX, 0.125f + imageLocationY),
		glm::vec2(0.0f, 0.0f), glm::vec2(308.f, 115.f / 2.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	Renderer()->UseShader(TEXT_SHADER);

	textLocationX = 10.0f / (float)x;
	textLocationY = 192.0f / (float)y;
	Renderer()->RenderText(L"WAIT FOR", 0.2f - middle + textLocationX, 0.125f + textLocationY,
		0.7f, glm::vec3(DEFAULT_BACKGROUND_COLOR));
	textLocationX = 12.0f / (float)x;
	textLocationY = 190.0f / (float)y;
	Renderer()->RenderText(L"WAIT FOR", 0.2f - middle + textLocationX, 0.125f + textLocationY,
		0.7f, DEFAULT_TEXT_COLOR);

	textLocationX = 10.0f / (float)x;
	textLocationY = 115.f / (float)y;
	Renderer()->RenderText(L"W", 0.2f - middle + textLocationX, 0.125f + textLocationY,
		0.8f, DEFAULT_TEXT_COLOR);

	textLocationX = 140.f / (float)x;
	textLocationY = 118.f / (float)y;
	Renderer()->RenderText(L"경계", 0.2f - middle + textLocationX, 0.125f + textLocationY,
		0.65f, DEFAULT_TEXT_COLOR);

	textLocationX = 20.0f / (float)x;
	textLocationY = 24.f / (float)y;
	Renderer()->RenderText(L"S", 0.2f - middle + textLocationX, 0.125f + textLocationY,
		0.8f, DEFAULT_TEXT_COLOR);

	textLocationX = 140.f / (float)x;
	textLocationY = 28.f / (float)y;
	Renderer()->RenderText(L"휴식", 0.2f - middle + textLocationX, 0.125f + textLocationY,
		0.65f, DEFAULT_TEXT_COLOR);
}

void CTurnBattleState::RenderActLogUI(int x, int y)
{
	float middle, textLocationX, textLocationY;

	Renderer()->UseShader(UI_SHADER);
	middle = 516.f / 2.f / (float)x;
	Renderer()->RenderTexture2D(m_UIActLogBarTextureID,
		glm::vec2(0.5f - middle, 0.8f), glm::vec2(0.0f, 0.0f),
		glm::vec2(516.f, 85.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	Renderer()->UseShader(TEXT_SHADER);

	textLocationX = 66.f / (float)x;
	textLocationY = 38.f / (float)y;
	Renderer()->RenderText(m_wLogText, 0.5f - middle + textLocationX, 0.8f + textLocationY,
		0.65f, DEFAULT_TEXT_COLOR);

	m_fShowLogTime -= BackBoard()->GetElapsedTimeInSec();

	if (m_fShowLogTime < 0.0f)
		m_bShowLog = false;
}

void CTurnBattleState::RenderChangingSkillDeckUIAnimation(int x, int y)
{
	if (!m_bIsChangedSkillDeckAnimationEnd) {
		m_fChangeAnimationTime += BackBoard()->GetElapsedTimeInSec();
	}
	
	if (!m_bIsChangedSkillDeckAnimationMovingEnd) {
		float locationX = 0.12f - ((167.f / 2.0f) / (float)x) + ((230.f * m_iSelectedSkillDeck) / (float)x);
		float locationY = 0.35f + ((210.0f) / (float)y);
		glm::vec2 location = glm::vec2(locationX, locationY);

		glm::vec2 direction = glm::normalize(glm::vec2(0.45f, 0.4f) - glm::vec2(locationX, locationY));

		float length = glm::distance(glm::vec2(0.45f, 0.4f), glm::vec2(locationX, locationY));

		glm::vec2 finalLocation = location + direction * length * m_fChangeAnimationTime;


		if (m_fChangeAnimationTime > 1.0f) {
			//Renderer()->SetVertexAttributeForChangeSkillDeckParticle();
			m_bIsChangedSkillDeckAnimationMovingEnd = true;
			m_fChangeAnimationTime = 0.0f;
		}
		if (m_fChangeAnimationTime > 1.0f) {
			finalLocation = glm::vec2(0.45f, 0.4f);
		}

		RenderSkillDeck(x, y, finalLocation.x - location.x, finalLocation.y - location.y, m_iSelectedSkillDeck);
	} 
	else {

		if (m_fChangeAnimationTime > 1.0f) {
			m_bIsChangedSkillDeckAnimationEnd = true;
			m_fChangeAnimationTime = 0.0f;
			m_bIsChangedSkillDeckAnimationMovingEnd = false;
		}
		Renderer()->RenderChangeSkillDeckParticle(m_UICardForSkillDeckTextureID, m_fChangeAnimationTime);	
	}
}

void CTurnBattleState::RenderSkillInformationUI(int x, int y)
{
	float middle, textLocationX, textLocationY;
	float imageLocationX, imageLocationY;
	middle = 308.f / 2.f / (float)x;

	Renderer()->UseShader(UI_SHADER);

	imageLocationX = 250.0f / (float)x;
	imageLocationY = 185.0f / (float)y;
	Renderer()->RenderTexture2D(m_UICardForSkillDeckTextureID,
		glm::vec2(0.2f - middle + imageLocationX, 0.125f + imageLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(760.f, 110.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	// 선택된 스킬 가져오기
	short skillIndex = m_HeroVector.at(m_iOrder)->GetSkillVector().at(m_iSelectedSkill);
	TurnBattle::SSkill* skill = m_SkillMap.find(skillIndex)->second;

	Renderer()->UseShader(TEXT_SHADER);

	textLocationX = 280.0f / (float)x;
	textLocationY = 260.0f / (float)y;
	Renderer()->RenderText(skill->informationLine1, 0.2f - middle + textLocationX, 0.125f + textLocationY,
		0.55f, DEFAULT_TEXT_COLOR);
	textLocationX = 280.0f / (float)x;
	textLocationY = 225.0f / (float)y;
	Renderer()->RenderText(skill->informationLine2, 0.2f - middle + textLocationX, 0.125f + textLocationY,
		0.55f, DEFAULT_TEXT_COLOR);
}

void CTurnBattleState::RenderAutomataInformationUI(int x, int y)
{
	float middle, textLocationX, textLocationY;
	float imageLocationX, imageLocationY;
	middle = 308.f / 2.f / (float)x;

	Renderer()->UseShader(UI_SHADER);

	imageLocationX = -100.0f / (float)x;
	imageLocationY = 250.0f / (float)y;
	Renderer()->RenderTexture2D(m_UICardForSkillDeckTextureID,
		glm::vec2(0.2f - middle + imageLocationX, 0.125f + imageLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(1000.f, 470.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	imageLocationX = -50.0f / (float)x;
	imageLocationY = 550.0f / (float)y;
	Renderer()->RenderTexture2D(m_WhiteRectTextureID,
		glm::vec2(0.2f - middle + imageLocationX, 0.125f + imageLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(900.f, 5.f), glm::vec4(DEFAULT_TEXT_COLOR, 1.0f));

	for (int i = 1; i < ATTRIBUTE_COUNT; ++i) {
		imageLocationX = (-50.0f + i * 100.0f) / (float)x;
		imageLocationY = 500.0f / (float)y;
		Renderer()->RenderTexture2D(m_WhiteRectTextureID,
			glm::vec2(0.2f - middle + imageLocationX, 0.125f + imageLocationY), glm::vec2(0.0f, 0.0f),
			glm::vec2(5.f, 110.f), glm::vec4(DEFAULT_TEXT_COLOR, 1.0f));
	}

	imageLocationX = -50.0f / (float)x;
	imageLocationY = 450.0f / (float)y;
	Renderer()->RenderTexture2D(m_WhiteRectTextureID,
		glm::vec2(0.2f - middle + imageLocationX, 0.125f + imageLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(900.f, 5.f), glm::vec4(DEFAULT_TEXT_COLOR, 1.0f));

	imageLocationX = 300.0f / (float)x;
	imageLocationY = 280.0f / (float)y;
	Renderer()->RenderTexture2D(m_WhiteRectTextureID,
		glm::vec2(0.2f - middle + imageLocationX, 0.125f + imageLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(5.f, 210.f), glm::vec4(DEFAULT_TEXT_COLOR, 1.0f));

	// 선택된 오토마타 데이터 가져오기
	short automataIndex = m_HeroVector.at(m_iOrder)->GetAutomataVector().at(m_iSelectedAutomata);
	TurnBattle::SAutomata* automata = m_AutomataMap.find(automataIndex)->second;

	Renderer()->UseShader(TEXT_SHADER);
	wchar_t tempText[MAX_INFORMATION_SIZE];

	// 이름
	wsprintf(tempText, L"Level. %hd   %s ", automata->status.level, automata->englishName);
	textLocationX = -30.0f / (float)x;
	textLocationY = 640.0f / (float)y;
	Renderer()->RenderText(tempText, 0.2f - middle + textLocationX, 0.125f + textLocationY,
		0.6f, DEFAULT_TEXT_COLOR);

	// 속성 정보
	wchar_t attributeName[ATTRIBUTE_COUNT][MAX_NAME_SIZE] = {
		L"화(I)", L"수(A)", L"지(T)", L"풍(V)", L"멸(P)", L"선(B)", L"악(M)", L"광(L)", L"암(N)"
	};
	for (int i = 0; i < ATTRIBUTE_COUNT; ++i) {
		textLocationX = (-30.0f + i * 100.0f) / (float)x;
		textLocationY = 570.0f / (float)y;
		Renderer()->RenderText(attributeName[i], 0.2f - middle + textLocationX, 0.125f + textLocationY,
			0.6f, DEFAULT_TEXT_COLOR);
	}
	wchar_t weaknessName[5][MAX_NAME_SIZE] = {
		 L"   NON", L"  HALF", L" ", L" WEAK", L"AWFUL"
	};
	for (int i = 0; i < ATTRIBUTE_COUNT; ++i) {
		textLocationX = (-40.0f + i * 100.0f) / (float)x;
		textLocationY = 520.0f / (float)y;

		short weakness = automata->status.attribute[i];
		glm::vec3 textColor = DAMAGE_FONT_COLOR_NORMAL;
		if (weakness == AWFUL) {
			textColor = DAMAGE_FONT_COLOR_AWFUL;
		}
		else if (weakness == WEAK) {
			textColor = DAMAGE_FONT_COLOR_WEAK;
		}
		else if (weakness == HALF) {
			textColor = DAMAGE_FONT_COLOR_HALF;
		}
		else if (weakness == NON) {
			textColor = DAMAGE_FONT_COLOR_NON;
		}

		Renderer()->RenderText(weaknessName[weakness + 2]
			, 0.2f - middle + textLocationX, 0.125f + textLocationY, 0.45f, textColor);
	}

	// 스탯 정보
	textLocationX = -40.0f / (float)x;
	textLocationY = 465.0f / (float)y;
	Renderer()->RenderText(L"Stats", 0.2f - middle + textLocationX, 0.125f + textLocationY,
		0.5f, DEFAULT_TEXT_COLOR);

	wchar_t statName[6][MAX_NAME_SIZE] = {
		L"물질 공격: ", L"물질 방어: ", L"정신 공격: ", L"정신 방어: ", L"명중률:      ", L"회피률:      " 
	};
	for (int i = 0; i < 6; ++i) {
		wsprintf(tempText, L"%s %hd", statName[i], automata->status.stat[i]);
		textLocationX = (-30.0f + (i % 2) * 180) / (float)x;
		textLocationY = (400.0f - 20 * (i - (i % 2))) / (float)y;
		Renderer()->RenderText(tempText, 0.2f - middle + textLocationX, 0.125f + textLocationY,
			0.5f, DEFAULT_TEXT_COLOR);
	}

	// 스킬 정보
	textLocationX = 315.0f / (float)x;
	textLocationY = 465.0f / (float)y;
	Renderer()->RenderText(L"Skills", 0.2f - middle + textLocationX, 0.125f + textLocationY,
		0.5f, DEFAULT_TEXT_COLOR);

	for (int i = 0; i < automata->skillSize; ++i) {
		textLocationX = (330.0f + (i / 3) * 250) / (float)x;
		textLocationY = (400.0f - 45 * (i % 3)) / (float)y;
		Renderer()->RenderText(m_SkillMap.find(automata->skillList[i])->second->name
			, 0.2f - middle + textLocationX, 0.125f + textLocationY, 0.5f, DEFAULT_TEXT_COLOR);
	}
}

void CTurnBattleState::RenderEnemyInformationUI(int x, int y)
{
	float middle, textLocationX, textLocationY;
	float imageLocationX, imageLocationY;
	middle = 516.f / 2.f / (float)x;

	Renderer()->UseShader(UI_SHADER);

	imageLocationX = -480.0f / (float)x;
	imageLocationY = -175.0f / (float)y;
	Renderer()->RenderTexture2D(m_UICardForSkillDeckTextureID,
		glm::vec2(0.5f - middle + imageLocationX, 0.8f + imageLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(960.f, 150.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	// 선택된 적 데이터 가져오기
	short enemyIndex = m_iTarget;
	CTurnBattleObject* enemy = m_EnemyMonsterVector.at(enemyIndex);

	bool analyzed = false;
	auto it = m_MonsterAnalyzeEndDataMap.find(enemy->GetMonsterId());
	if (it != m_MonsterAnalyzeEndDataMap.end()) {
		analyzed = it->second;
	}

	if (analyzed) {
		imageLocationX = -450.0f / (float)x;
		imageLocationY = -100.0f / (float)y;
		Renderer()->RenderTexture2D(m_WhiteRectTextureID,
			glm::vec2(0.5f - middle + imageLocationX, 0.8f + imageLocationY), glm::vec2(0.0f, 0.0f),
			glm::vec2(900.f, 5.f), glm::vec4(DEFAULT_TEXT_COLOR, 1.0f));

		for (int i = 1; i < ATTRIBUTE_COUNT; ++i) {
			imageLocationX = (-450.0f + i * 100.0f) / (float)x;
			imageLocationY = -150.0f / (float)y;
			Renderer()->RenderTexture2D(m_WhiteRectTextureID,
				glm::vec2(0.5f - middle + imageLocationX, 0.8f + imageLocationY), glm::vec2(0.0f, 0.0f),
				glm::vec2(5.f, 110.f), glm::vec4(DEFAULT_TEXT_COLOR, 1.0f));
		}

		Renderer()->UseShader(TEXT_SHADER);

		// 속성 정보
		wchar_t attributeName[ATTRIBUTE_COUNT][MAX_NAME_SIZE] = {
			L"화(I)", L"수(A)", L"지(T)", L"풍(V)", L"멸(P)", L"선(B)", L"악(M)", L"광(L)", L"암(N)"
		};
		for (int i = 0; i < ATTRIBUTE_COUNT; ++i) {
			textLocationX = (-430.0f + i * 100.0f) / (float)x;
			textLocationY = -80.0f / (float)y;
			Renderer()->RenderText(attributeName[i], 0.5f - middle + textLocationX, 0.8f + textLocationY,
				0.6f, DEFAULT_TEXT_COLOR);
		}
		wchar_t weaknessName[5][MAX_NAME_SIZE] = {
			 L"   NON", L"  HALF", L" ", L" WEAK", L"AWFUL"
		};
		for (int i = 0; i < ATTRIBUTE_COUNT; ++i) {
			textLocationX = (-440.0f + i * 100.0f) / (float)x;
			textLocationY = -130.0f / (float)y;

			short weakness = enemy->GetStatus().attribute[i];
			glm::vec3 textColor = DAMAGE_FONT_COLOR_NORMAL;
			if (weakness == AWFUL) {
				textColor = DAMAGE_FONT_COLOR_AWFUL;
			}
			else if (weakness == WEAK) {
				textColor = DAMAGE_FONT_COLOR_WEAK;
			}
			else if (weakness == HALF) {
				textColor = DAMAGE_FONT_COLOR_HALF;
			}
			else if (weakness == NON) {
				textColor = DAMAGE_FONT_COLOR_NON;
			}

			Renderer()->RenderText(weaknessName[weakness + 2]
				, 0.5f - middle + textLocationX, 0.8f + textLocationY, 0.45f, textColor);
		}
	}
	else {
		Renderer()->UseShader(TEXT_SHADER);
		textLocationX = -100.0f / (float)x;
		textLocationY = -120.0f / (float)y;
		Renderer()->RenderText(L"NO DATA", 0.5f - middle + textLocationX, 0.8f + textLocationY, 
			0.65f, DAMAGE_FONT_COLOR_NORMAL);
	}
}
