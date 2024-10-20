#include "stdafx.h"
#include "TurnBattleState.h"
#include "BossMonster.h"

void CTurnBattleState::TurnModeUIRender()
{
	int resolutionX, resolutionY;
	Renderer()->GetDisplayResolution(&resolutionX, &resolutionY);

	// 적 몬스터 이름 표시 UI 렌더링
	if (!m_bIsProcessing && m_bHeroTeam) {
		if (m_iMenu != CHANGE_DECK_MENU) {
			if (m_iMenu != WAIT_MENU && m_iMenu != AUTOMATA_MENU) {
				TurnModeRenderEnemyNameBarUI(resolutionX, resolutionY);
			}
			RenderTurns(resolutionX, resolutionY);
		}
		if (m_iMenu == MAIN_MENU) {
			// 메인 메뉴 UI 렌더링
			RenderMainMenuUI(resolutionX, resolutionY);
			// 키 설명 UI 렌더링
			if (m_iAct == NO_ACT_MENU) {
				TurnModeRenderKeyInformationUIForMainMenu(resolutionX, resolutionY);
			}
			else {
				RenderKeyInformationUIForActMenu(resolutionX, resolutionY);
			}
		}
		else {
			if (m_iMenu == SKILL_MENU) {
				RenderSkillListUI(resolutionX, resolutionY);
				RenderKeyInformationUIForSkillMenu(resolutionX, resolutionY);
				if (m_bIsShowSkillInformation) {
					RenderSkillInformationUI(resolutionX, resolutionY);
				}
			}
			else if (m_iMenu == AUTOMATA_MENU) {
				RenderAutomataListUI(resolutionX, resolutionY);
				RenderKeyInformationUIForAutomataMenu(resolutionX, resolutionY);
				if (m_bIsShowAutomataInformation) {
					RenderAutomataInformationUI(resolutionX, resolutionY);
				}
			}
			else if (m_iMenu == CHANGE_DECK_MENU) {
				RenderChangeSkillDeckUI(resolutionX, resolutionY);
				RenderKeyInformationUIForDeckChangeMenu(resolutionX, resolutionY);
			}
			else if (m_iMenu == STEAL_MENU) {
				RenderStealMenuUI(resolutionX, resolutionY);
				RenderKeyInformationUIForStillAndWait(resolutionX, resolutionY);
			}
			else if (m_iMenu == WAIT_MENU) {
				RenderWaitMenuUI(resolutionX, resolutionY);
				RenderKeyInformationUIForStillAndWait(resolutionX, resolutionY);
			}
		}
	}
	else {
		if (m_bIsAdditionalNormalAttackTime && m_bHeroTeam) {
			RenderTurns(resolutionX, resolutionY);
			RenderKeyInformationUIForMainMenu(resolutionX, resolutionY);
		}

		if (m_bIsSkillDeckChanged) {
			RenderChangingSkillDeckUIAnimation(resolutionX, resolutionY);
		}

		if (m_bShowLog) {
			RenderActLogUI(resolutionX, resolutionY);
		}
	}
	// HP, SP 바 UI 렌더링
	RenderHPSPBarUI(resolutionX, resolutionY);
}

void CTurnBattleState::TurnModeUpdate(float elapsedTimeInSec)
{
	// 행동트리 실행
	m_pBehaviorTree->Run();
	
	SetSkillHitAnimation();

	// 오토마타 메뉴이고 정보 표시를 하고 있을 경우 블러 효과 설정
	if (m_iMenu == AUTOMATA_MENU && m_bIsShowAutomataInformation && !m_bIsProcessing) {
		SetBlurEffect(true);
	}
	else {
		SetBlurEffect(false);
	}
	
	if (!m_bIsProcessing) {
		// 턴이 없을 경우 턴 모드를 닫고 메뉴 초기화
		if (!CanBeUsed()) {
			m_bIsTurnModeOpened = false;
			m_iMenu = MAIN_MENU;
			m_iAct = NO_ACT_MENU;
			m_iSelectedSkill = 0;
			m_iSelectedSkillDeck = 0;
			m_iSelectedAutomata = 0;
			m_iSelectedSkill = 0;
			m_iShowedSkillNum = 0;
			m_iShowedAutomataNum = 0;
		}
	}
}

void CTurnBattleState::TurnModeProcessInputs(float elapsedTimeInSec, GameInputs* inputs)
{
	if (m_bIsProcessing) {
		// 처리 중일 경우 리턴(조작 불능)
		return;
	}

	if (!m_bHeroTeam) {
		return;
	}

	TurnModeProcessInputsADMove(elapsedTimeInSec, inputs);
	TurnModeProcessInputsForEachMenuProcessing(elapsedTimeInSec, inputs);
	ProcessInputsForEachMenuSelect(elapsedTimeInSec, inputs);
}

void CTurnBattleState::TurnModeGettingTurnUpdate()
{
	CTurnBattleObject* object = m_HeroVector.at(m_iOrder);
	if (object->GetShowingTurnGauge() >= 1.0f) {
		TurnBattle::SBattleVariable bv = object->GetBattleVariable();
		if (bv.turn == 0) {
			TurnBattle::SStatus status = object->GetStatus();
			bv.turn = status.maxTurn;
			object->SetBattleVariable(bv);
			object->SetIsUpdatedTurnGaugeWhenEndTurn(false);
		}
	}
	else {
		TurnBattle::SBattleVariable bv = object->GetBattleVariable();
		bv.turn = 0;
		object->SetBattleVariable(bv);
	}

	CGameState::Update(BackBoard()->GetElapsedTimeInSec());
}

void CTurnBattleState::TurnModeProcessInputsForEachMenuProcessing(float elapsedTimeInSec, GameInputs* inputs)
{
	if (m_iMenu == MAIN_MENU) {
		TurnModeProcessInputsInMainMenu(elapsedTimeInSec, inputs);
	}
	else if (m_iMenu == SKILL_MENU) {
		ProcessInputsInSkillMenu(elapsedTimeInSec, inputs);
	}
	else if (m_iMenu == AUTOMATA_MENU) {
		ProcessInputsInAutomataMenu(elapsedTimeInSec, inputs);
	}
	else if (m_iMenu == CHANGE_DECK_MENU) {
		if (inputs->IsPressedDownKeyOnce(&inputs->KeyR, &inputs->processingEnd.KeyR)) {
			if (m_HeroVector.at(m_iOrder)->GetDeckVector().size() != 0) {
				m_HeroVector.at(m_iOrder)->RemoveDeck(m_iSelectedSkillDeck);
			}
		}
	}
}

