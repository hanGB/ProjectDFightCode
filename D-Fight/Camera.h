/*
* Camera
*/

#pragma once
#include "Renderer.h"
#include "Globals.h"

class CCamera
{
public:
	CCamera();
	~CCamera();

private:
	CRenderer* m_pRenderer = NULL;

	glm::vec3 m_vCameraPos;

	glm::vec3 m_vCameraDir;
	glm::vec3 m_vCameraRight;
	glm::vec3 m_vCameraUp;

	float m_fFOV;

	// ���ͺ� ī�޶� ������ ���� ����
	// ����: 180�� ����
	float m_fQuarterViewCameraHorizontalAngle = PI_VALUE;
	// ������: -45�� ����
	float m_fQuarterViewCameraVerticalAngle = -PI_VALUE / 4;

	glm::vec3 m_vQuarterViewCameraDirection;
	glm::vec3 m_vQuarterViewCameraRight;
	glm::vec3 m_vQuarterViewCameraUp;
	bool m_bIsQuarterViewComputeEnd = false;

	// �׽�Ʈ ī�޶� ������ ���� ����
	// ���� ��ǥ
	glm::vec3 m_vTestCameraPosition = glm::vec3(0, 0, 5);
	// ī�޶� ����
	glm::vec3 m_vTestCameraDirection;
	// ī�޶� ������ ����
	glm::vec3 m_vTestCameraRight;

	// ����: -Z ����
	float m_fTestCameraHorizontalAngle = PI_VALUE;
	// ������: 0
	float m_fTestCameraVerticalAngle = 0.0f;
	// FOV��: �ʱ� 45.0��
	float m_fTestCameraFov = 45.0f;
	// ī�޶� �̵� ���ǵ�
	float m_fTestCameraSpeed = 3.0f;
	// ī�޶� ȸ�� ���ǵ�
	float m_fTestCameraMouseSpeed = 0.05f;

	int m_iActiveAreaNumber = 0;

public:
	// ��� ���̴����� �ʿ��� ī�޶� ���� ����
	void UseCamera();

	void SetCamera(glm::vec3 cameraPos, 
		glm::vec3 cameraDir, glm::vec3 cameraRight, glm::vec3 cameraUp, 
		float fov);

	void GetCameraPositionAndDirection(glm::vec3* camerPos, glm::vec3* cameraDir);
	glm::vec3 GetCameraPosition() const;

	void SetCameraPosition(glm::vec3 cameraPos);
	// ���� 180��, ������ -45�� ī�޶� ����
	void SetQuarterViewCamera();
	// �Ÿ��� ���� ���� ���
	void SetQuarterCameraAngleWithDistance(float distance, float minDistance, float angleChangeStartDistance);
	// ���� ����
	void SetQuarterCameraAngle(float horizontalAngle, float verticalAngle);

	// �׽�Ʈ ī�޶�(1��Ī wasd, ���콺 ȸ������ ����)
	void TestCameraSetting();
	void TestCameraControl(float elapsedTimeInSec, GameInputs* inputs, bool mouse);
	void TestCameraWithFixedYControl(float elapsedTimeInSec, GameInputs * inputs, bool mouse);

	void SetTestCameraPosition(glm::vec3 pos);
	void SetAtiveAreaNumber(int number);

private:
	void SetUniformDataInShader(GLuint shader, glm::mat4 view, glm::mat4 projection);
	void SetUniformDataInShaderForBillboard(GLuint shader);
	void SetUniformDataInShaderForObject(GLuint shader);

};
