#include "stdafx.h"
#include "TurnBattleState.h"

CTurnBattleState::CTurnBattleState()
{
	// CGameFramework에서 CGameState를 사용하기 때문에 CGameState의 자식 클래스인 현 클래스에서 CGameFramework를 가져올 것!
	m_pGameFramework = CGameFramework::GetInstance();

	m_pGameOverState = new CGameOverState();
	m_pGameClearState = new CGameClearState();

	// 마우스 커서 숨기기
	glutSetCursor(GLUT_CURSOR_NONE);

	Camera()->SetTestCameraPosition(glm::vec3(0.f, 0.0f, 0.0f));
	Camera()->TestCameraSetting();

	glm::vec3 pos = glm::vec3(100.0f, 0.0f, 0.0f);
	glm::vec3 dir = glm::normalize(pos);

	LoadDirectionalLight(new CDirectionalLight(pos,
		glm::vec3(-dir.x, -dir.y, -dir.z),
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));
	DirectionalLight()->SetSkyLightColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	LoadPointLight(new CPointLight(glm::vec3(0.0f, 3.5f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));

	LoadData();
	AddObjectToManager();

	ReadSkillData("./resource/database/skill.data");
	ReadAutomataData("./resource/database/automata.data"); 
	ReadDeckData("./resource/database/deck.data");
	ReadMonsterData("./resource/database/monster.data");

	InitHeroStatus();

	// 사용되고 있는 스킬 정보 초기화
	memset(&m_UsedSkillData, 0, sizeof(m_UsedSkillData));

	BuildBehaviorTree();
	BuildBehaviorTreeInHybridBattleForEnemy();
}

CTurnBattleState::~CTurnBattleState()
{
	m_HeroModelID = Renderer()->DeleteModelAndSetEmptyID(m_HeroModelID);
	m_HeroTextureID = Renderer()->DeleteModelAndSetEmptyID(m_HeroTextureID);

	SoundPlayer()->DeleteBackGroundMusic(m_pBGM);

	m_battleBGMID = SoundPlayer()->DeleteSoundAndSetEmptyID(m_battleBGMID);

	m_UIEnemyNameBarTextureID = Renderer()->DeleteModelAndSetEmptyID(m_UIEnemyNameBarTextureID);
	m_UIMainMenuBarTextureID = Renderer()->DeleteModelAndSetEmptyID(m_UIMainMenuBarTextureID);
	m_UIMouseButtonInformationTextureID = Renderer()->DeleteModelAndSetEmptyID(m_UIMouseButtonInformationTextureID);
	m_UIKeyInformationTextureID = Renderer()->DeleteModelAndSetEmptyID(m_UIKeyInformationTextureID);
	m_UITurnNumberTextureID = Renderer()->DeleteModelAndSetEmptyID(m_UITurnNumberTextureID);
	m_UIHpSpBarTextureID = Renderer()->DeleteModelAndSetEmptyID(m_UIHpSpBarTextureID);
	m_UILongKeyButtonTextureID = Renderer()->DeleteModelAndSetEmptyID(m_UILongKeyButtonTextureID);
	m_UISmallKeyButtonTextureID = Renderer()->DeleteModelAndSetEmptyID(m_UISmallKeyButtonTextureID);
	m_UIListBarTextureID = Renderer()->DeleteModelAndSetEmptyID(m_UIListBarTextureID);
	m_UIListBarWithBoxTextureID = Renderer()->DeleteModelAndSetEmptyID(m_UIListBarWithBoxTextureID);
}

void CTurnBattleState::ProcessInputs(float elapsedTimeInSec, GameInputs* inputs)
{
	// 대기 중일때는 조작 불가
	if (m_bIsTurnBattleStartWait) {
		return;
	}

	// 턴 배틀로 강제 종료
	if (inputs->IsPressedDownKeyOnce(&inputs->KeyTab, &inputs->processingEnd.KeyTab)) {
		SetIsAlreadyStartedAndStillPlaying(false);
		m_pGameFramework->PopState(true);
	}

	// 카메라 변경
	if (inputs->IsPressedDownKeyOnce(&inputs->KeyC, &inputs->processingEnd.KeyC)) {
		m_bTestCameraOn = (m_bTestCameraOn + 1) % 2;
	}

	if (m_bTestCameraOn) {
		Camera()->TestCameraControl(elapsedTimeInSec, inputs, m_bMouseFixed);
	}
	else {
		ProcessInputsInTurnBattleMenu(elapsedTimeInSec, inputs);
	}

	// 마우스 고정
	if (inputs->IsPressedDownKeyOnce(&inputs->KeyESC, &inputs->processingEnd.KeyESC)) {
		if (m_bMouseFixed)
		{
			glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
			m_bMouseFixed = false;

			// 이전 포인터 위치로 이동
			glutWarpPointer(m_iPrevMousePosX, m_iPrevMousePosY);
		}
		else
		{
			glutSetCursor(GLUT_CURSOR_NONE);
			m_bMouseFixed = true;

			m_iPrevMousePosX = inputs->MouseXPos;
			m_iPrevMousePosY = inputs->MouseYPos;
		}
	}

	if (inputs->IsPressedDownKeyOnce(&inputs->KeyB, &inputs->processingEnd.KeyB)) {
		m_bShowBoundingBox = (m_bShowBoundingBox + 1) % 2;
	}

	// 주인공, 나머지 업데이트 파라미터 초기화
	GameUpdateParameters heroParam;
	GameUpdateParameters othersParam;

	memset(&heroParam, 0, sizeof(GameUpdateParameters));
	memset(&othersParam, 0, sizeof(GameUpdateParameters));

	// 업데이트 매개 변수를 오브젝트 매니저에 넘김
	ObjectManager()->SetUpdateParameters(heroParam, othersParam);
}

void CTurnBattleState::Update(float elapsedTimeInSec)
{
	int x, y;
	Renderer()->GetDisplayResolution(&x, &y);
	if (m_bMouseFixed)
		glutWarpPointer(x / 2, y / 2);

	UpdateDirectionalLightToTime(elapsedTimeInSec);

	// 턴 전투 대기
	if (m_bIsTurnBattleStartWait) { 
		if (m_fStartWaitTime > DEFUALT_STATE_CHANGE_WAIT_TIME) {
			m_bIsTurnBattleStartWait = false;
		}
		m_fStartWaitTime += elapsedTimeInSec;
	}

	if (m_fShowLogTime <= 0.0f) {
		if (m_bIsRunawaySuccess) {
			// 도주(턴 전투 종료)
			SetIsAlreadyStartedAndStillPlaying(false);
			m_pGameFramework->PopState(true);
		}
	}

	// 행동트리 실행
	m_pBehaviorTree->Run();

	// 오토마타 메뉴이고 정보 표시를 하고 있을 경우 블러 효과 설정
	if (m_iMenu == AUTOMATA_MENU && m_bIsShowAutomataInformation && !m_bIsProcessing) {
		SetBlurEffect(true);
	}
	else {
		SetBlurEffect(false);
	}

	// 턴이 없을 경우 다음 차례로 넘어감
	if (!m_bIsProcessing) {
		if (RemoveDeadObject()) {
			// 적이 모두 죽거나 주인공 사망시 리턴
			SetIsAlreadyStartedAndStillPlaying(false);
			if (m_bIsGameover) {
				m_bIsGameover = false;
				m_pGameFramework->PushState(m_pGameOverState);
			}
			else if (m_bIsGameclear) {
				m_bIsGameclear = false;
				m_pGameFramework->PushState(m_pGameClearState);
			}
			else {
				m_pGameFramework->PopState(true);
			}
			return;
		}

		if (m_bHeroTeam) {
			TurnBattle::SBattleVariable bv = m_HeroVector.at(m_iOrder)->GetBattleVariable();
			if (bv.turn <= 0) {
				for (auto hero : m_HeroVector) {
					hero->SetBorder(false);
				}

				if (bv.condition != NORMAL_CONDITION) {
					bv.conditionDuration -= RECOVER_CONDITION_AMOUNT;
					if (bv.conditionDuration <= 0.0f) {
						bv.condition = NORMAL_CONDITION;
					}
					m_HeroVector.at(m_iOrder)->SetBattleVariable(bv);
				}
				m_iOrder++;

				// 메뉴 위치 초기화
				m_iMenu = MAIN_MENU;
				m_iAct = NO_ACT_MENU;
				m_iSelectedSkill = 0;
				m_iShowedSkillNum = 0;

				if (m_iOrder == m_HeroVector.size()) {
					m_iOrder = 0;
					m_iTarget = 0;
					m_bHeroTeam = false;

					// 파티 턴 계산 및 다운 레벨, 회피 레벨 초기화
					for (auto enemy : m_EnemyMonsterVector) {
						short maxTurn = enemy->GetStatus().maxTurn;
						TurnBattle::SBattleVariable bv = enemy->GetBattleVariable();
						bv.turn = maxTurn - bv.down;
						bv.turn = glm::clamp(bv.turn, (short)0, maxTurn);
						bv.down = 0;
						bv.avoidLevel = 0;
						enemy->SetBattleVariable(bv);
						enemy->SetBorder(false);
					}
					if (m_EnemyMonsterVector.size() >= 1)
						m_EnemyMonsterVector.at(0)->SetBorder(true);
				}
				else {
					m_HeroVector.at(m_iOrder)->SetBorder(true);
				}					
			}
		}
		else {
			TurnBattle::SBattleVariable bv = m_EnemyMonsterVector.at(m_iOrder)->GetBattleVariable();
			if (bv.turn <= 0) {
				for (auto enemy : m_EnemyMonsterVector) {
					enemy->SetBorder(false);
				}
				if (bv.condition != NORMAL_CONDITION) {
					bv.conditionDuration -= RECOVER_CONDITION_AMOUNT;
					if (bv.conditionDuration <= 0.0f) {
						bv.condition = NORMAL_CONDITION;
					}
					m_HeroVector.at(m_iOrder)->SetBattleVariable(bv);
				}
				m_iOrder++;

				if (m_iOrder == m_EnemyMonsterVector.size()) {
					m_iOrder = 0;
					m_iTarget = 0;
					m_bHeroTeam = true;

					// 파티 턴 계산 및 다운 레벨, 회피 레벨 초기화
					for (auto hero : m_HeroVector) {
						short maxTurn = hero->GetStatus().maxTurn;
						TurnBattle::SBattleVariable bv = hero->GetBattleVariable();
						bv.turn = maxTurn - bv.down;
						bv.turn = glm::clamp(bv.turn, (short)0, maxTurn);
						bv.down = 0;
						bv.avoidLevel = 0;
						hero->SetBattleVariable(bv);
						hero->SetBorder(false);
					}
					if (m_HeroVector.size() >= 1)
						m_HeroVector.at(0)->SetBorder(true);
				}
				m_EnemyMonsterVector.at(m_iOrder)->SetBorder(true);
			}
		}
	}
	else {

		for (int i = 0; i < MAX_ENEMY_PARTY_MEMBER; ++i) {
			if (m_UsedSkillData.enemyParty[i].exisited) {
				if (m_UsedSkillData.enemyParty[i].targeted) {
					m_UsedSkillData.enemyParty[i].skillEffectTime -= elapsedTimeInSec;
				}
			}
		}
		for (int i = 0; i < MAX_HERO_PARTY_MEMBER; ++i) {
			if (m_UsedSkillData.heroParty[i].exisited) {
				if (m_UsedSkillData.heroParty[i].targeted) {
					m_UsedSkillData.heroParty[i].skillEffectTime -= elapsedTimeInSec;
				}
			}
		}
	}

	SetSkillHitAnimation();

	CGameState::Update(elapsedTimeInSec);

	if (!m_bTestCameraOn && !m_bIsProcessing && !m_bIsGameover) {
		// 현재 순서에 따라 카메라 위치 변경
		if (m_bHeroTeam)
		{
			if (m_iMenu == STEAL_MENU) {
				m_EnemyMonsterVector.at(m_iTarget)->SetCameraPosAndDirectionWithNumber(Camera(),
					m_iTarget, (int)m_EnemyMonsterVector.size(), true, true);
			}
			else if (m_iMenu == WAIT_MENU || m_iMenu == CHANGE_DECK_MENU) {
				m_HeroVector.at(m_iOrder)->SetCameraPosAndDirectionWithNumber(Camera(),
					m_iOrder, (int)m_HeroVector.size(), true, false);
			}
			else {
				m_HeroVector.at(m_iOrder)->SetCameraPosAndDirectionWithNumber(Camera(),
					m_iOrder, (int)m_HeroVector.size(), false, false);
			}
		}
		else
		{
			m_EnemyMonsterVector.at(m_iOrder)->SetCameraPosAndDirectionWithNumber(Camera(),
				m_iOrder, (int)m_EnemyMonsterVector.size(), false, true);
		}
		Camera()->SetQuarterViewCamera();
	}
}

void CTurnBattleState::RenderScene()
{
	if (m_bIsTurnBattleStartWait) {
		m_pGameFramework->GetPrevState()->Camera()->UseCamera();
		m_pGameFramework->GetPrevState()->SaveRenderingResultOnTexture();
		
		Camera()->UseCamera();

		DefualtRenderPipeline();

		glDisable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, Renderer()->GetDisplayFXAAFBO());
		glClear(GL_COLOR_BUFFER_BIT);
		Renderer()->RenderChangeStateEffect(m_fStartWaitTime, m_bIsStartedWithHeroWin);

		// 안티 엘리어싱된 화면 렌더링
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		Renderer()->UseShader(DISPLAY_FXAA_SHADER);
		Renderer()->RenderDisplayFXAA();
	}
	else {
		Camera()->UseCamera();
		CGameState::RenderScene();
	}
}

