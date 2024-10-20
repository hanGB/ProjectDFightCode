#version 330 core

layout(location = 0) in vec3 a_vPosition;

out vec3 vUV;

uniform mat4 u_mProjectionTransform;
uniform mat4 u_mViewTransform;

void main()
{
	vUV = a_vPosition;

	vec4 position =  u_mProjectionTransform * u_mViewTransform * vec4(a_vPosition, 1.0);
	gl_Position = position.xyww;
}