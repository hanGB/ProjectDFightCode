#include "stdafx.h"
#include "Camera.h"
#include "Globals.h"

CCamera::CCamera()
{
	m_pRenderer = CRenderer::GetInstance();
}

CCamera::~CCamera()
{

}

void CCamera::UseCamera()
{
	//std::cout << "ī�޶� ��ǥ: " << m_vCameraPos.x << ", " << m_vCameraPos.y << ", " << m_vCameraPos.z << "\n";

	int x, y;
	m_pRenderer->GetDisplayResolution(&x, &y);

	// ���� ��ȯ ���
	glm::mat4 projection = glm::perspective(
		glm::radians(m_fFOV), (float)x / (float)y, 0.1f, 300.f);

	// ī�޶� ��ȯ ���
	glm::mat4 view = glm::lookAt(
		m_vCameraPos,					// ī�޶��� ���� ��ǥ ��ġ
		m_vCameraPos + m_vCameraDir,	// ī�޶� ���� ��ġ
		m_vCameraUp						// ī�޶��� �� ����
	);

	GLuint shader;

	// ���� ��ȯ, �� ��ȯ, ī�޶� ��ġ ����
	int shaderIds[] = {
		DEFAULT_SHADER,
		INSTANCE_SHADER,
		BORDER_SHADER,
		BOUNDING_BOX_SHADER,
		BILLBOARD_SHADER,
		BILLBOARD_SPRITE_SHADER,
		BILLBOARD_TEXT_SHADER,
		SKYBOX_SHADER,
		DAMAGE_ZONE_SHADER,
		WATER_SHADER,
		GRAVITY_PARTICLE_SHADER
	};

	for (int i = 0; i < sizeof(shaderIds) / sizeof(int); ++i) {
		shader = m_pRenderer->GetShaderProgram(shaderIds[i]);
		SetUniformDataInShader(shader, view, projection);
	}

	// ������Ʈ�� Ȱ��ȭ�� ���� ��ȣ �ѱ�
	int shaderObjectIds[] = {
		DEFAULT_SHADER,
		INSTANCE_SHADER
	};

	for (int i = 0; i < sizeof(shaderObjectIds) / sizeof(int); ++i) {
		shader = m_pRenderer->GetShaderProgram(shaderObjectIds[i]);
		SetUniformDataInShaderForObject(shader);
	}

	// �����带 ���� ī�޶� right, up ���� ����
	int shaderBillboardIds[] = {
		BILLBOARD_SHADER,
		BILLBOARD_SPRITE_SHADER,
		BILLBOARD_TEXT_SHADER,
		GRAVITY_PARTICLE_SHADER
	};

	for (int i = 0; i < sizeof(shaderBillboardIds) / sizeof(int); ++i) {
		shader = m_pRenderer->GetShaderProgram(shaderBillboardIds[i]);
		SetUniformDataInShaderForBillboard(shader);
	}

	// Ư���� ������ �� ����

	// ��ī�̹ڽ� ���̴�
	shader = m_pRenderer->GetShaderProgram(SKYBOX_SHADER);
	glUseProgram(shader);
	glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(view));
	GLuint viewNoTranslationLocation = glGetUniformLocation(shader, "u_mViewTransform");
	glUniformMatrix4fv(viewNoTranslationLocation, 1, GL_FALSE, &viewNoTranslation[0][0]);

	glUseProgram(0);
}

void CCamera::SetCamera(glm::vec3 cameraPos, 
	glm::vec3 cameraDir, glm::vec3 cameraRight, glm::vec3 cameraUp, 
	float fov)
{
	m_vCameraPos = cameraPos;

	m_vCameraDir = cameraDir;
	m_vCameraRight = cameraRight;
	m_vCameraUp = cameraUp;

	m_fFOV = fov;
}

void CCamera::GetCameraPositionAndDirection(glm::vec3* camerPos, glm::vec3* cameraDir)
{
	*camerPos = m_vCameraPos;
	*cameraDir = m_vCameraDir;
}

glm::vec3 CCamera::GetCameraPosition() const
{
	return m_vCameraPos;
}

void CCamera::SetCameraPosition(glm::vec3 cameraPos)
{
	m_vCameraPos = cameraPos;
}