void CTurnBattleState::Enter()
{
	if (!GetIsAlreadyStartedAndStillPlaying()) {
		SetIsAlreadyStartedAndStillPlaying(true);
		InitTurnBattle();
		SetDirectionalLightToTime();
		SoundPlayer()->PlayAndStopBackGroundMusic(m_pBGM, true);
	}
}

void CTurnBattleState::Exit()
{
	if (!GetIsAlreadyStartedAndStillPlaying()) {
		SoundPlayer()->PlayAndStopBackGroundMusic(m_pBGM, false);
		m_pTimeOfDaySkybox->SetIsRainy(false);
		if (m_iAreaNumber == 1) {
			m_pRainDropParticle->SetLifeTime(-1.0f);
		}
		InitVariableAfterBattle();
	}
}

void CTurnBattleState::Pause()
{
	if (!GetIsAlreadyStartedAndStillPlaying()) {
		SoundPlayer()->PlayAndStopBackGroundMusic(m_pBGM, false);
		m_pTimeOfDaySkybox->SetIsRainy(false);
		if (m_iAreaNumber == 1) {
			m_pRainDropParticle->SetLifeTime(-1.0f);
		}
		InitVariableAfterBattle();
	}
}

void CTurnBattleState::Resume()
{
	if (!GetIsAlreadyStartedAndStillPlaying()) {
		SetIsAlreadyStartedAndStillPlaying(true);
		InitTurnBattle();
		SetDirectionalLightToTime();
		SoundPlayer()->PlayAndStopBackGroundMusic(m_pBGM, true);
	}
}

TurnBattle::SSkill* CTurnBattleState::GetSkillData(short index)
{
	return m_SkillMap.find(index)->second;
}

int CTurnBattleState::GetSkillSoundEffect(short id)
{
	return m_SkillSoundEffectMap.find(id)->second;;
}

TurnBattle::SUsedSkillData CTurnBattleState::GetUsedSkillData() const
{
	return m_UsedSkillData;
}

void CTurnBattleState::SetUsedSkillData(TurnBattle::SUsedSkillData usedSkillData)
{
	m_UsedSkillData = usedSkillData;
}

TurnBattle::SAutomata* CTurnBattleState::GetAutomataData(short index)
{
	return m_AutomataMap.find(index)->second;
}

TurnBattle::SDeck* CTurnBattleState::GetSkillDeckData(short index)
{
	return m_DeckMap.find(index)->second;
}

bool CTurnBattleState::GetIsSkillUsed() const
{
	return m_bIsSkillUsed;
}

void CTurnBattleState::SetIsSkillUsed(bool used)
{
	m_bIsSkillUsed = used;
}

bool CTurnBattleState::GetIsNormalAttackUsed() const
{
	return m_bIsNormalAttackUsed;
}

void CTurnBattleState::SetIsNormalAttackUsed(bool used)
{
	m_bIsNormalAttackUsed = used;
}

bool CTurnBattleState::GetIsAdditionalNormalAttackUsed() const
{
	return m_bIsAdditionalNormalAttackUsed;
}

void CTurnBattleState::SetIsAdditionalNormalAttackUsed(bool used)
{
	m_bIsAdditionalNormalAttackUsed = used;
}

bool CTurnBattleState::GetIsAdditionalNormalAttackTime() const
{
	return m_bIsAdditionalNormalAttackTime;
}

void CTurnBattleState::SetIsAdditionalNormalAttackTime(bool enable)
{
	m_bIsAdditionalNormalAttackTime = enable;
}

bool CTurnBattleState::GetIsAutomataSummoned() const
{
	return m_bIsAutomataSummoned;
}

void CTurnBattleState::SetIsAutomataSummoned(bool summoned)
{
	m_bIsAutomataSummoned = summoned;
}

bool CTurnBattleState::GetIsSkillDeckChanged() const
{
	return m_bIsSkillDeckChanged;
}

void CTurnBattleState::SetIsSkillDeckChanged(bool changed)
{
	m_bIsSkillDeckChanged = changed;
}

bool CTurnBattleState::GetIsMatterAttack() const
{
	return m_bIsMatterAttack;
}

void CTurnBattleState::SetIsMatterAttack(bool matter)
{
	m_bIsMatterAttack = matter;
}

bool CTurnBattleState::GetIsProcessing() const
{
	return m_bIsProcessing;
}

void CTurnBattleState::SetIsProcessing(bool processing)
{
	m_bIsProcessing = processing;
}

bool CTurnBattleState::GetIsHeroTurn() const
{
	return m_bHeroTeam; 
}

void CTurnBattleState::SetIsHeroTurn(bool turn)
{
	m_bHeroTeam = turn;
}

bool CTurnBattleState::GetIsChangedSkillDeckAnimationEnd() const
{
	return m_bIsChangedSkillDeckAnimationEnd;
}

void CTurnBattleState::SetIsChangedSkillDeckAnimationEnd(bool end)
{
	m_bIsChangedSkillDeckAnimationEnd = end;
}

bool CTurnBattleState::GetIsWaited() const
{
	return m_bIsWaited;
}

void CTurnBattleState::SetIsWaited(bool wait)
{
	m_bIsWaited = wait;
}

bool CTurnBattleState::GetIsRested() const
{
	return m_bIsRested;
}

void CTurnBattleState::SetIsRested(bool rest)
{
	m_bIsRested = rest;
}

bool CTurnBattleState::GetIsStolen() const
{
	return m_bIsStolen;
}

void CTurnBattleState::SetIsStolen(bool steal)
{
	m_bIsStolen = steal;
}

bool CTurnBattleState::GetIsStolenAutomata() const
{
	return m_bIsStolenAutomata;
}

void CTurnBattleState::SetIsStolenAutomata(bool automata)
{
	m_bIsStolenAutomata = automata;
}

bool CTurnBattleState::GetIsRunaway() const
{
	return m_bIsRunaway;
}

void CTurnBattleState::SetIsRunaway(bool runaway)
{
	m_bIsRunaway = runaway;
}

void CTurnBattleState::SetIsRunawaySuccess(bool success)
{
	m_bIsRunawaySuccess = success;
}


bool CTurnBattleState::GetIsRunawaySuccess()
{
	return m_bIsRunawaySuccess;
}

int CTurnBattleState::GetOrder() const
{
	return m_iOrder;
}

void CTurnBattleState::GetOrderSkillTargetAndIndex(int* order, int* target, short* selectedSkillIndex)
{
	*order = m_iOrder;
	*target = m_iTarget;
	*selectedSkillIndex = m_sSelectedSkillIndex;
}

void CTurnBattleState::GetSummonedAutomataInformation(short* index, int* location)
{
	*index = m_sSummonedAutomata[m_iSummonedAutomataLocation];
	*location = m_iSummonedAutomataLocation;
}

void CTurnBattleState::GetChangedSkillDeckInformation(short* index, int* location)
{
	*index = m_HeroVector.at(0)->GetDeckVector().at(m_iSelectedSkillDeck);
	*location = m_iSelectedSkillDeck;
}

void CTurnBattleState::SetLogText(const wchar_t* logtext)
{
	wsprintf(m_wLogText, L"%s", logtext);
}

void CTurnBattleState::SetShowLog(bool show)
{
	m_bShowLog = show;
	if (m_bShowLog) {
		m_fShowLogTime = SHOW_LOG_TIME;
	}
}

