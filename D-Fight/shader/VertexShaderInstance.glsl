#version 430 core

layout(location = 0) in vec3 a_vVertexPosition;
layout(location = 1) in vec2 a_vVertexUV;
layout(location = 2) in vec3 a_vVertexNormal;

layout(std430, binding = 0) buffer modelMatrices
{
	mat4 b_model[];
};

out vec3 vFragmentPos;
out vec2 vUV;
out vec4 vFragmentPosLightSpace;
out vec2 vVisible;		// bool값은 불가능, int시 오류로 인해 vec2 사용

uniform mat4 u_mViewTransform;	
uniform mat4 u_mProjectionTransform;
uniform mat4 u_mLightSpaceMatrix;
uniform float u_fTime;
uniform int u_iActiveArea;

const float PI_VALUE = 3.141592f;
const int c_AREA_WIDTH = 24;

void main()
{
	float bias = 0.00002f;
	float time = gl_InstanceID * bias + u_fTime;

	// 0 ~ 2파이 사이로 변경
	float rotation = mod(time, 2.f * PI_VALUE);

	if (rotation > PI_VALUE * 3 / 2) {
		rotation = (rotation - PI_VALUE * 3 / 2) * 0.25f + PI_VALUE * 15 / 8;
	}
	else if (rotation > PI_VALUE) {
		rotation = ((PI_VALUE * 3 / 2 - rotation) * 2 + rotation - PI_VALUE * 3 / 2) * 0.25f + PI_VALUE * 15 / 8;
	}
	else if (rotation > PI_VALUE / 2) {
		rotation = (PI_VALUE - rotation) * 0.25f;
	}
	else {
		rotation = rotation * 0.25f;
	}

	// 흔들림을 위한 z축 회전 행렬 생성
	mat4 grassWobbleMatrix = mat4(
		vec4(cos(rotation), sin(rotation), 0.0f, 0.0f), 
		vec4(-sin(rotation), cos(rotation), 0.0f, 0.0f), 
		vec4(0.0f, 0.0f, 1.0f, 0.0f), 
		vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);	

	// 월드 좌표계에서의 정점 좌표
	vFragmentPos = vec3(b_model[gl_InstanceID] * grassWobbleMatrix * vec4(a_vVertexPosition, 1.f));

	// 빛에서 보았을때의 정점 좌표
	vFragmentPosLightSpace = u_mLightSpaceMatrix * vec4(vFragmentPos, 1.0);

	// 텍스쳐 UV 좌표
	vUV = a_vVertexUV;

	vVisible = vec2(1);
	vec3 pos = vFragmentPos; pos.y = 0;
	float len = length(pos);

	if (u_iActiveArea == 1) {
		if (len > c_AREA_WIDTH * 2) {
			vVisible = vec2(0);
		}
	}
	else if (u_iActiveArea != 0) {
		if (c_AREA_WIDTH * u_iActiveArea > len || len > c_AREA_WIDTH * (u_iActiveArea + 1)) {
			vVisible = vec2(0);	
		}
	}

	gl_Position = 
		u_mProjectionTransform * u_mViewTransform * b_model[gl_InstanceID] * grassWobbleMatrix 
		* vec4(a_vVertexPosition, 1.f);
}