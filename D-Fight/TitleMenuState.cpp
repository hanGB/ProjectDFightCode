#include "stdafx.h"
#include "TitleMenuState.h"
#include <random>

CTitleMenuState::CTitleMenuState()
{
	// CGameFramework에서 CGameState를 사용하기 때문에 CGameState의 자식 클래스인 현 클래스에서 CGameFramework를 가져올 것!
	m_pGameFramework = CGameFramework::GetInstance();

	m_pTimeOfDaySkybox = new CTimeOfDaySkybox();

	// 마우스 커서 숨기기
	glutSetCursor(GLUT_CURSOR_NONE);

	Camera()->SetTestCameraPosition(glm::vec3(-10.0f, -1.0f, 45.0f));
	Camera()->TestCameraSetting();
	
	glm::vec3 pos = glm::vec3(100.0f, 0.0f, 0.0f);
	glm::vec3 dir = glm::normalize(pos);

	LoadDirectionalLight(new CDirectionalLight(pos,
		glm::vec3(-dir.x, -dir.y, -dir.z),
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));
	DirectionalLight()->SetSkyLightColor(NIGHT_LIGHT_COLOR);

	LoadPointLight(new CPointLight(glm::vec3(0.0f, 0.f, 0.f), glm::vec4(1.0f, 0.9f, 0.8f, 1.0f)));

	LoadData();
	AddObjectToManager();
}

CTitleMenuState::~CTitleMenuState()
{
}

void CTitleMenuState::ProcessInputs(float elapsedTimeInSec, GameInputs* inputs)
{
	ButtonSelectInputs(elapsedTimeInSec, inputs);

	if (m_ButtonVector[m_iSelectedButton]->SetIsUsed(inputs)) {
		UseButton(m_iSelectedButton);
	}
}

void CTitleMenuState::Update(float elapsedTimeInSec)
{
	m_fTimeForGrass += (elapsedTimeInSec * 2 * DEFAULT_WIND_SPEED);

	m_pTimeOfDaySkybox->Update(elapsedTimeInSec * 2, DirectionalLight());

	CGameState::Update(elapsedTimeInSec);
}

void CTitleMenuState::RenderScene()
{
	Camera()->UseCamera();

	Renderer()->UseShader(DEFAULT_SHADER);
	GLuint lightOn = glGetUniformLocation(Renderer()->GetShaderProgram(DEFAULT_SHADER), "u_bSpotLightOn");
	glUniform1i(lightOn, false);
	Renderer()->UseShader(INSTANCE_SHADER);
	lightOn = glGetUniformLocation(Renderer()->GetShaderProgram(INSTANCE_SHADER), "u_bSpotLightOn");
	glUniform1i(lightOn, false);
	Renderer()->UseShader(WATER_SHADER);
	lightOn = glGetUniformLocation(Renderer()->GetShaderProgram(WATER_SHADER), "u_bSpotLightOn");
	glUniform1i(lightOn, false);

	CGameState::RenderScene();
}

void CTitleMenuState::Enter()
{
	StartState();
}

void CTitleMenuState::Exit()
{
	EndState();
}

void CTitleMenuState::Pause()
{
	EndState();
}

void CTitleMenuState::Resume()
{
	StartState();
}

void CTitleMenuState::NormalRender()
{
	CGameState::NormalRender();

	RenderWaterObject(m_waterModelID);

	Renderer()->UseShader(INSTANCE_SHADER);
	DirectionalLight()->SetLightSpaceMatrix(INSTANCE_SHADER);
	DirectionalLight()->UseLight(INSTANCE_SHADER);
	PointLight()->UseLight(INSTANCE_SHADER);
	Renderer()->RenderInstance(m_grassModelID, m_grassTextureID, 
		m_GrassSSBOModelMatrices, m_fTimeForGrass, GRASS_AMOUNT_TITLE);

	// 스카이 박스 렌더링
	m_pTimeOfDaySkybox->RenderSkybox(DirectionalLight());

	Renderer()->UseShader(GRAVITY_PARTICLE_SHADER);
	ObjectManager()->RenderParticle();
}

void CTitleMenuState::UIRender()
{
	Renderer()->UseShader(UI_SHADER);

	// 타이틀 로고
	Renderer()->RenderTexture2D(m_LogoTextureID,
		glm::vec2(0.125f, 0.6f), glm::vec2(0.0f, 0.0f),
		glm::vec2(480.f, 280), glm::vec4(1.0f));

	int x, y;
	Renderer()->GetDisplayResolution(&x, &y);
	for (int i = 0; i < m_ButtonVector.size(); ++i) {
		float gap = (i * 75.0f + 250.0f) / (float)y;
		float margin = 10.0f / (float)x;
		m_ButtonVector[i]->Render(m_WhiteRectTextureID, glm::vec2(0.125f, 0.6f - gap),
			glm::vec2(250.0f, 35.0f), 0.65f, margin);
	}
}

