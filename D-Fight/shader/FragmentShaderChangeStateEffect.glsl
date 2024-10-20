#version 330 core

out vec4 FragColor;

in vec2 vUV;

uniform sampler2D u_prevStateRenderingResultTexture;
uniform sampler2D u_sceneTexture;
uniform float u_fTime;
uniform bool u_bHeroWin;

const vec2 c_vCenter = vec2(0.5f, 0.5f);
const float c_fColorStrengthBias = 2.0f;

vec4 NormalRender();
vec4 PrevStateRender();

void main()
{  
    float time = (u_fTime - 0.35f);

    if (time < 0.0f)
    {
        FragColor = PrevStateRender();  
    }
    else if ( (c_vCenter.x - time) < vUV.x && (c_vCenter.x + time) > vUV.x 
    && (c_vCenter.y - time) < vUV.y && (c_vCenter.y + time) > vUV.y )
    {
        FragColor = NormalRender();
    }
    else 
    {
        FragColor = PrevStateRender();  
    }
}

vec4 NormalRender()
{
    return texture(u_sceneTexture, vUV);
}

vec4 PrevStateRender()
{
    vec4 prevStateTextureColor = texture(u_prevStateRenderingResultTexture, vUV);

    float average = 0.21 * prevStateTextureColor.x + 0.71 * prevStateTextureColor.y + 0.08 * prevStateTextureColor.z;

    if (u_bHeroWin) {
        return vec4(0.f, average, average * 2.0f, 1.0f);
    }
    else {
        return prevStateTextureColor * vec4(average * 2.0f, 0.f, average, 1.0f);
    }
}