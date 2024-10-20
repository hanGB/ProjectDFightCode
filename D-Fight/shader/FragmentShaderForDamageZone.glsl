#version 330 core

in vec2 vUV;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform float u_fTimeRate;

const vec3 c_vColor = vec3(1.0f, 0.2, 0.2f);

void main()
{
	FragColor = vec4(c_vColor, u_fTimeRate);
	BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
