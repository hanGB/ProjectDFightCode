/*
* GameLight
*	���� �� ���� ȿ���� �����ϴ� class
*/
#pragma once
#include "Renderer.h"
#include "Globals.h"

class CGameLight {
public:
	CGameLight();
	~CGameLight();

private:
	CRenderer* m_pRenderer = NULL;

	// ���� ����
	glm::vec4 m_vLightAmbient;
	glm::vec4 m_vLightDiffuse;
	glm::vec4 m_vLightSpecular;

public:
	virtual void UseLight(int shaderType) = 0;

	void SetAmbientLight(glm::vec4 ambient);
	void SetDiffuseLight(glm::vec4 diffuse);
	void SetSpecularLight(glm::vec4 specular);

	glm::vec4 GetAmbientLight() const;
	glm::vec4 GetDiffuseLight() const;
	glm::vec4 GetSpecularLight() const;

	CRenderer* Renderer() { return m_pRenderer; }
};