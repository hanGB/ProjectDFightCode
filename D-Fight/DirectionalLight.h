#pragma once
#include "GameLight.h"

class CDirectionalLight : public CGameLight {
public:
	CDirectionalLight(glm::vec3 pos, glm::vec3 dir, glm::vec4 color);
	~CDirectionalLight();

private:
	glm::vec3 m_vPosition;
	glm::vec3 m_vDirection;
	glm::vec4 m_vSkyLightColor;

	// �׸��� ���� ����(�㿡 �׸��� ���ϰ� �ϱ� ���� ���)
	float m_fShadowStrength;
	
public:
	void UseLight(int shaderTpye);
	void SetLightSpaceMatrix(int type);

	glm::vec3 GetDirection() const;
	void SetDirection(glm::vec3 dir);
	glm::vec3 GetPosition() const;
	void SetPosition(glm::vec3 pos);
	glm::vec4 GetSkyLightColor() const;
	void SetSkyLightColor(glm::vec4 color);

	void SetShadowStrength(float strength);
};