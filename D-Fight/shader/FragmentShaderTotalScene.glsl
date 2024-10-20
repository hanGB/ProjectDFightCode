#version 330 core

out vec4 FragColor;

in vec2 vUV;

uniform sampler2D u_sceneTexture;
uniform sampler2D u_blurTexture;

uniform bool u_bBlur;
uniform bool u_bColorInvert;
uniform int u_iEffectOnOffLine;

const float c_GAMMA = 2.2f;

vec4 BlurEffectRender(sampler2D tex);
vec4 BloomRender(vec3 displayColor, float exposure);
vec4 BloomRenderNotSRGB(vec3 displayColor);
vec4 ContrastAndSaturation(vec4 color, float contrast, float saturation);
vec4 ColorFilter(vec4 color, vec4 blendingColor, float strength);

void main()
{  
    vec4 color;
    if (u_bBlur) {
        color = BlurEffectRender(u_sceneTexture);
    }
    else {
        color = texture(u_sceneTexture, vUV);
    }

    float exposure = 0.5f;                                  // ����
    float contrast = 1.25f;                                 // ���
    float saturation = 1.75f;                               // ä��
    vec4 blendingColor = vec4(1.0f, 0.8f, 1.0f, 1.0f);      // ȥ�� ����    
    float blendingStrength = 0.1f;                          // ȥ�� ����

    if (gl_FragCoord.x > u_iEffectOnOffLine) {
        color = BloomRenderNotSRGB(color.rgb);
    }
    else {
       color = BloomRender(color.rgb, exposure);
       color = ContrastAndSaturation(color, contrast, saturation);
       color = ColorFilter(color, blendingColor, blendingStrength);
    }

    FragColor = color;

    // ����(��� ����)
    if (u_bColorInvert) {  
        float strength = (FragColor.r + FragColor.g + FragColor.z) / 3.0f;
        FragColor = vec4(vec3(1.0 - FragColor), 1.0f);
        FragColor = vec4(normalize(vec3(FragColor)) * strength, 1.0f);
    }
}

vec4 BloomRender(vec3 displayColor, float exposure) 
{
	vec3 bloomColor = texture(u_blurTexture, vUV).rgb;
	vec3 hdrColor = displayColor + bloomColor;

	vec3 result = vec3(1.0f) - exp(-hdrColor * exposure);
	result = pow(result, vec3(1.0f / c_GAMMA));

	return vec4(result, 1.0f);
}

vec4 BloomRenderNotSRGB(vec3 displayColor) 
{
    vec3 bloomColor = texture(u_blurTexture, vUV).rgb;
	vec3 hdrColor = displayColor + bloomColor;

	return vec4(hdrColor, 1.0f);
}

vec4 ContrastAndSaturation(vec4 color, float contrast, float saturation)
{
    float average = (color.r + color.g + color.z) / 3.0f;
        
    // ��� ����
    vec4 result = vec4(1.0f);
    result.rgb = ((color.rgb - 0.5f) * contrast) + 0.5f;

    // ä�� ����
    result.r = average * (1.0f - saturation) + result.r * saturation;
    result.g = average * (1.0f - saturation) + result.g * saturation;
    result.b = average * (1.0f - saturation) + result.b * saturation;

    return result;
}

vec4 ColorFilter(vec4 color, vec4 blendingColor, float strength)
{
  // ���� ����
  vec4 result = color * (1.0f - strength) + blendingColor * strength;

  return result;
}

vec4 BlurEffectRender(sampler2D tex)
{
    // �ֺ� �ؽ��� ��ǥ
     vec2 offset = 1.0f / textureSize(tex, 0) * 2.0f;
     float ox   = offset.x;
     float ox2  = offset.x * 2.0;
     float oy   = offset.y;
     float oy2  = offset.y * 2.0;

    vec2 offsets[25] = vec2[](
         vec2(-ox2, oy2),   vec2(-ox, oy2),     vec2(0.0, oy2),     vec2(ox, oy2),  vec2(ox2, oy2), 
         vec2(-ox2, oy),    vec2(-ox, oy),      vec2(0.0, oy),      vec2(ox, oy),   vec2(ox2, oy), 
         vec2(-ox2, 0.0),   vec2(-ox, 0.0),     vec2(0.0, 0.0),     vec2(ox, 0.0),  vec2(ox2, 0.0), 
         vec2(-ox2, -oy),   vec2(-ox, -oy),     vec2(0.0, -oy),     vec2(ox, -oy),  vec2(ox2, -oy),
         vec2(-ox2, -oy2),  vec2(-ox, -oy2),    vec2(0.0, -oy2),    vec2(ox, -oy2), vec2(ox2, -oy2)
    );

    // 5 * 5 ����þ� ��
    float kernel[25] = float[](
       1.0 / 256.0,     4.0 / 256.0,    6.0 / 256.0,    4.0 / 256.0,    1.0 / 256.0,
       4.0 / 256.0,     16.0 / 256.0,   24.0 / 256.0,   16.0 / 256.0,   4.0 / 256.0,
       6.0 / 256.0,     24.0 / 256.0,   36.0 / 256.0,   24.0 / 256.0,   6.0 / 256.0,
       4.0 / 256.0,     16.0 / 256.0,   24.0 / 256.0,   16.0 / 256.0,   4.0 / 256.0,
       1.0 / 256.0,     4.0 / 256.0,    6.0 / 256.0,    4.0 / 256.0,    1.0 / 256.0
    );

    // �� �� ���
    vec3 sampleTex[25];
    for(int i = 0; i < 25; i++)
    {
        sampleTex[i] = vec3(texture(tex, vUV + offsets[i]));
    }

    vec3 effectColor = vec3(0.0);

    // Ŀ�� ȿ�� ����
    for(int i = 0; i < 25; i++)
        effectColor += sampleTex[i] * kernel[i];
    
    return vec4(effectColor, 1.0);
}
