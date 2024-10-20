#version 330 core

in vec2 vUV;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;


uniform sampler2D u_texture;
uniform vec4 u_vColor;

void main()
{
	FragColor = u_vColor * texture(u_texture, vUV);
	BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}