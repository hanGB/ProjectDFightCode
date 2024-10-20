#include "stdafx.h"
#include "FieldState.h"
#include "DamageZone.h"
#include "FieldMonster.h"
#include "FullCircleFieldMonster.h"
#include "ShootingBallEffectParticle.h"
#include <random>

CFieldState::CFieldState()
{
	// CGameFramework에서 CGameState를 사용하기 때문에 CGameState의 자식 클래스인 현 클래스에서 CGameFramework를 가져올 것!
	m_pGameFramework = CGameFramework::GetInstance();

	m_pSpotLight = new CSpotLight(glm::vec3(0.0f, 0.f, 0.f), glm::vec3(0.f, 0.f, -1.f), glm::vec4(1.f, 1.f, 1.f, 1.f));

	m_pTimeOfDaySkybox = new CTimeOfDaySkybox();

	m_pTurnBattleState = new CTurnBattleState();
	m_pTurnBattleState->SetTimeOfDaySkybox(m_pTimeOfDaySkybox);
	m_pHybridBattleState = new CHybridBattleState();
	m_pHybridBattleState->SetTimeOfDaySkybox(m_pTimeOfDaySkybox);

	ChangeObjectManager(new CFieldObjectManager());

	float* sizes = new float[5];
	sizes[0] = 46.0f; sizes[1] = 96.0f; sizes[2] = 144.0f;
	sizes[3] = 192.0f; sizes[4] = 240.0f;
	ObjectManager()->SetWallSizes(sizes, 5);

	// 마우스 커서 숨기기
	glutSetCursor(GLUT_CURSOR_NONE);

	Camera()->TestCameraSetting();

	glm::vec3 pos = glm::vec3(100.0f, 0.0f, 0.0f);
	glm::vec3 dir = glm::normalize(pos);

	LoadDirectionalLight(new CDirectionalLight(pos,
		glm::vec3(-dir.x, -dir.y, -dir.z),
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));
	DirectionalLight()->SetSkyLightColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	LoadPointLight(new CPointLight(glm::vec3(0.0f, 0.f, 0.f), glm::vec4(1.0f, 0.9f, 0.8f, 1.0f)));

	LoadData();
	AddObjectToManager();
}

CFieldState::~CFieldState()
{
	delete m_pSpotLight;
	delete m_pTurnBattleState;
	delete m_pHybridBattleState;
	delete m_pHeroObject;

	m_HeroModelID = Renderer()->DeleteModelAndSetEmptyID(m_HeroModelID);
	m_HeroTextureID = Renderer()->DeleteModelAndSetEmptyID(m_HeroTextureID);

	SoundPlayer()->DeleteBackGroundMusic(m_pBGM);

	m_fieldBGMID = SoundPlayer()->DeleteSoundAndSetEmptyID(m_fieldBGMID);
}

void CFieldState::ProcessInputs(float elapsedTimeInSec, GameInputs* inputs)
{
	// 턴 배틀로 강제 변경	
	if (inputs->IsPressedDownKeyOnce(&inputs->KeyTab, &inputs->processingEnd.KeyTab)) {
		SetNeedSettingBeforeEnterBattle();
	}

	// 그림자 시아 온오프
	if (inputs->IsPressedDownKeyOnce(&inputs->KeyF2, &inputs->processingEnd.KeyF2)) {
		m_bIsShadowSight = (m_bIsShadowSight + 1) % 2;
	}


	if (inputs->KeyPlus) {
		m_pTurnBattleState->HeroLevelUp(999);
	}

	// 주인공, 나머지 업데이트 파라미터 초기화
	GameUpdateParameters heroParam;
	GameUpdateParameters othersParam;

	memset(&heroParam, 0, sizeof(GameUpdateParameters));
	memset(&othersParam, 0, sizeof(GameUpdateParameters));

	// 스테이트 진입하자마자 충돌 방지
	if (!m_bIsStartHeroCollision) {
		if (inputs->KeyW || inputs->KeyA || inputs->KeyS || inputs->KeyD || inputs->KeyLeftShift || inputs->KeySpace) {
			m_pHeroObject->SetEnableCollision(true);
			m_bIsStartHeroCollision = true;
		}
	}

	if (inputs->IsPressedDownKeyOnce(&inputs->KeyC, &inputs->processingEnd.KeyC)) {
		m_bTestCameraOn = (m_bTestCameraOn + 1) % 2;

		if (m_bTestCameraOn) {
			Camera()->SetTestCameraPosition(BackBoard()->GetHeroPosition() + glm::vec3(0.0f, 2.0f, 0.0f));
		}
	}

	if (m_bTestCameraOn)
	{
		Camera()->TestCameraControl(elapsedTimeInSec, inputs, true);
	}
	else
	{
		// 카메라 줌 인, 줌 아웃
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

		m_pHeroObject->ControlBackViewCamera(Camera(), elapsedTimeInSec, inputs, true);

		m_pHeroObject->ProcessInputs(elapsedTimeInSec, inputs);
	}

	// 손전등 온오프
	if (inputs->IsPressedDownKeyOnce(&inputs->KeyF, &inputs->processingEnd.KeyF)) {
		m_bFlashLightOn = (m_bFlashLightOn + 1) % 2;
		SoundPlayer()->PlaySound(m_switchSoundID, 1.0f);
	}

	// 회복 스폿 조종
	bool recover = m_pRecoverSpot->ApplyPlayerControl(elapsedTimeInSec, inputs);
	if (recover) {
		m_pHeroObject->SetRecoverCoolTime();
		m_pHeroObject->GetTurnBattleObject()->InitBattleVariable();
	}

	if (inputs->IsPressedDownKeyOnce(&inputs->KeyB, &inputs->processingEnd.KeyB)) {
		m_bShowBoundingBox = (m_bShowBoundingBox + 1) % 2;
	}

	Renderer()->ContorlDisplayPostEffectOnOffLine(inputs, elapsedTimeInSec);

	// 업데이트 매개 변수를 오브젝트 매니저에 넘김
	ObjectManager()->SetUpdateParameters(heroParam, othersParam);
}

