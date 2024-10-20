#version 330 core

layout(location = 0) in vec4 a_vVertex;

out vec2 vUV;

uniform mat4 u_mProjectionTransform;

void main()
{
	vUV = a_vVertex.zw;


	gl_Position = u_mProjectionTransform * vec4(a_vVertex.xy, 0.0, 1.0);
}
