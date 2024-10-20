#version 330 core

layout(location = 0) in vec3 a_vVertexPosition;

uniform mat4 u_mLightSpaceMatrix;
uniform mat4 u_mModelTransform;

void main()
{
	gl_Position = u_mLightSpaceMatrix * u_mModelTransform * vec4(a_vVertexPosition, 1.0);
}