void CTurnBattleState::TurnModeProcessInputsInMainMenu(float elapsedTimeInSec, GameInputs* inputs)
{
	if (m_iAct == NO_ACT_MENU) {
		if (inputs->IsPressedDownKeyOnce(&inputs->MouseLeftButton, &inputs->processingEnd.MouseLeftButton)) {
			m_EnemyMonsterVector.at(m_iTarget)->GiveAttackDamage(m_HeroVector.at(m_iOrder), true);
			CSoundPlayer::GetInstance()->PlaySound(GetSkillSoundEffect(1), 1.0f);
			TurnBattle::SBattleVariable bv = m_HeroVector.at(m_iOrder)->GetBattleVariable();
			bv.turn--;
			m_HeroVector.at(m_iOrder)->SetBattleVariable(bv);
		}
		if (inputs->IsPressedDownKeyOnce(&inputs->MouseRightButton, &inputs->processingEnd.MouseRightButton)) {
			m_bIsTurnModeOpened = false;
		}
	}

	if (inputs->IsPressedDownKeyOnce(&inputs->KeyQ, &inputs->processingEnd.KeyQ)) {
		m_bIsShowEnemyInformation = (m_bIsShowEnemyInformation + 1) % 2;
	}

	if (inputs->IsPressedDownKeyOnce(&inputs->KeyE, &inputs->processingEnd.KeyE)) {
		m_iMenu = 1;
	}
	if (inputs->IsPressedDownKeyOnce(&inputs->KeyF, &inputs->processingEnd.KeyF)) {
		if (m_iOrder == 0) {
			m_iMenu = 2;
		}
	}
	if (inputs->KeyW) {
		if (inputs->IsPressedDownKeyOnce(&inputs->KeyW, &inputs->processingEnd.KeyW)) {
			m_fKeepPressKey = 0.0f;
			m_iAct--;
		}
		else {
			if (IsKeyResultWaitedEnough(elapsedTimeInSec)) {
				m_iAct--;
			}
		}
	}
	if (inputs->KeyS)
	{
		if (inputs->IsPressedDownKeyOnce(&inputs->KeyS, &inputs->processingEnd.KeyS)) {
			m_fKeepPressKey = 0.0f;
			m_iAct++;
		}
		else {
			if (IsKeyResultWaitedEnough(elapsedTimeInSec)) {
				m_iAct++;
			}
		}
	}
	m_iAct = glm::clamp(m_iAct, 0, 4);

	if (m_iAct != NO_ACT_MENU) {
		if (inputs->KeyLeftShift || inputs->MouseRightButton) {
			if (inputs->IsPressedDownKeyOnce(&inputs->KeyLeftShift, &inputs->processingEnd.KeyLeftShift)) {
				m_iAct = NO_ACT_MENU;
			}
			else if (inputs->IsPressedDownKeyOnce(&inputs->MouseRightButton, &inputs->processingEnd.MouseRightButton)) {
				m_iAct = NO_ACT_MENU;
			}
		}
		else if (inputs->KeySpace || inputs->MouseLeftButton) {
			if (inputs->IsPressedDownKeyOnce(&inputs->KeySpace, &inputs->processingEnd.KeySpace)) {
				// 실행
				m_iMenu = m_iAct + 2;

				// 주인공이 아닐 경우 사용 불가능한 메뉴일 경우 밖으로 이동
				if (m_iOrder != 0) {
					if (m_iMenu == CHANGE_DECK_MENU || m_iMenu == STEAL_MENU) {
						m_iMenu = MAIN_MENU;
					}
				}

				// 도주
				if (m_iAct == ACT_RUNAWAY) {
					m_bIsRunaway = true;
				}

			}
			else if (inputs->IsPressedDownKeyOnce(&inputs->MouseLeftButton, &inputs->processingEnd.MouseLeftButton)) {
				// 실행
				m_iMenu = m_iAct + 2;

				// 주인공이 아닐 경우 사용 불가능한 메뉴일 경우 밖으로 이동
				if (m_iOrder != 0) {
					if (m_iMenu == CHANGE_DECK_MENU || m_iMenu == STEAL_MENU) {
						m_iMenu = MAIN_MENU;
					}
				}

				// 도주
				if (m_iAct == ACT_RUNAWAY) {
					m_bIsRunaway = true;
				}
			}
		}
	}
}

void CTurnBattleState::TurnModeProcessInputsADMove(float elapsedTimeInSec, GameInputs* inputs)
{
	if (m_iMenu == CHANGE_DECK_MENU) {
		// 스킬 덱 선택
		if (inputs->KeyA) {
			if (inputs->IsPressedDownKeyOnce(&inputs->KeyA, &inputs->processingEnd.KeyA)) {
				m_fKeepPressKey = 0.0f;
				m_iSelectedSkillDeck--;
			}
			else {
				if (IsKeyResultWaitedEnough(elapsedTimeInSec)) {
					m_iSelectedSkillDeck--;
				}
			}
		}
		if (inputs->KeyD) {
			if (inputs->IsPressedDownKeyOnce(&inputs->KeyD, &inputs->processingEnd.KeyD)) {
				m_fKeepPressKey = 0.0f;
				m_iSelectedSkillDeck++;
			}
			else {
				if (IsKeyResultWaitedEnough(elapsedTimeInSec)) {
					m_iSelectedSkillDeck++;
				}
			}
		}
		m_iSelectedSkillDeck = glm::clamp(m_iSelectedSkillDeck, 0,
			int(m_HeroVector.at(0)->GetDeckVector().size() - 1));
	}
}

void CTurnBattleState::BuildBehaviorTreeInHybridBattleForEnemy()
{
	// 스킬 실행 노드
	CCheckIsSkillUsedAndShowSkillName* checkIsSkillUsedAndShowSkillName
		= new CCheckIsSkillUsedAndShowSkillName(this, "Check is skill used and show skill Name");
	CSetSkillDataInHybridBattleForEnemy* setSkillData
		= new CSetSkillDataInHybridBattleForEnemy(this, "Set skill data and wait");
	CGiveSkillDamageInHybridBattleForEnemy* giveSkillDamage
		= new CGiveSkillDamageInHybridBattleForEnemy(this, "Give skill damage");

	CSequenceNode* skillSequence = new CSequenceNode("Check and Processing Skill");
	skillSequence->AddChild(checkIsSkillUsedAndShowSkillName);
	skillSequence->AddChild(setSkillData);
	skillSequence->AddChild(giveSkillDamage);

	CSelectEnemyActionInHybridBattle* selectEnemyAction
		= new CSelectEnemyActionInHybridBattle(this, "Enemy select action");

	CSequenceNode* enemySkillSequence = new CSequenceNode("Enemy skill");
	enemySkillSequence->AddChild(selectEnemyAction);
	enemySkillSequence->AddChild(skillSequence);

	m_pBehaviorTreeInHybridBattleForEnemy = new CBehaviorTree("Hybrid Battle Enemy Processing AI", enemySkillSequence);
}

void CTurnBattleState::RenderActLogInHybridBattle()
{
	if (m_bShowLog) {
		int x, y;
		Renderer()->GetDisplayResolution(&x, &y);
		RenderActLogUI(x, y);
	}
}

