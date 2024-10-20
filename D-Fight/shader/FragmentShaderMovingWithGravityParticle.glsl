#version 330 core

in vec2 vUV;
in float fAlpha;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform sampler2D u_texture;
uniform vec4 u_vColor;

void main()
{
	FragColor = texture(u_texture, vUV) * u_vColor * vec4(1.0f, 1.0f, 1.0f, fAlpha);

	if ((FragColor.x + FragColor.y + FragColor.z) > 3.0f) {
		BrightColor = FragColor;
	}
	else {
		BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
}