void CTitleMenuState::LoadData()
{
	// 월드 구성
	m_floorModelID = Renderer()->LoadOBJModelWithIndex("./resource/obj/world/floor2.obj");
	m_floorTextureID = Renderer()->LoadPNGTexture(
		"./resource/texture/world/Grass/Ground037_2K_AmbientOcclusion.jpg",
		"./resource/texture/world/Grass/Ground037_2K_Color.jpg",
		"./resource/texture/AllZeroTexture.jpg", DEFAULT_SHININESS / 8.f,
		"./resource/texture/world/Grass/Ground037_2K_Normal.jpg");

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

	m_grassModelID = Renderer()->LoadOBJModelWithIndex("./resource/obj/world/grass.obj");
	m_grassTextureID = Renderer()->ReadPNGTextureFile("./resource/texture/world/grass_tex.jpg");

	m_titleBGMID = SoundPlayer()->LoadSound("./resource/sound/BGM/cityBGM.mp3");

	// bgm 설정
	m_pBGM = SoundPlayer()->SetBackGroundMusic(m_titleBGMID, 0.5f);

	// 분수대 물 파티클 
	int waterDrop = ObjectManager()->
		AddParticle<CFountainWaterDropParticle>(glm::vec3(0.0f, 11.f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f));
	ObjectManager()->GetParticle<CFountainWaterDropParticle>(waterDrop)->SetRadiusTopAndButtom(0.5f, 3.0f);
	ObjectManager()->GetParticle<CFountainWaterDropParticle>(waterDrop)->SetTextureID(
		Renderer()->ReadPNGTextureFile("./resource/texture/waterDrop.png"));
	ObjectManager()->GetParticle<CFountainWaterDropParticle>(waterDrop)->SetColor(glm::vec4(0.5f, 0.85f, 1.0f, 0.35f));
	ObjectManager()->GetParticle<CFountainWaterDropParticle>(waterDrop)->CreateVAO();

	// 타이틀 로고
	m_LogoTextureID = (int)Renderer()->ReadPNGTextureFile("./resource/ui/titleLogo.png");

	// 흰색 사각형 텍스쳐 로드
	m_WhiteRectTextureID = (int)Renderer()->ReadPNGTextureFile("./resource/ui/whiteRect.png");
}

void CTitleMenuState::AddObjectToManager()
{
	int floor = ObjectManager()->AddNoInteractionObject<CGameObject>(
		glm::vec3(0.f, -5.4f, 0.f),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(1.f, 1.f, 1.f),
		10000);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(floor)->SetModelIndex(m_floorModelID);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(floor)->SetTextureIndex(m_floorTextureID);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(floor)->SetType(TYPE_FLOOR);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(floor)->SetIsCullingFace(false);

	int floorLine = ObjectManager()->AddNoInteractionObject<CGameObject>(
		glm::vec3(0.f, -3.5f, 0.f),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		10000);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(floorLine)->SetModelIndex(m_floorLineModelID);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(floorLine)->SetTextureIndex(m_fountainTextureID);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(floorLine)->SetType(TYPE_FLOOR);

	int outLineMountain = ObjectManager()->AddNoInteractionObject<CGameObject>(
		glm::vec3(0.f, -3.5f, 0.f),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(1.f, 0.5f, 1.f),
		10000);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(outLineMountain)->SetModelIndex(m_floorOutLineModelID);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(outLineMountain)->SetTextureIndex(m_floorOutLineTextureID);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(outLineMountain)->SetType(TYPE_FLOOR);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(outLineMountain)->SetIsMadeShadow(false);

	int fountain = ObjectManager()->AddNoInteractionObject<CGameObject>(
		glm::vec3(0.f, -2.0f, 0.f),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.25f, 0.25f, 0.25f),
		10000);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(fountain)->SetModelIndex(m_fountainModelID);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(fountain)->SetTextureIndex(m_fountainTextureID);
	ObjectManager()->GetNoInteractionGameObject<CGameObject>(fountain)->SetType(TYPE_FIXED);

	MakeGrassInstance();

	m_ButtonVector.push_back(new CButton(L"Start Game"));
	m_ButtonVector.push_back(new CButton(L"Exit"));
}

void CTitleMenuState::MakeGrassInstance()
{
	// 잔디 인스턴스 모델 변환 저장
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[GRASS_AMOUNT_TITLE];
	int index = 0;
	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_real_distribution<float> urd(0.5f, 1.5f);

	for (int t = 50; t < 400; ++t)
	{
		for (float i = 0; i < 361; i = i + ((float)72.f / (float)t))
		{
			glm::mat4 trans = glm::mat4(1.0f);
			glm::mat4 scale = glm::mat4(1.0f);

			trans = glm::translate(trans,
				glm::vec3(glm::cos(glm::radians(i)) * 0.25 * t - 1.0 + urd(dre),
					-4.0f, glm::sin(glm::radians(i)) * 0.25 * t - 1.0 + urd(dre)));
			float size = urd(dre);
			scale = glm::scale(scale, glm::vec3(size, size + 1.0f, size));

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

void CTitleMenuState::RenderWaterObject(int modelID)
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
		glm::vec3(0.0f, -4.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 1.0f, 0.5f),
		cubeMap1, cubeMap2, rate, m_fTimeForGrass);
}

void CTitleMenuState::ButtonSelectInputs(float elapsedTimeInSec, GameInputs* inputs)
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

void CTitleMenuState::UseButton(int menu)
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

void CTitleMenuState::StartState()
{
	m_pTimeOfDaySkybox->SetTime(10.0f);
	m_fTimeForGrass = 0;
	m_iSelectedButton = 0;
	SoundPlayer()->PlayAndStopBackGroundMusic(m_pBGM, true);
	m_pGameFramework->SetIsEnableToEnterSystemMenu(false);
	glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
}

void CTitleMenuState::EndState()
{
	m_pGameFramework->SetIsEnableToEnterSystemMenu(true);
	SoundPlayer()->PlayAndStopBackGroundMusic(m_pBGM, false);
	glutSetCursor(GLUT_CURSOR_NONE);
	int x, y;
	Renderer()->GetDisplayResolution(&x, &y);
	glutWarpPointer(x / 2, y / 2);
}
