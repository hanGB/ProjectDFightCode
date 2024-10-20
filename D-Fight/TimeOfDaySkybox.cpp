#include "stdafx.h"
#include "TimeOfDaySkybox.h"

CTimeOfDaySkybox::CTimeOfDaySkybox()
{
	LoadSkyboxCubemapData();
}

CTimeOfDaySkybox::~CTimeOfDaySkybox()
{
}

void CTimeOfDaySkybox::Initialize(CDirectionalLight* dirLight)
{
	// ±¤¿ø À§Ä¡ º¯°æ
	if (6.0f <= m_fHour && m_fHour < 18.0f)
	{
		dirLight->SetShadowStrength(1.0f);

		float timeOfDay = (m_fHour + 18) * 15;
		glm::vec3 pos = glm::vec3(0.0f, glm::sin(glm::radians(timeOfDay)) * 100.0f,
			glm::cos(glm::radians(180 - timeOfDay)) * 100.0f);
		dirLight->SetPosition(pos);
		glm::vec3 dir = glm::normalize(pos);
		dirLight->SetDirection(glm::vec3(-dir.x, -dir.y, -dir.z));
	}
	else if (18.0f <= m_fHour || m_fHour < 6.0f)
	{
		dirLight->SetShadowStrength(1.3f);

		float timeOfDay = (m_fHour + 18 - 12) * 15;
		glm::vec3 pos = glm::vec3(0.0f, glm::sin(glm::radians(timeOfDay)) * 5 * DEFAULT_AREA_WIDTH,
			glm::cos(glm::radians(180 - timeOfDay)) * 5 * DEFAULT_AREA_WIDTH);
		dirLight->SetPosition(pos);
		glm::vec3 dir = glm::normalize(pos);
		dirLight->SetDirection(glm::vec3(-dir.x, -dir.y, -dir.z));
	}
}

void CTimeOfDaySkybox::Update(float elapsedTimeInSec, CDirectionalLight* dirLight)
{
	// ±¤¿ø À§Ä¡ º¯°æ
	m_fHour = m_fHour + (elapsedTimeInSec / HOUR_TO_SEC_IN_GAME_WORLD);
	if (m_fHour >= 24.0f)
		m_fHour = 0.0f;

	if (6.0f <= m_fHour && m_fHour < 18.0f)
	{
		if (6.0f <= m_fHour && m_fHour <= 6.5f)
		{
			dirLight->SetShadowStrength(1.0f);
		}
		float timeOfDay = (m_fHour + 18) * 15;
		glm::vec3 pos = glm::vec3(0.0f, glm::sin(glm::radians(timeOfDay)) * 5 * DEFAULT_AREA_WIDTH,
			glm::cos(glm::radians(180 - timeOfDay)) * 5 * DEFAULT_AREA_WIDTH);
		dirLight->SetPosition(pos);
		glm::vec3 dir = glm::normalize(pos);
		dirLight->SetDirection(glm::vec3(-dir.x, -dir.y, -dir.z));
	}
	else if (18.0f <= m_fHour || m_fHour < 6.0f)
	{
		if (18.0f <= m_fHour && m_fHour <= 18.5f)
		{
			dirLight->SetShadowStrength(1.25f);
		}
		float timeOfDay = (m_fHour + 18 - 12) * 15;
		glm::vec3 pos = glm::vec3(0.0f, glm::sin(glm::radians(timeOfDay)) * 5 * DEFAULT_AREA_WIDTH,
			glm::cos(glm::radians(180 - timeOfDay)) * 5 * DEFAULT_AREA_WIDTH);
		dirLight->SetPosition(pos);
		glm::vec3 dir = glm::normalize(pos);
		dirLight->SetDirection(glm::vec3(-dir.x, -dir.y, -dir.z));
	}
}

