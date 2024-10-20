#version 330 core

layout(location = 0) in vec3 a_vVertexPosition;
layout(location = 1) in vec2 a_vVertexUV;
layout(location = 2) in vec3 a_vVertexNormal;

out vec3 vFragmentPos;
out vec3 vNormal;
out vec4 vFragmentPosLightSpace;

uniform mat4 u_mModelTransform;
uniform mat4 u_mViewTransform;	
uniform mat4 u_mProjectionTransform;
uniform mat4 u_mLightSpaceMatrix;
uniform float u_fTime;

const float PI_VALUE = 3.141592f;

void main()
{
	// 월드 좌표계에서의 정점 좌표
	vFragmentPos = vec3(u_mModelTransform * vec4(a_vVertexPosition, 1.0));

	mat3 modelTansformForNormal = mat3(transpose(inverse(u_mModelTransform)));

	// 월드 좌표계에서의 노멀 벡터 (스캐일 사용시 역 전치 사용)
	vNormal = modelTansformForNormal * a_vVertexNormal;
	
	vFragmentPosLightSpace = u_mLightSpaceMatrix * vec4(vFragmentPos, 1.0);

	// 물 움직임 설정
	vec3 movement = vec3(0.0f);

//	vec3 pos = a_vVertexPosition;
//	float dis = pos.x;
//
//	float value = 10 * 2 * PI_VALUE * dis;
//	float height = sin(value + u_fTime) * 0.3f;
//
//	movement = vec3(0.0f, height, 0.0f);

	gl_Position = 
		u_mProjectionTransform * u_mViewTransform * u_mModelTransform * vec4(a_vVertexPosition + movement, 1.0);
}