CTurnBattleObject* CTurnBattleState::GetTurnBattleObject(bool hero, int index)
{
	if (hero)
		return m_HeroVector.at(index);
	else
		return m_EnemyMonsterVector.at(index);
}

void CTurnBattleState::SetBattleVariable(TurnBattle::SBattleVariable bv, bool hero, int index)
{
	if (hero)
		m_HeroVector.at(index)->SetBattleVariable(bv);
	else
		m_EnemyMonsterVector.at(index)->SetBattleVariable(bv);
}

void CTurnBattleState::SetCameraToOrderObject()
{
	if (m_bHeroTeam)
	{
		m_HeroVector.at(m_iOrder)->SetCameraPosAndDirectionWithNumber(Camera(),
			m_iOrder, (int)m_HeroVector.size(), false, false);
	}
	else
	{
		m_EnemyMonsterVector.at(m_iOrder)->SetCameraPosAndDirectionWithNumber(Camera(),
			m_iOrder, (int)m_EnemyMonsterVector.size(), false, true);
	}

	Camera()->SetQuarterViewCamera();
}

void CTurnBattleState::SetCameraToSelectedObject(bool heroParty, int index, bool self)
{
	if (heroParty)
	{
		m_HeroVector.at(index)->SetCameraPosAndDirectionWithNumber(Camera(),
			index, (int)m_HeroVector.size(), self, false);
	}
	else
	{
		m_EnemyMonsterVector.at(index)->SetCameraPosAndDirectionWithNumber(Camera(),
			index, (int)m_EnemyMonsterVector.size(), self, true);
	}

	Camera()->SetQuarterViewCamera();
}

void CTurnBattleState::SetMenuLocation(int location)
{
	m_iMenu = location;
}

void CTurnBattleState::SetSelectedSkillAndTarget(int skill, int target)
{
	m_sSelectedSkillIndex = skill;
	m_iTarget = target;
}

int CTurnBattleState::GetPartySize(bool heroParty) const
{
	if (heroParty) {
		return (int)m_HeroVector.size();
	}
	else {
		return (int)m_EnemyMonsterVector.size();
	}
}

void CTurnBattleState::SetAreaNumber(int areaNumber)
{
	m_iAreaNumber = areaNumber;
}

void CTurnBattleState::SetIsStartedWithHeroWin(bool win)
{
	m_bIsStartedWithHeroWin = win;
}

void CTurnBattleState::NormalRender()
{
	//CGameState::NormalRender();

	Renderer()->UseShader(DEFAULT_SHADER);
	DirectionalLight()->UseLight(DEFAULT_SHADER);
	DirectionalLight()->SetLightSpaceMatrix(DEFAULT_SHADER);
	PointLight()->UseLight(DEFAULT_SHADER);

	if (m_iAreaNumber == 4) {
		Renderer()->UseShader(INSTANCE_SHADER);
		DirectionalLight()->SetLightSpaceMatrix(INSTANCE_SHADER);
		DirectionalLight()->UseLight(INSTANCE_SHADER);
		PointLight()->UseLight(INSTANCE_SHADER);
		Renderer()->RenderInstance(m_grassModelID, m_grassTextureID, 
			m_GrassSSBOModelMatrices, m_fTimeForGrass, GRASS_AMOUNT_BATTLE);
	}

	m_pTimeOfDaySkybox->RenderSkybox(DirectionalLight());

	Renderer()->UseShader(DEFAULT_SHADER);
	ObjectManager()->Render();

	// 충돌 박스 렌더링
	if (m_bShowBoundingBox)
	{
		Renderer()->UseShader(BOUNDING_BOX_SHADER);
		ObjectManager()->BoundingBoxRender();
	}

	Renderer()->UseShader(GRAVITY_PARTICLE_SHADER);
	ObjectManager()->RenderParticle();

	RenderBillboards();
}

void CTurnBattleState::UIRender()
{
	int resolutionX, resolutionY;
	Renderer()->GetDisplayResolution(&resolutionX, &resolutionY);

	// 적 몬스터 이름 표시 UI 렌더링
	if (!m_bIsProcessing && m_bHeroTeam) {
		if (m_iMenu != CHANGE_DECK_MENU) {
			if (m_iMenu != WAIT_MENU && m_iMenu != AUTOMATA_MENU) {
				RenderEnemyNameBarUI(resolutionX, resolutionY);
			}
			RenderTurns(resolutionX, resolutionY);
		}
		if (m_iMenu == MAIN_MENU) {
			// 메인 메뉴 UI 렌더링
			RenderMainMenuUI(resolutionX, resolutionY);
			// 키 설명 UI 렌더링
			if (m_iAct == NO_ACT_MENU) {
				RenderKeyInformationUIForMainMenu(resolutionX, resolutionY);
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

void CTurnBattleState::LoadData()
{
	LoadBattleFieldData();

	m_battleBGMID = SoundPlayer()->LoadSound("./resource/sound/BGM/battleBGM.mp3");

	// bgm 설정
	m_pBGM = SoundPlayer()->SetBackGroundMusic(m_battleBGMID, 0.3f);

	// 잔디 모델, 텍스쳐 정보 불러오기
	m_grassModelID = Renderer()->LoadOBJModelWithIndex("./resource/obj/world/grass.obj");
	m_grassTextureID = Renderer()->ReadPNGTextureFile("./resource/texture/world/grass_tex.jpg");

	m_waterDropTexureID = Renderer()->ReadPNGTextureFile("./resource/texture/waterDrop.png");

	m_HeroModelID = Renderer()->LoadOBJModelWithIndex("./resource/obj/heroParty/player.obj");
	m_HeroTextureID = Renderer()->LoadPNGTexture(
		"./resource/texture/AllOneTexture.jpg",
		"./resource/texture/heroParty/player_color.png",
		"./resource/texture/heroParty/player_specular.png",
		DEFAULT_SHININESS,
		"./resource/texture/planNormalTexture.jpg");

	m_AutomataModelID = Renderer()->LoadOBJModelWithIndex("./resource/obj/heroParty/automata.obj");
	m_AutomataTextureID = Renderer()->LoadPNGTexture(
		"./resource/texture/AllOneTexture.jpg",
		"./resource/texture/heroParty/player_color_gray.png",
		"./resource/texture/heroParty/player_specular.png",
		DEFAULT_SHININESS,
		"./resource/texture/planNormalTexture.jpg");

	LoadMonsterData();
	LoadUIData();

	Renderer()->LoadModelWithBone("./resource/dae/hero/player_idle.dae", "player");
	Renderer()->LoadAnimation("./resource/dae/hero/player_idle.dae", "player", "player_idle");
	Renderer()->LoadAnimation("./resource/dae/hero/player_walk.dae", "player", "player_walk");
	Renderer()->LoadAnimation("./resource/dae/hero/player_attack.dae", "player", "player_attack");
	Renderer()->LoadAnimation("./resource/dae/hero/player_dash.dae", "player", "player_dash");
	Renderer()->LoadAnimation("./resource/dae/hero/player_hit.dae", "player", "player_hit");
	Renderer()->LoadAnimation("./resource/dae/hero/player_idle_long.dae", "player", "player_idle_long");
	Renderer()->LoadAnimation("./resource/dae/hero/player_skill01.dae", "player", "player_skill01");
	Renderer()->LoadAnimation("./resource/dae/hero/player_skill02.dae", "player", "player_skill02");
	Renderer()->LoadAnimation("./resource/dae/hero/player_skill03.dae", "player", "player_skill03");
}

void CTurnBattleState::AddObjectToManager()
{
	MakeGrassInstance();

	// 주인공 추가
	for (int i = 0; i < 1; ++i) 
	{
		int object = ObjectManager()->AddObject<CTurnBattleObject>(
			glm::vec3(-8.0f + 5.0f * i, -4.0f + 1.6f, 10.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			60.0f);
		m_HeroVector.push_back(ObjectManager()->GetGameObject<CTurnBattleObject>(object));
		m_HeroVector.at(i)->SetTextureIndex(m_HeroTextureID);
		m_HeroVector.at(i)->SetVolume(glm::vec3(0.8f, 1.6f, 0.4f));
		m_HeroVector.at(i)->SetEnableCollision(false);
		m_HeroVector.at(i)->SetType(TYPE_HERO);
		m_HeroVector.at(i)->SetState(STATE_GROUND);
		m_HeroVector.at(i)->SetIsAnimate(true);
		m_HeroVector.at(i)->SetIsCullingFace(false);
		m_HeroVector.at(i)->SetModelWithBone("player");
		m_HeroVector.at(i)->SetUIBackGround(m_WhiteRectTextureID);
		std::map<GameObjectAnimationState, std::string> heroAnimationDataMap;
		heroAnimationDataMap.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_IDLE, "player_idle"));
		heroAnimationDataMap.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_WALK, "player_walk"));
		heroAnimationDataMap.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_ATTACK, "player_attack"));
		heroAnimationDataMap.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_AVOID, "player_dash"));
		heroAnimationDataMap.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_HIT, "player_hit"));
		heroAnimationDataMap.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_IDLE_LONG, "player_idle_long"));
		heroAnimationDataMap.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_SKILL_1, "player_skill01"));
		heroAnimationDataMap.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_SKILL_2, "player_skill02"));
		heroAnimationDataMap.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_SKILL_3, "player_skill03"));
		heroAnimationDataMap.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_DEATH, "player_hit"));
		m_HeroVector.at(i)->SetAnimationDataNameMap(heroAnimationDataMap);
		m_HeroVector.at(i)->SetAnimationState(ANI_STATE_IDLE);
	}

	m_HeroVector.at(0)->SetBorder(true);
}

