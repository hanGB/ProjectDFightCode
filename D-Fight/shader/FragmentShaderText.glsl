#version 330 core

in vec2 vUV;
out vec4 color;

uniform sampler2D u_texture;
uniform vec3 u_vColor;

void main()
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_texture, vUV).r);
	
	color = vec4(u_vColor, 1.0) * sampled;
}