#include "stdafx.h"
#include "PointLight.h"
#include <vector>
#include <string>

CPointLight::CPointLight(glm::vec3 pos, glm::vec4 color)
{
	m_vPosition = pos;

	SetAmbientLight(glm::vec4(0.01f, 0.01f, 0.01f, 1.f));
	SetDiffuseLight(color);
	SetSpecularLight(color);

	m_fConstant		= 1.f;
	m_fLinear		= 0.045f;
	m_fQuadratic	= 0.0075f;
}

CPointLight::~CPointLight()
{

}

void CPointLight::UseLight(int shaderType)
{
	GLuint shaderProgramID = Renderer()->GetShaderProgram(shaderType);

	GLuint pos = glGetUniformLocation(shaderProgramID, "u_pointLight.position");

	GLuint ambient = glGetUniformLocation(shaderProgramID, "u_pointLight.ambient");
	GLuint diffuse = glGetUniformLocation(shaderProgramID, "u_pointLight.diffuse");
	GLuint specular = glGetUniformLocation(shaderProgramID, "u_pointLight.specular");

	GLuint constant = glGetUniformLocation(shaderProgramID, "u_pointLight.constant");
	GLuint linear = glGetUniformLocation(shaderProgramID, "u_pointLight.linear");
	GLuint quadratic = glGetUniformLocation(shaderProgramID, "u_pointLight.quadratic");

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

	glUniform1f(constant, m_fConstant);
	glUniform1f(linear, m_fLinear);
	glUniform1f(quadratic, m_fQuadratic);


	// 복수의 포인트 라이트에 대한 설정
	GLuint positions = glGetUniformLocation(shaderProgramID, "u_vPointLightPositions");
	GLuint on = glGetUniformLocation(shaderProgramID, "u_bIsPointLightOn");
	GLuint num = glGetUniformLocation(shaderProgramID, "u_iNumoOfPointLights");

	glUniform3fv(positions, m_iNumOfPointLights, (const GLfloat*)m_vPositions);
	glUniform1iv(on, m_iNumOfPointLights, (const GLint*)m_bIsPointLightOn);
	glUniform1i(num, m_iNumOfPointLights);
}

glm::vec3 CPointLight::GetPosition() const
{
	return m_vPosition;
}

void CPointLight::SetPosition(glm::vec3 pos)
{
	m_vPosition = pos;
}

void CPointLight::SetAttenuation(float constant, float linear, float quadratic)
{
	m_fConstant = constant;
	m_fLinear = linear;
	m_fQuadratic = quadratic;
}

void CPointLight::SetPointLights(glm::vec3* pos, int num)
{
	if (m_vPositions != NULL) {
		delete[] m_vPositions;
		delete[] m_bIsPointLightOn;
	}
	m_vPositions = pos;
	m_iNumOfPointLights = num;
	m_bIsPointLightOn = new int[num];
}

void CPointLight::SetIsPointLightsOn(bool on)
{
	memset(m_bIsPointLightOn, (int)on, sizeof(int) * m_iNumOfPointLights);
}
