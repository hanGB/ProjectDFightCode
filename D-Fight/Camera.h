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

	// 쿼터뷰 카메라 조작을 위한 변수
	// 수평각: 180도 방향
	float m_fQuarterViewCameraHorizontalAngle = PI_VALUE;
	// 수직각: -45도 방향
	float m_fQuarterViewCameraVerticalAngle = -PI_VALUE / 4;

	glm::vec3 m_vQuarterViewCameraDirection;
	glm::vec3 m_vQuarterViewCameraRight;
	glm::vec3 m_vQuarterViewCameraUp;
	bool m_bIsQuarterViewComputeEnd = false;

	// 테스트 카메라 조작을 위한 변수
	// 현재 좌표
	glm::vec3 m_vTestCameraPosition = glm::vec3(0, 0, 5);
	// 카메라 방향
	glm::vec3 m_vTestCameraDirection;
	// 카메라 오른쪽 벡터
	glm::vec3 m_vTestCameraRight;

	// 수평각: -Z 방향
	float m_fTestCameraHorizontalAngle = PI_VALUE;
	// 수직각: 0
	float m_fTestCameraVerticalAngle = 0.0f;
	// FOV값: 초기 45.0도
	float m_fTestCameraFov = 45.0f;
	// 카메라 이동 스피드
	float m_fTestCameraSpeed = 3.0f;
	// 카메라 회전 스피드
	float m_fTestCameraMouseSpeed = 0.05f;

	int m_iActiveAreaNumber = 0;

public:
	// 모든 쉐이더에게 필요한 카메라 정보 전달
	void UseCamera();

	void SetCamera(glm::vec3 cameraPos, 
		glm::vec3 cameraDir, glm::vec3 cameraRight, glm::vec3 cameraUp, 
		float fov);

	void GetCameraPositionAndDirection(glm::vec3* camerPos, glm::vec3* cameraDir);
	glm::vec3 GetCameraPosition() const;

	void SetCameraPosition(glm::vec3 cameraPos);
	// 수평각 180도, 수직각 -45도 카메라 설정
	void SetQuarterViewCamera();
	// 거리를 통해 각도 계산
	void SetQuarterCameraAngleWithDistance(float distance, float minDistance, float angleChangeStartDistance);
	// 각도 설정
	void SetQuarterCameraAngle(float horizontalAngle, float verticalAngle);

	// 테스트 카메라(1인칭 wasd, 마우스 회전으로 조정)
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
