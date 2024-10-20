#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform vec4 u_vColor;

void main()
{
	FragColor = u_vColor;

	if (u_vColor.x > 1.5f ||  u_vColor.y > 1.5f || u_vColor.z > 1.5f) BrightColor = u_vColor;
	else BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