void CTimeOfDaySkybox::RenderSkybox(CDirectionalLight* dirLight)
{
	CRenderer* renderer = CRenderer::GetInstance();

	// ½ºÄ«ÀÌ ¹Ú½º ·»´õ¸µ
	renderer->UseShader(SKYBOX_SHADER);
	
	// ºñ
	if (m_bIsRainy) {
		dirLight->SetSkyLightColor(RAINY_SKY_LIGHT_COLOR);
		renderer->DrawSkybox(m_skyboxRainyTextureID, 0, 0.0f);
		dirLight->SetDiffuseLight(RAINY_LIGHT_COLOR);
		return;
	}

	// ÇÑ³·
	if (m_fHour >= 9.0f && m_fHour < 14.5f) {
		dirLight->SetSkyLightColor(DAYTIME_SKY_LIGHT_COLOR);
		renderer->DrawSkybox(m_skyboxDayTextureID, 0, 0.0f);
		dirLight->SetDiffuseLight(DAYTIME_LIGHT_COLOR);
	}
	// ¿ÀÀü
	else if (m_fHour >= 7.0f && m_fHour < 9.0f) {
		float rate = (m_fHour - 7.0f) / 2.0f;
		dirLight->SetSkyLightColor(DAYTIME_SKY_LIGHT_COLOR * rate + SUNRISE_SKY_LIGHT_COLOR * (1.0f - rate));
		renderer->DrawSkybox(m_skyboxSunriseTextureID, m_skyboxDayTextureID, rate);
		dirLight->SetDiffuseLight(DAYTIME_LIGHT_COLOR * rate + SUNRISE_SUNSET_LIGHT_COLOR * (1.0f - rate));
	}
	// ÀÏÃâ
	else if (m_fHour >= 6.5f && m_fHour < 7.0f) {
		dirLight->SetSkyLightColor(SUNRISE_SKY_LIGHT_COLOR);
		renderer->DrawSkybox(m_skyboxSunriseTextureID, 0, 0.0f);
		dirLight->SetDiffuseLight(SUNRISE_SUNSET_LIGHT_COLOR);
	}
	// »õº®
	else if (m_fHour >= 4.5f && m_fHour < 6.5f) {
		float rate = (m_fHour - 4.5f) / 2.0f;
		dirLight->SetSkyLightColor(SUNRISE_SKY_LIGHT_COLOR * rate + NIGHT_SKY_LIGHT_COLOR * (1.0f - rate));
		renderer->DrawSkybox(m_skyboxNightTextureID, m_skyboxSunriseTextureID, rate);
		dirLight->SetDiffuseLight(SUNRISE_SUNSET_LIGHT_COLOR * rate + NIGHT_LIGHT_COLOR * (1.0f - rate));
	}
	// ¿ÀÈÄ
	else if (m_fHour >= 14.5f && m_fHour < 16.5f) {
		float rate = (m_fHour - 14.5f) / 2.0f;
		dirLight->SetSkyLightColor(SUNSET_SKY_LIGHT_COLOR * rate + DAYTIME_SKY_LIGHT_COLOR * (1.0f - rate));
		renderer->DrawSkybox(m_skyboxDayTextureID, m_skyboxSunsetTextureID, rate);
		dirLight->SetDiffuseLight(SUNRISE_SUNSET_LIGHT_COLOR * rate + DAYTIME_LIGHT_COLOR * (1.0f - rate));
	}
	// ÀÏ¸ô
	else if (m_fHour >= 16.5f && m_fHour < 17.0f) {
		dirLight->SetSkyLightColor(SUNSET_SKY_LIGHT_COLOR);
		renderer->DrawSkybox(m_skyboxSunsetTextureID, 0, 0.0f);
		dirLight->SetDiffuseLight(SUNRISE_SUNSET_LIGHT_COLOR);
	}
	// Àú³á
	else if (m_fHour >= 17.0f && m_fHour < 19.0f) {
		float rate = (m_fHour - 17.0f) / 2.0f;
		dirLight->SetSkyLightColor(NIGHT_SKY_LIGHT_COLOR * rate + SUNSET_SKY_LIGHT_COLOR * (1.0f - rate));
		renderer->DrawSkybox(m_skyboxSunsetTextureID, m_skyboxNightTextureID, rate);
		dirLight->SetDiffuseLight(NIGHT_LIGHT_COLOR * rate + SUNRISE_SUNSET_LIGHT_COLOR * (1.0f - rate));
	}
	// ÇÑ¹ãÁß
	else {
		dirLight->SetSkyLightColor(NIGHT_SKY_LIGHT_COLOR);
		renderer->DrawSkybox(m_skyboxNightTextureID, 0, 0.0f);
		dirLight->SetDiffuseLight(NIGHT_LIGHT_COLOR);
	}
}

bool CTimeOfDaySkybox::IsDayTime() const
{
	if (6.0f <= m_fHour && m_fHour < 18.0f) {
		return true;
	}
	return false;
}

bool CTimeOfDaySkybox::IsPassedHalfDay() const
{
	if (6.0f <= m_fHour && m_fHour <= 6.5f) {
		return true;
	}
	if (18.0f <= m_fHour && m_fHour <= 18.5f) {
		return true;
	}
	return false;
}