void CTurnBattleState::MakeBattleField(int areaNumber)
{
	// 이전 전투 필드 구성 오브젝트 제거
	ObjectManager()->DeleteAllObjectWithType(TYPE_FIXED);
	ObjectManager()->DeleteAllNoInteractionObjectWithType(TYPE_FIXED);
	ObjectManager()->DeleteAllNoInteractionObjectWithType(TYPE_FLOOR);
	m_vLampLightVector.clear();
	m_vLampLightVector.resize(0);

	SBattleMapData data = m_BattleFieldDataMap.find(areaNumber)->second;

	int floor = ObjectManager()->AddNoInteractionObject<CGameObject>(
		glm::vec3(0.f, -5.4f, 0.f),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(1.f, 1.f, 1.f),
		10000);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(floor)->SetModelIndex(data.fieldModel);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(floor)->SetTextureIndex(data.fieldTexture);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(floor)->SetType(TYPE_FLOOR);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(floor)->SetIsCullingFace(false);

	// 벽
	for (float i = 0; i < 361; i = i + ((float)16 / (float)2))
	{
		float x = glm::cos(glm::radians(i)) * 24 * 2;
		float z = glm::sin(glm::radians(i)) * 24 * 2;

		float angle = glm::atan(z, -x) * 180.0f / PI_VALUE;
		angle += 90.0f;

		int wall = ObjectManager()->AddNoInteractionObject<CGameObject>(
			glm::vec3(x, -4.0f, z), glm::vec3(0.f, angle, 0.f), glm::vec3(1.8f, 1.3, 1.3f), 10000);
		ObjectManager()->GetNoInteractionGameObject<CGameObject>(wall)->SetModelIndex(data.wallModel);
		ObjectManager()->GetNoInteractionGameObject<CGameObject>(wall)->SetTextureIndex(data.wallTexture);
		ObjectManager()->GetNoInteractionGameObject<CGameObject>(wall)->SetType(TYPE_FIXED);
	}

	// 가로등
	glm::vec3* positions = new glm::vec3[MAX_POINT_LIGHTS_NUM];
	int num = 0;
	for (float i = 0; i < 360; i = i + ((float)180 / (float)6))
	{
		float locationX = glm::cos(glm::radians(i)) * 24 * 2 - glm::cos(glm::radians(i)) * 5;
		float locationY = glm::sin(glm::radians(i)) * 24 * 2 - glm::sin(glm::radians(i)) * 5;
		int lamp = ObjectManager()->AddObject<CGameObject>(glm::vec3(locationX, -1.1f, locationY),
			glm::vec3(0.f, 0.0f, 0.f), glm::vec3(1.5f, 1.5f, 1.5f), 10000);
		ObjectManager()->GetGameObject<CGameObject>(lamp)->SetModelIndex(
			data.lampModel);
		ObjectManager()->GetGameObject<CGameObject>(lamp)->SetTextureIndex(
			data.lampTexture);
		ObjectManager()->GetGameObject<CGameObject>(lamp)->SetType(TYPE_FIXED);
		ObjectManager()->GetGameObject<CGameObject>(lamp)->SetRadiusAndHeight(0.3f, 3.2f);

		positions[num] = glm::vec3(locationX, 2.0f, locationY);
		num++;

		int light = ObjectManager()->AddObject<CGameObject>(glm::vec3(locationX, -1.1f, locationY),
			glm::vec3(0.f, 0.0f, 0.f), glm::vec3(1.575f, 1.5f, 1.575f), 10000);
		ObjectManager()->GetGameObject<CGameObject>(light)->SetModelIndex(data.lampLightModel);
		ObjectManager()->GetGameObject<CGameObject>(light)->SetTextureIndex(data.lampTexture);
		ObjectManager()->GetGameObject<CGameObject>(light)->SetType(TYPE_FIXED);
		ObjectManager()->GetGameObject<CGameObject>(light)->SetIsLightSource(true);
		ObjectManager()->GetGameObject<CGameObject>(light)->SetColor(data.lightColor);

		m_vLampLightVector.push_back(ObjectManager()->GetGameObject<CGameObject>(light));
	}
	PointLight()->SetPointLights(positions, num);
	PointLight()->SetDiffuseLight(glm::vec4(1.0f, 1.0f, 0.75f, 1.0f));
	PointLight()->SetIsPointLightsOn(false);
}

void CTurnBattleState::InitHeroStatus()
{
	// 주인공 초기 스테이터스 설정
	TurnBattle::SStatus status;
	memset(&status, 0, sizeof(status));
	status.level = 5;
	status.maxHp = 80;
	status.maxSp = 60;
	status.maxTurn = 1;
	for (int i = MATTER_ATTACK; i <= AVOIDABILITY; ++i) {
		status.stat[i] = 10;
	}

	TurnBattle::SBattleVariable battleVariable;
	memset(&battleVariable, 0, sizeof(battleVariable));

	m_HeroVector.at(0)->SetStatus(status);
	m_HeroVector.at(0)->InitBattleVariable();

	m_HeroVector.at(0)->GiveSkill(20);
	m_HeroVector.at(0)->GiveSkill(40);
	m_HeroVector.at(0)->GiveSkill(990);

	m_HeroVector.at(0)->GiveAutomata(1);
	m_HeroVector.at(0)->GiveAutomata(2);

	m_HeroVector.at(0)->GiveDeck(1);
	m_HeroVector.at(0)->GiveDeck(2);

	m_UsedSkillData.heroParty[0].exisited = true;
	m_HeroVector.at(0)->SetName(L"NOMEN");
	m_HeroVector.at(0)->SetAttackAttribute(-1);

	m_sSummonedAutomata[0] = 0;
	m_sSummonedAutomata[1] = 0;
}