void CFieldState::Update(float elapsedTimeInSec)
{
	// 잘못된 위치에 주인공이 있을 경우 위치 초기화
	glm::vec3 heroPos = m_pHeroObject->GetPosition();
	if ((-1000.0f > heroPos.x) || (heroPos.x > 1000.0f)
		|| (-1000.0f > heroPos.y) || (heroPos.y > 1000.0f)
		|| (-1000.0f > heroPos.z) || (heroPos.z > 1000.0f)) {
		InitLocationForBugFix();
	}

	// 손전등이 켜져 있을 경우 스테미나 회복 속도가 1/3이 되도록 설정
	if (m_bFlashLightOn) {
		m_pHeroObject->UseStemina(elapsedTimeInSec * (DEFAULT_STAMINA_RECOVER_SPEED * 2.0f / 3.0f));
		
		glm::vec3 pos, dir;
		Camera()->GetCameraPositionAndDirection(&pos, &dir);
		// 스포트라이트 위치 방향 설정
		m_pSpotLight->SetPosition(pos);
		m_pSpotLight->SetDirection(dir);
	}

	UpdateDirectionalLightToTime(elapsedTimeInSec);

	glm::vec3 heroPosition = m_pHeroObject->GetPosition();
	BackBoard()->SetHeroPosition(heroPosition);
	m_pHeroObject->SpectrumUpdater(elapsedTimeInSec, ObjectManager());
	m_pHeroObject->SetRecoverAnimation(m_pRecoverSpot->GetPressOnTime());

	// 마우스 고정
	int x, y;
	Renderer()->GetDisplayResolution(&x, &y);
	glutWarpPointer(x / 2, y / 2);

	// 전투 진입 여부 체크
	CFieldObjectManager* monsterManager =
		dynamic_cast<CFieldObjectManager*>(ObjectManager());
	if (monsterManager->GetIsEnteredBattle()) {
		monsterManager->SetIsEnteredBattle(false);

		SetNeedSettingBeforeEnterBattle();
	}
	CGameState::Update(elapsedTimeInSec);
}

