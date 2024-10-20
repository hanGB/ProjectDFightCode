#version 330 core

layout (location = 0) in vec4 a_vPositonTexture;

out vec2 vUV;

uniform mat4 u_mProjectionTransform;
uniform mat4 u_mViewTransform;

uniform vec3 u_vTranslateTransform;
uniform vec2 u_vScaleTransform;

uniform vec3 u_vCameraRight;
uniform vec3 u_vCameraUp;

void main()
{
	vUV = a_vPositonTexture.zw;

	vec3 vertexPosition =
		u_vTranslateTransform 
		+ u_vCameraRight * a_vPositonTexture.x * u_vScaleTransform.x 
		+ u_vCameraUp * a_vPositonTexture.y * u_vScaleTransform.y;

	gl_Position = u_mProjectionTransform * u_mViewTransform * vec4(vertexPosition, 1.0);
}