void CCamera::SetQuarterViewCamera()
{
	if (!m_bIsQuarterViewComputeEnd)
	{
		m_vQuarterViewCameraDirection = glm::vec3(
			cos(m_fQuarterViewCameraVerticalAngle) * sin(m_fQuarterViewCameraHorizontalAngle),
			sin(m_fQuarterViewCameraVerticalAngle),
			cos(m_fQuarterViewCameraVerticalAngle) * cos(m_fQuarterViewCameraHorizontalAngle));

		m_vQuarterViewCameraRight = glm::vec3(
			sin(m_fQuarterViewCameraHorizontalAngle - PI_VALUE / 2.0f),
			0,
			cos(m_fQuarterViewCameraHorizontalAngle - PI_VALUE / 2.0f));

		// Up ����
		m_vQuarterViewCameraUp = glm::cross(m_vQuarterViewCameraRight, m_vQuarterViewCameraDirection);
		m_bIsQuarterViewComputeEnd = true;
	}

	// ��� ��� ����
	m_vCameraDir = m_vQuarterViewCameraDirection;
	m_vCameraRight = m_vQuarterViewCameraRight;
	m_vCameraUp = m_vQuarterViewCameraUp;
	m_fFOV = 60.0f;
}

void CCamera::SetQuarterCameraAngleWithDistance(float distance, float minDistance, float angleChangeStartDistance)
{
	// �ִ� �� �� �� ���� ����
	if (distance < angleChangeStartDistance) {
		float angle = -PI_VALUE / 4 * ((distance - minDistance) / angleChangeStartDistance);
		m_fQuarterViewCameraVerticalAngle = angle;
	}
	else {
		m_fQuarterViewCameraVerticalAngle = -PI_VALUE / 4.0f;
	}

	// ���� ������ �ٽ� ���
	m_bIsQuarterViewComputeEnd = false;	
}

void CCamera::SetQuarterCameraAngle(float horizontalAngle, float verticalAngle)
{
	m_fQuarterViewCameraHorizontalAngle = horizontalAngle;
	m_fQuarterViewCameraVerticalAngle = verticalAngle;

	// ���� ������ �ٽ� ���
	m_bIsQuarterViewComputeEnd = false;
}

void CCamera::TestCameraSetting()
{
	m_vTestCameraDirection = glm::vec3(
		cos(m_fTestCameraVerticalAngle) * sin(m_fTestCameraHorizontalAngle),
		sin(m_fTestCameraVerticalAngle),
		cos(m_fTestCameraVerticalAngle) * cos(m_fTestCameraHorizontalAngle));

	m_vTestCameraRight = glm::vec3(
		sin(m_fTestCameraHorizontalAngle - PI_VALUE / 2.0f),
		0,
		cos(m_fTestCameraHorizontalAngle - PI_VALUE / 2.0f));

	// Up ����
	glm::vec3 up = glm::cross(m_vTestCameraRight, m_vTestCameraDirection);

	// ��� ��� ����
	m_vCameraDir = m_vTestCameraDirection;
	m_vCameraRight = m_vTestCameraRight;
	m_vCameraUp = up;
	m_vCameraPos = m_vTestCameraPosition;
	m_fFOV = m_fTestCameraFov;
}

void CCamera::TestCameraControl(float elapsedTimeInSec, GameInputs* inputs, bool mouse)
{
	// ���콺 �̵��� ���� ī�޶� ȸ�� ���
	if (mouse)
	{
		int x, y;
		m_pRenderer->GetDisplayResolution(&x, &y);

		m_fTestCameraHorizontalAngle +=
			m_fTestCameraMouseSpeed * elapsedTimeInSec * float(x / 2 - inputs->MouseXPos);

		m_fTestCameraVerticalAngle +=
			m_fTestCameraMouseSpeed * elapsedTimeInSec * float(y / 2 - inputs->MouseYPos);

		// ���� �� ����
		m_fTestCameraVerticalAngle = glm::clamp(m_fTestCameraVerticalAngle, -PI_VALUE / 2, PI_VALUE / 2);
	}

	// fov ����
	if (inputs->KeyPlus)
		m_fTestCameraFov = m_fTestCameraFov + 50 * elapsedTimeInSec;
	if (inputs->KeyMinus)
		m_fTestCameraFov = m_fTestCameraFov - 50 * elapsedTimeInSec;

	m_fTestCameraFov = glm::clamp(m_fTestCameraFov, 20.f / 2, 100.f);

	// ī�޶� �̵� ���
	if (inputs->KeyW)
	{
		m_vTestCameraPosition += m_vTestCameraDirection * elapsedTimeInSec * m_fTestCameraSpeed;
	}
	if (inputs->KeyS)
	{
		m_vTestCameraPosition -= m_vTestCameraDirection * elapsedTimeInSec * m_fTestCameraSpeed;
	}
	if (inputs->KeyA)
	{
		m_vTestCameraPosition -= m_vTestCameraRight * elapsedTimeInSec * m_fTestCameraSpeed;
	}
	if (inputs->KeyD)
	{
		m_vTestCameraPosition += m_vTestCameraRight * elapsedTimeInSec * m_fTestCameraSpeed;
	}

	// ���� ���ͷ� �ʿ� ���� ��� �� ����
	TestCameraSetting();
}