void CFieldState::RenderScene()
{
	int activeAreaNumber = ActiveAreaNumber();
	if (!m_bIsShadowSight) activeAreaNumber = 0;

	ObjectManager()->SetAtiveArea(activeAreaNumber);
	Camera()->SetAtiveAreaNumber(activeAreaNumber);
	Camera()->UseCamera();

	// 손전등 계산
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

void CFieldState::Enter()
{
	if (!GetIsAlreadyStartedAndStillPlaying()) {
		SetIsAlreadyStartedAndStillPlaying(true);
		m_bIsStartHeroCollision = false;
		m_pHeroObject->InitStemina();
		SetDirectionalLightToTime();
		SoundPlayer()->PlayAndStopBackGroundMusic(m_pBGM, true);
	}
}

void CFieldState::Exit()
{
	if (!GetIsAlreadyStartedAndStillPlaying()) {
		SoundPlayer()->PlayAndStopBackGroundMusic(m_pBGM, false);
	}
}

void CFieldState::Pause()
{
	if (!GetIsAlreadyStartedAndStillPlaying()) {
		SoundPlayer()->PlayAndStopBackGroundMusic(m_pBGM, false);
	}
}

void CFieldState::Resume()
{
	if (!GetIsAlreadyStartedAndStillPlaying()) {
		SetIsAlreadyStartedAndStillPlaying(true);
		m_bIsStartHeroCollision = false;
		m_pHeroObject->InitStemina();
		if (m_pPauseMenuState->GetBattleMode() == BATTLE_MODE_TURN) {
		}
		else {
			m_pHeroObject->SetPosition(m_vPrevHeroPosition);
			m_bFlashLightOn = m_pHybridBattleState->GetFlashLightOn();
		}
		SetDirectionalLightToTime();

		SoundPlayer()->PlayAndStopBackGroundMusic(m_pBGM, true);
	}
}

void CFieldState::SetPauseMenuState(CPauseMenuState* state)
{
	m_pPauseMenuState = state;
}

void CFieldState::NormalRender()
{
	//CGameState::NormalRender();

	Renderer()->UseShader(DEFAULT_SHADER);
	DirectionalLight()->UseLight(DEFAULT_SHADER);
	DirectionalLight()->SetLightSpaceMatrix(DEFAULT_SHADER);
	PointLight()->UseLight(DEFAULT_SHADER);

	Renderer()->UseShader(INSTANCE_SHADER);
	DirectionalLight()->SetLightSpaceMatrix(INSTANCE_SHADER);
	DirectionalLight()->UseLight(INSTANCE_SHADER);
	PointLight()->UseLight(INSTANCE_SHADER);
	Renderer()->RenderInstance(m_grassModelID, m_grassTextureID, 
		m_GrassSSBOModelMatrices, m_fTimeForGrass, GRASS_AMOUNT_FIELD);

	// 스카이 박스 렌더링
	m_pTimeOfDaySkybox->RenderSkybox(DirectionalLight());

	Renderer()->UseShader(DEFAULT_SHADER);
	ObjectManager()->Render();

	RenderWaterObject(m_waterModelID);

	// 충돌 박스 렌더링
	if (m_bShowBoundingBox)
	{
		Renderer()->UseShader(BOUNDING_BOX_SHADER);
		ObjectManager()->BoundingBoxRender();
	}

	Renderer()->UseShader(GRAVITY_PARTICLE_SHADER);
	ObjectManager()->RenderParticle();
}

void CFieldState::UIRender()
{
	int x, y;
	Renderer()->GetDisplayResolution(&x, &y);

	RenderHPSPBarUI(x, y);
	RenderNameAndLevelTextUI(x, y);
	RenderKeyInformationUI(x, y);
	m_pRecoverSpot->RenderKeyUI(m_WhiteRectTextureID, x, y);
}

void CFieldState::LoadData()
{
	// 월드 구성
	m_areafloorModelID[0] = Renderer()->LoadOBJModelWithIndex("./resource/obj/world/area1Floor.obj");
	m_areafloorModelID[1] = Renderer()->LoadOBJModelWithIndex("./resource/obj/world/area2Floor.obj");
	m_areafloorModelID[2] = Renderer()->LoadOBJModelWithIndex("./resource/obj/world/area3Floor.obj");
	m_areafloorModelID[3] = Renderer()->LoadOBJModelWithIndex("./resource/obj/world/area4Floor.obj");

	m_areaBorderModelID = Renderer()->LoadOBJModelWithIndex("./resource/obj/world/areaBorder.obj");
	m_areaBorderTextureID = Renderer()->LoadPNGTexture(
		"./resource/texture/AllOneTexture.jpg",
		"./resource/texture/world/Metal034_2K_Color.jpg",
		"./resource/texture/world/Metal034_2K_Metalness.jpg", DEFAULT_SHININESS,
		"./resource/texture/world/Metal034_2K_NormalGL.jpg");

	m_floorLineModelID = Renderer()->LoadOBJModelWithIndex("./resource/obj/world/floorLine.obj");

	m_floorOutLineModelID = Renderer()->LoadOBJModelWithIndex("./resource/obj/world/OutLineLandScape.obj");
	m_floorOutLineTextureID = Renderer()->LoadPNGTexture(
		"./resource/texture/world/Moss004_1K_AmbientOcclusion.jpg",
		"./resource/texture/world/Moss004_1K_Color.jpg",
		"./resource/texture/AllZeroTexture.jpg", DEFAULT_SHININESS / 2.0f,
		"./resource/texture/world/Moss004_1K_Normal.jpg");

	m_fountainModelID = Renderer()->LoadOBJModelWithIndex("./resource/obj/world/fountain.obj");
	m_fountainTextureID = Renderer()->LoadPNGTexture("./resource/texture/AllOneTexture.jpg",
		"./resource/texture/world/fountainTextereDiffuse.png",
		"./resource/texture/AllOneTexture.jpg", DEFAULT_SHININESS,
		"./resource/texture/planNormalTexture.jpg");

	m_waterModelID = Renderer()->LoadOBJModelWithIndex("./resource/obj/world/water.obj");

	m_lampModelID = Renderer()->LoadOBJModelWithIndex("./resource/obj/world/lamp.obj");
	m_lampTexturelID = Renderer()->LoadPNGTexture("./resource/texture/AllOneTexture.jpg",
		"./resource/texture/world/lamp.png",
		"./resource/texture/AllOneTexture.jpg", DEFAULT_SHININESS,
		"./resource/texture/planNormalTexture.jpg");

	m_lightModelID = Renderer()->LoadOBJModelWithIndex("./resource/obj/world/lampLight.obj");

	m_grassModelID = Renderer()->LoadOBJModelWithIndex("./resource/obj/world/grass.obj");
	m_grassTextureID = Renderer()->ReadPNGTextureFile("./resource/texture/world/grass_tex.jpg");

	m_fieldBGMID = SoundPlayer()->LoadSound("./resource/sound/BGM/cityBGM.mp3");

	// bgm 설정
	m_pBGM = SoundPlayer()->SetBackGroundMusic(m_fieldBGMID, 0.5f);

	m_switchSoundID = SoundPlayer()->LoadSound("./resource/sound/Effect/switchEffect.wav");

	// 주인공 및 몬스터
	m_HeroModelID = Renderer()->LoadOBJModelWithIndex("./resource/obj/heroParty/player.obj");
	m_HeroTextureID = Renderer()->LoadPNGTexture(
		"./resource/texture/AllOneTexture.jpg",
		"./resource/texture/heroParty/player_color.png",
		"./resource/texture/heroParty/player_specular.png",	
		DEFAULT_SHININESS,
		"./resource/texture/planNormalTexture.jpg");

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
	Renderer()->LoadAnimation("./resource/dae/hero/player_recover.dae", "player", "player_recover");

	LoadUIData();
}

void CFieldState::AddObjectToManager()
{
	for (int i = 0; i < 4; ++i) {
		int areaFloor = ObjectManager()->AddNoInteractionObject<CGameObject>(
			glm::vec3(0.f, -5.0f, 0.f),
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(1.f, 1.f, 1.f),
			10000);
		ObjectManager()->GetNoInteractionGameObject<CGameObject>(areaFloor)->SetModelIndex(m_areafloorModelID[i]);
		ObjectManager()->GetNoInteractionGameObject<CGameObject>(areaFloor)->SetTextureIndex(
			m_pTurnBattleState->GetBattleFieldData(i + 1).fieldTexture);
		ObjectManager()->GetNoInteractionGameObject<CGameObject>(areaFloor)->SetType(TYPE_FLOOR);
		// 언리얼에서 가져온 모델이여서 앞면이 다르기 때문에 컬링을 끔 
		ObjectManager()->GetNoInteractionGameObject<CGameObject>(areaFloor)->SetIsCullingFace(false);
	}
	int areaBorder = ObjectManager()->AddNoInteractionObject<CGameObject>(
		glm::vec3(0.f, -4.95f, 0.f),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(1.f, 1.f, 1.f),
		10000);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(areaBorder)->SetModelIndex(m_areaBorderModelID);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(areaBorder)->SetTextureIndex(m_areaBorderTextureID);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(areaBorder)->SetType(TYPE_FLOOR);

	int floorLine = ObjectManager()->AddNoInteractionObject<CGameObject>(
		glm::vec3(0.f, -3.25f, 0.f),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(1.f, 1.f, 1.f),
		10000);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(floorLine)->SetModelIndex(m_floorLineModelID);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(floorLine)->SetTextureIndex(m_fountainTextureID);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(floorLine)->SetType(TYPE_FLOOR);

	int outLineMountain = ObjectManager()->AddNoInteractionObject<CGameObject>(
		glm::vec3(0.f, -4.5f, 0.f),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(1.f, 0.5f, 1.f),
		10000);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(outLineMountain)->SetModelIndex(m_floorOutLineModelID);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(outLineMountain)->SetTextureIndex(m_floorOutLineTextureID);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(outLineMountain)->SetType(TYPE_FLOOR);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(outLineMountain)->SetIsMadeShadow(false);

	int fountain = ObjectManager()->AddNoInteractionObject<CGameObject>(
		glm::vec3(0.f, -1.5f, 0.f),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		10000);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(fountain)->SetModelIndex(m_fountainModelID);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(fountain)->SetTextureIndex(m_fountainTextureID);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(fountain)->SetType(TYPE_FIXED);

	// 분수대 주변을 회복 스폿으로 설정
	int recoverSpot = ObjectManager()->AddObject<CRecoverSpot>(
		glm::vec3(0.f, -2.f, 0.f),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(1.f, 1.0f, 1.0f),
		10000);
	m_pRecoverSpot = ObjectManager()->GetGameObject<CRecoverSpot>(recoverSpot);
	m_pRecoverSpot->SetWhetherToRender(false);
	m_pRecoverSpot->SetType(TYPE_RECOVER_SPOT);
	m_pRecoverSpot->SetRadiusAndHeight(25.0f, 5.0f);

	for (int t = 2; t < 6; ++t) {
		float startAngle = 0;
		if (t % 2 == 0) {
			startAngle = 60;
		}
		int wallModel = m_pTurnBattleState->GetBattleFieldData(t - 1).wallModel;
		int wallTexture = m_pTurnBattleState->GetBattleFieldData(t - 1).wallTexture;

		for (float i = 0; i < 361; i = i + ((float)16 / (float)t))
		{
			float x = glm::cos(glm::radians(i)) * 24 * t;
			float z = glm::sin(glm::radians(i)) * 24 * t;

			float angle = glm::atan(z, -x) * 180.0f / PI_VALUE;
			angle += 90.0f;

			if (!(((i - startAngle) > 0 && (i - startAngle) < 15) 
				|| ((i - startAngle) > 120 && (i - startAngle) < 135) 
				|| ((i - startAngle) > 240 && (i - startAngle) < 255)) || t == 5) {

				int wall = ObjectManager()->AddNoInteractionObject<CGameObject>(
					glm::vec3(x, -4.0f, z), glm::vec3(0.f, angle, 0.f), glm::vec3(1.8f, 1.3, 1.3f), 10000);
				ObjectManager()->GetNoInteractionGameObject<CGameObject>(wall)->SetModelIndex(wallModel);
				ObjectManager()->GetNoInteractionGameObject<CGameObject>(wall)->SetTextureIndex(wallTexture);
				ObjectManager()->GetNoInteractionGameObject<CGameObject>(wall)->SetType(TYPE_FLOOR);			
			}
			else {
				int collisionIgnoreArea = ObjectManager()->AddObject<CGameObject>(
					glm::vec3(x, -3.0f, z), glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.0f, 1.0f, 1.0f), 10000);
				ObjectManager()->GetGameObject<CGameObject>(collisionIgnoreArea)->SetRadiusAndHeight(3.5f, 3.5f);
				ObjectManager()->GetGameObject<CGameObject>(collisionIgnoreArea)->SetWhetherToRender(false);
				ObjectManager()->GetGameObject<CGameObject>(collisionIgnoreArea)->SetType(TYPE_COLLISION_IGNORE_AREA);
			}
		}
	}

	glm::vec3* positions = new glm::vec3[MAX_POINT_LIGHTS_NUM];
	int num = 0;
	for (int t = 2; t < 6; ++t) {
		for (float i = 0; i < 360; i = i + ((float)180 / (float)t))
		{
			int lamp = ObjectManager()->AddObject<CGameObject>(
				glm::vec3(glm::cos(glm::radians(i)) * 24 * t - glm::cos(glm::radians(i)) * 20,
					-1.1f, glm::sin(glm::radians(i)) * 24 * t - glm::sin(glm::radians(i)) * 20),
				glm::vec3(0.f, 0.0f, 0.f), glm::vec3(1.5f, 1.5f, 1.5f), 10000);
			ObjectManager()->GetGameObject<CGameObject>(lamp)->SetModelIndex(
				m_pTurnBattleState->GetBattleFieldData(t - 1).lampModel);
			ObjectManager()->GetGameObject<CGameObject>(lamp)->SetTextureIndex(
				m_pTurnBattleState->GetBattleFieldData(t - 1).lampTexture);
			ObjectManager()->GetGameObject<CGameObject>(lamp)->SetType(TYPE_FIXED);
			ObjectManager()->GetGameObject<CGameObject>(lamp)->SetRadiusAndHeight(0.3f, 3.2f);


			positions[num] = glm::vec3(glm::cos(glm::radians(i)) * 24 * t - glm::cos(glm::radians(i)) * 20,
				2.0f, glm::sin(glm::radians(i)) * 24 * t - glm::sin(glm::radians(i)) * 20);
			num++;

			int light = ObjectManager()->AddObject<CGameObject>(
				glm::vec3(glm::cos(glm::radians(i)) * 24 * t - glm::cos(glm::radians(i)) * 20,
					-1.1f, glm::sin(glm::radians(i)) * 24 * t - glm::sin(glm::radians(i)) * 20),
				glm::vec3(0.f, 0.0f, 0.f), glm::vec3(1.575f, 1.5f, 1.575f), 10000);
			ObjectManager()->GetGameObject<CGameObject>(light)->SetModelIndex(
				m_pTurnBattleState->GetBattleFieldData(t - 1).lampLightModel);
			ObjectManager()->GetGameObject<CGameObject>(light)->SetTextureIndex(
				m_pTurnBattleState->GetBattleFieldData(t - 1).lampTexture);
			ObjectManager()->GetGameObject<CGameObject>(light)->SetType(TYPE_FIXED);
			ObjectManager()->GetGameObject<CGameObject>(light)->SetIsLightSource(true);
			ObjectManager()->GetGameObject<CGameObject>(light)->SetColor(
				m_pTurnBattleState->GetBattleFieldData(t - 1).lightColor);

			m_vLampLightVector.push_back(ObjectManager()->GetGameObject<CGameObject>(light));
		}
	}
	PointLight()->SetPointLights(positions, num);
	PointLight()->SetDiffuseLight(glm::vec4(1.0f, 1.0f, 0.75f, 1.0f));
	PointLight()->SetIsPointLightsOn(false);
	MakeGrassInstance();

	// 주인공 추가
	int heroID = ObjectManager()->AddObject<CHeroObject>(
		glm::vec3(0.0f, -2.5f, 110.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		60.0f);
	m_pHeroObject = ObjectManager()->GetGameObject<CHeroObject>(heroID);
	m_pHeroObject->SetVolume(glm::vec3(0.7f, 1.7f, 0.4f));
	m_pHeroObject->SetModelIndex(m_HeroModelID);
	m_pHeroObject->SetTextureIndex(m_HeroTextureID);
	m_pHeroObject->SetIsCullingFace(false);
	m_pHeroObject->SetType(TYPE_HERO);
	m_pHeroObject->SetState(STATE_GROUND);
	m_pHeroObject->SetTurnBattleObject(m_pTurnBattleState->GetTurnBattleObject(true, 0));
	m_pHeroObject->SetEnableCollision(false);
	m_pHeroObject->SetLimitVelocity(3.0f);
	m_pHeroObject->SetBloodTextureID(Renderer()->ReadPNGTextureFile("./resource/texture/blood.png"));
	m_pHeroObject->SetIsAnimate(true);
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
	heroAnimationDataMap.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_RECOVER, "player_recover"));
	heroAnimationDataMap.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_DEATH, "player_hit"));
	m_pHeroObject->SetAnimationDataNameMap(heroAnimationDataMap);
	m_pHeroObject->SetAnimationState(ANI_STATE_IDLE);

	m_vPrevHeroPosition = m_pHeroObject->GetPosition();

	int weaponIndex = ObjectManager()->AddObject<CGameObject>(
		glm::vec3(0.5f, 0.f, -1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		5.0f);
	CGameObject* weapon = ObjectManager()->GetGameObject<CGameObject>(weaponIndex);
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


	// 하이브리드 전투 스테이트 설정
	m_pHybridBattleState->SetHeroObject(m_pHeroObject, weapon);
	m_pHybridBattleState->SetTurnBattleState(m_pTurnBattleState);

	// 필드 몬스터 추가
	for (int track = 1; track < 5; ++track) {
		short modelId = 1;
		short textureId = 1;
		glm::vec3 volume = glm::vec3(1.0f);
		float speed = 25.0f;
		float angleSpeed = (5 - track) * 5.0f;
		if (track == 1) {
			modelId = 4;
			textureId = 10;
			volume = glm::vec3(1.25f, 2.0f, 4.0f);
		}
		else if (track == 2) {
			modelId = 3;
			textureId = 8;
			volume = glm::vec3(1.0f, 2.0f, 0.5f);
			speed = 20.0f;
			angleSpeed = 10.0f;
		}
		else if (track == 3) {
			modelId = 2;
			textureId = 7;
			volume = glm::vec3(1.0f, 0.8f, 3.0f);
		}
		else if (track == 4) {
			modelId = 1;
			textureId = 4;
			volume = glm::vec3(1.1f, 0.8f, 1.1f);
		}

		for (float angle = 0; angle < 360; angle = angle + ((float)180 / (float)track))
		{
			if (track == 4) {
				if (angle > 30 && angle < 100)
					continue;
			}
			CFieldMonster* monster;
			if (track == 1) {
				int filedMonsterIndex = ObjectManager()->AddObject<CFullCircleFieldMonster>(
					glm::vec3(glm::cos(glm::radians(angle)) * 24 * track + glm::cos(glm::radians(angle)) * 12
						, 5.f, glm::sin(glm::radians(angle)) * 24 * track + glm::sin(glm::radians(angle)) * 12),
					glm::vec3(0.f, 0.f, 0.f),
					glm::vec3(1.0f, 1.0f, 1.0f),
					100);
				monster = ObjectManager()->GetGameObject<CFullCircleFieldMonster>(filedMonsterIndex);
				monster->SetVolume(volume);
				monster->SetScoutRange(0.0f, 360.0f);
				monster->SetAngleSpeed(15.0f);
			}
			else {
				int filedMonsterIndex = ObjectManager()->AddObject<CFieldMonster>(
					glm::vec3(glm::cos(glm::radians(angle)) * 24 * track + glm::cos(glm::radians(angle)) * 12
						, 0.0f, glm::sin(glm::radians(angle)) * 24 * track + glm::sin(glm::radians(angle)) * 12),
					glm::vec3(0.f, 0.f, 0.f),
					glm::vec3(1.0f, 1.0f, 1.0f),
					100);
				monster = ObjectManager()->GetGameObject<CFieldMonster>(filedMonsterIndex);
				monster->SetVolume(volume);
				monster->SetScoutRange(angle, angle + ((float)180 / (float)track));
				monster->SetAngleSpeed(angleSpeed);
				monster->SetSpeed(speed);
			}
			monster->SetIsAnimate(true);
			monster->SetModelWithBone(m_pTurnBattleState->GetMonsterModelInMonsterModelMap(modelId));
			monster->SetAnimationDataNameMap(m_pTurnBattleState->GetMonsterAnimationsInMonsterAnimationsMap(modelId));
			monster->SetTextureIndex(m_pTurnBattleState->GetMonsterTextureInMonsterTextureMap(textureId));
			monster->SetAnimationState(ANI_STATE_WALK);
			monster->SetType(TYPE_ENEMY);
			monster->SetState(STATE_FALLING);
			monster->SetAreaNumbe(track);
			monster->SetLimitVelocity(3.0f);
			monster->SetBloodTextureID(m_pTurnBattleState->GetMonsterBloodTextureInMonsterBloodTextureMap(textureId));
			monster->SetDefaultSoundID(m_pTurnBattleState->GetMonsterDefaultSoundInMonsterDefaultSoundMap(textureId));
			monster->SetHitSoundID(m_pTurnBattleState->GetMonsterDefaultSoundInMonsterDefaultSoundMap(
				HIT_SOUND_START_INDEX_IN_DEFAULT_SOUND_MAP + textureId));
			if (track == 4) {
				monster->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.75f));
			}
			else if (track == 3) {
				monster->SetSize(glm::vec3(0.8f, 0.8f, 0.8f));
			}
			else if (track == 1) {
				// 1번에는 하나만 추가하기 위해 루프 중단
				break;
			}
		}
	}

	// 분수대 물 파티클 
	int waterDrop = ObjectManager()->
		AddParticle<CFountainWaterDropParticle>(glm::vec3(0.0f, 26.f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	ObjectManager()->GetParticle<CFountainWaterDropParticle>(waterDrop)->SetRadiusTopAndButtom(0.5f, 4.5f);
	ObjectManager()->GetParticle<CFountainWaterDropParticle>(waterDrop)->SetTextureID(
		Renderer()->ReadPNGTextureFile("./resource/texture/waterDrop.png"));
	ObjectManager()->GetParticle<CFountainWaterDropParticle>(waterDrop)->SetColor(glm::vec4(0.5f, 0.85f, 1.0f, 0.35f));
	ObjectManager()->GetParticle<CFountainWaterDropParticle>(waterDrop)->CreateVAO();
}

void CFieldState::InitLocationForBugFix()
{
	m_pHeroObject->SetPosition(glm::vec3(0.0f, -2.5f, 110.0f));
}

void CFieldState::MakeGrassInstance()
{
	// 잔디 인스턴스 모델 변환 저장
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[GRASS_AMOUNT_FIELD];
	int index = 0;
	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_real_distribution<float> urd(0.5f, 1.5f);

	for (int t = 388; t < 480; ++t)
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
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::mat4) * GRASS_AMOUNT_FIELD, modelMatrices, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_GrassSSBOModelMatrices);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	delete[] modelMatrices;
}