void CTurnBattleState::InitTurnBattle()
{
	// 광원 위치 변경
	m_pTimeOfDaySkybox->Initialize(DirectionalLight());

	// 지역 번호에 따라 적 생성으로 변경할 것
	memset(&m_UsedSkillData, 0, sizeof(m_UsedSkillData));
	
	// 기존에 있던 적 제거
	for (auto enemy : m_EnemyMonsterVector) {
		enemy->SetLifeTime(0.0f);
	}
	m_EnemyMonsterVector.clear();
	if (m_iAreaNumber == 1) {
		InsertMonsterToEnemyVectorBoss(m_bIsStartedWithHeroWin, m_iAreaNumber);

		// 비 파티클
		int rainDrop = ObjectManager()->
			AddParticle<CRainDropParticle>(glm::vec3(0.0f, -1.f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		m_pRainDropParticle = ObjectManager()->GetParticle<CRainDropParticle>(rainDrop);
		m_pRainDropParticle->SetTextureID(m_waterDropTexureID);
		m_pRainDropParticle->SetColor(glm::vec4(0.7f, 0.0f, 0.0f, 0.3f));
		m_pRainDropParticle->CreateVAO();
	}
	else {
		InsertMonsterToEnemyVector(m_bIsStartedWithHeroWin, m_iAreaNumber);
	}

	// 전투 초기화
	m_iOrder = 0;
	m_bHeroTeam = true;
	m_iTarget = 0;
	m_iAct = 0;
	m_iMenu = 0;
	m_bIsGameover = false;
	m_bIsRunaway = false;
	m_bIsRunawaySuccess = false;
	m_bIsProcessing = false;
	m_bIsWaited = false;
	m_bIsStolen = false;
	m_bIsNormalAttackUsed = false;
	m_bIsSkillUsed = false;
	m_bIsAutomataSummoned = false;


	// 주인공 파티 배틀 정보 초기화
	for (int i = 0; i < m_HeroVector.size(); ++i) {
		TurnBattle::SStatus status = m_HeroVector.at(i)->GetStatus();
		TurnBattle::SBattleVariable bv = m_HeroVector.at(i)->GetBattleVariable();
		bv.turn = status.maxTurn;
		bv.attackBuff = 0;
		bv.avoidLevel = 0;
		bv.defenseBuff = 0;
		bv.down = 0;
		m_HeroVector.at(i)->SetBattleVariable(bv);
		m_HeroVector.at(i)->SetIsUpdatedTurnGaugeWhenEndTurn(false);

		if (m_bIsStartedWithHeroWin) {
			m_HeroVector.at(i)->SetGettingTurnGauge(1.0f);
		}
		else {
			m_HeroVector.at(i)->SetGettingTurnGauge(0.0f);
		}

		// 위치 초기화
		m_HeroVector.at(i)->SetPosition(glm::vec3(-8.0f + 5.0f * i, -4.0f + 1.6, 10.0f));
	}

	MakeBattleField(m_iAreaNumber);

	m_bIsTurnModeOpened = false;

	// 턴 전투 시작 대기
	m_bIsTurnBattleStartWait = true;
	m_fStartWaitTime = 0.0f;

	// 카메라 초기화
	m_HeroVector.at(0)->SetCameraPosAndDirectionWithNumber(Camera(),
		m_iOrder, (int)m_HeroVector.size(), false, false);
	Camera()->SetQuarterViewCamera();

	if (m_iAreaNumber == 1) {
		m_pTimeOfDaySkybox->SetIsRainy(true);
	}
}

void CTurnBattleState::InsertMonsterToEnemyVector(bool isWin, int areaNumber)
{
	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_int_distribution<int> uidMemberSize(2, MAX_ENEMY_PARTY_MEMBER);
	short startIndex, endIndex;

	glm::vec3 volume = glm::vec3(1.0f);
	if (m_iAreaNumber == 4) {
		startIndex = 1;
		endIndex = 4;
		volume = glm::vec3(1.1f, 0.8f, 1.1f);
	}
	else if (m_iAreaNumber == 3) {
		startIndex = 5;
		endIndex = 7;
		volume = glm::vec3(1.0f, 0.8f, 3.0f);
	}
	else if (m_iAreaNumber == 2) {
		startIndex = 8;
		endIndex = 9;
		volume = glm::vec3(2.0f, 2.0f, 1.0f);
	}

	std::uniform_int_distribution<short> uidType(startIndex, endIndex);

	// 몬스터 오브젝트 생성
	int memberSize = uidMemberSize(dre);
	if (areaNumber == 1) {
		memberSize = 1;
	}

	for (int i = 0; i < memberSize; ++i) {
		int object = ObjectManager()->AddObject<CTurnBattleObject>(
			glm::vec3(-15.0f + 6.0f * i, -4.0f + volume.y, -10.0f),
			glm::vec3(0.0f, 180.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			60.0f);
		m_EnemyMonsterVector.push_back(ObjectManager()->GetGameObject<CTurnBattleObject>(object));
		m_EnemyMonsterVector.at(i)->SetVolume(volume);
		m_EnemyMonsterVector.at(i)->SetType(TYPE_ENEMY);
		m_EnemyMonsterVector.at(i)->SetState(STATE_GROUND);
		m_EnemyMonsterVector.at(i)->SetEnableCollision(false);
		m_EnemyMonsterVector.at(i)->SetUIBackGround(m_WhiteRectTextureID);

	}

	// 생성된 오브젝트에 데이터 추가
	for (int index = 0; index < m_EnemyMonsterVector.size(); ++index) {
		short id = uidType(dre);

		TurnBattle::SMonster* monsterData = m_MonsterMap.find(id)->second;
		CTurnBattleObject* monster = m_EnemyMonsterVector.at(index);
		
		// 모델 텍스쳐 설정
		monster->SetModelWithBone(m_MonsterModelMap.find(monsterData->modelId)->second);
		monster->SetAnimationDataNameMap(m_MonsterAnimationsMap.find(monsterData->modelId)->second);
		monster->SetTextureIndex(m_MonsterTextureMap.find(monsterData->textureId)->second);
		monster->SetBloodTextureID(m_MonsterBloodTextureMap.find(monsterData->textureId)->second);
		monster->SetDefaultSoundID(m_MonsterDefaultSoundMap.find(monsterData->textureId)->second);
		monster->SetHitSoundID(m_MonsterDefaultSoundMap.find(HIT_SOUND_START_INDEX_IN_DEFAULT_SOUND_MAP + monsterData->textureId)->second);
		// 애니메이션 설정
		monster->SetIsAnimate(true);
		monster->SetAnimationState(ANI_STATE_IDLE);

		// 몬스터 스테이터스 설정
		monster->SetName(monsterData->name);
		monster->SetStatus(monsterData->status);
		monster->SetIndexBingStolen(monsterData->automata, monsterData->deck);
		for (int i = 0; i < monsterData->skillSize; ++i) {
			monster->GiveSkill(monsterData->skillList[i]);
		}
		monster->SetAttackAttribute(monsterData->attackAttribute);
		monster->SetMonsterId(id);

		// 전투변수 초기화
		monster->InitBattleVariable();

		if (isWin) {
			TurnBattle::SBattleVariable bv = monster->GetBattleVariable();
			bv.down = 1;
			monster->SetBattleVariable(bv);
		}

		if (m_iAreaNumber == 4) {
			monster->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.75f));
		}
	}
	m_EnemyMonsterVector.at(0)->SetBorder(true);
}

void CTurnBattleState::InsertMonsterToEnemyVectorBoss(bool isWin, int areaNumber)
{
	for (int i = 0; i < 3; ++i) {
		int object = ObjectManager()->AddObject<CTurnBattleObject>(
			glm::vec3(-15.0f + 5.0f + 5.0f * i, -4.0f + 2.0f, -10.0f),
			glm::vec3(0.0f, 180.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			60.0f);
		m_EnemyMonsterVector.push_back(ObjectManager()->GetGameObject<CTurnBattleObject>(object));
		m_EnemyMonsterVector.at(i)->SetVolume(glm::vec3(1.25f, 2.0f, 4.2f));
		m_EnemyMonsterVector.at(i)->SetType(TYPE_ENEMY);
		m_EnemyMonsterVector.at(i)->SetState(STATE_GROUND);
		m_EnemyMonsterVector.at(i)->SetEnableCollision(false);
		m_EnemyMonsterVector.at(i)->SetUIBackGround(m_WhiteRectTextureID); 
	}

	// 생성된 오브젝트에 데이터 추가
	for (int index = 0; index < m_EnemyMonsterVector.size(); ++index) {
		short id = 10;
		if (index == 0) id = 11;
		if (index == 2) id = 12;

		TurnBattle::SMonster* monsterData = m_MonsterMap.find(id)->second;
		CTurnBattleObject* monster = m_EnemyMonsterVector.at(index);

		// 모델 텍스쳐 설정
		monster->SetModelWithBone(m_MonsterModelMap.find(monsterData->modelId)->second);
		monster->SetAnimationDataNameMap(m_MonsterAnimationsMap.find(monsterData->modelId)->second);
		monster->SetTextureIndex(m_MonsterTextureMap.find(monsterData->textureId)->second);
		monster->SetBloodTextureID(m_MonsterBloodTextureMap.find(monsterData->textureId)->second);
		monster->SetDefaultSoundID(m_MonsterDefaultSoundMap.find(monsterData->textureId)->second);
		monster->SetHitSoundID(m_MonsterDefaultSoundMap.find(HIT_SOUND_START_INDEX_IN_DEFAULT_SOUND_MAP + monsterData->textureId)->second);

		// 애니메이션 설정
		monster->SetIsAnimate(true);
		monster->SetAnimationState(ANI_STATE_IDLE);

		// 몬스터 스테이터스 설정
		monster->SetName(monsterData->name);
		monster->SetStatus(monsterData->status);
		monster->SetIndexBingStolen(monsterData->automata, monsterData->deck);
		for (int i = 0; i < monsterData->skillSize; ++i) {
			monster->GiveSkill(monsterData->skillList[i]);
		}
		monster->SetAttackAttribute(monsterData->attackAttribute);
		monster->SetMonsterId(id);
		// 전투변수 초기화
		monster->InitBattleVariable();

		if (id != 10) {
			monster->SetWhetherToRender(false);
			monster->SetIsMadeShadow(false);
			monster->SetIsAnimate(false);
			monster->SetIsSubObject(true);
			monster->SetIsFixed(true);
		}
		else {
			monster->SetIsBoss(true);
		}

		if (isWin) {
			TurnBattle::SBattleVariable bv = monster->GetBattleVariable();
			bv.down = 1;
			monster->SetBattleVariable(bv);
		}
	}
	m_EnemyMonsterVector.at(0)->SetParent(m_EnemyMonsterVector.at(1));
	m_EnemyMonsterVector.at(2)->SetParent(m_EnemyMonsterVector.at(1));
}

void CTurnBattleState::InitVariableAfterBattle()
{
	for (auto hero : m_HeroVector) {
		TurnBattle::SBattleVariable bv = hero->GetBattleVariable();
		bv.condition = NORMAL_CONDITION;
		hero->SetBattleVariable(bv);
	}

	// 전투 초기화
	m_iOrder = 0;
	m_bHeroTeam = true;
	m_iTarget = 0;
	m_iAct = 0;
	m_iMenu = 0;
	m_bIsGameover = false;
	m_bIsRunawaySuccess = false;
}

void CTurnBattleState::MakeGrassInstance()
{
	// 잔디 인스턴스 모델 변환 저장
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[GRASS_AMOUNT_BATTLE];
	int index = 0;
	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_real_distribution<float> urd(0.5f, 1.5f);

	for (int t = 1; t < 190; ++t)
	{
		for (float i = 0; i < 361; i = i + ((float)72.f / (float)t))
		{
			glm::mat4 trans = glm::mat4(1.0f);
			glm::mat4 scale = glm::mat4(1.0f);

			trans = glm::translate(trans,
				glm::vec3(glm::cos(glm::radians(i)) * 0.25 * t - 1.0 + urd(dre),
					-4.0f, glm::sin(glm::radians(i)) * 0.25 * t - 1.0 + urd(dre)));
			float size = urd(dre);
			scale = glm::scale(scale, glm::vec3(size, size + 0.5f, size));

			modelMatrices[index] = trans * scale;
			index++;
		}
	}
	//std::cout << index << std::endl;

	glGenBuffers(1, &m_GrassSSBOModelMatrices);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_GrassSSBOModelMatrices);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::mat4) * GRASS_AMOUNT_BATTLE, modelMatrices, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_GrassSSBOModelMatrices);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	delete[] modelMatrices;
}

