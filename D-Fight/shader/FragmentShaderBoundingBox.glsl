#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

void main()
{
	vec3 green = vec3(0.0, 1.0, 0.0);

	// 최종 출력
	FragColor = vec4(green, 1.0);
	BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