void CFieldState::RenderWaterObject(int modelID)
{
	Renderer()->UseShader(WATER_SHADER);
	DirectionalLight()->SetLightSpaceMatrix(WATER_SHADER);
	DirectionalLight()->UseLight(WATER_SHADER);
	PointLight()->UseLight(WATER_SHADER);

	int cubeMap1;
	int cubeMap2 = 0;
	float rate = 0.0f;

	m_pTimeOfDaySkybox->GetNowCubemap(&cubeMap1, &cubeMap2, &rate);

	Renderer()->DrawWaterObject(modelID, 
		glm::vec3(0.0f, -4.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		cubeMap1, cubeMap2, rate, m_fTimeForGrass);

	Renderer()->DrawWaterObject(modelID,
		glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.575f, 0.6f, 0.575f),
		cubeMap1, cubeMap2, rate, m_fTimeForGrass);
}

void CFieldState::UpdateDirectionalLightToTime(float elapsedTimeInSec)
{
	m_fTimeForGrass += (elapsedTimeInSec * DEFAULT_WIND_SPEED);

	m_pTimeOfDaySkybox->Update(elapsedTimeInSec, DirectionalLight());

	if (m_pTimeOfDaySkybox->IsPassedHalfDay()) {
		if (m_pTimeOfDaySkybox->IsDayTime()) {
			TurnOnLampLight(false);
		}
		else {
			TurnOnLampLight(true);
		}
	}
}

