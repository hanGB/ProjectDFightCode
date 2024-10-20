#version 330 core

layout(location = 0) in vec4 a_vVertex;
layout(location = 1) in vec2 a_vVelocity;

out vec2 vUV;
out float fDying;

uniform mat4 u_mProjectionTransform;
uniform float u_fTime;
uniform float u_fLifeTime;

void main()
{
	vUV = a_vVertex.zw;

	vec2 newPos = vec2(a_vVertex.x + a_vVelocity.x * u_fTime, a_vVertex.y + a_vVelocity.y * u_fTime);

	gl_Position = u_mProjectionTransform * vec4(newPos, 0.0, 1.0);
	fDying = 1.0f - u_fTime / u_fLifeTime;
}