void CTurnBattleState::RenderBillboards()
{
	// 스킬 데미지 표시
	Renderer()->UseShader(BILLBOARD_TEXT_SHADER);

	for (auto enemy : m_EnemyMonsterVector) {
		enemy->RenderDamageText(Renderer(), false);
	}
	for (auto hero : m_HeroVector) {
		hero->RenderDamageText(Renderer(), true);
	}
	// 다운 레벨 표시
	for (auto enemy : m_EnemyMonsterVector) {
		enemy->RenderDownLevelText(Renderer(), false);
	}
	for (auto hero : m_HeroVector) {
		hero->RenderDownLevelText(Renderer(), true);
	}
	
	Renderer()->UseShader(BILLBOARD_SHADER);

	// 일반 텍스쳐 빌보드가 하나도 렌더링되지 않을 경우 흰 화면 버그 수정을 위해 빌보드 하나 렌더링(원인 불명)
	Renderer()->RenderTexture3D(m_WhiteRectTextureID, glm::vec3(-1000.0f, -1000.0f, -1000.0f),
		glm::vec2(0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	if (m_bHeroTeam) {
		if (m_iMenu != STEAL_MENU && !(m_iMenu == AUTOMATA_MENU && m_bIsShowAutomataInformation)) {
			m_EnemyMonsterVector.at(m_iTarget)->RenderHPBarOnTop(Renderer(), false);
		}
	}
	else {
		m_HeroVector.at(m_iTarget)->RenderHPBarOnTop(Renderer(), true);

	}

	// 스킬의 타겟이 된 경우 hp바 표시	
	for (int i = 0; i < MAX_ENEMY_PARTY_MEMBER; ++i) {
		if (m_UsedSkillData.enemyParty[i].exisited) {
			if (m_UsedSkillData.enemyParty[i].IsShowedHpbar) {
				m_EnemyMonsterVector.at(i)->RenderHPBarOnTop(Renderer(), false);
			}
		}
	}
	for (int i = 0; i < MAX_HERO_PARTY_MEMBER; ++i) {
		if (m_UsedSkillData.heroParty[i].exisited) {
			if (m_UsedSkillData.heroParty[i].IsShowedHpbar) {
				m_HeroVector.at(i)->RenderHPBarOnTop(Renderer(), true);
			}
		}
	}

	// 빌보드 애니메이션 렌더링
	Renderer()->UseShader(BILLBOARD_SPRITE_SHADER);

	float additionalDistance = 1.0f;
	for (int i = 0; i < MAX_ENEMY_PARTY_MEMBER; ++i) {
		if (m_UsedSkillData.enemyParty[i].exisited) {
			if (m_UsedSkillData.enemyParty[i].targeted) {
				glm::vec3 position = m_EnemyMonsterVector.at(i)->GetPosition();
				float dis = m_EnemyMonsterVector.at(i)->GetVolume().z;
				Renderer()->RenderSpriteSheet3D(m_UsedSkillData.enemyParty[i].skillEffectData,
					SKILL_EFFECT_FRAME, 1, (int)((2.0f - m_UsedSkillData.enemyParty[i].skillEffectTime) * SKILL_EFFECT_FRAME), 0,
					glm::vec3(position.x, position.y, position.z + dis + additionalDistance),
					glm::vec2(1.5f, 1.5f),
					glm::vec4(1.f, 1.f, 1.f, 1.f));
			}
		}
	}

	additionalDistance = 0.5f;
	for (int i = 0; i < MAX_HERO_PARTY_MEMBER; ++i) {
		if (m_UsedSkillData.heroParty[i].exisited) {
			if (m_UsedSkillData.heroParty[i].targeted) {
				glm::vec3 position = m_HeroVector.at(i)->GetPosition();
				float dis = m_HeroVector.at(i)->GetVolume().z;

				Renderer()->RenderSpriteSheet3D(m_UsedSkillData.heroParty[i].skillEffectData,
					SKILL_EFFECT_FRAME, 1, (int)((2.0f - m_UsedSkillData.heroParty[i].skillEffectTime) * SKILL_EFFECT_FRAME), 0,
					glm::vec3(position.x, position.y, position.z - dis - additionalDistance),
					glm::vec2(1.5f, 1.5f),
					glm::vec4(1.f, 1.f, 1.f, 1.f));
				Renderer()->RenderSpriteSheet3D(m_UsedSkillData.heroParty[i].skillEffectData,
					SKILL_EFFECT_FRAME, 1, (int)((2.0f - m_UsedSkillData.heroParty[i].skillEffectTime) * SKILL_EFFECT_FRAME), 0,
					glm::vec3(position.x, position.y, position.z + dis + additionalDistance),
					glm::vec2(1.5f, 1.5f),
					glm::vec4(1.f, 1.f, 1.f, 1.f));
			}
		}
	}
}

void CTurnBattleState::ProcessInputsInTurnBattleMenu(float elapsedTimeInSec, GameInputs* inputs)
{
	if (m_bIsProcessing) {
		// 추가 일반 공격
		if (m_bIsAdditionalNormalAttackTime)
		{
			if (inputs->IsPressedDownKeyOnce(&inputs->MouseLeftButton, &inputs->processingEnd.MouseLeftButton)) {
				UseNormalAttack(true, true);
			}
			if (inputs->IsPressedDownKeyOnce(&inputs->MouseRightButton, &inputs->processingEnd.MouseRightButton)) {
				UseNormalAttack(false, true);
			}
		}

		// 처리 중일 경우 리턴(조작 불능)
		return;
	}

	if (!m_bHeroTeam) {
		return;
	}

	ProcessInputsADMove(elapsedTimeInSec, inputs);
	ProcessInputsForEachMenuProcessing(elapsedTimeInSec, inputs);
	ProcessInputsForEachMenuSelect(elapsedTimeInSec, inputs);
}

void CTurnBattleState::ProcessInputsADMove(float elapsedTimeInSec, GameInputs* inputs)
{
	// 어떤 메뉴든지 상관없는 조작
	if (m_iMenu != CHANGE_DECK_MENU) {
		// 적 선택 변경
		m_EnemyMonsterVector.at(m_iTarget)->SetBorder(false);
		if (inputs->KeyA) {
			if (inputs->IsPressedDownKeyOnce(&inputs->KeyA, &inputs->processingEnd.KeyA)) {
				m_fKeepPressKey = 0.0f;
				m_iTarget--;
			}
			else {
				if (IsKeyResultWaitedEnough(elapsedTimeInSec)) {
					m_iTarget--;
				}
			}
		}
		if (inputs->KeyD) {
			if (inputs->IsPressedDownKeyOnce(&inputs->KeyD, &inputs->processingEnd.KeyD)) {
				m_fKeepPressKey = 0.0f;
				m_iTarget++;
			}
			else {
				if (IsKeyResultWaitedEnough(elapsedTimeInSec)) {
					m_iTarget++;
				}
			}
		}
		m_iTarget = glm::clamp(m_iTarget, 0, (int)m_EnemyMonsterVector.size() - 1);
		m_EnemyMonsterVector.at(m_iTarget)->SetBorder(true);
	}
	else {
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
		if (m_HeroVector.at(0)->GetDeckVector().size() != 0) {
			m_iSelectedSkillDeck = glm::clamp(m_iSelectedSkillDeck, 0,
				int(m_HeroVector.at(0)->GetDeckVector().size() - 1));
		}
		else {
			m_iSelectedSkillDeck = 0;
		}

	}
}

void CTurnBattleState::ProcessInputsForEachMenuSelect(float elapsedTimeInSec, GameInputs* inputs)
{
	// 각 메뉴안 실행 및 메인으로 나가기
	if (m_iMenu != MAIN_MENU) {
		if (inputs->KeyLeftShift || inputs->MouseRightButton) {
			if (inputs->IsPressedDownKeyOnce(&inputs->KeyLeftShift, &inputs->processingEnd.KeyLeftShift)) {
				m_iMenu = MAIN_MENU;
			}
			else if (inputs->IsPressedDownKeyOnce(&inputs->MouseRightButton, &inputs->processingEnd.MouseRightButton)) {
				m_iMenu = MAIN_MENU;
			}
		}
		else if (inputs->KeySpace || inputs->MouseLeftButton)
		{
			if (inputs->IsPressedDownKeyOnce(&inputs->KeySpace, &inputs->processingEnd.KeySpace)) {
				// 실행
				ExecuteInMenu();
			}
			else if (inputs->IsPressedDownKeyOnce(&inputs->MouseLeftButton, &inputs->processingEnd.MouseLeftButton)) {
				// 실행
				ExecuteInMenu();
			}
		}

		// 스틸, 대기 메뉴에서 선택
		else if (inputs->IsPressedDownKeyOnce(&inputs->KeyW, &inputs->processingEnd.KeyW)) {
			if (m_iMenu == STEAL_MENU) {
				StealAction(true);
			}
			else if (m_iMenu == WAIT_MENU) {
				WaitAction(false);
			}
		}
		else if (inputs->IsPressedDownKeyOnce(&inputs->KeyS, &inputs->processingEnd.KeyS)) {
			if (m_iMenu == STEAL_MENU) {
				StealAction(false);
			}
			else if (m_iMenu == WAIT_MENU) {
				WaitAction(true);
			}
		}
	}
}

void CTurnBattleState::ProcessInputsForEachMenuProcessing(float elapsedTimeInSec, GameInputs* inputs)
{
	if (m_iMenu == MAIN_MENU) {
		ProcessInputsInMainMenu(elapsedTimeInSec, inputs);
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

void CTurnBattleState::ProcessInputsInMainMenu(float elapsedTimeInSec, GameInputs* inputs)
{
	// 일반 공격 사용
	int condition = m_HeroVector.at(m_iOrder)->GetBattleVariable().condition;

	if (m_iAct == NO_ACT_MENU) {
		if (condition != BURN_CONDITION) {
			if (inputs->IsPressedDownKeyOnce(&inputs->MouseLeftButton, &inputs->processingEnd.MouseLeftButton)) {
				UseNormalAttack(true, false);
			}
		}
		if (condition != ANXIETY_CONDITION) {
			if (inputs->IsPressedDownKeyOnce(&inputs->MouseRightButton, &inputs->processingEnd.MouseRightButton)) {
				UseNormalAttack(false, false);
			}
		}
	}

	if (inputs->IsPressedDownKeyOnce(&inputs->KeyQ, &inputs->processingEnd.KeyQ)) {
		m_bIsShowEnemyInformation = (m_bIsShowEnemyInformation + 1) % 2;
	}

	if (condition != ANXIETY_CONDITION) {
		if (inputs->IsPressedDownKeyOnce(&inputs->KeyE, &inputs->processingEnd.KeyE)) {
			m_iMenu = 1;
		}
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
		else{	
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

void CTurnBattleState::ProcessInputsInSkillMenu(float elapsedTimeInSec, GameInputs* inputs)
{
	if (inputs->KeyW) {
		if (inputs->IsPressedDownKeyOnce(&inputs->KeyW, &inputs->processingEnd.KeyW)) {
			m_fKeepPressKey = 0.0f;
			m_iSelectedSkill--;
			m_iShowedSkillNum--;
		}
		else if (IsKeyResultWaitedEnough(elapsedTimeInSec)) {
			m_iSelectedSkill--;
			m_iShowedSkillNum--;
		}
	}
	if (inputs->KeyS) {
		if (inputs->IsPressedDownKeyOnce(&inputs->KeyS, &inputs->processingEnd.KeyS)) {
			m_fKeepPressKey = 0.0f;
			m_iSelectedSkill++;
			m_iShowedSkillNum++;
		}
		else if (IsKeyResultWaitedEnough(elapsedTimeInSec)) {
			m_iSelectedSkill++;
			m_iShowedSkillNum++;
		}
	}
	int skillListSize = (int)m_HeroVector.at(m_iOrder)->GetSkillVector().size();
	if (skillListSize != 0) {
		m_iSelectedSkill = glm::clamp(m_iSelectedSkill, 0, skillListSize - 1);
	}
	else {
		m_iSelectedSkill = 0;
	}
	m_iShowedSkillNum = glm::clamp(m_iShowedSkillNum, 0, skillListSize - 1);
	m_iShowedSkillNum = glm::clamp(m_iShowedSkillNum, 0, 3);

	if (inputs->IsPressedDownKeyOnce(&inputs->KeyQ, &inputs->processingEnd.KeyQ)) {
		m_bIsShowSkillInformation = (m_bIsShowSkillInformation + 1) % 2;
	}

	else if (inputs->IsPressedDownKeyOnce(&inputs->KeyR, &inputs->processingEnd.KeyR)) {
		// 스킬 단계 높임
		short selectedSkillIndex = m_HeroVector.at(m_iOrder)->GetSkillVector().at(m_iSelectedSkill);
		short selectedSkillIndexTurn = selectedSkillIndex % 10 + 1;

		// 파트, 메트(버프 디버프 스킬)는 따로 처리
		if (1000 <= selectedSkillIndex && selectedSkillIndex <= 1020) {
			if (selectedSkillIndexTurn <= 4) {
				if (selectedSkillIndexTurn >= 3) {
					m_HeroVector.at(m_iOrder)->ChangeSkillAt(m_iSelectedSkill, selectedSkillIndex - 2);
				}
				else {
					m_HeroVector.at(m_iOrder)->ChangeSkillAt(m_iSelectedSkill, selectedSkillIndex + 2);
				}
			}
		}
		else {
			if (selectedSkillIndexTurn < 4) {
				if (selectedSkillIndexTurn == 3) {
					m_HeroVector.at(m_iOrder)->ChangeSkillAt(m_iSelectedSkill, selectedSkillIndex - 2);
				}
				else {
					m_HeroVector.at(m_iOrder)->ChangeSkillAt(m_iSelectedSkill, selectedSkillIndex + 1);
				}
			}
		}
	}
}

void CTurnBattleState::ProcessInputsInAutomataMenu(float elapsedTimeInSec, GameInputs* inputs)
{
	if (inputs->KeyW) {
		if (inputs->IsPressedDownKeyOnce(&inputs->KeyW, &inputs->processingEnd.KeyW)) {
			m_fKeepPressKey = 0.0f;
			m_iSelectedAutomata--;
			m_iShowedAutomataNum--;
		}
		else if (IsKeyResultWaitedEnough(elapsedTimeInSec)) {
			m_iSelectedAutomata--;
			m_iShowedAutomataNum--;
		}
	}
	if (inputs->KeyS){
		if (inputs->IsPressedDownKeyOnce(&inputs->KeyS, &inputs->processingEnd.KeyS)) {
			m_fKeepPressKey = 0.0f;
			m_iSelectedAutomata++;
			m_iShowedAutomataNum++;
		}
		else if (IsKeyResultWaitedEnough(elapsedTimeInSec)) {
			m_iSelectedAutomata++;
			m_iShowedAutomataNum++;
		}
	}
	int automataListSize = (int)m_HeroVector.at(m_iOrder)->GetAutomataVector().size();
	if (automataListSize != 0) {
		m_iSelectedAutomata = glm::clamp(m_iSelectedAutomata, 0, automataListSize - 1);
	}
	else {
		m_iSelectedAutomata = 0;
	}
	m_iShowedAutomataNum = glm::clamp(m_iShowedAutomataNum, 0, automataListSize - 1);
	m_iShowedAutomataNum = glm::clamp(m_iShowedAutomataNum, 0, 3);

	if (inputs->IsPressedDownKeyOnce(&inputs->KeyQ, &inputs->processingEnd.KeyQ)) {
		if (m_HeroVector.at(m_iOrder)->GetAutomataVector().size() != 0) {
			m_bIsShowAutomataInformation = (m_bIsShowAutomataInformation + 1) % 2;
		}
	}
	else if (inputs->IsPressedDownKeyOnce(&inputs->KeyR, &inputs->processingEnd.KeyR)) {
		if (m_HeroVector.at(m_iOrder)->GetAutomataVector().size() != 0) {
			// 소환된 오토마타 제거
			short automataIndex = m_HeroVector.at(m_iOrder)->GetAutomataVector().at(m_iSelectedAutomata);
			for (int i = 0; i < 2; ++i) {
				if (m_sSummonedAutomata[i] == automataIndex) {
					size_t size = m_HeroVector.size();
					m_HeroVector.at(i + 1)->SetLifeTime(0.0f);
					m_HeroVector.erase(m_HeroVector.begin() + i + 1);
					m_sSummonedAutomata[i] = 0;

					// 중간이 제거될 경우 끝에 있는 오토마타를 중간으로 이동
					if (i == 0) {
						if (size == 3) {
							m_sSummonedAutomata[0] = m_sSummonedAutomata[1];
							m_sSummonedAutomata[1] = 0;
							m_HeroVector.at(1)->SetPosition(glm::vec3(-6.0f + 4.0f * 1, -3.0f, 5.0f));
						}
					}

					m_HeroVector.resize(size - 1);
					break;
				}
			}
		}
	}
}

bool CTurnBattleState::IsKeyResultWaitedEnough(float time)
{
	m_fKeepPressKey += time * DEFAULT_MOVE_IN_MENU_SPEED;
	if (m_fKeepPressKey >= 1.0f) {
		m_fKeepPressKey = 0.0f;
		return true;
	}
	return false;
}

void CTurnBattleState::ExecuteInMenu()
{
	switch (m_iMenu) {
	case SKILL_MENU:
		UseSkill();
		break;

	case AUTOMATA_MENU: 
		SummonAutomata();
		break;

	case CHANGE_DECK_MENU:
		ChangeSkillDeck();
		break;
	}
}

void CTurnBattleState::UseSkill()
{
	TurnBattle::SBattleVariable heroBattleVariable = m_HeroVector.at(m_iOrder)->GetBattleVariable();
	short skillIndex = m_HeroVector.at(m_iOrder)->GetSkillVector().at(m_iSelectedSkill);
	TurnBattle::SSkill* skill = m_SkillMap.find(skillIndex)->second;

	if (m_HeroVector.at(m_iOrder)->CheckEnableSkill(skill)) {

		heroBattleVariable.sp -= skill->sp;
		heroBattleVariable.turn -= skill->turn;

		m_HeroVector.at(m_iOrder)->SetBattleVariable(heroBattleVariable);

		m_sSelectedSkillIndex = skillIndex;
		m_bIsSkillUsed = true;
	}
}

void CTurnBattleState::SummonAutomata()
{
	if (m_HeroVector.at(m_iOrder)->GetAutomataVector().size() != 0) {
		if (m_HeroVector.size() < 3) {
			short automataIndex = m_HeroVector.at(m_iOrder)->GetAutomataVector().at(m_iSelectedAutomata);

			if (m_sSummonedAutomata[0] == automataIndex || m_sSummonedAutomata[1] == automataIndex)
				return;

			// 스테이터스 데이터 추가
			TurnBattle::SAutomata* automata = m_AutomataMap.find(automataIndex)->second;

			short heroSp = m_HeroVector.at(m_iOrder)->GetBattleVariable().sp;
			short sp = automata->status.level * 2;

			if (heroSp >= sp) {
				TurnBattle::SBattleVariable heroBattleVariable = m_HeroVector.at(m_iOrder)->GetBattleVariable();
				heroBattleVariable.sp = heroSp - sp;
				heroBattleVariable.turn -= 1;

				m_HeroVector.at(m_iOrder)->SetBattleVariable(heroBattleVariable);

				int index = (int)m_HeroVector.size();

				// 오브젝트 생성
				int object = ObjectManager()->AddObject<CTurnBattleObject>(
					glm::vec3(-8.0f + 5.0f * index, -4.0f + 1.6f, 10.0f),
					glm::vec3(0.0f, 0.0f, 0.0f),
					glm::vec3(1.0f, 1.0f, 1.0f),
					60.0f);
				m_HeroVector.push_back(ObjectManager()->GetGameObject<CTurnBattleObject>(object));
				m_HeroVector.at(index)->SetModelIndex(m_AutomataModelID);
				m_HeroVector.at(index)->SetTextureIndex(m_AutomataTextureID);
				m_HeroVector.at(index)->SetVolume(glm::vec3(0.8f, 1.6f, 0.4f));
				m_HeroVector.at(index)->SetType(TYPE_HERO);
				m_HeroVector.at(index)->SetState(STATE_GROUND);
				m_HeroVector.at(index)->SetEnableCollision(false);
				m_HeroVector.at(index)->SetUIBackGround(m_WhiteRectTextureID);
				m_HeroVector.at(index)->SetIsAnimate(true);
				m_HeroVector.at(index)->SetIsCullingFace(false);
				m_HeroVector.at(index)->SetModelWithBone("player");
				std::map<GameObjectAnimationState, std::string> heroAnimationDataMap;
				heroAnimationDataMap.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_IDLE, "player_idle"));
				heroAnimationDataMap.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_WALK, "player_walk"));
				heroAnimationDataMap.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_ATTACK, "player_attack"));
				heroAnimationDataMap.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_AVOID, "player_dash"));
				heroAnimationDataMap.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_HIT, "player_hit"));
				heroAnimationDataMap.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_IDLE_LONG, "player_idle_long"));
				heroAnimationDataMap.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_SKILL_1, "player_skill01"));
				heroAnimationDataMap.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_SKILL_2, "player_skill02"));
				heroAnimationDataMap.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_SKILL_3, "player_skill03"));
				heroAnimationDataMap.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_DEATH, "player_hit"));
				m_HeroVector.at(index)->SetAnimationDataNameMap(heroAnimationDataMap);
				m_HeroVector.at(index)->SetAnimationState(ANI_STATE_IDLE);

				TurnBattle::SStatus status;
				status.level = automata->status.level;
				status.maxHp = automata->status.maxHp;
				status.maxSp = automata->status.maxSp;
				status.maxTurn = automata->status.maxTurn;
				memcpy(status.attribute, automata->status.attribute, sizeof(status.attribute));
				memcpy(status.stat, automata->status.stat, sizeof(status.stat));

				TurnBattle::SBattleVariable battleVariable;
				battleVariable.hp = status.maxHp;
				battleVariable.sp = status.maxSp;
				battleVariable.turn = status.maxTurn;
				battleVariable.attackBuff = 0;
				battleVariable.defenseBuff = 0;
				battleVariable.exp = 0;

				m_HeroVector.at(index)->SetStatus(status);
				m_HeroVector.at(index)->SetBattleVariable(battleVariable);
				m_HeroVector.at(index)->SetName(automata->englishName);
				m_HeroVector.at(index)->SetWhetherToRender(false);
				m_HeroVector.at(index)->SetAttackAttribute(automata->attackAttribute);

				for (int i = 0; i < automata->skillSize; ++i) {
					m_HeroVector.at(index)->GiveSkill(automata->skillList[i]);
				}

				// 소환된 오토마타의 인덱스 저장(0번 1번 슬롯)
				m_sSummonedAutomata[index - 1] = automataIndex;

				// 소환했다는 정보 저장
				m_bIsAutomataSummoned = true;
				m_iSummonedAutomataLocation = index - 1;

				m_UsedSkillData.heroParty[index].exisited = true;
			}
		}
	}
}

