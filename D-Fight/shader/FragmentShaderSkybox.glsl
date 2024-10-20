#version 330 core

in vec3 vUV;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;


uniform samplerCube u_cubemap;
uniform samplerCube u_cubemap2;
uniform float u_fBlendRate;

void main()
{
	if (u_fBlendRate != 0.0f) {
		FragColor = texture(u_cubemap, vUV) * (1.0f - u_fBlendRate) + texture(u_cubemap2, vUV) * u_fBlendRate;
	}
	else {
		FragColor = texture(u_cubemap, vUV);
	}

	BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}