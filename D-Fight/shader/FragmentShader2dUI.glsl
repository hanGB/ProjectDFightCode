#version 330 core

in vec2 vUV;
out vec4 color;

uniform sampler2D u_texture;
uniform vec4 u_vColor;

void main()
{
	color = u_vColor * texture(u_texture, vUV);
}