void CTimeOfDaySkybox::GetNowCubemap(int* cubeMap1, int* cubeMap2, float* blendingRate)
{
	if (m_bIsRainy) {
		*cubeMap1 = m_skyboxRainyTextureID;
		return;
	}

	// ÇÑ³·
	if (m_fHour >= 9.0f && m_fHour < 14.5f) {
		*cubeMap1 = m_skyboxDayTextureID;
	}
	// ¿ÀÀü
	else if (m_fHour >= 7.0f && m_fHour < 9.0f) {
		*cubeMap1 = m_skyboxSunriseTextureID;
		*cubeMap2 = m_skyboxDayTextureID;
		*blendingRate = (m_fHour - 7.0f) / 2.0f;
	}
	// ÀÏÃâ
	else if (m_fHour >= 6.5f && m_fHour < 7.0f) {
		*cubeMap1 = m_skyboxSunriseTextureID;
	}
	// »õº®
	else if (m_fHour >= 4.5f && m_fHour < 6.5f) {
		*cubeMap1 = m_skyboxNightTextureID;
		*cubeMap2 = m_skyboxSunriseTextureID;
		*blendingRate = (m_fHour - 4.5f) / 2.0f;
	}
	// ¿ÀÈÄ
	else if (m_fHour >= 14.5f && m_fHour < 16.5f) {
		*cubeMap1 = m_skyboxDayTextureID;
		*cubeMap2 = m_skyboxSunsetTextureID;
		*blendingRate = (m_fHour - 14.5f) / 2.0f;
	}
	// ÀÏ¸ô
	else if (m_fHour >= 16.5f && m_fHour < 17.0f) {
		*cubeMap1 = m_skyboxSunsetTextureID;
	}
	// Àú³á
	else if (m_fHour >= 17.0f && m_fHour < 19.0f) {
		*cubeMap1 = m_skyboxSunsetTextureID;
		*cubeMap2 = m_skyboxNightTextureID;
		*blendingRate = (m_fHour - 17.0f) / 2.0f;
	}
	// ÇÑ¹ãÁß
	else {
		*cubeMap1 = m_skyboxNightTextureID;
	}
}

void CTimeOfDaySkybox::SetTime(float time)
{
	m_fHour = time;
}

void CTimeOfDaySkybox::SetIsRainy(bool rain)
{
	m_bIsRainy = rain;
}

void CTimeOfDaySkybox::LoadSkyboxCubemapData()
{
	CRenderer* renderer = CRenderer::GetInstance();

	std::string skyboxDayFiles[] = {
		"./resource/skybox/day/px.png",
		"./resource/skybox/day/nx.png",
		"./resource/skybox/day/py.png",
		"./resource/skybox/day/ny.png",
		"./resource/skybox/day/pz.png",
		"./resource/skybox/day/nz.png"
	};
	m_skyboxDayTextureID = renderer->LoadSkybox("day", skyboxDayFiles);

	std::string skyboxNightFiles[] = {
		"./resource/skybox/night/px.png",
		"./resource/skybox/night/nx.png",
		"./resource/skybox/night/py.png",
		"./resource/skybox/night/ny.png",
		"./resource/skybox/night/pz.png",
		"./resource/skybox/night/nz.png"
	};
	m_skyboxNightTextureID = renderer->LoadSkybox("night", skyboxNightFiles);

	std::string skyboxSunriseFiles[] = {
		"./resource/skybox/sunrise/px.png",
		"./resource/skybox/sunrise/nx.png",
		"./resource/skybox/sunrise/py.png",
		"./resource/skybox/sunrise/ny.png",
		"./resource/skybox/sunrise/pz.png",
		"./resource/skybox/sunrise/nz.png"
	};
	m_skyboxSunriseTextureID = renderer->LoadSkybox("sunrise", skyboxSunriseFiles);

	std::string skyboxSunsetFiles[] = {
		"./resource/skybox/sunset/px.png",
		"./resource/skybox/sunset/nx.png",
		"./resource/skybox/sunset/py.png",
		"./resource/skybox/sunset/ny.png",
		"./resource/skybox/sunset/pz.png",
		"./resource/skybox/sunset/nz.png"
	};
	m_skyboxSunsetTextureID = renderer->LoadSkybox("sunset", skyboxSunsetFiles);

	std::string skyboxRainyFiles[] = {
		"./resource/skybox/rainy/px.png",
		"./resource/skybox/rainy/nx.png",
		"./resource/skybox/rainy/py.png",
		"./resource/skybox/rainy/ny.png",
		"./resource/skybox/rainy/pz.png",
		"./resource/skybox/rainy/nz.png"
	};
	m_skyboxRainyTextureID = renderer->LoadSkybox("rainy", skyboxRainyFiles);
}
