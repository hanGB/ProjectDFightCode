#include "stdafx.h"
#include "GameLight.h"

CGameLight::CGameLight()
{
	m_pRenderer = CRenderer::GetInstance();
}

CGameLight::~CGameLight()
{

}

void CGameLight::SetAmbientLight(glm::vec4 ambient)
{
	m_vLightAmbient = ambient;
}

void CGameLight::SetDiffuseLight(glm::vec4 diffuse)
{
	m_vLightDiffuse = diffuse;
}

void CGameLight::SetSpecularLight(glm::vec4 specular)
{
	m_vLightSpecular = specular;
}

glm::vec4 CGameLight::GetAmbientLight() const
{
	return m_vLightAmbient;
}

glm::vec4 CGameLight::GetDiffuseLight() const
{
	return m_vLightDiffuse;
}

glm::vec4 CGameLight::GetSpecularLight() const
{
	return m_vLightSpecular;
}