void CTurnBattleState::TurnModeRenderHPSPBarUI(int x, int y)
{
	float middle, textLocationX, textLocationY;
	float barLocationX, barLocationY;
	middle = 498.f / 2.f / (float)x;

	// 변경 표시
	if (m_HeroVector.size() >= 2) {
		Renderer()->UseShader(UI_SHADER);
		barLocationX = -125.f / (float)x;
		barLocationY = 0.f / (float)y;
		Renderer()->RenderTexture2D(m_UIKeyInformationTextureID,
			glm::vec2(0.25f - middle + barLocationX, 0.125f + barLocationY), glm::vec2(0.0f, 0.0f),
			glm::vec2(225.f, 45.f), glm::vec4(1.0f, 1.0f, 1.0f, 0.85f));

		Renderer()->UseShader(TEXT_SHADER);
		textLocationX = -117.f / (float)x;
		textLocationY = 17.f / (float)y;
		Renderer()->RenderText(L"Q", 0.25f - middle + textLocationX, 0.125f + textLocationY,
			0.45f, DEFAULT_TEXT_COLOR);

		textLocationX = -57.f / (float)x;
		textLocationY = 13.f / (float)y;
		Renderer()->RenderText(L"캐릭터 변경", 0.25f - middle + textLocationX, 0.125f + textLocationY,
			0.525f, DEFAULT_TEXT_COLOR);
	}

	float gap = -45.f;
	for (int i = 0; i < m_HeroVector.size(); ++i)
	{
		if (i == m_iOrder) {
			continue;
		}

		// 위치 설정(컨트롤 중인 파티 맴버에 대해서는 렌더링 하지 않음)
		int slot = 2;
		if (i == 0) {
			slot = 1;
		}
		else if (i == 1) {
			if (m_iOrder == 0) {
				slot = 1;
			}
		}

		// HP, SP 바
		Renderer()->UseShader(UI_SHADER);

		// 흰색으로 채우기
		float colorLocationX = -35.0f / (float)x;
		float colorLocationY = (5.0f + slot * gap) / (float)y;
		Renderer()->RenderTexture2D(m_WhiteRectTextureID,
			glm::vec2(0.25f - middle + colorLocationX, 0.125f + colorLocationY), glm::vec2(0.0f, 0.0f),
			glm::vec2(238.f, 30.f), DEFAULT_BACKGROUND_COLOR);

		TurnBattle::SStatus status = m_HeroVector.at(i)->GetStatus();
		TurnBattle::SBattleVariable battleVariable = m_HeroVector.at(i)->GetBattleVariable();

		int hp = battleVariable.hp;
		int maxHp = status.maxHp;

		glm::vec4 hpColor;

		if (i == 0) {
			hpColor = glm::vec4(1.0f, 0.0f, 0.5f, 0.85f);
		}
		else {
			hpColor = glm::vec4(1.0f, 0.5f, 0.0f, 0.85f);
		}

		// 약한 빨간색으로 채우기
		colorLocationX = -32.0f / (float)x;
		colorLocationY = (5.0f + slot * gap) / (float)y;
		Renderer()->RenderTexture2D(m_WhiteRectTextureID,
			glm::vec2(0.25f - middle + colorLocationX, 0.125f + colorLocationY),
			glm::vec2(0.0f, 0.0f), glm::vec2(230.f * (float)hp / (float)maxHp, 30.f), hpColor);

		// 바 테두리
		colorLocationX = -37.0f / (float)x;
		colorLocationY = (slot * gap) / (float)y;
		Renderer()->RenderSpriteSheet2D(m_UIHpSpBarTextureID, 2, 1, 0, 0,
			glm::vec2(0.25f - middle + colorLocationX, 0.125f + colorLocationY),
			glm::vec2(0.f, 0.f), glm::vec2(498.f / 2.0f, 42.f), glm::vec4(1.0f, 1.0f, 1.0f, 0.85f));
		
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
				colorLocationX = 220.0f / (float)x;
				colorLocationY = (slot * gap) / (float)y;
				Renderer()->RenderTexture2D(m_WhiteRectTextureID,
					glm::vec2(0.25f - middle + colorLocationX, 0.125f + colorLocationY),
					glm::vec2(0.0f, 0.0f), glm::vec2(40, 40), DEFAULT_BACKGROUND_COLOR
					* glm::vec4(1.0f, 1.0f, 1.0f, battleVariable.conditionDuration / RECOVER_CONDITION_TIME));
				Renderer()->RenderTexture2D(conditionTextureID,
					glm::vec2(0.25f - middle + colorLocationX, 0.125f + colorLocationY),
					glm::vec2(0.0f, 0.0f), glm::vec2(40, 40),
					glm::vec4(1.0f, 1.0f, 1.0f, battleVariable.conditionDuration / RECOVER_CONDITION_TIME));
			}
		}

		Renderer()->UseShader(TEXT_SHADER);

		// 이름 표시
		wchar_t nameText[MAX_NAME_SIZE];

		wsprintf(nameText, L"%s", m_HeroVector.at(i)->GetName());
		textLocationX = -83.f / (float)x;
		textLocationY = (12.0f + slot * gap) / (float)y;
		Renderer()->RenderText(nameText, 0.25f - middle + textLocationX, 0.125f + textLocationY,
			0.6f, glm::vec3(DEFAULT_BACKGROUND_COLOR));

		textLocationX = -85.f / (float)x;
		textLocationY = (10.0f + slot * gap) / (float)y;
		Renderer()->RenderText(nameText, 0.25f - middle + textLocationX, 0.125f + textLocationY,
			0.6f, DEFAULT_TEXT_COLOR);
	}
}

void CTurnBattleState::TurnModeRenderKeyInformationUIForMainMenu(int x, int y)
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

	textLocationX = 110.f / (float)x;
	textLocationY = 70.f / (float)y;
	Renderer()->RenderText(L"저격", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.625f, DEFAULT_TEXT_COLOR);

	textLocationX = 95.f / (float)x;
	textLocationY = 17.5f / (float)y;
	Renderer()->RenderText(L"턴 종료", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.625f, DEFAULT_TEXT_COLOR);

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

