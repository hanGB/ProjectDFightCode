#include "stdafx.h"
#include "DirectionalLight.h"

CDirectionalLight::CDirectionalLight(glm::vec3 pos, glm::vec3 dir, glm::vec4 color)
{
	m_vPosition = pos;
	m_vDirection = dir;

	SetAmbientLight(glm::vec4(0.05f, 0.05f, 0.05f, 1.0f));
	SetDiffuseLight(color);
	SetSpecularLight(glm::vec4(1.f, 1.0f, 1.0f, 1.0f));

	m_fShadowStrength = 1.0f;
}

CDirectionalLight::~CDirectionalLight()
{
}

void CDirectionalLight::UseLight(int shaderType)
{
	GLuint shaderProgramID = Renderer()->GetShaderProgram(shaderType);

	GLuint dir = glGetUniformLocation(shaderProgramID, "u_dirLight.direction");

	GLuint ambient = glGetUniformLocation(shaderProgramID, "u_dirLight.ambient");
	GLuint diffuse = glGetUniformLocation(shaderProgramID, "u_dirLight.diffuse");
	GLuint specular = glGetUniformLocation(shaderProgramID, "u_dirLight.specular");
	GLuint skylight = glGetUniformLocation(shaderProgramID, "u_dirLight.skylight");

	glUniform3f(dir, m_vDirection.x, m_vDirection.y, m_vDirection.z);

	if (shaderType == INSTANCE_SHADER) {
		glUniform3f(ambient, GetAmbientLight().x, GetAmbientLight().y, GetAmbientLight().z);
		glUniform3f(diffuse, GetDiffuseLight().x, GetDiffuseLight().y, GetDiffuseLight().z);
		glUniform3f(skylight, m_vSkyLightColor.x, m_vSkyLightColor.y, m_vSkyLightColor.z);
	}
	else {	
		glUniform4f(ambient, GetAmbientLight().x, GetAmbientLight().y, GetAmbientLight().z, GetSpecularLight().w);
		glUniform4f(diffuse, GetDiffuseLight().x, GetDiffuseLight().y, GetDiffuseLight().z, GetSpecularLight().w);
		glUniform4f(specular, GetSpecularLight().x, GetSpecularLight().y, GetSpecularLight().z, GetSpecularLight().w);
		glUniform4f(skylight, m_vSkyLightColor.x, m_vSkyLightColor.y, m_vSkyLightColor.z, m_vSkyLightColor.w);
	}

	GLuint shadowStrength = glGetUniformLocation(shaderProgramID, "u_dirLight.shadowStrength");

	GLuint shadowMapLocation = glGetUniformLocation(shaderProgramID, "u_shadowMap");

	GLuint fbo, map;
	Renderer()->GetDepthMap(&fbo, &map);

	glUniform1i(shadowMapLocation, SHADOW_TEXTURE_NUMBER_START + 0);
	glActiveTexture(GL_TEXTURE0 + SHADOW_TEXTURE_NUMBER_START + 0);
	glBindTexture(GL_TEXTURE_2D, map);

	glUniform1f(shadowStrength, m_fShadowStrength);
}

void CDirectionalLight::SetLightSpaceMatrix(int type)
{
	GLuint shaderProgramID = Renderer()->GetShaderProgram(type);

	GLuint lightMatrix = glGetUniformLocation(shaderProgramID, "u_mLightSpaceMatrix");

	float nearPlane = 0.1f, farPlane = 400.0f;

	glm::mat4 lightProjection = glm::ortho(-150.0f, 150.0f, -150.0f, 150.0f, nearPlane, farPlane);

	glm::mat4 lightView = glm::lookAt(
		glm::vec3(m_vPosition.x, m_vPosition.y, m_vPosition.z),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;
	 
	glUniformMatrix4fv(lightMatrix, 1, GL_FALSE, &lightSpaceMatrix[0][0]);
}

glm::vec3 CDirectionalLight::GetDirection() const
{
	return m_vDirection;
}

void CDirectionalLight::SetPosition(glm::vec3 pos)
{
	m_vPosition = pos;
}

glm::vec4 CDirectionalLight::GetSkyLightColor() const
{
	return m_vSkyLightColor;
}

void CDirectionalLight::SetSkyLightColor(glm::vec4 color)
{
	m_vSkyLightColor = color;
}

void CDirectionalLight::SetShadowStrength(float strength)
{
	m_fShadowStrength = strength;
}

void CDirectionalLight::SetDirection(glm::vec3 dir)
{
	m_vDirection = dir;
}

glm::vec3 CDirectionalLight::GetPosition() const
{
	return m_vPosition;
}
