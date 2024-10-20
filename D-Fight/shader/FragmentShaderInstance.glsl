#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 vFragmentPos;
in vec2 vUV;
in vec4 vFragmentPosLightSpace;
in vec2 vVisible;

uniform sampler2D u_texture;

// ī�޶� ��ġ
uniform vec3 u_vViewPos;
uniform bool u_bSpotLightOn;

// ������ ��� ����
vec3 c_vNormalVector = vec3(0.f, 1.f, 0.f);

// �𷺼ų� ����Ʈ(������ �� �Ÿ��� �������� �վ��� ������ ��, �¾�), �ν��Ͻ������� ���� ���͸� ��� ���
struct DirectionalLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 skylight;

	float shadowStrength;
};
uniform DirectionalLight u_dirLight;
vec3 CalculateDirectionalLight(DirectionalLight dirLight, bool visible);

// �𷺼ų� ����Ʈ �׸��� ���
uniform sampler2D u_shadowMap;
float ShadowCalculationForDirectionalLight(float strength, float bias);


// ����Ʈ ����Ʈ(��� �������� �վ��� ������ ��, ����), �ν��Ͻ������� �ں��Ʈ ���� �̿��Ͽ� ���
struct PointLight {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;

	float constant;
	float linear;
	float quadratic;
};
uniform PointLight u_pointLight;
uniform vec3 u_vPointLightPositions[100];
uniform bool u_bIsPointLightOn[100];
uniform int u_iNumoOfPointLights;
vec3 CalculatePointLight(PointLight pointLight, vec3 fragPos, vec3 lightPos);

// ����Ʈ ����Ʈ(�� ������ ���� ������� �վ��� ������ ��, ������)
struct SpotLight{
	vec3 position;
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;

	float cutOff;
	float outherCutOff;

	float distanceBias;
};
uniform SpotLight u_spotLight;
vec3 CalculateSpotLight(SpotLight spotLight, vec3 fragPos);