void CTurnBattleState::ChangeSkillDeck()
{
	TurnBattle::SBattleVariable bv = m_HeroVector.at(m_iOrder)->GetBattleVariable();
	if (bv.turn > 0 && m_HeroVector.at(m_iOrder)->GetDeckVector().size() != 0) {
		m_bIsSkillDeckChanged = true;

		// 선택된 스킬 덱으로 변경
		TurnBattle::SDeck* deck 
			= m_DeckMap.find(m_HeroVector.at(m_iOrder)->GetDeckVector().at(m_iSelectedSkillDeck))->second;
		m_HeroVector.at(0)->SetAttributeAndSkillWithDeck(deck);
		bv.turn--;
		m_HeroVector.at(m_iOrder)->SetBattleVariable(bv);

		// 스킬 선택 초기화
		m_iSelectedSkill = 0;
		m_iShowedSkillNum = 0;
	}
}

void CTurnBattleState::UseNormalAttack(bool matter, bool additional)
{
	m_bIsMatterAttack = matter;
	TurnBattle::SBattleVariable heroBattleVariable = m_HeroVector.at(m_iOrder)->GetBattleVariable();

	TurnBattle::SSkill skill;
	skill.turn = 1;
	if (m_bIsMatterAttack) skill.sp = 0;
	else skill.sp = 1;

	if (m_HeroVector.at(m_iOrder)->CheckEnableSkill(&skill)) {
		heroBattleVariable.sp -= skill.sp;
		heroBattleVariable.turn -= skill.turn;

		m_HeroVector.at(m_iOrder)->SetBattleVariable(heroBattleVariable);
		m_bIsNormalAttackUsed = true;

		if (additional)
			m_bIsAdditionalNormalAttackUsed = true;
	}
}