void CFieldState::SetDirectionalLightToTime()
{
	m_pTimeOfDaySkybox->Initialize(DirectionalLight());

	if (m_pTimeOfDaySkybox->IsDayTime()) {
		TurnOnLampLight(false);
	}
	else {
		TurnOnLampLight(true);
	}
}

void CFieldState::TurnOnLampLight(bool on)
{
	for (auto light : m_vLampLightVector) {
		light->SetWhetherToRender(on);
	}
	PointLight()->SetIsPointLightsOn(on);
}

void CFieldState::SetNeedSettingBeforeEnterBattle()
{
	CFieldObjectManager* monsterManager =
		dynamic_cast<CFieldObjectManager*>(ObjectManager());

	m_vPrevHeroPosition = m_pHeroObject->GetPosition();

	m_pTurnBattleState->SetAreaNumber(monsterManager->GetAreaNumber());
	m_pTurnBattleState->SetIsStartedWithHeroWin(monsterManager->GetIsHeroWin());

	if (m_pPauseMenuState->GetBattleMode() == BATTLE_MODE_TURN) {
		SetIsAlreadyStartedAndStillPlaying(false);
		m_pGameFramework->PushState(m_pTurnBattleState);
	}
	else {
		m_pHybridBattleState->SetAreaNumber(monsterManager->GetAreaNumber());
		m_pHybridBattleState->SetIsStartedWithHeroWin(monsterManager->GetIsHeroWin());
		m_pHybridBattleState->SetFlashLightOn(m_bFlashLightOn);

		SetIsAlreadyStartedAndStillPlaying(false);
		m_pGameFramework->PushState(m_pHybridBattleState);
	}
}