void CCamera::TestCameraWithFixedYControl(float elapsedTimeInSec, GameInputs* inputs, bool mouse)
{
	// ���콺 �̵��� ���� ī�޶� ȸ�� ���
	if (mouse)
	{
		int x, y;
		m_pRenderer->GetDisplayResolution(&x, &y);

		m_fTestCameraHorizontalAngle +=
			m_fTestCameraMouseSpeed * elapsedTimeInSec * float(x / 2 - inputs->MouseXPos);

		m_fTestCameraVerticalAngle +=
			m_fTestCameraMouseSpeed * elapsedTimeInSec * float(y / 2 - inputs->MouseYPos);

		// ���� �� ����
		m_fTestCameraVerticalAngle = glm::clamp(m_fTestCameraVerticalAngle, -PI_VALUE / 2, PI_VALUE / 2);
	}

	// fov ����
	if (inputs->KeyPlus)
		m_fTestCameraFov = m_fTestCameraFov + 50 * elapsedTimeInSec;
	if (inputs->KeyMinus)
		m_fTestCameraFov = m_fTestCameraFov - 50 * elapsedTimeInSec;

	m_fTestCameraFov = glm::clamp(m_fTestCameraFov, 20.f / 2, 100.f);

	// ī�޶� �̵� ���
	if (inputs->KeyW)
	{
		m_vTestCameraPosition += glm::vec3(m_vTestCameraDirection.x, 0.0f, m_vTestCameraDirection.z)
			* elapsedTimeInSec * m_fTestCameraSpeed;
	}
	if (inputs->KeyS)
	{
		m_vTestCameraPosition -= glm::vec3(m_vTestCameraDirection.x, 0.0f, m_vTestCameraDirection.z) 
			* elapsedTimeInSec * m_fTestCameraSpeed;
	}
	if (inputs->KeyA)
	{
		m_vTestCameraPosition -= glm::vec3(m_vTestCameraRight.x, 0.0f, m_vTestCameraRight.z) 
			* elapsedTimeInSec * m_fTestCameraSpeed;
	}
	if (inputs->KeyD)
	{
		m_vTestCameraPosition += glm::vec3(m_vTestCameraRight.x, 0.0f, m_vTestCameraRight.z)
			* elapsedTimeInSec * m_fTestCameraSpeed;
	}

	// ���� ���ͷ� �ʿ� ���� ��� �� ����
	TestCameraSetting();
}

void CCamera::SetTestCameraPosition(glm::vec3 pos)
{
	m_vTestCameraPosition = pos;
}

void CCamera::SetAtiveAreaNumber(int number)
{
	m_iActiveAreaNumber = number;
}

void CCamera::SetUniformDataInShader(GLuint shader, glm::mat4 view, glm::mat4 projection)
{
	glUseProgram(shader);

	int viewLocation = glGetUniformLocation(shader, "u_mViewTransform");
	int projectionLocation = glGetUniformLocation(shader, "u_mProjectionTransform");
	int camaraPositionLocation = glGetUniformLocation(shader, "u_vCameraPosition");

	if (viewLocation >= 0) glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
	if (projectionLocation >= 0) glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	if (camaraPositionLocation >= 0) glUniform3f(camaraPositionLocation, m_vCameraPos.x, m_vCameraPos.y, m_vCameraPos.z);
}

void CCamera::SetUniformDataInShaderForBillboard(GLuint shader)
{
	glUseProgram(shader);

	int cameraRightLocation = glGetUniformLocation(shader, "u_vCameraRight");
	int cameraUpLocation = glGetUniformLocation(shader, "u_vCameraUp");

	if (cameraRightLocation >= 0) glUniform3f(cameraRightLocation, m_vCameraRight.x, m_vCameraRight.y, m_vCameraRight.z);
	if (cameraUpLocation >= 0) glUniform3f(cameraUpLocation, m_vCameraUp.x, m_vCameraUp.y, m_vCameraUp.z);
}

void CCamera::SetUniformDataInShaderForObject(GLuint shader)
{
	glUseProgram(shader);

	int numberLocation = glGetUniformLocation(shader, "u_iActiveArea");
	glUniform1i(numberLocation, m_iActiveAreaNumber);
}
