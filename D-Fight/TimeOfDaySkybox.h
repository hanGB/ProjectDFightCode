#pragma once
#include "Globals.h"
#include "Renderer.h"
#include "DirectionalLight.h"

class CTimeOfDaySkybox {
public:
	CTimeOfDaySkybox();
	~CTimeOfDaySkybox();

private:
	int m_skyboxDayTextureID = EMPTY_VALUE;
	int m_skyboxNightTextureID = EMPTY_VALUE;
	int m_skyboxSunriseTextureID = EMPTY_VALUE;
	int m_skyboxSunsetTextureID = EMPTY_VALUE;
	int m_skyboxRainyTextureID = EMPTY_VALUE;

	float m_fHour = 12.0f;

	bool m_bIsRainy = false;

public:
	void Initialize(CDirectionalLight* dirLight);

	void Update(float elapsedTimeInSec, CDirectionalLight* dirLight);
	void RenderSkybox(CDirectionalLight* dirLight);

	bool IsDayTime() const;
	bool IsPassedHalfDay() const;

	void GetNowCubemap(int* cubeMap1, int* cubeMap2, float* blendingRate);

	void SetTime(float time);

	void SetIsRainy(bool rain);

private:
	void LoadSkyboxCubemapData();

};