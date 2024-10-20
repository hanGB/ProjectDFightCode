#version 330 core

layout(location = 0) in vec3 in_vVertexPosition;
layout(location = 1) in vec2 in_vVertexUV;
layout(location = 2) in vec3 in_vVertexNormal;

uniform mat4 u_mModelTransform;
uniform mat4 u_mViewTransform;	
uniform mat4 u_mProjectionTransform;

void main()
{
	gl_Position = 
		u_mProjectionTransform * u_mViewTransform * u_mModelTransform * vec4(in_vVertexPosition, 1.f);
}