void CTurnBattleState::TurnModeRenderEnemyNameBarUI(int x, int y)
{
	float middle, textLocationX, textLocationY;

	Renderer()->UseShader(UI_SHADER);
	
	middle = 516.f / 2.f / (float)x;
	Renderer()->RenderTexture2D(m_UIActLogBarTextureID,
		glm::vec2(0.5f - middle, 0.8f), glm::vec2(0.0f, 0.0f),
		glm::vec2(516.f, 85.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	
	Renderer()->UseShader(TEXT_SHADER);
	wchar_t enemyLevelNameText[MAX_INFORMATION_SIZE];
	short level = m_EnemyMonsterVector.at(m_iTarget)->GetStatus().level;
	wchar_t nameText[MAX_NAME_SIZE];
	wcscpy_s(nameText, m_EnemyMonsterVector.at(m_iTarget)->GetName());
	wsprintf(enemyLevelNameText, L"lv.%d   %s", level, nameText);

	textLocationX = 80.f / (float)x;
	textLocationY = 38.f / (float)y;
	Renderer()->RenderText(enemyLevelNameText, 0.5f - middle + textLocationX, 0.8f + textLocationY,
		0.65f, DEFAULT_TEXT_COLOR);
}

void CTurnBattleState::TurnModeRenderRenderBillboards(std::vector<CBattleMonster*> monsterVector, glm::vec3 heroPos)
{
	for (int i = 0; i < monsterVector.size(); ++i) {
		m_EnemyMonsterVector.at(i)->SetPosition(monsterVector.at(i)->GetPosition());
	}
	m_HeroVector.at(m_iOrder)->SetPosition(heroPos);

	// 스킬 데미지 표시
	Renderer()->UseShader(BILLBOARD_TEXT_SHADER);

	if (!m_bIsProcessingInHybridBattleForEnemy) {
		for (auto enemy : m_EnemyMonsterVector) {
			enemy->RenderDamageText(Renderer(), false);
		}
	}
	else {
		m_EnemyMonsterVector.at(m_iHybridBattleMonsterOrder)->RenderDamageText(Renderer(), false);
	}
	m_HeroVector.at(m_iOrder)->RenderDamageText(Renderer(), true);

	
	Renderer()->UseShader(BILLBOARD_SHADER);

	// 일반 텍스쳐 빌보드가 하나도 렌더링되지 않을 경우 흰 화면 버그 수정을 위해 빌보드 하나 렌더링(원인 불명)
	Renderer()->RenderTexture3D(m_WhiteRectTextureID, glm::vec3(-1000.0f, -1000.0f, -1000.0f),
		glm::vec2(0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	if (!m_bIsProcessingInHybridBattleForEnemy) {
		if (m_bHeroTeam) {
			if (m_iMenu != STEAL_MENU &&
				!(m_iMenu == AUTOMATA_MENU && m_bIsShowAutomataInformation)
				&& m_iMenu != WAIT_MENU
				&& m_iMenu != CHANGE_DECK_MENU) {
				for (int i = 0; i < m_EnemyMonsterVector.size(); ++i) {
					Renderer()->UseShader(BILLBOARD_SHADER);
					m_EnemyMonsterVector.at(i)->RenderHPBarOnTopForHybridBattle(Renderer());
					Renderer()->UseShader(BILLBOARD_TEXT_SHADER);
					m_EnemyMonsterVector.at(i)->RenderDownLevelText(Renderer(), false);
				}
			}
		}
	}

	// 빌보드 애니메이션 렌더링
	Renderer()->UseShader(BILLBOARD_SPRITE_SHADER);

	if (!m_bIsProcessingInHybridBattleForEnemy) {
		for (int i = 0; i < MAX_ENEMY_PARTY_MEMBER; ++i) {
			if (m_UsedSkillData.enemyParty[i].exisited) {
				if (m_UsedSkillData.enemyParty[i].targeted) {
					glm::vec3 heroPos = BackBoard()->GetHeroPosition();
					glm::vec3 position = monsterVector.at(i)->GetPosition();
					glm::vec3 dir = position - heroPos;
					dir = glm::normalize(dir);

					glm::vec3 volume = monsterVector.at(i)->GetVolume();
					glm::vec3 size = monsterVector.at(i)->GetSize();
					float diagonalSize = glm::length(glm::vec3(volume.x * size.x, 0.0f, volume.z * size.z));

					Renderer()->RenderSpriteSheet3D(m_UsedSkillData.enemyParty[i].skillEffectData,
						SKILL_EFFECT_FRAME, 1,
						(int)((2.0f - m_UsedSkillData.enemyParty[i].skillEffectTime) * SKILL_EFFECT_FRAME), 0,
						glm::vec3(position.x - dir.x * diagonalSize, position.y + 0.15f, position.z - dir.z * diagonalSize),
						glm::vec2(1.0f * diagonalSize, 1.0f * diagonalSize),
						glm::vec4(1.f, 1.f, 1.f, 1.f));
				}
			}
		}
	}
	else {
		int i = m_iHybridBattleMonsterOrder;
		if (m_UsedSkillData.enemyParty[i].exisited) {
			if (m_UsedSkillData.enemyParty[i].targeted) {
				glm::vec3 position = monsterVector.at(i)->GetPosition();
				glm::vec3 cameraPos, temp;
				Camera()->GetCameraPositionAndDirection(&cameraPos, &temp);
				glm::vec3 dir = position - cameraPos;
				dir = glm::normalize(dir);

				glm::vec3 volume = monsterVector.at(i)->GetVolume();
				glm::vec3 size = monsterVector.at(i)->GetSize();
				float diagonalSize = glm::length(glm::vec3(volume.x * size.x, 0.0f, volume.z * size.z));

				Renderer()->RenderSpriteSheet3D(m_UsedSkillData.enemyParty[i].skillEffectData,
					SKILL_EFFECT_FRAME, 1,
					(int)((2.0f - m_UsedSkillData.enemyParty[i].skillEffectTime) * SKILL_EFFECT_FRAME), 0,
					glm::vec3(position.x - dir.x * diagonalSize, position.y + 0.15f, position.z - dir.z * diagonalSize),
					glm::vec2(1.0f * diagonalSize, 1.0f * diagonalSize),
					glm::vec4(1.f, 1.f, 1.f, 1.f));
			}
		}
	}

	if (m_UsedSkillData.heroParty[m_iOrder].exisited) {
		if (m_UsedSkillData.heroParty[m_iOrder].targeted) {
			glm::vec3 position = BackBoard()->GetHeroPosition();
			glm::vec3 cameraPos, temp;
			Camera()->GetCameraPositionAndDirection(&cameraPos, &temp);
			glm::vec3 dir = position - cameraPos;
			dir = glm::normalize(dir);
			
			glm::vec3 volume = m_HeroVector.at(m_iOrder)->GetVolume();
			glm::vec3 size = m_HeroVector.at(m_iOrder)->GetSize();
			float diagonalSize = glm::length(glm::vec3(volume.x * size.x, 0.0f, volume.z * size.z));

			Renderer()->RenderSpriteSheet3D(m_UsedSkillData.heroParty[m_iOrder].skillEffectData,
				SKILL_EFFECT_FRAME, 1,
				(int)((2.0f - m_UsedSkillData.heroParty[m_iOrder].skillEffectTime) * SKILL_EFFECT_FRAME), 0,
				glm::vec3(position.x - dir.x * diagonalSize, position.y + 0.25, position.z - dir.z * diagonalSize),
				glm::vec2(1.f * diagonalSize, 1.f * diagonalSize),
				glm::vec4(1.f, 1.f, 1.f, 1.f));
		}
	}
}

void CTurnBattleState::SetEnemyTurnZeroInHyBridBattleStart()
{
	for (auto enemy : m_EnemyMonsterVector) {
		auto bv = enemy->GetBattleVariable();
		bv.turn = 0;
		enemy->SetBattleVariable(bv);
	}
}

bool CTurnBattleState::SetEnemyTurnStart()
{
	if (!m_bIsProcessing) {
		if (!m_bIsProcessingInHybridBattleForEnemy) {
			int i = 0;
			for (auto enemy : m_EnemyMonsterVector) {
				auto bv = enemy->GetBattleVariable();
				if (bv.turn > 0) {
					m_iHybridBattleMonsterOrder = i;
					m_bIsProcessingInHybridBattleForEnemy = true;
					return true;
				}
				i++;
			}
		}
	}
	return false;
}

int CTurnBattleState::GetMenu() const
{
	return m_iMenu;
}

void CTurnBattleState::SetMenu(int menu)
{
	m_iMenu = menu;
}

void CTurnBattleState::SetIsTurnModeOpened(bool open)
{
	m_bIsTurnModeOpened = open;
}

bool CTurnBattleState::GetIsTurnModeOpened() const
{
	return m_bIsTurnModeOpened;
}

bool CTurnBattleState::CanBeUsed()
{
	if (m_HeroVector.at(m_iOrder)->GetBattleVariable().turn <= 0) {
		m_HeroVector.at(m_iOrder)->IncreaseGettingTurnGauge(-1.0f);
		return false;
	}
	return true;
}

bool CTurnBattleState::IsHadPartyMamber()
{
	if (m_HeroVector.size() >= 2) {
		return true;
	}
	return false;
}

void CTurnBattleState::ChangeOrder()
{
	if (IsHadPartyMamber()) {
		m_iOrder = (m_iOrder + 1) % (int)m_HeroVector.size();
	}
}

int CTurnBattleState::GetTarget()
{
	if (m_EnemyMonsterVector.size() <= m_iTarget) {
		m_iTarget = (int)m_EnemyMonsterVector.size() - 1;
	}

	return m_iTarget;
}

void CTurnBattleState::SetTarget(int target)
{
	m_iTarget = target;
}

int CTurnBattleState::GetAutomataModelID() const
{
	return m_HeroModelID;
}

int CTurnBattleState::GetAutomataTextureID() const
{
	return m_AutomataTextureID;
}

int CTurnBattleState::GetHybridBattleMonsterOrder() const
{
	return m_iHybridBattleMonsterOrder;
}

void CTurnBattleState::RunBTForEnemyInHybridBattle()
{
	m_pBehaviorTreeInHybridBattleForEnemy->Run();
}

void CTurnBattleState::SetIsProcessingInHybridBattleForEnemy(bool processing)
{
	m_bIsProcessingInHybridBattleForEnemy = processing;
}

bool CTurnBattleState::GetIsProcessingInHybridBattleForEnemy() const
{
	return m_bIsProcessingInHybridBattleForEnemy;
}

void CTurnBattleState::UpdateUsedSkillData(float elapsedTimeInSec)
{
	GameUpdateParameters temp;
	memset(&temp, 0, sizeof(GameUpdateParameters));

	for (auto enemy : m_EnemyMonsterVector) {
		enemy->Update(elapsedTimeInSec, temp);
	}
	for (auto hero : m_HeroVector) {
		hero->Update(elapsedTimeInSec, temp);
	}

	for (int i = 0; i < MAX_ENEMY_PARTY_MEMBER; ++i) {
		if (m_UsedSkillData.enemyParty[i].exisited) {
			if (m_UsedSkillData.enemyParty[i].targeted) {
				m_UsedSkillData.enemyParty[i].skillEffectTime 
					-= elapsedTimeInSec * 1.0f / DEFULAT_SKILL_EFFECT_TIME_LENGTH;
			}
		}
	}
	for (int i = 0; i < MAX_HERO_PARTY_MEMBER; ++i) {
		if (m_UsedSkillData.heroParty[i].exisited) {
			if (m_UsedSkillData.heroParty[i].targeted) {
				m_UsedSkillData.heroParty[i].skillEffectTime
					-= elapsedTimeInSec * 1.0f / DEFULAT_SKILL_EFFECT_TIME_LENGTH;
			}
		}
	}
}

void CTurnBattleState::SetTimeOfDaySkybox(CTimeOfDaySkybox* timeOfDaySkybox)
{
	m_pTimeOfDaySkybox = timeOfDaySkybox;
}

void CTurnBattleState::SetFountainWaterDropParticle(CFountainWaterDropParticle* fountainWaterDropParticle)
{
	ObjectManager()->AddParticle<CFountainWaterDropParticle>(fountainWaterDropParticle);
}

std::string CTurnBattleState::GetMonsterModelInMonsterModelMap(short id)
{
	return m_MonsterModelMap.find(id)->second;
}

std::map<GameObjectAnimationState, std::string> CTurnBattleState::GetMonsterAnimationsInMonsterAnimationsMap(short id)
{
	return m_MonsterAnimationsMap.find(id)->second;
}

int CTurnBattleState::GetMonsterTextureInMonsterTextureMap(short id)
{
	return m_MonsterTextureMap.find(id)->second;
}

int CTurnBattleState::GetMonsterBloodTextureInMonsterBloodTextureMap(short id)
{
	return m_MonsterBloodTextureMap.find(id)->second;
}

int CTurnBattleState::GetMonsterDefaultSoundInMonsterDefaultSoundMap(short id)
{
	return m_MonsterDefaultSoundMap.find(id)->second;
}

void CTurnBattleState::SetSkillHitAnimation()
{
	for (int i = 0; i < MAX_ENEMY_PARTY_MEMBER; ++i) {
		if (m_UsedSkillData.enemyParty[i].exisited) {
			if (m_UsedSkillData.enemyParty[i].targeted) {
				if (m_UsedSkillData.enemyParty[i].skillEffectTime <= DEFULAT_SKILL_EFFECT_TIME_LENGTH / 2.f) {
					if (m_EnemyMonsterVector.at(i)->GetAnimationState() != ANI_STATE_HIT) {
						m_EnemyMonsterVector.at(i)->SetAnimationState(ANI_STATE_HIT);
						m_EnemyMonsterVector.at(i)->GetAnimator()->SetIsLoop(false);
					}
				}
			}
		}
	}
	for (int i = 0; i < MAX_HERO_PARTY_MEMBER; ++i) {
		if (m_UsedSkillData.heroParty[i].exisited) {
			if (m_UsedSkillData.heroParty[i].targeted) {
				if (m_UsedSkillData.heroParty[i].skillEffectTime <= DEFULAT_SKILL_EFFECT_TIME_LENGTH / 2.f) {
					if (m_HeroVector.at(i)->GetAnimationState() != ANI_STATE_HIT) {
						m_HeroVector.at(i)->SetAnimationState(ANI_STATE_HIT);
						m_HeroVector.at(i)->GetAnimator()->SetIsLoop(false);
					}
				}
			}
		}
	}
}

bool CTurnBattleState::GetIsGameover() const
{
	return m_bIsGameover;
}

void CTurnBattleState::SetIsGameover(bool over)
{
	m_bIsGameover = over;
}

bool CTurnBattleState::GetIsGameclear() const
{
	return m_bIsGameclear;
}

void CTurnBattleState::SetIsGameclear(bool clear)
{
	m_bIsGameclear = clear;
}

CGameOverState* CTurnBattleState::GetGameOverState()
{
	return m_pGameOverState;
}

CGameClearState* CTurnBattleState::GetGameClearState()
{
	return m_pGameClearState;
}

bool CTurnBattleState::TurnModeRemoveDeadObject()
{
	// 죽은 몬스터 삭제
	bool deadMonster = true;
	while (deadMonster) {
		deadMonster = false;
		int size = (int)m_EnemyMonsterVector.size();

		for (int i = 0; i < size; ++i) {
			if (m_EnemyMonsterVector.at(i)->GetBattleVariable().hp <= 0) {
				if (m_iAreaNumber == 1) {
					if (m_EnemyMonsterVector.at(i)->GetIsBoss()) {
						if (size == 1) {
							if (m_EnemyMonsterVector.at(i)->GetMonsterId() == 13) {
								m_bIsGameclear = true;
								m_HeroVector.at(0)->InitBattleVariable();
								return true;
							}
						}
						else {
							TurnBattle::SBattleVariable vb = m_EnemyMonsterVector.at(i)->GetBattleVariable();
							vb.hp += (size - 1) * 500;
							m_EnemyMonsterVector.at(i)->SetBattleVariable(vb);
							break;
						}
					}
					else {
						if (i == 0) {
							if (m_EnemyMonsterVector.at(2)->GetBattleVariable().hp > 0) {
								TurnBattle::SBattleVariable vb = m_EnemyMonsterVector.at(i)->GetBattleVariable();
								vb.hp += 500;
								m_EnemyMonsterVector.at(i)->SetBattleVariable(vb);
								break;
							}
							else {
								SetMonsterData(13, m_EnemyMonsterVector.at(1));
								m_EnemyMonsterVector.at(1)->SetAnimationState(ANI_STATE_PHASE_CHANGE);
								m_EnemyMonsterVector.at(1)->GetAnimator()->SetIsLoop(false);
								m_EnemyMonsterVector.at(1)->SetGettingTurnGauge(0.0f);
								m_EnemyMonsterVector.at(1)->InitBattleVariable();
								auto boss = dynamic_cast<CBossMonster*>(m_EnemyMonsterVector.at(1)->GetParent());
								boss->InitWithTurnBattleObject();
								boss->SetIsChangedPhase(true);
								boss->SetTurnGauge(0.0f);
								TurnBattle::SBattleVariable vb = m_EnemyMonsterVector.at(1)->GetBattleVariable();
								vb.turn = 0;
								m_EnemyMonsterVector.at(1)->SetBattleVariable(vb);
								m_iTarget = 0;
							}
						}
						else if (i == 2) {
							if (m_EnemyMonsterVector.at(0)->GetBattleVariable().hp > 0) {
								TurnBattle::SBattleVariable vb = m_EnemyMonsterVector.at(i)->GetBattleVariable();
								vb.hp += 500;
								m_EnemyMonsterVector.at(i)->SetBattleVariable(vb);
								break;
							}
						}
						auto head = dynamic_cast<CBossHead*>(m_EnemyMonsterVector.at(i)->GetParent());
						dynamic_cast<CBossMonster*>(head->GetParent())->SetHeadNull(head->GetIsRightHead());
					}
				}


				// 주인공에게 경험치 추가
				HeroLevelUp(m_EnemyMonsterVector.at(i)->GetStatus().level);

				// 사용된 스킬 데이터 변경
				for (int t = i; t < size - 1; ++t) {
					m_EnemyMonsterVector.at(t + 1)->SetPosition(glm::vec3(-10.0f + 4.0f * t, -3.0f, -5.0f));
					m_UsedSkillData.enemyParty[t].exisited = m_UsedSkillData.enemyParty[t + 1].exisited;
					m_UsedSkillData.enemyParty[t + 1].exisited = false;
				}
				// 타겟 중이였는데 맨 끝이였을 경우 타겟 변경
				if (i == m_iTarget) {
					if (i == size - 1) {
						m_iTarget--;
						if (m_iTarget < 0) {
							m_iTarget = 0;
						}
					}
				}
				// 부모 오브젝트에게 죽었다고 알림
				dynamic_cast<CBattleMonster*>(m_EnemyMonsterVector.at(i)->GetParent())->SetIsDead(true);

				// 삭제
				m_EnemyMonsterVector.at(i)->SetLifeTime(0.0f);
				m_EnemyMonsterVector.erase(m_EnemyMonsterVector.begin() + i);
				m_EnemyMonsterVector.resize(size - 1);
				if ((size - 1) == 0) {
					return true;
				}

				deadMonster = true;
				break;
			}
		}
	}

	// 죽은 오토마타 삭제
	bool deadAutomata = true;
	while (deadAutomata) {
		deadAutomata = false;
		int size = (int)m_HeroVector.size();

		for (int i = 0; i < size; ++i) {
			if (m_HeroVector.at(i)->GetBattleVariable().hp <= 0) {
				if (i == 0) {
					// 주인공 사망으로 게임 오버 설정
					m_bIsGameover = true;

					// 일단 다시 hp 추가(이후 게임 오버 화면으로 이동)
					m_HeroVector.at(i)->InitBattleVariable();

					return true;
					break;
				}
				m_iOrder = 0;

				// 사용된 스킬 데이터 변경
				for (int t = i; t < size - 1; ++t) {
					m_HeroVector.at(t + 1)->SetPosition(glm::vec3(-10.0f + 4.0f * t, -3.0f, -5.0f));
					m_UsedSkillData.heroParty[t].exisited = m_UsedSkillData.heroParty[t + 1].exisited;
					m_UsedSkillData.heroParty[t + 1].exisited = false;
					m_sSummonedAutomata[t] = m_sSummonedAutomata[t + 1];
					m_sSummonedAutomata[t + 1] = 0;
				}
				// 마지막으로 소환된 오토마타 사망시(1번만 있을 경우 1번, 1, 2번 둘다 있을 경우 2번)
				if (i == size - 1) {
					m_sSummonedAutomata[i - 1] = 0;
					m_UsedSkillData.heroParty[i].exisited = false;
				}

				// 타겟 중이였는데 맨 끝이였을 경우 타겟 변경
				if (i == m_iTarget) {
					m_iTarget = 0;
				}
				// 삭제
				m_HeroVector.at(i)->SetLifeTime(0.0f);
				m_HeroVector.erase(m_HeroVector.begin() + i);
				m_HeroVector.resize(size - 1);

				deadAutomata = true;
				break;
			}
		}
	}

	// 존재 유무 재설정
	for (int i = 0; i < MAX_HERO_PARTY_MEMBER; ++i) {
		m_UsedSkillData.heroParty[i].exisited = false;
	}
	for (int i = 0; i < m_HeroVector.size(); ++i) {
		m_UsedSkillData.heroParty[i].exisited = true;
	}
	for (int i = 0; i < MAX_ENEMY_PARTY_MEMBER; ++i) {
		m_UsedSkillData.enemyParty[i].exisited = false;
	}
	for (int i = 0; i < m_EnemyMonsterVector.size(); ++i) {
		m_UsedSkillData.enemyParty[i].exisited = true;
	}

	return false;
}


BehaviorTreeState CSelectEnemyActionInHybridBattle::Run()
{
	CTurnBattleObject* object = state->GetTurnBattleObject(false, state->GetHybridBattleMonsterOrder());
	TurnBattle::SBattleVariable bv = object->GetBattleVariable();
	short sp = bv.sp;
	short turn = bv.turn;
	short usableSkillList[MAX_SKILL_COUNT];
	int skillSize = 0;

	// 턴이 없을 경우 행동을 선택하지 않고 실패 리턴
	if (bv.turn <= 0) {
		state->SetIsProcessing(false);
		state->SetIsSkillUsed(false);
		state->SetIsProcessingInHybridBattleForEnemy(false);
		return BT_FAILURE;
	}

	// 화상이나 불안일 경우 행동을 선택하지 않고 실패 리턴
	if (bv.condition == BURN_CONDITION || bv.condition == ANXIETY_CONDITION) {
		state->SetIsProcessing(false);
		state->SetIsSkillUsed(false);
		state->SetIsProcessingInHybridBattleForEnemy(false);
		return BT_FAILURE;
	}

	std::random_device rd;
	std::default_random_engine dre(rd());

	// 타겟 설정
	int targetedObject = state->GetOrder();
	
	// 사용 가능한 스킬 위치 저장
	for (int i = 0; i < (int)object->GetSkillVector().size(); ++i) {
		TurnBattle::SSkill* skill = state->GetSkillData(object->GetSkillVector().at(i));
		if (skill->turn <= turn) {
			if (skill->sp <= sp) {
				usableSkillList[skillSize] = object->GetSkillVector().at(i);
				skillSize++;
			}
		}
	}

	if (skillSize == 0) {
		bv.turn = 0;
		object->SetBattleVariable(bv);
		return BT_FAILURE;
	}

	int selectedSkill = 0;
	if (skillSize != 1) {
		std::uniform_int_distribution<int> uidSkill(0, skillSize - 1);
		selectedSkill = uidSkill(dre);
	}

	// 스킬 자원 소모
	bv.turn -= state->GetSkillData(usableSkillList[selectedSkill])->turn;
	bv.sp -= state->GetSkillData(usableSkillList[selectedSkill])->sp;
	object->SetBattleVariable(bv);

	// 선택 스킬, 타겟, 스킬 사용 설정
	state->SetSelectedSkillAndTarget(usableSkillList[selectedSkill], targetedObject);
	state->SetIsSkillUsed(true);

	// 애니메이션 설정
	object->SetAnimationState(ANI_STATE_SKILL_1);
	object->GetAnimator()->SetIsLoop(false);

	return BT_SUCCESS;
}

// CSetSkillData ------------------------------------------------------

BehaviorTreeState CSetSkillDataInHybridBattleForEnemy::Run()
{
	if (!m_bSetSkillData) {
		SetUsedSkillData();
		m_fWaitTime = 1.0f;
		m_bSetSkillData = true;
		return BT_RUNNING;
	}
	else {
		if (m_fWaitTime <= 0.0f) {
			m_bSetSkillData = false;
			return BT_SUCCESS;
		}
		m_fWaitTime -= BackBoard()->GetElapsedTimeInSec();
		return BT_RUNNING;
	}
}

void CSetSkillDataInHybridBattleForEnemy::SetUsedSkillData()
{
	// 스킬과 타겟 정보 얻기
	int order, target;
	short index;
	state->GetOrderSkillTargetAndIndex(&order, &target, &index);
	TurnBattle::SSkill* skill = state->GetSkillData(index);
	bool heroTurn = false;

	CSoundPlayer::GetInstance()->PlaySound(state->GetSkillSoundEffect(index), 1.0f);

	order = state->GetHybridBattleMonsterOrder();
	target = state->GetOrder();

	// 본인 파티를 타켓으로 하는 스킬인 경우 
	if (skill->selfParty) {
		heroTurn = (heroTurn + 1) % 2;
		target = order;
	}

	TurnBattle::SUsedSkillData usedSkillData = state->GetUsedSkillData();

	CSetSkillData setSkillData(state, "name");

	// 스킬에 따라 타겟 설정
	if (std::abs(skill->target) == ONE_TARGET) {
		setSkillData.FillUsedSkillData(usedSkillData, heroTurn, target, index, skill->effectTime, skill->effectData);
	}
	else if (std::abs(skill->target) == ALL_TARGET) {
		int max = MAX_ENEMY_PARTY_MEMBER;
		if (!heroTurn) max = MAX_HERO_PARTY_MEMBER;
		for (int i = 0; i < max; ++i) {
			setSkillData.FillUsedSkillData(usedSkillData, heroTurn, i, index, skill->effectTime, skill->effectData);
		}
	}
	else if (std::abs(skill->target) == ONE_AND_ALL_TARGET) {
		int max = MAX_ENEMY_PARTY_MEMBER;
		if (!heroTurn) max = MAX_HERO_PARTY_MEMBER;
		// 특수 효과 스킬
		TurnBattle::SSkill* effectSkill = state->GetSkillData(index + SPECIAL_SKILL_INDEX_START);

		for (int i = 0; i < max; ++i) {
			if (index < 70) {
				if (i == target) {
					setSkillData.FillUsedSkillData(usedSkillData, heroTurn, i, index, skill->effectTime, skill->effectData);
				}
				else {
					setSkillData.FillUsedSkillData(usedSkillData, heroTurn, i,
						index + SPECIAL_SKILL_INDEX_START, effectSkill->effectTime, effectSkill->effectData);
				}
			}
			else {
				if (i == target) {
					setSkillData.FillUsedSkillData(usedSkillData, heroTurn, i,
						index + SPECIAL_SKILL_INDEX_START, effectSkill->effectTime, effectSkill->effectData);
				}
				else {
					setSkillData.FillUsedSkillData(usedSkillData, heroTurn, i, index, skill->effectTime, skill->effectData);
				}
			}
		}
	}
	else if (std::abs(skill->target) == OTHER_TARGET) {
		int max = MAX_ENEMY_PARTY_MEMBER;
		if (!heroTurn) max = MAX_HERO_PARTY_MEMBER;
		for (int i = 0; i < max; ++i) {
			if (i != target) {
				setSkillData.FillUsedSkillData(usedSkillData, heroTurn, i, index, skill->effectTime, skill->effectData);
			}
		}
	}


	state->SetUsedSkillData(usedSkillData);
}

// CGiveSkillDamage ----------------------------------------------

BehaviorTreeState CGiveSkillDamageInHybridBattleForEnemy::Run()
{
	CGiveSkillDamage giveSkillDamage(state, "");

	if (!m_bGiveDamageEnd) {
		// 스킬 데미지 적용
		ApplySkillDamage();
		// 타겟팅 종료
		giveSkillDamage.TargetingEndInUsedSkillData();

		m_bGiveDamageEnd = true;
		m_fWaitTime = 0.75f;

		return BT_RUNNING;
	}
	else {
		if (m_fWaitTime < 0.0f) {
			// hp바 표시 종료
			m_bGiveDamageEnd = false;
			state->SetIsProcessing(false);
			state->SetIsSkillUsed(false);
			state->SetIsProcessingInHybridBattleForEnemy(false);
			return BT_SUCCESS;
		}

		m_fWaitTime -= BackBoard()->GetElapsedTimeInSec();
		return BT_RUNNING;
	}
}

void CGiveSkillDamageInHybridBattleForEnemy::ApplySkillDamage()
{
	CGiveSkillDamage giveSkillDamage(state, "");

	TurnBattle::SUsedSkillData usedSkillData = state->GetUsedSkillData();

	int userIndex = state->GetHybridBattleMonsterOrder();
	bool heroTurn = false;
	TurnBattle::SStatus userStatus = state->GetTurnBattleObject(heroTurn, userIndex)->GetStatus();
	TurnBattle::SBattleVariable userBV = state->GetTurnBattleObject(heroTurn, userIndex)->GetBattleVariable();

	for (int i = 0; i < MAX_ENEMY_PARTY_MEMBER; ++i) {
		if (usedSkillData.enemyParty[i].exisited) {
			if (usedSkillData.enemyParty[i].targeted) {
				short skillIndex = usedSkillData.enemyParty[i].skillIndex;
				TurnBattle::SSkill* skill = state->GetSkillData(skillIndex);
				TurnBattle::SStatus targetStatus = state->GetTurnBattleObject(false, i)->GetStatus();
				TurnBattle::SBattleVariable targetBV =
					state->GetTurnBattleObject(false, i)->GetBattleVariable();

				wchar_t damageText[MAX_NAME_SIZE];
				glm::vec4 damageTextColor;
				TurnBattle::SBattleVariable finalBV;
				if (skillIndex <= MAXIUM_GIVE_DAMAGE_SKILL_INDEX) {
					finalBV = giveSkillDamage.CalculateSkillDamage(skill, userStatus, userBV, targetStatus, targetBV,
						damageText, &damageTextColor);
				}
				else if (skillIndex < SPECIAL_SKILL_INDEX_START) {
					finalBV = giveSkillDamage.CalculateSkillEffect(skill, userStatus, userBV, targetStatus, targetBV,
						damageText, &damageTextColor);
				}
				else {
					finalBV = giveSkillDamage.CalculateSkillSpecial(skill, userStatus, userBV, targetStatus, targetBV,
						damageText, &damageTextColor);
				}

				state->GetTurnBattleObject(false, i)->SetDamageText(damageText);
				state->GetTurnBattleObject(false, i)->SetDamageTextColor(damageTextColor);
				state->GetTurnBattleObject(false, i)->SetIsShowedDamageText(true);
				state->GetTurnBattleObject(false, i)->SetShowingDamageTextTime(DEFAULT_SHOW_DAMAGE_TEXT_TIME);
				state->GetTurnBattleObject(false, i)->SetBattleVariable(finalBV);

				if (finalBV.hp <= 0) {
					state->GetTurnBattleObject(false, i)->SetAnimationState(ANI_STATE_DEATH);
					state->GetTurnBattleObject(false, i)->GetAnimator()->SetIsLoop(false);
				}
			}
		}
	}
	for (int i = 0; i < MAX_HERO_PARTY_MEMBER; ++i) {
		if (usedSkillData.heroParty[i].exisited) {
			if (usedSkillData.heroParty[i].targeted) {
				short skillIndex = usedSkillData.heroParty[i].skillIndex;
				TurnBattle::SSkill* skill = state->GetSkillData(skillIndex);
				TurnBattle::SStatus targetStatus = state->GetTurnBattleObject(true, i)->GetStatus();
				TurnBattle::SBattleVariable targetBV =
					state->GetTurnBattleObject(true, i)->GetBattleVariable();

				wchar_t damageText[MAX_NAME_SIZE];
				glm::vec4 damageTextColor;

				TurnBattle::SBattleVariable finalBV;
				if (skillIndex <= MAXIUM_GIVE_DAMAGE_SKILL_INDEX) {
					finalBV = giveSkillDamage.CalculateSkillDamage(skill, userStatus, userBV, targetStatus, targetBV,
						damageText, &damageTextColor);
				}
				else if (skillIndex < SPECIAL_SKILL_INDEX_START) {
					finalBV = giveSkillDamage.CalculateSkillEffect(skill, userStatus, userBV, targetStatus, targetBV,
						damageText, &damageTextColor);
				}
				else {
					finalBV = giveSkillDamage.CalculateSkillSpecial(skill, userStatus, userBV, targetStatus, targetBV,
						damageText, &damageTextColor);
				}
				state->GetTurnBattleObject(true, i)->SetDamageText(damageText);
				state->GetTurnBattleObject(true, i)->SetDamageTextColor(damageTextColor);
				state->GetTurnBattleObject(true, i)->SetIsShowedDamageText(true);
				state->GetTurnBattleObject(true, i)->SetShowingDamageTextTime(DEFAULT_SHOW_DAMAGE_TEXT_TIME);
				state->GetTurnBattleObject(true, i)->SetBattleVariable(finalBV);

				if (finalBV.hp <= 0) {
					state->GetTurnBattleObject(true, i)->SetAnimationState(ANI_STATE_DEATH);
					state->GetTurnBattleObject(true, i)->GetAnimator()->SetIsLoop(false);
				}
			}
		}
	}

	state->SetUsedSkillData(usedSkillData);
}
