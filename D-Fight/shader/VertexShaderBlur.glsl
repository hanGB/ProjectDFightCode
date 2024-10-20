#version 330 core

layout (location = 0) in vec2 a_vPosition;
layout (location = 1) in vec2 a_vUV;

out vec2 vUV;

void main()
{
	vUV = a_vUV;
	gl_Position = vec4(a_vPosition.x, a_vPosition.y, 0.0, 1.0);
}