void main()
{
	bool visible = true;
	if (vVisible == vec2(0)) {
		visible = false;
	}

	vec3 result = vec3(0.0);
	
	// �𷺼ų� ����Ʈ ���
	result += CalculateDirectionalLight(u_dirLight, visible);

	// ����Ʈ ����Ʈ ���
	if (visible)
		result += CalculatePointLight(u_pointLight, vFragmentPos, u_pointLight.position);

	for (int i = 0; i < u_iNumoOfPointLights; ++i) {
		if (u_bIsPointLightOn[i] && visible) {
			result += CalculatePointLight(u_pointLight, vFragmentPos, u_vPointLightPositions[i]);
		}
	}

	if (u_bSpotLightOn && visible)
		result += CalculateSpotLight(u_spotLight, vFragmentPos);

	// ���� ���
	FragColor = vec4(result, 1.0) * texture(u_texture, vUV);
	
	// ���� ó��
	float near = 0.1f, far = 500.0f;
	float ndc = gl_FragCoord.z * 2.0 - 1.0;	
	float linearDepth = (2.0 * near * far) / (far + near - ndc * (far - near)) / far;
	
	// ���� ���
	FragColor = vec4(vec3(FragColor.rgb * (1.0f - linearDepth) + vec3(u_dirLight.skylight) * linearDepth), 1.0f);

	// ���� �κи� ���� ����
	float brightness = dot(FragColor.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
	if (brightness > 1.0f) {
		BrightColor = vec4(FragColor.rgb, 1.0f);
	}
	else {
		BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
}

vec3 CalculateDirectionalLight(DirectionalLight dirLight, bool visible)
{
	vec3 lightDir = normalize(-dirLight.direction);

	// ���� ���� ���: Ambient light
	vec3 ambient = dirLight.ambient;

	// ��� �ݻ� ���� ���: Diffuse light
	// ��� ���Ͱ� ���� ������� �������� ���� ����
	float diffuseColor = max( dot(c_vNormalVector, lightDir), 0.f );
	vec3 diffuse = diffuseColor * dirLight.diffuse;

	// ǥ�� ���̺��� ���̰� �۾������� ���̾ �߰�
	float bias = 0.001;
	float shadowStrengthBiasForInstance = 1.1;

	float shadow = 0.9f;
	if (visible) shadow = ShadowCalculationForDirectionalLight(dirLight.shadowStrength * shadowStrengthBiasForInstance, bias);

	if (shadow > 1.0f) {
		return ambient;
	}
	return (ambient + (1.0 - shadow) * diffuse);
}

float ShadowCalculationForDirectionalLight(float strength, float bias)
{
	vec3 projCoords = vFragmentPosLightSpace.xyz / vFragmentPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;

	// ������ ���� ���� �� �׸��� ���� ����
	if (projCoords.z > 1.0)
		return 0.0;

	float currentDepth = projCoords.z;

	float shadow = 0.0;

	vec2 texelSize = 1.0 / textureSize(u_shadowMap, 0);
	for (int x = -2; x <= 2; ++x)
	{
		for (int y = -2; y <= 2; ++y)
		{
			float pcfDepth = texture(u_shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			
			shadow *= strength;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

	shadow = shadow / 25.0;

	return shadow;
}


vec3 CalculatePointLight(PointLight pointLight, vec3 fragPos, vec3 lightPos)
{
	// ���� ���� ���
	vec3 lightDir = normalize(lightPos - fragPos);

	// ������� �Ÿ�
	float distance = length(lightPos - fragPos);
	
	// ���� ���
	float attenuation = 1.0 / ( 
		pointLight.constant 
		+ pointLight.linear * distance 
		+ pointLight.quadratic * distance * distance );


	// ���� ���� ���: Ambient light
	vec3 ambient = pointLight.ambient;
	float diffuseColor = max( dot(c_vNormalVector, lightDir), 0.f );
	vec3 diffuse = diffuseColor * pointLight.diffuse;
	
	ambient  *= attenuation;
	diffuse  *= attenuation;

	return ambient + diffuse;
}

vec3 CalculateSpotLight(SpotLight spotLight, vec3 fragPos)
{
	// �ü� ����, ǥ���� ������ ����ȭ
	vec3 viewDir = normalize(u_vViewPos - fragPos);

	// ǥ�����ΰ��� ��, ǥ���� ������ ����ȭ
	vec3 lightDir = normalize(spotLight.position - fragPos);

	// �⺻ ���� ���� ���
	// ���� ���� ���: Ambient light
	vec3 ambient = spotLight.ambient;

	// ��� �ݻ� ���� ���: Diffuse light
	// ��� ���Ͱ� ���� ������� �������� ���� ����
	float diffuseColor = max( dot(c_vNormalVector, lightDir), 0.f );
	vec3 diffuse = diffuseColor * spotLight.diffuse;


	// �ε巯�� �����ڸ��� ���� ���
	float theta = dot(lightDir, normalize(-spotLight.direction));
	float epsilon = spotLight.cutOff - spotLight.outherCutOff;
	float intensity = clamp((theta - spotLight.outherCutOff) / epsilon, 0.0, 1.0);

	// ��ǻ� �� ���� ����
	diffuse *= intensity;


	// �� ���� ���(���� ���� ȿ���� �����ϱ⿡�� �ʹ� Ŀ�� ���̾ ����)
	float distance = length(spotLight.position - fragPos);
	distance *= spotLight.distanceBias;
	float attenuation = 1.0 / (distance * distance);

	// ��� �׿� �� ���� ����
	ambient *= attenuation;
	diffuse *= attenuation;

	// �ʹ� ��ų� ��ο� ��� ����
	ambient = clamp(ambient, 0.0f, 1.0f);
	diffuse = clamp(diffuse, 0.0f, 1.0f);

	return (ambient + diffuse);
}