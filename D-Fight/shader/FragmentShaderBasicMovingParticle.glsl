#version 330 core

in vec2 vUV;
in float fDying;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform sampler2D u_texture;
uniform vec4 u_vColor;

void main()
{
	vec4 textureColor = u_vColor * texture(u_texture, vUV);
	FragColor = vec4(textureColor.rgb, textureColor.a * fDying);
	BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}