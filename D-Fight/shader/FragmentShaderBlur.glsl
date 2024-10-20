#version 330 core

in vec2 vUV;

out vec4 FragColor;

uniform sampler2D u_texture;
uniform bool u_bHorizontal;

const float c_fWeights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
	vec2 offset = 1.0 / textureSize(u_texture, 0);
	
	vec3 result = texture(u_texture, vUV).rgb * c_fWeights[0];

	if (u_bHorizontal)
	{
		for (int i = 1; i < 5; ++i) {
			result += texture(u_texture, vUV + vec2(offset.x * i, 0.0)).rgb * c_fWeights[i];
			result += texture(u_texture, vUV - vec2(offset.x * i, 0.0)).rgb * c_fWeights[i];
		}
	}
	else
	{
		for (int i = 1; i < 5; ++i) {
			result += texture(u_texture, vUV + vec2(0.0, offset.y * i)).rgb * c_fWeights[i];
			result += texture(u_texture, vUV - vec2(0.0, offset.y * i)).rgb * c_fWeights[i];
		}
	}
	FragColor = vec4(result, 1.0f);
}