int CFieldState::ActiveAreaNumber()
{
	glm::vec3 pos = m_pHeroObject->GetPosition(); pos.y = 0.0f;
	float len = glm::length(pos);

	if (len <= DEFAULT_AREA_WIDTH * 2) return 1;
	else if (len <= DEFAULT_AREA_WIDTH * 3) return 2;
	else if (len <= DEFAULT_AREA_WIDTH * 4) return 3;
	else if (len <= DEFAULT_AREA_WIDTH * 5) return 4;
	else return 0;
}

void CFieldState::LoadUIData()
{
	// 흰색 사각형 텍스쳐 로드
	m_WhiteRectTextureID = (int)Renderer()->ReadPNGTextureFile("./resource/ui/whiteRect.png");

	m_UIHpSpBarTextureID = (int)Renderer()->ReadPNGTextureFile("./resource/ui/turnBattleAlpha/HpSpBar.png");
	m_UIMouseButtonInformationTextureID =
		(int)Renderer()->ReadPNGTextureFile("./resource/ui/turnBattleAlpha/mouseButtonInformationBar.png");
	m_UILongButtonInformationTextureID 
		= (int)Renderer()->ReadPNGTextureFile("./resource/ui/LongButtonInformationBar.png");
}

void CFieldState::RenderHPSPBarUI(int x, int y)
{
	CTurnBattleObject* hero = m_pHeroObject->GetTurnBattleObject();

	short maxHp = hero->GetStatus().maxHp;
	short maxSp = hero->GetStatus().maxSp;
	short hp = hero->GetBattleVariable().hp;
	short sp = hero->GetBattleVariable().sp;
	short level = hero->GetStatus().level;

	// HP, SP 바
	Renderer()->UseShader(UI_SHADER);

	// 배경색으로 채우기
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

	// 약한 빨간색으로 채우기
	colorLocationX = 0.0f / (float)x;
	colorLocationY = -1.0f / (float)y;
	Renderer()->RenderTexture2D(m_WhiteRectTextureID,
		glm::vec2(0.05f + colorLocationX, 0.9f + colorLocationY),
		glm::vec2(0.0f, 0.0f), glm::vec2(290.f * (float)hp / (float)maxHp, 30.f), hpColor);

	// 투명한 약한 파란색으로 채우기
	colorLocationX = 0.0f / (float)x;
	colorLocationY = -46.0f / (float)y;
	Renderer()->RenderTexture2D(m_WhiteRectTextureID,
		glm::vec2(0.05f + colorLocationX, 0.9f + colorLocationY),
		glm::vec2(0.0f, 0.0f), glm::vec2(290.f * (float)sp / (float)maxSp, 30.f), alphaSpColor);

	// 약한 파란색으로 채우기
	float stemina;
	float maxStemina;
	m_pHeroObject->GetStemina(&stemina, &maxStemina);

	colorLocationX = 0.0f / (float)x;
	colorLocationY = -46.0f / (float)y;
	Renderer()->RenderTexture2D(m_WhiteRectTextureID,
		glm::vec2(0.05f + colorLocationX, 0.9f + colorLocationY),
		glm::vec2(0.0f, 0.0f), 
		glm::vec2(290.f * (float)sp / (float)maxSp * stemina / maxStemina, 30.f), spColor);

	// 바 테두리
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
}