void CTurnBattleState::WaitAction(bool rest)
{
	TurnBattle::SBattleVariable bv = m_HeroVector.at(m_iOrder)->GetBattleVariable();
	m_bIsWaited = true;
	m_bIsRested = rest;

	if (rest) {
		// 약간 회복 (1턴당 1%)
		TurnBattle::SStatus stat = m_HeroVector.at(m_iOrder)->GetStatus();
		short recoverHp = (short)((float)stat.maxHp * ((float)bv.turn / 100.f));
		short recoverSp = (short)((float)stat.maxSp * ((float)bv.turn / 100.f));

		bv.hp += recoverHp;
		bv.sp += recoverSp;

		bv.hp = glm::clamp(bv.hp, (short)0, stat.maxHp);
		bv.sp = glm::clamp(bv.sp, (short)0, stat.maxSp);
	}
	else {
		// 회피 레벨 증가
		bv.avoidLevel = bv.turn;
	}

	// 턴 종료(턴을 0로 변경)은 행동트리에서
	m_HeroVector.at(m_iOrder)->SetBattleVariable(bv);
}

void CTurnBattleState::StealAction(bool automata)
{
	TurnBattle::SStatus orderStatus = m_HeroVector.at(m_iOrder)->GetStatus();
	TurnBattle::SStatus targetStatus = m_EnemyMonsterVector.at(m_iTarget)->GetStatus();

	if (orderStatus.level >= targetStatus.level) {
		m_bIsStolen = true;
		m_bIsStolenAutomata = automata;
	}
}

bool CTurnBattleState::RemoveDeadObject()
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
								// 게임 클리어 설정
								m_bIsGameclear = true;
								m_HeroVector.at(0)->InitBattleVariable();

								return true;
							}
						}
						else {
							TurnBattle::SBattleVariable vb = m_EnemyMonsterVector.at(i)->GetBattleVariable();
							vb.hp += 1250;
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
					}
				}
				// 주인공에게 경험치 추가
				HeroLevelUp(m_EnemyMonsterVector.at(i)->GetStatus().level);

				auto it = m_MonsterAnalyzeEndDataMap.find(m_EnemyMonsterVector.at(i)->GetMonsterId());
				if (it != m_MonsterAnalyzeEndDataMap.end()) {
					it->second = true;
				}

				// 사용된 스킬 데이터, 위치 변경
				for (int t = i; t < size - 1; ++t) {
					if (m_iAreaNumber != 1) {
						float height = m_EnemyMonsterVector.at(t + 1)->GetVolume().y;
						m_EnemyMonsterVector.at(t + 1)->SetPosition(
							glm::vec3(-15.0f + 6.0f * t, -4.0f + height, -10.0f));
					}
					m_UsedSkillData.enemyParty[t].exisited = m_UsedSkillData.enemyParty[t + 1].exisited;
					m_UsedSkillData.enemyParty[t + 1].exisited = false;
				}
				// 타겟 중이였는데 맨 끝이였을 경우 타겟 변경
				if (i == m_iTarget) {
					if (i == size - 1) {
						m_iTarget = size - 1 - 1;
						if (m_iTarget < 0) {
							m_iTarget = 0;
						}
					}
				}
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

	if (m_bHeroTeam) {
		if (m_iTarget >= m_EnemyMonsterVector.size()) {
			m_iTarget = (int)m_EnemyMonsterVector.size() - 1;
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
				// 사용된 스킬 데이터 변경
				for (int t = i; t < size - 1; ++t) {
					float height = m_HeroVector.at(t + 1)->GetVolume().y;
					m_HeroVector.at(t + 1)->SetPosition(glm::vec3(-8.0f + 5.0f * t, -4.0f + height, 10.0f));
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
					if (i == size - 1) {
						m_iTarget--;
					}
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

	if (!m_bHeroTeam) {
		if (m_iTarget >= m_HeroVector.size()) {
			m_iTarget = (int)m_HeroVector.size() - 1;
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

void CTurnBattleState::SetMonsterData(short id, CTurnBattleObject* object)
{
	TurnBattle::SMonster* monsterData = m_MonsterMap.find(id)->second;
	CTurnBattleObject* monster = object;

	// 모델 텍스쳐 설정
	monster->SetModelWithBone(m_MonsterModelMap.find(monsterData->modelId)->second);
	monster->SetAnimationDataNameMap(m_MonsterAnimationsMap.find(monsterData->modelId)->second);
	monster->SetTextureIndex(m_MonsterTextureMap.find(monsterData->textureId)->second);
	monster->SetBloodTextureID(m_MonsterBloodTextureMap.find(monsterData->textureId)->second);
	monster->SetDefaultSoundID(m_MonsterDefaultSoundMap.find(monsterData->textureId)->second);
	monster->SetHitSoundID(m_MonsterDefaultSoundMap.find(HIT_SOUND_START_INDEX_IN_DEFAULT_SOUND_MAP + monsterData->textureId)->second);

	// 애니메이션 설정
	monster->SetIsAnimate(true);
	monster->SetAnimationState(ANI_STATE_IDLE);

	// 몬스터 스테이터스 설정
	monster->SetName(monsterData->name);
	monster->SetStatus(monsterData->status);
	monster->SetIndexBingStolen(monsterData->automata, monsterData->deck);
	monster->RemoveAllSkill();
	for (int i = 0; i < monsterData->skillSize; ++i) {
		monster->GiveSkill(monsterData->skillList[i]);
	}

	monster->SetAttackAttribute(monsterData->attackAttribute);
	monster->SetMonsterId(id);
	monster->SetIsBoss(true);
	// 전투변수 초기화
	monster->InitBattleVariable();
}

void CTurnBattleState::HeroLevelUp(short exp)
{
	TurnBattle::SBattleVariable bv = m_HeroVector.at(0)->GetBattleVariable();
	TurnBattle::SStatus status = m_HeroVector.at(0)->GetStatus();

	bv.exp += exp;
	if (bv.exp >= status.level * NEED_TO_LEVEL_UP_MONSTER_AMOUNT) {
		bv.exp -= status.level * NEED_TO_LEVEL_UP_MONSTER_AMOUNT;

		std::vector<short> stat1 = { MATTER_ATTACK, MIND_ATTACK, MATTER_DEFENSE, MIND_DEFENSE };
		std::vector<short> stat2 = { ACCURACY, AVOIDABILITY };

		std::random_device rd;
		std::default_random_engine dre(rd());

		// 스텟 증가
		std::shuffle(stat1.begin(), stat1.end(), dre);
		std::shuffle(stat2.begin(), stat2.end(), dre);
		int leftStat = 2;
		for (int i = 0; i < stat1.size(); ++i) {
			if (status.stat[stat1[i]] < 99) {
				status.stat[stat1[i]]++;
				leftStat--;
				if (leftStat == 0) {
					break;
				}
			}
		}
		leftStat = 1;
		for (int i = 0; i < stat2.size(); ++i) {
			if (status.stat[stat2[i]] < 99) {
				status.stat[stat2[i]]++;
				leftStat--;
				if (leftStat == 0) {
					break;
				}
			}
		}

		// hp, sp 증가
		std::uniform_int_distribution<short> uid(0, 20);
		short additionalHp = uid(dre);

		short leftHp = MAX_HP_SP - (status.maxHp + additionalHp);

		if ((leftHp) < 0) {
			leftHp *=- 1;

			additionalHp -= leftHp;
			status.maxHp = MAX_HP_SP;
		}
		else {
			status.maxHp += additionalHp;
		}
		status.maxSp += (UP_HP_SP - additionalHp);

		status.maxHp = glm::clamp(status.maxHp, (short)1, (short)MAX_HP_SP);
		status.maxSp = glm::clamp(status.maxSp, (short)1, (short)MAX_HP_SP);

		status.level++;
		if (status.level == 10 || status.level == 30 || status.level == 50 || status.level == 70) {
			status.maxTurn++;
		}

		// 레벨 업 시 절반까지는 회복
		if (bv.hp < status.maxHp / 2) {
			bv.hp = status.maxHp / 2;
		}
		if (bv.sp < status.maxSp / 2) {
			bv.sp = status.maxSp / 2;
		}
	}

	m_HeroVector.at(0)->SetBattleVariable(bv);
	m_HeroVector.at(0)->SetStatus(status);
}

SBattleMapData CTurnBattleState::GetBattleFieldData(int number)
{
	return m_BattleFieldDataMap.find(number)->second;
}

GLuint CTurnBattleState::GetGrassSSBOModelMatrices() const
{
	return m_GrassSSBOModelMatrices;
}

bool CTurnBattleState::IsShowingLogEnd()
{
	return m_fShowLogTime <= 0.0f;
}

void CTurnBattleState::UpdateDirectionalLightToTime(float elapsedTimeInSec)
{
	// 시간을 필드보다 1.5배 느리게
	elapsedTimeInSec /= 1.5f;

	// 잔디 시간 계산
	m_fTimeForGrass += (elapsedTimeInSec * DEFAULT_WIND_SPEED);

	m_pTimeOfDaySkybox->Update(elapsedTimeInSec, DirectionalLight());

	if (m_pTimeOfDaySkybox->IsPassedHalfDay()) {
		if (m_pTimeOfDaySkybox->IsDayTime()) {
			for (auto light : m_vLampLightVector) {
				light->SetWhetherToRender(false);
			}
			PointLight()->SetIsPointLightsOn(false);
		}
		else {
			for (auto light : m_vLampLightVector) {
				light->SetWhetherToRender(true);
			}
			PointLight()->SetIsPointLightsOn(true);
		}
	}
}

void CTurnBattleState::SetDirectionalLightToTime()
{
	m_pTimeOfDaySkybox->Initialize(DirectionalLight());

	if (m_pTimeOfDaySkybox->IsDayTime()) {
		for (auto light : m_vLampLightVector) {
			light->SetWhetherToRender(false);
		}
		PointLight()->SetIsPointLightsOn(false);
	}
	else {
		for (auto light : m_vLampLightVector) {
			light->SetWhetherToRender(true);
		}
		PointLight()->SetIsPointLightsOn(true);
	}
}
