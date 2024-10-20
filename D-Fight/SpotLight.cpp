#include "stdafx.h"
#include "SpotLight.h"

CSpotLight::CSpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec4 color)
{
	m_vPosition = pos;
	m_vDirection = dir;

	SetAmbientLight(glm::vec4(0.01f, 0.01f, 0.01f, 0.01f));
	SetDiffuseLight(color);
	SetSpecularLight(color);

	m_fCutOff = 0.91f;
	m_fOutherCutOff = 0.82f;
	m_fDistanceBias = 0.05f;
}

CSpotLight::~CSpotLight()
{
}

void CSpotLight::UseLight(int shaderType)
{
	GLuint shaderProgramID = Renderer()->GetShaderProgram(shaderType);

	GLuint dir = glGetUniformLocation(shaderProgramID, "u_spotLight.direction");
	GLuint pos = glGetUniformLocation(shaderProgramID, "u_spotLight.position");

	GLuint ambient = glGetUniformLocation(shaderProgramID, "u_spotLight.ambient");
	GLuint diffuse = glGetUniformLocation(shaderProgramID, "u_spotLight.diffuse");
	GLuint specular = glGetUniformLocation(shaderProgramID, "u_spotLight.specular");

	GLuint cutOff = glGetUniformLocation(shaderProgramID, "u_spotLight.cutOff");
	GLuint outherCutOff = glGetUniformLocation(shaderProgramID, "u_spotLight.outherCutOff");

	GLuint distanceBias = glGetUniformLocation(shaderProgramID, "u_spotLight.distanceBias");

	glUniform3f(dir, m_vDirection.x, m_vDirection.y, m_vDirection.z);
	glUniform3f(pos, m_vPosition.x, m_vPosition.y, m_vPosition.z);

	if (shaderType == INSTANCE_SHADER) {
		glUniform3f(ambient, GetAmbientLight().x, GetAmbientLight().y, GetAmbientLight().z);
		glUniform3f(diffuse, GetDiffuseLight().x, GetDiffuseLight().y, GetDiffuseLight().z);
	}
	else {
		glUniform4f(ambient, GetAmbientLight().x, GetAmbientLight().y, GetAmbientLight().z, GetSpecularLight().w);
		glUniform4f(diffuse, GetDiffuseLight().x, GetDiffuseLight().y, GetDiffuseLight().z, GetSpecularLight().w);
		glUniform4f(specular, GetSpecularLight().x, GetSpecularLight().y, GetSpecularLight().z, GetSpecularLight().w);
	}

	glUniform1f(cutOff, m_fCutOff);
	glUniform1f(outherCutOff, m_fOutherCutOff);

	glUniform1f(distanceBias, m_fDistanceBias);

}

glm::vec3 CSpotLight::GetPosition() const
{
	return m_vPosition;
}

void CSpotLight::SetPosition(glm::vec3 pos)
{
	m_vPosition = pos;
}

glm::vec3 CSpotLight::GetDirection() const
{
	return m_vDirection;
}

void CSpotLight::SetDirection(glm::vec3 dir)
{
	m_vDirection = dir;
}

void CSpotLight::SetCutOff(float cutOff)
{
	m_fCutOff = cutOff;
}
