#include "stdafx.h"
#include "HybridBattleState.h"
#include "DamageZone.h"
#include "FieldMonster.h"
#include "BossMonster.h"
#include "GameOverState.h"
#include <random>

CHybridBattleState::CHybridBattleState()
{
	// CGameFramework���� CGameState�� ����ϱ� ������ CGameState�� �ڽ� Ŭ������ �� Ŭ�������� CGameFramework�� ������ ��!
	m_pGameFramework = CGameFramework::GetInstance();

	m_pSpotLight = new CSpotLight(glm::vec3(0.0f, 0.f, 0.f), glm::vec3(0.f, 0.f, -1.f), glm::vec4(1.f, 1.f, 1.f, 1.f));

	ChangeObjectManager(new CHybridBattleObjectManager());
	ObjectManager()->SetIsEnabledCollionWithEnemies(true);

	// ���콺 Ŀ�� �����
	glutSetCursor(GLUT_CURSOR_NONE);

	Camera()->TestCameraSetting();

	glm::vec3 pos = glm::vec3(100.0f, 0.0f, 0.0f);
	glm::vec3 dir = glm::normalize(pos);

	LoadDirectionalLight(new CDirectionalLight(pos,
		glm::vec3(-dir.x, -dir.y, -dir.z),
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));
	DirectionalLight()->SetSkyLightColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	LoadPointLight(new CPointLight(glm::vec3(0.f, 3.5f, 0.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));

	LoadData();
	AddObjectToManager();
}

CHybridBattleState::~CHybridBattleState()
{
	delete m_pSpotLight;
	delete m_pTurnBattleState;
	delete m_pHeroObject;

	m_fieldBGMID = SoundPlayer()->DeleteSoundAndSetEmptyID(m_fieldBGMID);
}

void CHybridBattleState::ProcessInputs(float elapsedTimeInSec, GameInputs* inputs)
{
	if (m_bIsBattleStartWait) {
		return;
	}

	// �ʵ�� ���� ����	
	if (inputs->IsPressedDownKeyOnce(&inputs->KeyTab, &inputs->processingEnd.KeyTab)) {
		SetIsAlreadyStartedAndStillPlaying(false);
		m_pGameFramework->PopState(true);
	}

	// ���ΰ�, ������ ������Ʈ �Ķ���� �ʱ�ȭ
	GameUpdateParameters heroParam;
	GameUpdateParameters othersParam;

	memset(&heroParam, 0, sizeof(GameUpdateParameters));
	memset(&othersParam, 0, sizeof(GameUpdateParameters));

	if (inputs->IsPressedDownKeyOnce(&inputs->KeyC, &inputs->processingEnd.KeyC))
	{
		m_bTestCameraOn = (m_bTestCameraOn + 1) % 2;

		if (m_bTestCameraOn) {
			Camera()->SetTestCameraPosition(BackBoard()->GetHeroPosition() + glm::vec3(0.0f, 2.0f, 0.0f));
		}
	}

	if (m_bTestCameraOn)
	{
		Camera()->TestCameraControl(elapsedTimeInSec, inputs, m_bMouseFixed);
	}
	else
	{
		// ī�޶� �� ��, �� �ƿ�
		if (inputs->MouseWheelUp)
		{
			m_pHeroObject->SetScrollWheel(Camera(), elapsedTimeInSec, true);
			inputs->MouseWheelUp = false;
		}
		else if (inputs->MouseWheelDown)
		{
			m_pHeroObject->SetScrollWheel(Camera(), elapsedTimeInSec, false);
			inputs->MouseWheelDown = false;
		}

		// ���ΰ�, ��ü ���� ������ ���� �ʱ�ȭ
		InitVisiblity();
		m_bIsRenderParticle = true;

		if (m_bIsTurnMode) {
			// �޴� �� ī�޶� ����
			int menu = m_pTurnBattleState->GetMenu();
			if (menu == WAIT_MENU || menu == CHANGE_DECK_MENU) {
				m_pHeroObject->SetFrontViewCameraInTurnMode(Camera(), m_fTargetHorizontalAngle);

				// ���� ���������� ����
				SetVisiblity(false, false);
			}
			else if (menu == STEAL_MENU) {	
				SetTargetWithMouse(elapsedTimeInSec, inputs);
				m_pHeroObject->ControlLookAtCameraInTurnMode(Camera(),
					elapsedTimeInSec, inputs, m_fTargetHorizontalAngle,
					m_MonsterVector.at(m_pTurnBattleState->GetTarget())->GetPosition());

				// ���ΰ� �������� ���� ����
				SetVisiblity(true, false);
			}
			else {
				SetTargetWithMouse(elapsedTimeInSec, inputs);
				m_pHeroObject->ControlBackViewCameraInTurnMode(Camera(),
					elapsedTimeInSec, inputs, m_fTargetHorizontalAngle);
			}

			m_pTurnBattleState->TurnModeProcessInputs(elapsedTimeInSec, inputs);
 			return;
		}
		else {
			if (m_bIsEnemyUseSkill) {
				// ��ƼŬ �������� ���� ����
				m_bIsRenderParticle = false;

				// �ڽ��� ������ ���� ���������� ����
				SetVisiblity(false, false);
				if (!m_MonsterVector.at(m_pTurnBattleState->GetHybridBattleMonsterOrder())
					->GetTurnBattleObject()->GetIsSubObject()) {
					m_MonsterVector.at(m_pTurnBattleState->GetHybridBattleMonsterOrder())->SetWhetherToRender(true);
				}
				m_MonsterVector.at(m_pTurnBattleState->GetHybridBattleMonsterOrder())->UseSkillCameraSetting(Camera());
				m_pTurnBattleState->Camera()->SetCameraPosition(Camera()->GetCameraPosition());
			}
			else {
				m_pHeroObject->ControlBackViewCamera(Camera(), elapsedTimeInSec, inputs, m_bMouseFixed);


				if (inputs->IsPressedDownKeyOnce(&inputs->KeyQ, &inputs->processingEnd.KeyQ)) {
					m_pTurnBattleState->ChangeOrder();
					m_pHeroObject->
						SetTurnBattleObject(m_pTurnBattleState->GetTurnBattleObject(true, m_pTurnBattleState->GetOrder()));
				}

				if (inputs->IsPressedDownKeyOnce(&inputs->MouseRightButton, &inputs->processingEnd.MouseRightButton)) {
					if (m_pHeroObject->GetTurnBattleObject()->GetBattleVariable().condition != ANXIETY_CONDITION) {
						m_bIsTurnMode = (m_bIsTurnMode + 1) % 2;
					}
				}
				if (inputs->IsPressedDownKeyOnce(&inputs->KeyT, &inputs->processingEnd.KeyT)) {
					m_bIsTurnMode = (m_bIsTurnMode + 1) % 2;
				}
				m_pHeroObject->ProcessInputs(elapsedTimeInSec, inputs);
			}
		}
		
	}

	// ������ �¿���
	if (inputs->IsPressedDownKeyOnce(&inputs->KeyF, &inputs->processingEnd.KeyF)) {
		m_bFlashLightOn = (m_bFlashLightOn + 1) % 2;

		SoundPlayer()->PlaySound(m_switchSoundID, 1.0f);
	}

	// ���콺 ����
	if (inputs->IsPressedDownKeyOnce(&inputs->KeyESC, &inputs->processingEnd.KeyESC)) {
		if (m_bMouseFixed)
		{
			glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
			m_bMouseFixed = false;

			// ���� ������ ��ġ�� �̵�
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

	// �浹 �ڽ� ǥ��
	if (inputs->IsPressedDownKeyOnce(&inputs->KeyB, &inputs->processingEnd.KeyB)) {
		m_bShowBoundingBox = (m_bShowBoundingBox + 1) % 2;
	}

	// ������Ʈ �Ű� ������ ������Ʈ �Ŵ����� �ѱ�
	ObjectManager()->SetUpdateParameters(heroParam, othersParam);
}

void CHybridBattleState::Update(float elapsedTimeInSec)
{
	// �߸��� ��ġ�� ���ΰ��� ���� ��� ��ġ �ʱ�ȭ
	glm::vec3 heroPos = m_pHeroObject->GetPosition();
	if ((-1000.0f > heroPos.x) || (heroPos.x > 1000.0f)
		|| (-1000.0f > heroPos.y) || (heroPos.y > 1000.0f)
		|| (-1000.0f > heroPos.z) || (heroPos.z > 1000.0f)) {
		InitLocationForBugFix();
	}

	int x, y;
	Renderer()->GetDisplayResolution(&x, &y);
	if (m_bMouseFixed)
		glutWarpPointer(x / 2, y / 2);

	// ���� ���
	if (m_bIsBattleStartWait) {
		if (m_fStartWaitTime > DEFUALT_STATE_CHANGE_WAIT_TIME) {
			m_bIsBattleStartWait = false;
		}
		m_fStartWaitTime += elapsedTimeInSec;
	}

	if (m_pTurnBattleState->IsShowingLogEnd()) {
		if (m_pTurnBattleState->GetIsRunawaySuccess()) {
			// ����(�� ���� ����)
			m_pTurnBattleState->SetIsAlreadyStartedAndStillPlaying(false);
			SetIsAlreadyStartedAndStillPlaying(false);
			m_pGameFramework->PopState(true);
			return;
		}
	}


	// ���� �� ��Ʋ ������Ʈ ���� �� ��� �׾��� ��� ������Ʈ ����
	if (!m_pTurnBattleState->GetIsProcessing()) {
		if (m_pTurnBattleState->TurnModeRemoveDeadObject()) {
			SetIsAlreadyStartedAndStillPlaying(false);
			m_pTurnBattleState->SetIsAlreadyStartedAndStillPlaying(false);
			if (m_pTurnBattleState->GetIsGameover()) {
				m_pTurnBattleState->SetIsGameover(false);
				m_pGameFramework->PushState(m_pTurnBattleState->GetGameOverState());
			}
			else if (m_pTurnBattleState->GetIsGameclear()) {
				m_pTurnBattleState->SetIsGameclear(false);
				m_pGameFramework->PushState(m_pTurnBattleState->GetGameClearState());
			}
			else {
				m_pGameFramework->PopState(true);
			}
			return;
		}
	}
	m_pHeroObject->SetTurnBattleObject(m_pTurnBattleState->GetTurnBattleObject(true, m_pTurnBattleState->GetOrder()));

	// �� ��Ʋ ������Ʈ�� �׾� ���� ���� ������Ʈ ����
	bool deadMonster = true;
	while (deadMonster) {
		deadMonster = false;
		int size = (int)m_MonsterVector.size();

		for (int i = 0; i < size; ++i) {
			if (m_MonsterVector.at(i)->GetIsDead()) {
				// ����
				m_MonsterVector.at(i)->SetLifeTime(0.0f);
				m_MonsterVector.erase(m_MonsterVector.begin() + i);
				m_MonsterVector.resize(size - 1);
				deadMonster = true;
				break;
			}
		}
	}

	// ��ų ��������Ʈ ������Ʈ 
	m_pTurnBattleState->UpdateUsedSkillData(elapsedTimeInSec);

	// ���� �ܰ��� ȿ��(���� ȿ��) ��ü ����
	for (auto monster : m_MonsterVector) {
		monster->SetBorder(false);
	}

	if (m_bIsTurnMode) {
		m_pTurnBattleState->TurnModeUpdate(elapsedTimeInSec);
		m_bIsTurnMode = m_pTurnBattleState->GetIsTurnModeOpened();

		if (m_bIsTurnMode) {
			if (!m_bInitHeroAnimationAfterEndTurnMode) {
				m_bInitHeroAnimationAfterEndTurnMode = true;
			}

			m_pHeroObject->MatchAnimationWithTurnBattleObject();
			m_pHeroObject->CalculateVerticesAfterAnimation(Renderer());

			m_bInitHeroAnimationAfterEndTurnMode = true;
			m_MonsterVector.at(m_pTurnBattleState->GetTarget())->SetBorder(true);

			ProcessingSummonAutomata(elapsedTimeInSec);
		}
		return;
	}
	else {
		if (m_bInitHeroAnimationAfterEndTurnMode) {
			// �� ��尡 ����� �� �ִϸ��̼� �ʱ�ȭ
			m_pHeroObject->SetAnimationState(ANI_STATE_IDLE);
			m_pHeroObject->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
			m_bInitHeroAnimationAfterEndTurnMode = false;
		}

		m_pTurnBattleState->SetIsTurnModeOpened(true);
		m_pTurnBattleState->SetEnemyTurnStart();

		m_bIsEnemyUseSkill = m_pTurnBattleState->GetIsProcessingInHybridBattleForEnemy();
		if (m_bIsEnemyUseSkill) {
			if (!m_bInitEnemyAnimationAfterEndTurnMode) {
				m_bInitEnemyAnimationAfterEndTurnMode = true;
				for (auto monster : m_MonsterVector) {
					monster->BackupAnimation();
				}
			}
			m_pTurnBattleState->RunBTForEnemyInHybridBattle();
			
			for (auto monster : m_MonsterVector) {
				monster->MatchAnimationWithTurnBattleObject();
				monster->CalculateVerticesAfterAnimation(Renderer());
			}
		}
		else {
			if (m_bInitEnemyAnimationAfterEndTurnMode) {
				m_bInitEnemyAnimationAfterEndTurnMode = false;
				for (auto monster : m_MonsterVector) {
					monster->RestoreAnimation();
				}
			}
		}

		// ���� �̻� ó��
		TurnBattle::SBattleVariable bv = m_pHeroObject->GetTurnBattleObject()->GetBattleVariable();
		if (bv.condition != NORMAL_CONDITION) {
			bv.conditionDuration -= elapsedTimeInSec;
			if (bv.conditionDuration <= 0) {
				bv.condition = NORMAL_CONDITION;
			}
			m_pHeroObject->GetTurnBattleObject()->SetBattleVariable(bv);
		}
	}

	// �� �������� ���� �� ������Ʈ
	m_pTurnBattleState->TurnModeGettingTurnUpdate();
	
	// �������� ���� ���� ��� ���׹̳� ȸ�� �ӵ��� ������ �ǵ��� ����
	if (m_bFlashLightOn) {
		m_pHeroObject->UseStemina(elapsedTimeInSec * (DEFAULT_STAMINA_RECOVER_SPEED * 0.5f));

		glm::vec3 pos, dir;
		Camera()->GetCameraPositionAndDirection(&pos, &dir);
		// ����Ʈ����Ʈ ��ġ ���� ����
		m_pSpotLight->SetPosition(pos);
		m_pSpotLight->SetDirection(dir);
	}

	m_pHeroObject->SpectrumUpdater(elapsedTimeInSec, ObjectManager());

	UpdateDirectionalLightToTime(elapsedTimeInSec);

	glm::vec3 heroPosition = m_pHeroObject->GetPosition();
	BackBoard()->SetHeroPosition(heroPosition);

	// ���� ��� ���� ��� ������Ʈ ������Ʈ ���� ����
	if (m_bIsBattleStartWait) {
		return;
	}

	if (m_bIsEnemyUseSkill) {
		return;
	}

	CGameState::Update(elapsedTimeInSec);
}

void CHybridBattleState::RenderScene()
{
	if (m_bIsBattleStartWait) {
		m_pGameFramework->GetPrevState()->Camera()->UseCamera();
		m_pGameFramework->GetPrevState()->SaveRenderingResultOnTexture();

		Camera()->UseCamera();

		DefualtRenderPipeline();

		glDisable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, Renderer()->GetDisplayFXAAFBO());
		glClear(GL_COLOR_BUFFER_BIT);
		Renderer()->RenderChangeStateEffect(m_fStartWaitTime, m_bIsStartedWithHeroWin);

		// ��Ƽ ������̵� ȭ�� ������
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		Renderer()->UseShader(DISPLAY_FXAA_SHADER);
		Renderer()->RenderDisplayFXAA();
	}
	else {
		Camera()->UseCamera();

		// ������ ���
		if (m_bFlashLightOn) {
			Renderer()->UseShader(DEFAULT_SHADER);
			m_pSpotLight->UseLight(DEFAULT_SHADER);
			Renderer()->UseShader(INSTANCE_SHADER);
			m_pSpotLight->UseLight(INSTANCE_SHADER);
			Renderer()->UseShader(WATER_SHADER);
			m_pSpotLight->UseLight(WATER_SHADER);
		}
		Renderer()->UseShader(DEFAULT_SHADER);
		GLuint lightOn = glGetUniformLocation(Renderer()->GetShaderProgram(DEFAULT_SHADER), "u_bSpotLightOn");
		glUniform1i(lightOn, m_bFlashLightOn);
		Renderer()->UseShader(INSTANCE_SHADER);
		lightOn = glGetUniformLocation(Renderer()->GetShaderProgram(INSTANCE_SHADER), "u_bSpotLightOn");
		glUniform1i(lightOn, m_bFlashLightOn);
		Renderer()->UseShader(WATER_SHADER);
		lightOn = glGetUniformLocation(Renderer()->GetShaderProgram(WATER_SHADER), "u_bSpotLightOn");
		glUniform1i(lightOn, m_bFlashLightOn);

		CGameState::RenderScene();
	}
}

void CHybridBattleState::InitLocationForBugFix()
{
	m_pHeroObject->SetPosition(glm::vec3(-6.0f + 4.0f, -2.0f, 5.0f));

	for (int i = 0; i < m_MonsterVector.size(); ++i) {
		m_MonsterVector.at(i)->SetPosition(glm::vec3(-10.0f + 4.0f * i, -1.0f, -5.0f));
	}
}

void CHybridBattleState::SetHeroObject(CHeroObject* heroObject, CGameObject* weaponObject)
{
	m_HeroIndex = ObjectManager()->AddObject<CHeroObject>(
		heroObject->GetPosition(),
		heroObject->GetRotation(),
		heroObject->GetSize(),
		heroObject->GetMass());
	m_pHeroObject = ObjectManager()->GetGameObject<CHeroObject>(m_HeroIndex);
	m_pHeroObject->SetVolume(heroObject->GetVolume());
	m_pHeroObject->SetModelIndex(heroObject->GetModelIndex());
	m_pHeroObject->SetTextureIndex(heroObject->GetTextureIndex());
	m_pHeroObject->SetIsCullingFace(false);
	m_pHeroObject->SetType(TYPE_HERO);
	m_pHeroObject->SetState(STATE_GROUND);
	m_pHeroObject->SetLimitVelocity(3.0f);
	m_pHeroObject->SetIsAnimate(true);
	m_pHeroObject->SetBloodTextureID(Renderer()->ReadPNGTextureFile("./resource/texture/blood.png"));
	m_pHeroObject->SetModelWithBone("player");
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
	m_pHeroObject->SetAnimationDataNameMap(heroAnimationDataMap);
	m_pHeroObject->SetAnimationState(ANI_STATE_IDLE);

	m_WeaponIndex = ObjectManager()->AddObject<CGameObject>(
		glm::vec3(0.5f, 0.f, -1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		5.0f);
	CGameObject* weapon = ObjectManager()->GetGameObject<CGameObject>(m_WeaponIndex);
	weapon->SetRadiusAndHeight(0.65f, 0.5f);
	weapon->SetType(TYPE_WEAPON);
	weapon->SetState(STATE_GROUND);
	weapon->SetParent(m_pHeroObject);
	weapon->StickToParent();
	weapon->SetEnableCollision(false);
	weapon->SetWhetherToRender(false);
	weapon->SetIsMadeShadow(false);
	m_pHeroObject->SetWeaponObject(weapon);

	int cameraCollisionIndex = ObjectManager()->AddObject<CGameObject>(
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		1.0f);
	CGameObject* cameraCollision = ObjectManager()->GetGameObject<CGameObject>(cameraCollisionIndex);
	cameraCollision->SetVolume(glm::vec3(1.0f, 1.0f, 1.0f));
	cameraCollision->SetWhetherToRender(false);
	cameraCollision->SetType(TYPE_CAMERA_COLLISION);
	cameraCollision->SetState(STATE_GROUND);
	cameraCollision->SetParent(m_pHeroObject);
	cameraCollision->SetEnableCollision(false);
	m_pHeroObject->SetCameraCollisionObject(cameraCollision);
}

void CHybridBattleState::SetTurnBattleState(CTurnBattleState* turnBattleState)
{
	m_pTurnBattleState = turnBattleState;
	m_pHeroObject->SetTurnBattleObject(m_pTurnBattleState->GetTurnBattleObject(true, 0));
	m_GrassSSBOModelMatrices = m_pTurnBattleState->GetGrassSSBOModelMatrices();
}

void CHybridBattleState::Enter()
{
	if (!GetIsAlreadyStartedAndStillPlaying()) {
		SetIsAlreadyStartedAndStillPlaying(true);
		m_pTurnBattleState->Enter();
		InitHybridBattle();
		SetDirectionalLightToTime();
		CGameState::Update(0.0f);
	}
}

void CHybridBattleState::Exit()
{
	if (!GetIsAlreadyStartedAndStillPlaying()) {
		m_pHeroObject->SetTurnBattleObject(m_pTurnBattleState->GetTurnBattleObject(true, 0));
		RemoveMonsterAndDamageZone();
		ObjectManager()->DeleteAllParticle();
		m_pTurnBattleState->SetIsAlreadyStartedAndStillPlaying(false);
		m_pTurnBattleState->Exit();
		
		if (m_iAreaNumber == 1) {
			m_pRainDropParticle->SetLifeTime(-1.0f);
		}
	}
}

void CHybridBattleState::Pause()
{
	if (!GetIsAlreadyStartedAndStillPlaying()) {
		m_pHeroObject->SetTurnBattleObject(m_pTurnBattleState->GetTurnBattleObject(true, 0));
		RemoveMonsterAndDamageZone();
		ObjectManager()->DeleteAllParticle();
		m_pTurnBattleState->SetIsAlreadyStartedAndStillPlaying(false);
		m_pTurnBattleState->Pause();

		if (m_iAreaNumber == 1) {
			m_pRainDropParticle->SetLifeTime(-1.0f);
		}
	}
}

void CHybridBattleState::Resume()
{
	if (!GetIsAlreadyStartedAndStillPlaying()) {
		SetIsAlreadyStartedAndStillPlaying(true);
		m_pTurnBattleState->Resume();
		InitHybridBattle();
		SetDirectionalLightToTime();
		CGameState::Update(0.0f);
	}
}

void CHybridBattleState::SetAreaNumber(int areaNumber)
{
	m_iAreaNumber = areaNumber;
}

void CHybridBattleState::SetTimeForGrass(float time)
{
	m_fTimeForGrass = time;
}

void CHybridBattleState::SetIsStartedWithHeroWin(bool win)
{
	m_bIsStartedWithHeroWin = win;
}

void CHybridBattleState::SetTimeOfDaySkybox(CTimeOfDaySkybox* timeOfDaySkybox)
{
	m_pTimeOfDaySkybox = timeOfDaySkybox;
}

float CHybridBattleState::GetFlashLightOn() const
{
	return m_bFlashLightOn;
}

void CHybridBattleState::SetFlashLightOn(bool on)
{
	m_bFlashLightOn = on;
}

CHeroObject* CHybridBattleState::GetHeroObject()
{
	return m_pHeroObject;
}

void CHybridBattleState::NormalRender()
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

	// �浹 �ڽ� ������
	if (m_bShowBoundingBox)
	{
		Renderer()->UseShader(BOUNDING_BOX_SHADER);
		ObjectManager()->BoundingBoxRender();
	}

	Renderer()->UseShader(DEFAULT_SHADER);
	ObjectManager()->Render();

	Renderer()->UseShader(GRAVITY_PARTICLE_SHADER);
	if (m_bIsRenderParticle)
		ObjectManager()->RenderParticle();

	m_pTurnBattleState->TurnModeRenderRenderBillboards(m_MonsterVector, m_pHeroObject->GetPosition());
}

void CHybridBattleState::UIRender()
{
	if (!m_bIsTurnMode) {
		int x, y;
		Renderer()->GetDisplayResolution(&x, &y);

		RenderHPSPBarUI(x, y);
		RenderNameAndTurnUI(x, y);
		RenderKeyInformationUI(x, y);
		m_pTurnBattleState->TurnModeRenderHPSPBarUI(x, y);

		if (m_bIsEnemyUseSkill) {
			m_pTurnBattleState->RenderActLogInHybridBattle();
		}
	}
	else {
		m_pTurnBattleState->TurnModeUIRender();
	}
}

void CHybridBattleState::LoadData()
{
	m_grassModelID = Renderer()->LoadOBJModelWithIndex("./resource/obj/world/grass.obj");
	m_grassTextureID = Renderer()->ReadPNGTextureFile("./resource/texture/world/grass_tex.jpg");

	m_fieldBGMID = SoundPlayer()->LoadSound("./resource/sound/BGM/cityBGM.mp3");

	m_switchSoundID = SoundPlayer()->LoadSound("./resource/sound/Effect/switchEffect.wav");

	m_waterModelID = Renderer()->LoadOBJModelWithIndex("./resource/obj/world/water.obj");

	m_dustTexureID = Renderer()->ReadPNGTextureFile("./resource/texture/fog.png");
	m_ballTexureID = Renderer()->ReadPNGTextureFile("./resource/texture/waterDrop.png");
	m_waterDropTexureID = Renderer()->ReadPNGTextureFile("./resource/texture/waterDrop.png");

	LoadUIData();
}

void CHybridBattleState::AddObjectToManager()
{

}

void CHybridBattleState::InitHybridBattle()
{
	m_pHeroObject->SetPosition(glm::vec3(-6.0f + 4.0f, -2.0f, 5.0f));
	m_pHeroObject->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
	m_pHeroObject->SetWeaponPosition();
	m_pHeroObject->InitStemina();
	m_pHeroObject->InitCameraCollisionObject();
	m_pHeroObject->SetCameraAngle(PI_VALUE, -PI_VALUE / 8.f);
	m_pHeroObject->CalculateBackViewCamera(Camera());
	m_pHeroObject->SetIsHeldByEnemy(false);

	MakeBattleField(m_iAreaNumber);

	if (m_iAreaNumber == 1) {
		AddMonsterToMonsterVectorBoss();

		// �� ��ƼŬ
		int rainDrop = ObjectManager()->
			AddParticle<CRainDropParticle>(glm::vec3(0.0f, -1.f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		m_pRainDropParticle = ObjectManager()->GetParticle<CRainDropParticle>(rainDrop);
		m_pRainDropParticle->SetTextureID(m_waterDropTexureID);
		m_pRainDropParticle->SetColor(glm::vec4(0.7f, 0.0f, 0.0f, 0.3f));
		m_pRainDropParticle->CreateVAO();
	}
	else {
		AddMonsterToMonsterVector();
	}

	m_bIsBattleStartWait = true;
	m_fStartWaitTime = 0.0f;

	m_bIsTurnMode = false;
	m_bIsEnemyUseSkill = false;
	m_bInitHeroAnimationAfterEndTurnMode = false;
	m_bInitEnemyAnimationAfterEndTurnMode = false;
}

void CHybridBattleState::AddMonsterToMonsterVector()
{
	glm::vec3 volume = glm::vec3(1.0f);
	float speed = 20.0f;
	if (m_iAreaNumber == 4) {
		volume = glm::vec3(1.1f, 0.8f, 1.1f);
	}
	else if (m_iAreaNumber == 3) {
		volume = glm::vec3(1.0f, 0.8f, 3.0f);
	}
	else if (m_iAreaNumber == 2) {
		volume = glm::vec3(2.0f, 2.0f, 1.0f);
		speed = 15.0f;
	}

	for (int i = 0; i < m_pTurnBattleState->GetPartySize(false); ++i) {
		int object = ObjectManager()->AddObject<CBattleMonster>(
			glm::vec3(-10.0f + 4.0f * i, -1, -5.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			100.0f);

		m_MonsterVector.push_back(ObjectManager()->GetGameObject<CBattleMonster>(object));
		m_MonsterVector.at(i)->SetVolume(volume);
		m_MonsterVector.at(i)->SetType(TYPE_ENEMY);
		m_MonsterVector.at(i)->SetState(STATE_GROUND);
		m_MonsterVector.at(i)->SetSpeed(speed);
		m_MonsterVector.at(i)->SetTurnBattleObject(m_pTurnBattleState->GetTurnBattleObject(false, i));
		m_MonsterVector.at(i)->GetTurnBattleObject()->SetParent(m_MonsterVector.at(i));
		m_MonsterVector.at(i)->InitWithTurnBattleObject();
		m_MonsterVector.at(i)->SetObjectManager(ObjectManager());
		m_MonsterVector.at(i)->SetDustTextureID(m_dustTexureID);
		m_MonsterVector.at(i)->SetBallTextureID(m_ballTexureID);
		m_MonsterVector.at(i)->SetHero(m_pHeroObject);
		if (m_iAreaNumber == 4) {
			m_MonsterVector.at(i)->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.75f));
		}
		if (m_iAreaNumber == 4) {
			short pattern[] = { 1, 3, 3, 1, 1, 3,  4, 4, 1 };
			m_MonsterVector.at(i)->SetAttackPatternList(pattern);
		}
		else if (m_iAreaNumber == 3) {
			short pattern[] = { 3, 3, 3, 5, 5, 2, 4, 4, 2 };
			m_MonsterVector.at(i)->SetAttackPatternList(pattern);
			m_MonsterVector.at(i)->SetSize(glm::vec3(0.8f, 0.8f, 0.8f));
		}
		else if (m_iAreaNumber == 2) {
			short pattern[] = { 1, 3, 3, 2, 1, 1, 4, 4, 2 };
			m_MonsterVector.at(i)->SetAttackPatternList(pattern);
		}
	}

	// ��ü �� �� ��Ʋ ������Ʈ �� 0���� ����
	m_pTurnBattleState->SetEnemyTurnZeroInHyBridBattleStart();
}

void CHybridBattleState::AddMonsterToMonsterVectorBoss()
{
	for (int i = 0; i < m_pTurnBattleState->GetPartySize(false); ++i) {
		int object;

		if (i == 1) {
			object = ObjectManager()->AddObject<CBossMonster>(
				glm::vec3(-10.0f + 4.0f * i, -1, -5.0f),
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(1.0f, 1.0f, 1.0f),
				100.0f);
		}
		else {
			object = ObjectManager()->AddObject<CBossHead>(
				glm::vec3(-10.0f + 4.0f * i, -1, -5.0f),
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(1.0f, 1.0f, 1.0f),
				100.0f);
			if (i == 0) {
				ObjectManager()->GetGameObject<CBossHead>(object)->SetIsRightHead(true);
			}
		}

		m_MonsterVector.push_back(ObjectManager()->GetGameObject<CBattleMonster>(object));
		m_MonsterVector.at(i)->SetVolume(glm::vec3(1.25f, 2.0f, 4.2f));
		m_MonsterVector.at(i)->SetType(TYPE_BOSS);
		m_MonsterVector.at(i)->SetState(STATE_GROUND);
		m_MonsterVector.at(i)->SetTurnBattleObject(m_pTurnBattleState->GetTurnBattleObject(false, i));
		m_MonsterVector.at(i)->GetTurnBattleObject()->SetParent(m_MonsterVector.at(i));
		m_MonsterVector.at(i)->InitWithTurnBattleObject();
		m_MonsterVector.at(i)->SetObjectManager(ObjectManager());
		m_MonsterVector.at(i)->SetDustTextureID(m_dustTexureID);
		m_MonsterVector.at(i)->SetBallTextureID(m_ballTexureID);
		m_MonsterVector.at(i)->SetHero(m_pHeroObject);
		m_MonsterVector.at(i)->SetLimitVelocity(10.0f);

		if (i != 1) {
			m_MonsterVector.at(i)->SetVolume(glm::vec3(1.0f, 1.0f, 2.0f));
			m_MonsterVector.at(i)->SetWhetherToRender(false);
			m_MonsterVector.at(i)->SetIsMadeShadow(false);
			m_MonsterVector.at(i)->SetType(TYPE_BOSS_HEAD);

			m_MonsterVector.at(i)->GetTurnBattleObject()->InitBattleVariable();
		}

		short pattern[] = { 3, 3, 2, 2, 1, 1, 1, 4, 4 };
		m_MonsterVector.at(i)->SetAttackPatternList(pattern);
	}

	dynamic_cast<CBossMonster*>(m_MonsterVector.at(1))->SetHead(
		dynamic_cast<CBossHead*>(m_MonsterVector.at(2)),
		dynamic_cast<CBossHead*>(m_MonsterVector.at(0)));

	// ��ü �� �� ��Ʋ ������Ʈ �� 0���� ����
	m_pTurnBattleState->SetEnemyTurnZeroInHyBridBattleStart();
}

void CHybridBattleState::RemoveMonsterAndDamageZone()
{
	for (auto enemy : m_MonsterVector) {
		enemy->SetLifeTime(0.0f);
	}
	m_MonsterVector.clear();

	ObjectManager()->DeleteAllObjectWithType(TYPE_DAMAGE_ZONE);
}

void CHybridBattleState::MakeBattleField(int areaNumber)
{
	// ���� ���� �ʵ� ���� ������Ʈ ����
	ObjectManager()->DeleteAllObjectWithType(TYPE_FIXED);
	ObjectManager()->DeleteAllNoInteractionObjectWithType(TYPE_FIXED);
	ObjectManager()->DeleteAllNoInteractionObjectWithType(TYPE_FLOOR);
	m_vLampLightVector.clear();
	m_vLampLightVector.resize(0);

	SBattleMapData data = m_pTurnBattleState->GetBattleFieldData(areaNumber);

	int floor = ObjectManager()->AddNoInteractionObject<CGameObject>(
		glm::vec3(0.f, -5.4f, 0.f),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(1.f, 1.f, 1.f),
		10000);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(floor)->SetModelIndex(data.fieldModel);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(floor)->SetTextureIndex(data.fieldTexture);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(floor)->SetType(TYPE_FLOOR);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(floor)->SetIsCullingFace(false);

	// ��
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

	// ���ε�
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

void CHybridBattleState::UpdateDirectionalLightToTime(float elapsedTimeInSec)
{
	// �ð��� �ʵ庸�� 1.5�� ������
	elapsedTimeInSec /= 1.5f;

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

void CHybridBattleState::SetDirectionalLightToTime()
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

void CHybridBattleState::SetTargetWithMouse(float elapsedTimeInSec, GameInputs* inputs)
{
	int x, y;
	CRenderer::GetInstance()->GetDisplayResolution(&x, &y);

	float tempAngle = m_fTurnModeCameraHorizontalAngle;

	m_fTurnModeCameraHorizontalAngle +=
		0.35f * elapsedTimeInSec * float(x / 2 - inputs->MouseXPos);
	
	float changeSpeed = 5.0f;

	int targetSize = (int)m_MonsterVector.size();

	if (inputs->IsPressedDownKeyOnce(&inputs->KeyA, &inputs->processingEnd.KeyA)) {
		m_fTurnModeCameraHorizontalAngle += 2.0f * PI_VALUE / (float)targetSize;
	}
	else {
		if (inputs->KeyA) {
			m_fTurnModeCameraHorizontalAngle += elapsedTimeInSec * changeSpeed;
		}
	}
	
	if (inputs->IsPressedDownKeyOnce(&inputs->KeyD, &inputs->processingEnd.KeyD)) {
		m_fTurnModeCameraHorizontalAngle -= 2.0f * PI_VALUE / (float)targetSize;
	}
	else {
		if (inputs->KeyD) {
			m_fTurnModeCameraHorizontalAngle -= elapsedTimeInSec * changeSpeed;
		}
	}

	// ��� �� �������� ȸ���� �ݴ������� ���� �ʵ��� 
	if (m_fTurnModeCameraHorizontalAngle < 0) {
		m_fTurnModeCameraHorizontalAngle = tempAngle;
	}
	else if (m_fTurnModeCameraHorizontalAngle > 2.0f * PI_VALUE) {
		m_fTurnModeCameraHorizontalAngle = tempAngle;
	}

	float* targetAngle = new float[targetSize];

	for (int i = 0; i < targetSize; ++i) {
		CBattleMonster* object = m_MonsterVector.at(i);
		glm::vec3 targetPos = object->GetPosition();
		glm::vec3 heroPos = m_pHeroObject->GetPosition();

		glm::vec3 dir = targetPos - heroPos;
		dir = glm::normalize(glm::vec3(dir.x, 0.0f, dir.z));

		targetAngle[i] = glm::atan(dir.z, -dir.x) - PI_VALUE / 2.0f;
		targetAngle[i] = glm::mod(targetAngle[i], 2.0f * PI_VALUE);
	}

	std::map<float, int> data;
	for (int i = 0; i < targetSize; ++i) {
		data.insert(std::pair<float, int>(targetAngle[i], i));
	}
	
	int index = (int)(m_fTurnModeCameraHorizontalAngle / (2.0 * PI_VALUE / (float)targetSize));
	index = glm::clamp(index, 0, targetSize - 1);

	int i = 0;
	float finalAngle = 0;
	int finalIndex = 0;
	for (auto it = data.begin(); it != data.end(); ++it) {
		if (i == index) {
			finalAngle = it->first;
			finalIndex = it->second;
			break;
		}
		i++;
	}

	m_fTargetHorizontalAngle = finalAngle;
	m_pTurnBattleState->SetTarget(finalIndex);

	delete[] targetAngle;
}

void CHybridBattleState::InitVisiblity()
{
	m_pHeroObject->SetWhetherToRender(true);

	if (m_iAreaNumber != 1) {
		for (auto monster : m_MonsterVector) {
			monster->SetWhetherToRender(true);
		}
	}
	else {
		for (auto monster : m_MonsterVector) {
			if (monster->GetTurnBattleObject()->GetIsBoss())
				monster->SetWhetherToRender(true);
		}
	}
}

void CHybridBattleState::SetVisiblity(bool hero, bool visible)
{
	if (hero) {
		m_pHeroObject->SetWhetherToRender(visible);
		m_pHeroObject->GetWeaponObject()->SetWhetherToRender(visible);
	}
	else {
		for (auto monster : m_MonsterVector) {
			monster->SetWhetherToRender(visible);
		}
	}
}

void CHybridBattleState::ProcessingSummonAutomata(float elapsedTimeInSec)
{
	if (m_pTurnBattleState->GetIsAutomataSummoned()) {
		if (m_pTurnBattleState->GetIsProcessing()) {

			if (!m_bIsSummonedTempAutomata) {
				if (!m_bIsSummonedAutomataOn) {
					if (m_pTempAutomata != NULL) {
						m_pTempAutomata->SetLifeTime(0.0f);
					}
					int automata = ObjectManager()->AddObject<CGameObject>(
						m_pHeroObject->GetPosition() + glm::vec3(1.0f, 0.25f, 0.0f),
						m_pHeroObject->GetRotation(),
						glm::vec3(1.35f, 1.35f, 1.35f),
						m_pHeroObject->GetMass());
					m_pTempAutomata = ObjectManager()->GetGameObject<CGameObject>(automata);
					m_pTempAutomata->SetModelIndex(m_pTurnBattleState->GetAutomataModelID());
					m_pTempAutomata->SetTextureIndex(m_pTurnBattleState->GetAutomataTextureID());
					m_pTempAutomata->SetType(TYPE_FIXED);
					m_pTempAutomata->SetState(STATE_GROUND);
					m_bIsSummonedTempAutomata = true;
					m_fAutomataSummonTime = DEFAULT_AUTOMATA_SUMMON_EFFECT_TIME;
					m_bIsSummonedAutomataOn = true;
				}
			}
			else {
				if (m_fAutomataSummonTime > 0.0f) {
					UpdateAutomataSummonEffect(elapsedTimeInSec);
				}
			}
		}
	}
	else {
		m_bIsSummonedAutomataOn = false;
	}
}

void CHybridBattleState::UpdateAutomataSummonEffect(float elapsedTimeInSec)
{
	m_fAutomataSummonTime -= elapsedTimeInSec;

	m_pTempAutomata->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, m_fAutomataSummonTime / DEFAULT_AUTOMATA_SUMMON_EFFECT_TIME));

	glm::vec3 dir = m_pHeroObject->GetPosition() - m_pTempAutomata->GetPosition();
	m_pTempAutomata->SetPosition(m_pHeroObject->GetPosition() - normalize(dir) * m_fAutomataSummonTime * 2.0f);

	if (m_fAutomataSummonTime <= 0.0f) {
		// ��� ���� �ǵ��� ����
		m_pTempAutomata->SetLifeTime(-1.0f);	
		m_bIsSummonedTempAutomata = false;
		m_pTempAutomata = NULL;
		
		// ������ ó�� ����
		ObjectManager()->DoGarbegeCollection(0.0f);
		return;
	}
}

void CHybridBattleState::LoadUIData()
{
	// ��� �簢�� �ؽ��� �ε�
	m_WhiteRectTextureID = (int)Renderer()->ReadPNGTextureFile("./resource/ui/whiteRect.png");

	m_UIHpSpBarTextureID = (int)Renderer()->ReadPNGTextureFile("./resource/ui/turnBattleAlpha/HpSpBar.png");
	m_UIMouseButtonInformationTextureID =
		(int)Renderer()->ReadPNGTextureFile("./resource/ui/turnBattleAlpha/mouseButtonInformationBar.png");
	m_UILongButtonInformationTextureID 
		= (int)Renderer()->ReadPNGTextureFile("./resource/ui/LongButtonInformationBar.png");
	m_UISmallKeyButtonTextureID 
		= (int)Renderer()->ReadPNGTextureFile("./resource/ui/turnBattleAlpha/smallKeyButton.png");
	m_UITurnGaugeTextureID = (int)Renderer()->ReadPNGTextureFile("./resource/ui/turnGauge.png");

	m_UIBurnConditionTextureID
		= (int)Renderer()->ReadPNGTextureFile("./resource/ui/burn.png");
	m_UIAnxietyConditionTextureID
		= (int)Renderer()->ReadPNGTextureFile("./resource/ui/anxiety.png");
}

void CHybridBattleState::RenderHPSPBarUI(int x, int y)
{
	float textLocationX, textLocationY;

	int order = m_pTurnBattleState->GetOrder();
	CTurnBattleObject* character = m_pTurnBattleState->GetTurnBattleObject(true, order);

	short maxHp = character->GetStatus().maxHp;
	short maxSp = character->GetStatus().maxSp;
	short hp = character->GetBattleVariable().hp;
	short sp = character->GetBattleVariable().sp;
	short level = character->GetStatus().level;

	// HP, SP ��
	Renderer()->UseShader(UI_SHADER);

	// �������� ä���
	float colorLocationX = 0.0f / (float)x;
	float colorLocationY = -1.0f / (float)y;
	Renderer()->RenderTexture2D(m_WhiteRectTextureID,
		glm::vec2(0.05f + colorLocationX, 0.9f + colorLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(298.f, 30.f), DEFAULT_BACKGROUND_COLOR);
	colorLocationX = 0.0f / (float)x;
	colorLocationY = -46.0f / (float)y;
	Renderer()->RenderTexture2D(m_WhiteRectTextureID,
		glm::vec2(0.05f + colorLocationX, 0.9f + colorLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(298.f, 30.f), DEFAULT_BACKGROUND_COLOR);

	
	glm::vec4 hpColor = glm::vec4(1.0f, 0.0f, 0.5f, 1.0f);
	glm::vec4 spColor = glm::vec4(0.0f, 0.5f, 1.0f, 1.0f);
	glm::vec4 alphaSpColor = glm::vec4(0.0f, 0.5f, 1.0f, 0.5f);
	if (order != 0) {
		// ���ΰ��� �ƴ� ��� ���� ����
		hpColor = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
		spColor = glm::vec4(0.0f, 0.5f, 0.5f, 1.0f);
		alphaSpColor = glm::vec4(0.0f, 0.5f, 0.5f, 0.5f);
	}

	// ���� ���������� ä���
	colorLocationX = 0.0f / (float)x;
	colorLocationY = -1.0f / (float)y;
	Renderer()->RenderTexture2D(m_WhiteRectTextureID,
		glm::vec2(0.05f + colorLocationX, 0.9f + colorLocationY),
		glm::vec2(0.0f, 0.0f), glm::vec2(290.f * (float)hp / (float)maxHp, 30.f), hpColor);

	// ������ ���� �Ķ������� ä���
	colorLocationX = 0.0f / (float)x;
	colorLocationY = -46.0f / (float)y;
	Renderer()->RenderTexture2D(m_WhiteRectTextureID,
		glm::vec2(0.05f + colorLocationX, 0.9f + colorLocationY),
		glm::vec2(0.0f, 0.0f), glm::vec2(290.f * (float)sp / (float)maxSp, 30.f), alphaSpColor);

	// ���� �Ķ������� ä���
	float stemina;
	float maxStemina;
	m_pHeroObject->GetStemina(&stemina, &maxStemina);

	colorLocationX = 0.0f / (float)x;
	colorLocationY = -46.0f / (float)y;
	Renderer()->RenderTexture2D(m_WhiteRectTextureID,
		glm::vec2(0.05f + colorLocationX, 0.9f + colorLocationY),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(290.f * (float)sp / (float)maxSp * stemina / maxStemina, 30.f), spColor);

	// �� �׵θ�
	colorLocationX = -3.0f / (float)x;
	colorLocationY = -5.0f / (float)y;
	Renderer()->RenderSpriteSheet2D(m_UIHpSpBarTextureID, 2, 1, 0, 0,
		glm::vec2(0.05f + colorLocationX, 0.9f + colorLocationY),
		glm::vec2(0.0f, 0.0f), glm::vec2(310.f, 42.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	colorLocationX = -3.0f / (float)x;
	colorLocationY = -50.0f / (float)y;
	Renderer()->RenderSpriteSheet2D(m_UIHpSpBarTextureID, 2, 1, 0, 0,
		glm::vec2(0.05f + colorLocationX, 0.9f + colorLocationY),
		glm::vec2(0.0f, 0.0f), glm::vec2(310.f, 42.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	// ���� �̻� ǥ��
	short conditionType = character->GetBattleVariable().condition;
	if (conditionType != NORMAL_CONDITION) {
		int conditionTextureID = EMPTY_VALUE;
		if (conditionType == BURN_CONDITION) {
			conditionTextureID = m_UIBurnConditionTextureID;
		}
		else if (conditionType == ANXIETY_CONDITION) {
			conditionTextureID = m_UIAnxietyConditionTextureID;
		}

		if (conditionTextureID != EMPTY_VALUE) {
			colorLocationX = 375.0f / (float)x;
			colorLocationY = -33.0f / (float)y;
			Renderer()->RenderTexture2D(m_WhiteRectTextureID,
				glm::vec2(0.05f + colorLocationX, 0.9f + colorLocationY),
				glm::vec2(0.0f, 0.0f), glm::vec2(50, 50), DEFAULT_BACKGROUND_COLOR 
				* glm::vec4(1.0f, 1.0f, 1.0f, character->GetBattleVariable().conditionDuration / RECOVER_CONDITION_TIME));
			Renderer()->RenderTexture2D(conditionTextureID,
				glm::vec2(0.05f + colorLocationX, 0.9f + colorLocationY),
				glm::vec2(0.0f, 0.0f), glm::vec2(50, 50),
				glm::vec4(1.0f, 1.0f, 1.0f, character->GetBattleVariable().conditionDuration / RECOVER_CONDITION_TIME));
		}
	}

	// HP, SP ���� �ؽ�Ʈ
	Renderer()->UseShader(TEXT_SHADER);

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

	textLocationX = 305.f / (float)x;
	textLocationY = 2.f / (float)y;
	Renderer()->RenderText(hpText, 0.05f + textLocationX, 0.9f + textLocationY,
		0.65f, glm::vec3(DEFAULT_BACKGROUND_COLOR));

	textLocationX = 307.f / (float)x;
	textLocationY = 4.f / (float)y;
	Renderer()->RenderText(hpText, 0.05f + textLocationX, 0.9f + textLocationY,
		0.65f, DEFAULT_TEXT_COLOR);

	textLocationX = 305.f / (float)x;
	textLocationY = -43.f / (float)y;
	Renderer()->RenderText(spText, 0.05f + textLocationX, 0.9f + textLocationY,
		0.65f, glm::vec3(DEFAULT_BACKGROUND_COLOR));

	textLocationX = 307.f / (float)x;
	textLocationY = -41.f / (float)y;
	Renderer()->RenderText(spText, 0.05f + textLocationX, 0.9f + textLocationY,
		0.65f, DEFAULT_TEXT_COLOR);
}

void CHybridBattleState::RenderNameAndTurnUI(int x, int y)
{
	float textLocationX, textLocationY;
	float imageLocationX, imageLocationY;

	int order = m_pTurnBattleState->GetOrder();
	CTurnBattleObject* character = m_pTurnBattleState->GetTurnBattleObject(true, order);
	short level = character->GetStatus().level;
	float gettingTurnGauge = character->GetShowingTurnGauge();

	Renderer()->UseShader(UI_SHADER);

	// �� ��� ���� ǥ�� ��ư ���
	imageLocationX = -5.0f / (float)x;
	imageLocationY = -152.0f / (float)y;
	Renderer()->RenderTexture2D(m_UISmallKeyButtonTextureID,
		glm::vec2(0.05f + imageLocationX, 0.9f + imageLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(52.f, 52.f), DEFAULT_BACKGROUND_COLOR);

	// ���� ����ϱ� ���� ������ �� ���
	imageLocationX = 43.0f / (float)x;
	imageLocationY = -135.0f / (float)y;
	Renderer()->RenderTexture2D(m_UITurnGaugeTextureID,
		glm::vec2(0.05f + imageLocationX, 0.9f + imageLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(200.f, 20.f), DEFAULT_BACKGROUND_COLOR);

	// ���� ����ϱ� ���� ������ ��
	imageLocationX = 43.0f / (float)x;
	imageLocationY = -135.0f / (float)y;
	Renderer()->RenderTexture2D(m_UITurnGaugeTextureID,
		glm::vec2(0.05f + imageLocationX, 0.9f + imageLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(200.f * gettingTurnGauge, 20.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	// �� ��� ���� ǥ�� ��ư
	glm::vec4 imageColor = glm::vec4(DEFAULT_DISABLE_TEXT_COLOR, 1.0f);
	if (m_pTurnBattleState->CanBeUsed()) {
		imageColor = glm::vec4(DEFAULT_TEXT_COLOR, 1.0f);
	}

	imageLocationX = -3.0f / (float)x;
	imageLocationY = -150.0f / (float)y;
	Renderer()->RenderTexture2D(m_UISmallKeyButtonTextureID,
		glm::vec2(0.05f + imageLocationX, 0.9f + imageLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(48.f, 48.f), imageColor);
	

	Renderer()->UseShader(TEXT_SHADER);

	// �̸� ���� ǥ��
	wchar_t heroLevelNameText[20];
	short heroLevel = level;
	wchar_t heroNameText[10];
	wcscpy_s(heroNameText, character->GetName());
	wsprintf(heroLevelNameText, L"lv.%d     %s", heroLevel, heroNameText);

	textLocationX = -2.0f / (float)x;
	textLocationY = -88.0f / (float)y;
	Renderer()->RenderText(heroLevelNameText, 0.05f + textLocationX, 0.9f + textLocationY,
		0.7f, glm::vec3(DEFAULT_BACKGROUND_COLOR));
	textLocationX = 0.0f / (float)x;
	textLocationY = -90.0f / (float)y;
	Renderer()->RenderText(heroLevelNameText, 0.05f + textLocationX, 0.9f + textLocationY,
		0.7f, DEFAULT_TEXT_COLOR);

	// �� ��� ���� ����
	glm::vec3 textColor = DEFAULT_DISABLE_TEXT_COLOR;
	if (m_pTurnBattleState->CanBeUsed()) {
		textColor = DEFAULT_TEXT_COLOR;
	}

	textLocationX = 7.0f / (float)x;
	textLocationY = -142.0f / (float)y;
	Renderer()->RenderText(L"T", 0.05f + textLocationX, 0.9f + textLocationY,
		0.85f, textColor);
}

void CHybridBattleState::RenderKeyInformationUI(int x, int y)
{
	float middle, textLocationX, textLocationY;
	float barLocationX, barLocationY;

	// ��ư ���� ǥ�� ��
	Renderer()->UseShader(UI_SHADER);
	middle = 251.f / 2.f / (float)x;

	barLocationY = 105.f / (float)y;
	Renderer()->RenderTexture2D(m_UIMouseButtonInformationTextureID,
		glm::vec2(0.89f - middle, 0.3f + barLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(251.f, 112.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	barLocationX = -45.f / (float)x;
	barLocationY = 0.f / (float)y;
	Renderer()->RenderTexture2D(m_UILongButtonInformationTextureID,
		glm::vec2(0.89f - middle + barLocationX, 0.3f + barLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(290.f, 112.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	Renderer()->UseShader(TEXT_SHADER);
	int condition = m_pHeroObject->GetTurnBattleObject()->GetBattleVariable().condition;

	glm::vec3 textColor = DEFAULT_DISABLE_TEXT_COLOR;
	if (condition != BURN_CONDITION) {
		textColor = DEFAULT_TEXT_COLOR;
	}
	textLocationX = 110.f / (float)x;
	textLocationY = 175.f / (float)y;
	Renderer()->RenderText(L"����", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.625f, textColor);

	textColor = DEFAULT_DISABLE_TEXT_COLOR;
	if (m_pTurnBattleState->CanBeUsed() && condition != ANXIETY_CONDITION) {
		textColor = DEFAULT_TEXT_COLOR;
	}

	textLocationX = 95.f / (float)x;
	textLocationY = 122.f / (float)y;
	Renderer()->RenderText(L"�� ���", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.625f, textColor);

	textLocationX = -33.f / (float)x;
	textLocationY = 75.f / (float)y;
	Renderer()->RenderText(L"Space", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.45f, DEFAULT_TEXT_COLOR);

	textLocationX = -28.f / (float)x;
	textLocationY = 20.f / (float)y;
	Renderer()->RenderText(L"Shift", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.45f, DEFAULT_TEXT_COLOR);

	textLocationX = 110.f / (float)x;
	textLocationY = 70.f / (float)y;
	Renderer()->RenderText(L"����", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.625f, DEFAULT_TEXT_COLOR);

	textLocationX = 110.f / (float)x;
	textLocationY = 18.f / (float)y;
	Renderer()->RenderText(L"ȸ��", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.625f, DEFAULT_TEXT_COLOR);
}
