#version 330 core

layout(location = 0) in vec3 a_vVertexPosition;
layout(location = 1) in vec2 a_vVertexUV;
layout(location = 2) in vec3 a_vVertexNormal;

out vec2 vUV;

uniform mat4 u_mModelTransform;
uniform mat4 u_mViewTransform;	
uniform mat4 u_mProjectionTransform;

void main()
{
	// 텍스쳐 UV 좌표
	vUV = a_vVertexUV;

	gl_Position = 
		u_mProjectionTransform * u_mViewTransform * u_mModelTransform * vec4(a_vVertexPosition, 1.f);
}