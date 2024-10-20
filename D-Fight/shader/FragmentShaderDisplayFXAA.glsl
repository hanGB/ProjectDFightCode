#version 330 core

out vec4 FragColor;

in vec2 vUV;

uniform sampler2D u_texture;
uniform vec2 u_vResolution;

const float c_FXAA_SPAN_MAX = 8.0;
const float c_FXAA_REDUCE_MUL = 1.0 / 8.0;
const float c_FXAA_REDUCE_MIN = 1.0 / 128.0;

void main()
{  
    vec3 rgbNW = texture2D(u_texture, vUV+(vec2(-1.0,-1.0)/u_vResolution)).xyz;
    vec3 rgbNE = texture2D(u_texture,  vUV+(vec2(1.0,-1.0)/u_vResolution)).xyz;
    vec3 rgbSW = texture2D(u_texture, vUV+(vec2(-1.0,1.0)/u_vResolution)).xyz;
    vec3 rgbSE = texture2D(u_texture, vUV+(vec2(1.0,1.0)/u_vResolution)).xyz;
    vec3 rgbM  = texture2D(u_texture, vUV).xyz;

    vec3  luma = vec3(0.299, 0.587, 0.114);
    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM  = dot(rgbM,  luma);

    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    vec2 dir = vec2( -((lumaNW + lumaNE) - (lumaSW + lumaSE)), ((lumaNW + lumaSW) - (lumaNE + lumaSE)) );

    float dirReduce = max( (lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * c_FXAA_REDUCE_MUL), c_FXAA_REDUCE_MIN );

    float rcpDirMin = 1.0 / (min( abs(dir.x), abs(dir.y)) + dirReduce );

    dir = min(vec2( c_FXAA_SPAN_MAX,  c_FXAA_SPAN_MAX),
          max(vec2(-c_FXAA_SPAN_MAX, -c_FXAA_SPAN_MAX),
          dir * rcpDirMin)) / u_vResolution;

    vec3 rgbA = (1.0/2.0) * (
        texture2D(u_texture, vUV.xy + dir * (1.0/3.0 - 0.5)).xyz +
        texture2D(u_texture, vUV.xy + dir * (2.0/3.0 - 0.5)).xyz);

    vec3 rgbB = rgbA * (1.0/2.0) + (1.0/4.0) * (
        texture2D(u_texture, vUV.xy + dir * (0.0/3.0 - 0.5)).xyz +
        texture2D(u_texture, vUV.xy + dir * (3.0/3.0 - 0.5)).xyz);

    float lumaB = dot(rgbB, luma);

    if ((lumaB < lumaMin) || (lumaB > lumaMax)) {
       FragColor = vec4(rgbA, 1.0);
    }
    else{
        FragColor = vec4(rgbB, 1.0);
    }
}
