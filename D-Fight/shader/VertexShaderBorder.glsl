#version 330 core

layout(location = 0) in vec3 a_vVertexPosition;

uniform mat4 u_mModelTransform;
uniform mat4 u_mViewTransform;	
uniform mat4 u_mProjectionTransform;
void main()
{
	gl_Position = 
		u_mProjectionTransform * u_mViewTransform * u_mModelTransform * vec4(a_vVertexPosition, 1.f);
}