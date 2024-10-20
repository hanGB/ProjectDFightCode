#version 330 core

layout(location = 0) in vec4 a_vVertex;				// 버텍스 x, y, z, UV s;
layout(location = 1) in vec4 a_vVelocity;			// 속도 x, y, z,   UV t;
layout(location = 2) in vec4 a_vSizeAndLifeTime;	// 크기 x, y, 라이프 타임, 시작 시간;

out vec2 vUV;
out float fAlpha;

uniform mat4 u_mProjectionTransform;
uniform mat4 u_mViewTransform;

uniform vec3 u_vTranslateTransform;
uniform vec3 u_vScaleTransform;

uniform vec3 u_vCameraRight;
uniform vec3 u_vCameraUp;

uniform float u_fTime;
uniform bool u_bLoop;
uniform bool u_bApplyGravity;

const float c_GRAVITY = -9.8f;
const vec3 c_ORIGIN = vec3(0.0f, 0.0f, 0.0f);

void main()
{
	if (u_fTime < 0.f) {
		gl_Position = vec4(0.0f);
		return;
	}

	vUV = vec2(a_vVertex.w, a_vVelocity.w);

	float newTime = mod(u_fTime + a_vSizeAndLifeTime.z, a_vSizeAndLifeTime.z);
	newTime -= a_vSizeAndLifeTime.w;

	vec3 newPos = vec3(0.0);
	 
	 fAlpha = 1.0f;
	 if (newTime > a_vSizeAndLifeTime.z + 0.3f) {
		fAlpha = (a_vSizeAndLifeTime.z - newTime) /  0.3f;
	 }

	if (!u_bLoop) {
		if (u_fTime > a_vSizeAndLifeTime.z + a_vSizeAndLifeTime.w) newTime = -1.0f;
	}

	if (newTime >= 0) {	
		vec2 uvInOneToMinusOne = vec2(vUV.x * 2 -1, vUV.y * 2 -1);

		newPos = vec3(uvInOneToMinusOne.x * a_vSizeAndLifeTime.x, uvInOneToMinusOne.y * a_vSizeAndLifeTime.y, 0.0f);

		vec3 cameraAt = u_vCameraRight * u_vCameraUp;
		
		newPos =
			u_vTranslateTransform + vec3(a_vVertex) +
				+ u_vCameraRight * newPos.x * u_vScaleTransform.x
				+ u_vCameraUp * newPos.y * u_vScaleTransform.y;

		if (u_bApplyGravity) {		
			newPos = vec3(
				newPos.x + a_vVelocity.x * newTime,
				newPos.y + a_vVelocity.y * newTime + 0.5f * -9.8f * newTime * newTime,
				newPos.z + a_vVelocity.z * newTime);
		}
		else {
			newPos = vec3(
				newPos.x + a_vVelocity.x * newTime,
				newPos.y + a_vVelocity.y * newTime,
				newPos.z + a_vVelocity.z * newTime);
		}
	}

	gl_Position = u_mProjectionTransform * u_mViewTransform * vec4(newPos, 1.0);
}
