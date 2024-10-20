#include "stdafx.h"
#include "TestState.h"
#include "TestObject.h"
#include <random>

CTestState::CTestState()
{
	// CGameFramework에서 CGameState를 사용하기 때문에 CGameState의 자식 클래스인 현 클래스에서 CGameFramework를 가져올 것!
	m_pGameFramework = CGameFramework::GetInstance();

	m_pSpotLight = new CSpotLight(glm::vec3(0.0f, 0.f, 0.f), glm::vec3(0.f, 0.f, -1.f), glm::vec4(1.f, 1.f, 1.f, 1.f));

	// 마우스 커서 숨기기
	glutSetCursor(GLUT_CURSOR_NONE);

	Camera()->TestCameraSetting();

	glm::vec3 pos = glm::vec3(100.0f, 0.0f, 0.0f);
	glm::vec3 dir = glm::normalize(pos);
	
	LoadDirectionalLight(new CDirectionalLight(pos,
		glm::vec3(-dir.x, -dir.y, -dir.z),
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));

	LoadPointLight(new CPointLight(glm::vec3(0.0f, 2.f, 0.f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));

	LoadData();
	AddObjectToManager();
}

CTestState::~CTestState()
{
	m_HeroModelID = Renderer()->DeleteModelAndSetEmptyID(m_HeroModelID);
	m_HeroTextureID = Renderer()->DeleteModelAndSetEmptyID(m_HeroTextureID);

	SoundPlayer()->DeleteBackGroundMusic(m_pBGM);

	m_floorModelID = Renderer()->DeleteModelAndSetEmptyID(m_floorModelID);
	m_floorTextureID = Renderer()->DeleteTextureAndSetEmptyID(m_floorTextureID);

	m_enemyModelID = Renderer()->DeleteModelAndSetEmptyID(m_enemyModelID);
	m_enemyTextureID = Renderer()->DeleteTextureAndSetEmptyID(m_enemyTextureID);

	m_enemy2ModelID = Renderer()->DeleteModelAndSetEmptyID(m_enemy2ModelID);
	m_enemy2TextureID = Renderer()->DeleteTextureAndSetEmptyID(m_enemy2TextureID);

	m_boxModelID = Renderer()->DeleteModelAndSetEmptyID(m_boxModelID);
	m_boxTextureID = Renderer()->DeleteTextureAndSetEmptyID(m_boxTextureID);


	m_cityBGMID = SoundPlayer()->DeleteSoundAndSetEmptyID(m_cityBGMID);

	delete m_pSpotLight;
}

void CTestState::ProcessInputs(float elapsedTimeInSec, GameInputs* inputs)
{
	// 주인공, 나머지 업데이트 파라미터 초기화
	GameUpdateParameters heroParam;
	GameUpdateParameters othersParam;

	memset(&heroParam, 0, sizeof(GameUpdateParameters));
	memset(&othersParam, 0, sizeof(GameUpdateParameters));

	if (inputs->IsPressedDownKeyOnce(&inputs->KeyC, &inputs->processingEnd.KeyC)) {
		m_bTestCameraOn = (m_bTestCameraOn + 1) % 2;
	}

	if (m_bTestCameraOn)
	{
		Camera()->TestCameraControl(elapsedTimeInSec, inputs, m_bMouseFixed);
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
		
		m_pHeroObject->SetCameraPosWithHeroPos(Camera());
		Camera()->SetQuarterViewCamera();

		// 이동
		glm::vec3 force = glm::vec3(0.0f, 0.0f, 0.0f);

		if (inputs->KeyW) force.z--;
		if (inputs->KeyS) force.z++;
		if (inputs->KeyA) force.x--;
		if (inputs->KeyD) force.x++;
		if (inputs->KeySpace) force.y++;

		float horizonForceAmount = 1200.0f;
		float verticalForceAmount = 24000.0f;

		heroParam.force = glm::vec3(
			force.x * horizonForceAmount,
			force.y * verticalForceAmount, 
			force.z * horizonForceAmount);

		// 공격
		if (inputs->MouseLeftButton)
		{
			float coolTime = m_pHeroObject->GetCoolTime();
			if (coolTime <= 0.0f)
			{
				glm::vec3 dir = m_pHeroObject->GetDirection();
				int box = ObjectManager()->AddObject<CGameObject>(
					glm::vec3(dir.x, dir.y, dir.z),
					m_pHeroObject->GetRotation(),
					glm::vec3(0.2f, 0.2f, 0.2f),
					10.0f
					);
				CGameObject* boxObject = ObjectManager()->GetGameObject<CGameObject>(box);
				boxObject->SetDirection(dir);
				boxObject->SetModelIndex(m_boxModelID);
				boxObject->SetTextureIndex(m_boxTextureID);
				boxObject->SetType(TYPE_BULLET);
				boxObject->SetState(STATE_FALLING);
				boxObject->SetParent(m_pHeroObject);
				boxObject->StickToParent();
				boxObject->SetLifeTime(10.0f);
				boxObject->SetLimitVelocity(30.0f);
				boxObject->SetVolume(glm::vec3(1.0f, 1.0f, 1.0f));
				float boxForce = 100.0f;
				boxObject->SetVelocity(glm::vec3(dir.x * boxForce, dir.y * boxForce, dir.z * boxForce));

				m_pHeroObject->SetCoolTime(0.2f);
			}
		}
	}

	// 손전등 온오프
	if (inputs->IsPressedDownKeyOnce(&inputs->KeyE, &inputs->processingEnd.KeyE)) {
		m_bFlashLightOn = (m_bFlashLightOn + 1) % 2;
		SoundPlayer()->PlaySound(m_switchSoundID, 1.0f);
	}

	if (inputs->IsPressedDownKeyOnce(&inputs->KeyArrowUp, &inputs->processingEnd.KeyArrowUp)) {
		m_pGameFramework->ChangeState(new CTestState(), false);
	}
	if (inputs->IsPressedDownKeyOnce(&inputs->KeyArrowDown, &inputs->processingEnd.KeyArrowDown)){
		m_pGameFramework->PushState(new CTestState());
	}
	if (inputs->IsPressedDownKeyOnce(&inputs->KeyArrowRight, &inputs->processingEnd.KeyArrowRight)) {
		m_pGameFramework->PopState(false);
	}
	if (inputs->IsPressedDownKeyOnce(&inputs->KeyArrowLeft, &inputs->processingEnd.KeyArrowLeft)) {
		m_pGameFramework->ConvertWithPrevState();
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

	// 업데이트 매개 변수를 오브젝트 매니저에 넘김
	ObjectManager()->SetUpdateParameters(heroParam, othersParam);
}

void CTestState::Update(float elapsedTimeInSec)
{
	glm::vec3 pos, dir;
	Camera()->GetCameraPositionAndDirection(&pos, &dir);
	// 스포트라이트 위치 방향 설정
	m_pSpotLight->SetPosition(pos);
	m_pSpotLight->SetDirection(dir);

	
	glm::vec3 heroPosition = m_pHeroObject->GetPosition();
	BackBoard()->SetHeroPosition(heroPosition);

	// 조명 위치 설정
	PointLight()->SetPosition(glm::vec3(heroPosition.x, heroPosition.y + 1.0f, heroPosition.z));

	int x, y;
	Renderer()->GetDisplayResolution(&x, &y);
	if (m_bMouseFixed)
		glutWarpPointer(x / 2, y / 2);

	// 프레임 계산
	m_fSecond += elapsedTimeInSec;
	if (m_fSecond > 0.3)
	{
		int fps = (int)(1.0f / elapsedTimeInSec);

		wsprintf(m_wFpsText, L"FPS: %d", fps);
		m_fSecond = 0;
	}

	m_fTimeForGrass += (elapsedTimeInSec * DEFAULT_WIND_SPEED);

	// 스프라이트 시트 프레임 계산
	int frameCnt = 4;
	int frameSpeed = 2;
	m_fFrameTime = m_fFrameTime + frameSpeed * frameCnt * elapsedTimeInSec;
	m_iFrame = (int)m_fFrameTime % frameCnt;

	// 광원 위치 변경
	m_fHour = m_fHour + (elapsedTimeInSec / 2);
	if (m_fHour >= 24.0f)
		m_fHour = 0.0f;

	if (6.0f <= m_fHour && m_fHour < 18.0f)
	{
		if (6.0f <= m_fHour && m_fHour <= 6.5f)
		{
			DirectionalLight()->SetDiffuseLight(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			DirectionalLight()->SetSpecularLight(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			DirectionalLight()->SetShadowStrength(1.0f);
		}
		float timeOfDay = (m_fHour + 18) * 15;
		glm::vec3 pos = glm::vec3(glm::cos(glm::radians(timeOfDay)) * 100.0f,
			glm::sin(glm::radians(timeOfDay)) * 100.0f, 0.0f);
		DirectionalLight()->SetPosition(pos);
		glm::vec3 dir = glm::normalize(pos);
		DirectionalLight()->SetDirection(glm::vec3(-dir.x, -dir.y, -dir.z));
	}
	else if (18.0f <= m_fHour || m_fHour < 6.0f)
	{
		if (18.0f <= m_fHour && m_fHour <= 18.5f)
		{
			DirectionalLight()->SetDiffuseLight(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
			DirectionalLight()->SetSpecularLight(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
			DirectionalLight()->SetShadowStrength(1.3f);
		}
		float timeOfDay = (m_fHour + 18 - 12) * 15;
		glm::vec3 pos = glm::vec3(glm::cos(glm::radians(timeOfDay)) * 100.0f,
			glm::sin(glm::radians(timeOfDay)) * 100.0f, 0.0f);
		DirectionalLight()->SetPosition(pos);
		glm::vec3 dir = glm::normalize(pos);
		DirectionalLight()->SetDirection(glm::vec3(-dir.x, -dir.y, -dir.z));
	}

	CGameState::Update(elapsedTimeInSec);
}

void CTestState::RenderScene()
{
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

void CTestState::Enter()
{
	// SoundPlayer()->PlayAndStopBackGroundMusic(m_pBGM, true);
}

void CTestState::Exit()
{
	// SoundPlayer()->PlayAndStopBackGroundMusic(m_pBGM, false);
}

void CTestState::Pause()
{
	// SoundPlayer()->PlayAndStopBackGroundMusic(m_pBGM, false);
}

void CTestState::Resume()
{
	// SoundPlayer()->PlayAndStopBackGroundMusic(m_pBGM, true);
}

void CTestState::NormalRender()
{
	CGameState::NormalRender();

	Renderer()->UseShader(INSTANCE_SHADER);
	DirectionalLight()->SetLightSpaceMatrix(INSTANCE_SHADER);
	DirectionalLight()->UseLight(INSTANCE_SHADER);
	PointLight()->UseLight(INSTANCE_SHADER);
	Renderer()->RenderInstance(m_grassModelID, m_grassTextureID, 
		m_GrassSSBOModelMatrices, m_fTimeForGrass, GRASS_AMOUNT_TITLE);

	// 충돌 박스 렌더링
	if (m_bShowBoundingBox)
	{
		Renderer()->UseShader(BOUNDING_BOX_SHADER);
		ObjectManager()->BoundingBoxRender();
	}

	// 스카이 박스 렌더링
	Renderer()->UseShader(SKYBOX_SHADER);
	if (m_fHour >= 6.0f && m_fHour < 18.0f)
		Renderer()->DrawSkybox(m_skyboxTextureID, 0, 0.0f);
	else
		Renderer()->DrawSkybox(m_skyboxDarkTextureID, 0, 0.0f);

	// 빌보드 렌더링
	Renderer()->UseShader(BILLBOARD_SHADER);
	Renderer()->RenderTexture3D(m_billboardTextureID,
		glm::vec3(0.f, 3.f, 0.f),
		glm::vec2(1.f, 1.f),
		glm::vec4(1.f, 1.f, 1.f, 1.f));

	glm::vec3 directionalPos = DirectionalLight()->GetPosition();
	Renderer()->RenderTexture3D(m_moonTextureID,
		glm::vec3(directionalPos.x, directionalPos.y, directionalPos.z),
		glm::vec2(1.f, 1.f),
		glm::vec4(1.f, 1.f, 1.f, 1.f));

	// 빌보드 애니메이션 렌더링
	Renderer()->UseShader(BILLBOARD_SPRITE_SHADER);
	Renderer()->RenderSpriteSheet3D(m_spriteSheetID,
		4, 1, m_iFrame, 0,
		glm::vec3(0.f, 5.f, 0.f),
		glm::vec2(1.f, 1.f),
		glm::vec4(1.f, 1.f, 1.f, 1.f));
}

void CTestState::UIRender()
{
	// 2d UI 렌더링
	Renderer()->UseShader(UI_SHADER);
	Renderer()->RenderTexture2D(m_uiTextureID,
		glm::vec2(0.08, 0.08), glm::vec2(0, 0), glm::vec2(500, 100), glm::vec4(1, 1, 1, 1));

	Renderer()->RenderSpriteSheet2D(m_spriteSheetID, 4, 1, m_iFrame, 0,
		glm::vec2(0.8, 0.05), glm::vec2(0, 0), glm::vec2(200, 300), glm::vec4(1, 1, 1, 1));

	// 텍스트 렌더링
	Renderer()->UseShader(TEXT_SHADER);
	Renderer()->RenderText(m_wFpsText, 0.02f, 0.93f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	if (m_bMouseFixed)
		Renderer()->RenderText(L"ESC로 마우스 고정 해제 및 포인터 표시", 0.01f, 0.02f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
	else
		Renderer()->RenderText(L"ESC로 마우스 고정 및 포인터 숨기기", 0.01f, 0.02f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
}

void CTestState::LoadData()
{
	m_floorModelID = Renderer()->LoadOBJModelWithIndex("./resource/obj/flloorWithWall.obj");
	m_floorTextureID = Renderer()->LoadPNGTexture(
		"./resource/texture/Ground037_1K_AmbientOcclusion.png",
		"./resource/texture/Ground037_1K_Color.png",
		"./resource/texture/Ground037_1K_Color_Specular.png", DEFAULT_SHININESS / 8,
		"./resource/texture/Ground037_1K_Normal.png");

	m_enemyModelID = Renderer()->LoadOBJModelWithIndex("./resource/obj/enemyTwo.obj");
	m_enemyTextureID = Renderer()->LoadBMPTexture(
		"./resource/texture/uvtemplate.bmp",
		"./resource/texture/uvtemplate.bmp",
		"./resource/texture/uvtemplate.bmp", DEFAULT_SHININESS / 2,
		"./resource/texture/planNormalTexture.bmp");

	m_enemy2ModelID = Renderer()->LoadOBJModelWithIndex("./resource/obj/enemyOne.obj");
	m_enemy2TextureID = Renderer()->LoadBMPTexture(
		"./resource/texture/uvtemplateInverse.bmp",
		"./resource/texture/uvtemplateInverse.bmp",
		"./resource/texture/uvtemplateInverse.bmp", DEFAULT_SHININESS / 2,
		"./resource/texture/planNormalTexture.bmp");

	m_boxModelID = Renderer()->LoadOBJModelWithIndex("./resource/obj/box.obj");
	m_boxTextureID = Renderer()->LoadPNGTexture(
		"./resource/texture/Bricks069_2K_AmbientOcclusion.jpg",
		"./resource/texture/Bricks069_2K_Color.jpg",
		"./resource/texture/AllOneTexture.jpg", DEFAULT_SHININESS,
		"./resource/texture/Bricks069_2K_Normal.jpg");

	m_grassModelID = Renderer()->LoadOBJModelWithIndex("./resource/obj/grass0.obj");
	m_grassTextureID = Renderer()->ReadPNGTextureFile("./resource/texture/grass_tex.jpg");

	m_cityBGMID = SoundPlayer()->LoadSound("./resource/sound/BGM/cityBGM.mp3");

	// bgm 설정
	m_pBGM = SoundPlayer()->SetBackGroundMusic(m_cityBGMID, 0.5f);

	m_uiTextureID = Renderer()->ReadPNGTextureFile("./resource/texture/boxTexture.png");
	m_billboardTextureID = Renderer()->ReadPNGTextureFile("./resource/texture/boxTexture.png");

	// 달 텍스쳐
	m_moonTextureID = Renderer()->ReadPNGTextureFile("./resource/texture/moonTexture.png");

	m_spriteSheetID = Renderer()->ReadPNGTextureFile("./resource/sprite/igAni.png");

	std::string skyboxFiles[] = {
		"./resource/texture/skybox/right.jpg",
		"./resource/texture/skybox/left.jpg",
		"./resource/texture/skybox/top.jpg",
		"./resource/texture/skybox/bottom.jpg",
		"./resource/texture/skybox/front.jpg",
		"./resource/texture/skybox/back.jpg"
	};
	m_skyboxTextureID = Renderer()->LoadSkybox("testDay", skyboxFiles);

	std::string skyboxDarkFiles[] = {
		"./resource/texture/skyboxDark/right.jpg",
		"./resource/texture/skyboxDark/left.jpg",
		"./resource/texture/skyboxDark/top.jpg",
		"./resource/texture/skyboxDark/bottom.jpg",
		"./resource/texture/skyboxDark/front.jpg",
		"./resource/texture/skyboxDark/back.jpg"
	};
	m_skyboxDarkTextureID = Renderer()->LoadSkybox("testNight", skyboxDarkFiles);

	m_switchSoundID = SoundPlayer()->LoadSound("./resource/sound/Effect/switchEffect.wav");

	m_HeroModelID = Renderer()->LoadOBJModelWithIndex("./resource/obj/char.obj");
	m_HeroTextureID = Renderer()->LoadPNGTexture(
		"./resource/ui/whiteRect.png",
		"./resource/texture/heroParty/body_coatless.png",
		"./resource/texture/heroParty/body_coatless.png",
		DEFAULT_SHININESS,
		"./resource/texture/planNormalTexture.jpg");
}

void CTestState::AddObjectToManager()
{
	int floor = ObjectManager()->AddObject<CGameObject>(
		glm::vec3(0.f, -2.5f, 0.f),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(1.f, 1.f, 1.f),
		10000);
	ObjectManager()->GetGameObject<CGameObject>(floor)->SetModelIndex(m_floorModelID);
	ObjectManager()->GetGameObject<CGameObject>(floor)->SetTextureIndex(m_floorTextureID);
	ObjectManager()->GetGameObject<CGameObject>(floor)->SetType(TYPE_FLOOR);

	int boss = ObjectManager()->AddObject<CGameObject>(
		glm::vec3(0.f, -2.5f, 0.f),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(1.f, 1.f, 1.f),
		1000);
	ObjectManager()->GetGameObject<CGameObject>(boss)->SetModelIndex(m_enemyModelID);
	ObjectManager()->GetGameObject<CGameObject>(boss)->SetTextureIndex(m_enemyTextureID);
	ObjectManager()->GetGameObject<CGameObject>(boss)->SetVolume(glm::vec3(5.0f, 1.3f, 5.0f));
	ObjectManager()->GetGameObject<CGameObject>(boss)->SetBorder(true);

	int box = ObjectManager()->AddObject<CTestObject>(
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(1.f, 1.f, 1.f),
		100);
	ObjectManager()->GetGameObject<CTestObject>(box)->SetModelIndex(m_boxModelID);
	ObjectManager()->GetGameObject<CTestObject>(box)->SetTextureIndex(m_boxTextureID);
	ObjectManager()->GetGameObject<CTestObject>(box)->SetVolume(glm::vec3(1.0f, 1.0f, 1.0f));
	ObjectManager()->GetGameObject<CTestObject>(box)->SetType(TYPE_ENEMY);
	ObjectManager()->GetGameObject<CTestObject>(box)->SetState(STATE_FALLING);
	ObjectManager()->GetGameObject<CGameObject>(box)->SetBorder(true);

	for (int t = 2; t < 6; ++t) {
		for (float i = 0; i < 361; i = i + ((float)72 / (float)t))
		{
			int enemy = ObjectManager()->AddObject<CGameObject>(
				glm::vec3(glm::cos(glm::radians(i)) * 20 * t - glm::cos(glm::radians(i)) * 10
					, -1.5f, glm::sin(glm::radians(i)) * 20 * t - glm::sin(glm::radians(i)) * 10),
				glm::vec3(-90.f, 0.f, 0.f),
				glm::vec3(1.f, 1.f, 1.f),
				100);
			ObjectManager()->GetGameObject<CGameObject>(enemy)->SetModelIndex(m_enemy2ModelID);
			ObjectManager()->GetGameObject<CGameObject>(enemy)->SetTextureIndex(m_enemy2TextureID);
			//ObjectManager()->GetGameObject<CGameObject>(enemy)->SetVolume(glm::vec3(2.0f, 2.0f, 3.0f));

			int collision = ObjectManager()->AddObject<CGameObject>(
				glm::vec3(glm::cos(glm::radians(i)) * 20 * t - glm::cos(glm::radians(i)) * 10
					, -2.8f, glm::sin(glm::radians(i)) * 20 * t - glm::sin(glm::radians(i)) * 10),
				glm::vec3(0.f, 0.f, 0.f),
				glm::vec3(1.f, 1.f, 1.f),
				100);
			ObjectManager()->GetGameObject<CGameObject>(collision)->SetVolume(glm::vec3(2.5f, 1.0f, 2.5f));
			ObjectManager()->GetGameObject<CGameObject>(collision)->SetWhetherToRender(false);

			collision = ObjectManager()->AddObject<CGameObject>(
				glm::vec3(glm::cos(glm::radians(i)) * 20 * t - glm::cos(glm::radians(i)) * 10
					, -0.5f, glm::sin(glm::radians(i)) * 20 * t - glm::sin(glm::radians(i)) * 10),
				glm::vec3(0.f, 0.f, 0.f),
				glm::vec3(1.f, 1.f, 1.f),
				100);
			ObjectManager()->GetGameObject<CGameObject>(collision)->SetVolume(glm::vec3(1.0f, 2.0f, 1.0f));
			ObjectManager()->GetGameObject<CGameObject>(collision)->SetWhetherToRender(false);
		}
	}

	MakeGrassInstance();

	// 주인공 추가
	int heroID = ObjectManager()->AddObject<CHeroObject>(
		glm::vec3(5.0f, -2.5f, 5.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		60.0f);
	m_pHeroObject = ObjectManager()->GetGameObject<CHeroObject>(heroID);
	m_pHeroObject->SetVolume(glm::vec3(0.8f, 1.65f, 0.4f));
	m_pHeroObject->SetBorder(true);
}

void CTestState::MakeGrassInstance()
{
	// 잔디 인스턴스 모델 변환 저장
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[GRASS_AMOUNT_TITLE];
	int index = 0;
	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_real_distribution<float> urd(0.5f, 1.5f);

	for (int t = 80; t < 400; ++t)
	{
		for (float i = 0; i < 361; i = i + ((float)36 / (float)t))
		{
			glm::mat4 trans = glm::mat4(1.0f);
			glm::mat4 scale = glm::mat4(1.0f);

			trans = glm::translate(trans,
				glm::vec3(glm::cos(glm::radians(i)) * 0.25 * t - 1.0 + urd(dre),
					-4.0f, glm::sin(glm::radians(i)) * 0.25 * t - 1.0 + urd(dre)));
			float size = urd(dre);
			scale = glm::scale(scale, glm::vec3(size, size, size));

			modelMatrices[index] = trans * scale;
			index++;
		}
	}
	//std::cout << index << std::endl;

	glGenBuffers(1, &m_GrassSSBOModelMatrices);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_GrassSSBOModelMatrices);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::mat4) * GRASS_AMOUNT_TITLE, modelMatrices, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_GrassSSBOModelMatrices);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	delete[] modelMatrices;
}
