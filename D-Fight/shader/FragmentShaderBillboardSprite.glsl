#version 330 core

in vec2 vUV;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform sampler2D u_texture;
uniform vec4 u_vColor;
uniform vec4 u_vFrame;

vec4 ContrastAndSaturation(vec4 color, float contrast, float saturation);

void main()
{	
    float contrast = 1.0f;                                 // 대비
    float saturation = 1.5f;                               // 채도

	vec2 spriteCnt = vec2(u_vFrame.x, u_vFrame.y);
	vec2 frame = vec2(u_vFrame.z, u_vFrame.w);

	vec2 finalUV = vec2( (vUV.x / spriteCnt.x) + (1.0 / spriteCnt.x) * frame.x, 
	(vUV.y / spriteCnt.y) +  (1.0 / spriteCnt.y) * frame.y);

    vec4 color = u_vColor * texture(u_texture, finalUV);
	FragColor = ContrastAndSaturation(color, contrast, saturation);
	BrightColor = FragColor;
}

vec4 ContrastAndSaturation(vec4 color, float contrast, float saturation)
{
    float average = (color.r + color.g + color.z) / 3.0f;
        
    // 대비 설정
    vec4 result = vec4(color.a);
    result.rgb = ((color.rgb - 0.5f) * contrast) + 0.5f;

    // 채도 설정
    result.r = average * (1.0f - saturation) + result.r * saturation;
    result.g = average * (1.0f - saturation) + result.g * saturation;
    result.b = average * (1.0f - saturation) + result.b * saturation;

    return result;
}
