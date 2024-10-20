#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform vec3 u_vColor;

void main()
{ 
	FragColor = vec4(u_vColor, 1.0f);

	// 밝은 부분만 따로 저장
	float brightness = dot(FragColor.rgb, vec3(0.2126f, 0.7152f, 0.0722f));

	if (brightness > 1.0f) {
		BrightColor = vec4(FragColor.rgb, 1.0f);
	}
	else {
		BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
}