void CFieldState::RenderNameAndLevelTextUI(int x, int y)
{
	float textLocationX, textLocationY;

	int order = m_pTurnBattleState->GetOrder();
	CTurnBattleObject* character = m_pTurnBattleState->GetTurnBattleObject(true, order);
	short level = character->GetStatus().level;

	Renderer()->UseShader(TEXT_SHADER);

	// 이름 레벨 표시
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
}

void CFieldState::RenderKeyInformationUI(int x, int y)
{
	float middle, textLocationX, textLocationY;
	float barLocationX, barLocationY;

	// 버튼 정보 표시 바
	Renderer()->UseShader(UI_SHADER);
	middle = 251.f / 2.f / (float)x;

	barLocationY = 108.f / (float)y;
	Renderer()->RenderSpriteSheet2D(m_UIMouseButtonInformationTextureID, 1, 2, 0, 1,
		glm::vec2(0.89f - middle, 0.3f + barLocationY),
		glm::vec2(0.0f, 0.0f), glm::vec2(251.f, 112.f / 2.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	barLocationX = -45.f / (float)x;
	barLocationY = 0.f / (float)y;
	Renderer()->RenderTexture2D(m_UILongButtonInformationTextureID,
		glm::vec2(0.89f - middle + barLocationX, 0.3f + barLocationY), glm::vec2(0.0f, 0.0f),
		glm::vec2(290.f, 112.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	Renderer()->UseShader(TEXT_SHADER);
	textLocationX = 110.f / (float)x;
	textLocationY = 122.f / (float)y;
	Renderer()->RenderText(L"공격", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.625f, DEFAULT_TEXT_COLOR);

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
	Renderer()->RenderText(L"점프", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.625f, DEFAULT_TEXT_COLOR);

	textLocationX = 110.f / (float)x;
	textLocationY = 18.f / (float)y;
	Renderer()->RenderText(L"회피", 0.89f - middle + textLocationX, 0.3f + textLocationY,
		0.625f, DEFAULT_TEXT_COLOR);
}
