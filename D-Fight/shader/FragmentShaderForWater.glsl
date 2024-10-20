#version 330 core

in vec3 vFragmentPos;
in vec3 vNormal;
in vec4 vFragmentPosLightSpace;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

// ī�޶� ��ġ
uniform vec3 u_vCameraPosition;
uniform bool u_bSpotLightOn;

// ��ī�� �ڽ�
uniform samplerCube u_cubemap;
uniform samplerCube u_cubemap2;
uniform float u_fBlendRate;

// �𷺼ų� ����Ʈ(������ �� �Ÿ��� �������� �վ��� ������ ��, �¾�)
struct DirectionalLight {
	vec3 direction;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 skylight;

	float shadowStrength;
};
uniform DirectionalLight u_dirLight;
vec4 CalculateDirectionalLight(DirectionalLight dirLight, vec3 normalVector, vec3 viewDir);

// �𷺼ų� ����Ʈ �׸��� ���
uniform sampler2D u_shadowMap;
float ShadowCalculationForDirectionalLight(float strength, float bias);


// ����Ʈ ����Ʈ(��� �������� �վ��� ������ ��, ����)
struct PointLight {
	vec3 position;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	float constant;
	float linear;
	float quadratic;
};
uniform PointLight u_pointLight;
uniform vec3 u_vPointLightPositions[100];
uniform int u_bIsPointLightOn[100];			// uniform�� bool���� �迭�� ��Ȯ�ϰ� ���� �� �������� int�� ��� ���
uniform int u_iNumoOfPointLights;
vec4 CalculatePointLight(PointLight pointLight, vec3 normalVector, vec3 fragPos, vec3 viewDir, vec3 pos);


// ����Ʈ ����Ʈ(�� ������ ���� ������� �վ��� ������ ��, ������)
struct SpotLight{
	vec3 position;
	vec3 direction;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	float cutOff;
	float outherCutOff;

	float distanceBias;
};
uniform SpotLight u_spotLight;
vec4 CalculateSpotLight(SpotLight spotLight, vec3 normalVector, vec3 fragPos, vec3 viewDir);


void main()
{
	// �ü� ����, ǥ���� ������ ����ȭ
	vec3 viewDir = normalize(u_vCameraPosition - vFragmentPos);
	vec3 normalVector = normalize(vNormal);

	vec4 result = vec4(0.0f);
	
	vec4 material;

	float ratio = 1.0f / 1.33f;
	vec3 l = -viewDir;
	vec3 R = reflect(l, normalVector);

	if (u_fBlendRate != 0.0f) {
		material = texture(u_cubemap, R) * (1.0f - u_fBlendRate) + texture(u_cubemap2, R) * u_fBlendRate;
	}
	else {
		material = texture(u_cubemap, R);
	}
	
	material.a = 0.35f;

	// �𷺼ų� ����Ʈ ���
	result += CalculateDirectionalLight(u_dirLight, normalVector, viewDir);

	// ����Ʈ ����Ʈ ���
	for (int i = 0; i < u_iNumoOfPointLights; ++i) {
		if (u_bIsPointLightOn[i] != 0) {
			result += CalculatePointLight(u_pointLight, normalVector, vFragmentPos, viewDir, u_vPointLightPositions[i]);
		}
	}

	// ����Ʈ ����Ʈ ���
	if (u_bSpotLightOn)
		result += CalculateSpotLight(u_spotLight, normalVector, vFragmentPos, viewDir);	
	
	// ���� ���� �ؽ��� ����
	result *= material;

	// ���� ó��
	float near = 0.1f, far = 500.0f;
	float ndc = gl_FragCoord.z * 2.0 - 1.0;	
	float linearDepth = (2.0 * near * far) / (far + near - ndc * (far - near)) / far;
	
	// ���� ���
	FragColor = vec4(vec3((result.rgb) * (1.0f - linearDepth) + vec3(u_dirLight.skylight) * linearDepth), result.a);

	// ���� �κи� ���� ����
	float brightness = dot(FragColor.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
	if (brightness > 1.0f) {
		BrightColor = vec4(FragColor.rgb, 1.0f);
	}
	else {
		BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
}

vec4 CalculateDirectionalLight(DirectionalLight dirLight, vec3 normalVector, vec3 viewDir)
{
	vec3 lightDir = normalize(-dirLight.direction);
	vec3 halfWayDir = normalize(lightDir + viewDir);		// Blinn-Phong�� ���� ���

	// ���� ���� ���: Ambient light
	vec4 ambient = dirLight.ambient * dirLight.skylight;

	// ��� �ݻ� ���� ���: Diffuse light
	// ��� ���Ͱ� ���� ������� �������� ���� ����
	float diffuseColor = max( dot(normalVector, lightDir), 0.f );
	vec4 diffuse = diffuseColor * dirLight.diffuse;


	// �ſ� �ݻ� ����(���ݻ� ����) ���: Specular light
	// �ݻ� ����
	// vec3 reflectDir = reflect(-lightDir, normalVector);
	// �ü� ����� �ݻ� ������� �������� ���� ����, ��¦�� ��� ������ ���̶���Ʈ ����
	float specularColor = pow( max( dot(normalVector, halfWayDir), 0.f ), 32.0);
	vec4 specular =  specularColor * dirLight.specular;	

	//float bias = max( 0.05 * (1.0 - dot(normalVector, lightDir)), 0.005 );
	// ǥ�� ���̺��� ���̰� �۾������� ���̾ �߰�
	float bias = 0.005;		
	float shadow = ShadowCalculationForDirectionalLight(dirLight.shadowStrength, bias);

	return (ambient + (vec4(1.0 - shadow, 1.0 - shadow, 1.0 - shadow, 1.0)) * (diffuse + specular));
}

vec4 CalculatePointLight(PointLight pointLight, vec3 normalVector, vec3 fragPos, vec3 viewDir, vec3 pos)
{
	// ǥ�����ΰ��� ��, ǥ���� ������ ����ȭ
	vec3 lightDir = normalize(pos - fragPos);
	vec3 halfWayDir = normalize(lightDir + viewDir);

	// ������� �Ÿ�
	float distance = length(pos - fragPos);
	
	// ���� ���
	float attenuation = 1.0 / ( 
		pointLight.constant 
		+ pointLight.linear * distance 
		+ pointLight.quadratic * distance * distance );


	// ���� ���� ���: Ambient light
	vec4 ambient = pointLight.ambient;

	// ��� �ݻ� ���� ���: Diffuse light
	// ��� ���Ͱ� ���� ������� �������� ���� ����
	float diffuseColor = max( dot(normalVector, lightDir), 0.f );
	vec4 diffuse = diffuseColor * pointLight.diffuse;

	// �ſ� �ݻ� ����(���ݻ� ����) ���: Specular light
	// �ݻ� ����
	// vec3 reflectDir = reflect(-lightDir, normalVector);
	// �ü� ����� �ݻ� ������� �������� ���� ����, ��¦�� ��� ������ ���̶���Ʈ ����
	float specularColor = pow( max( dot(normalVector, halfWayDir), 0.f ), 32.0f);
	vec4 specular = specularColor * pointLight.specular;

	ambient  *= attenuation;
	diffuse	 *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec4 CalculateSpotLight(SpotLight spotLight, vec3 normalVector, vec3 fragPos, vec3 viewDir)
{
	// ǥ�����ΰ��� ��, ǥ���� ������ ����ȭ
	vec3 lightDir = normalize(spotLight.position - fragPos);
	vec3 halfWayDir = normalize(lightDir + viewDir);

	// �⺻ ���� ���� ���
	// ���� ���� ���: Ambient light
	vec4 ambient = spotLight.ambient;

	// ��� �ݻ� ���� ���: Diffuse light
	// ��� ���Ͱ� ���� ������� �������� ���� ����
	float diffuseColor = max( dot(normalVector, lightDir), 0.f );
	vec4 diffuse = diffuseColor * spotLight.diffuse;

	// �ſ� �ݻ� ����(���ݻ� ����) ���: Specular light
	// �ü� ����� �ݻ� ������� �������� ���� ����, ��¦�� ��� ������ ���̶���Ʈ ����
	float specularColor = pow( max( dot(normalVector, halfWayDir), 0.f ), 32.0);
	vec4 specular = specularColor * spotLight.specular;


	// �ε巯�� �����ڸ��� ���� ���
	float theta = dot(lightDir, normalize(-spotLight.direction));
	float epsilon = spotLight.cutOff - spotLight.outherCutOff;
	float intensity = clamp((theta - spotLight.outherCutOff) / epsilon, 0.0, 1.0);

	// ��ǻ��� ����ŧ���� ���� ���� ����
	diffuse *= intensity;
	specular *= intensity;


	// �� ���� ���(���� ���� ȿ���� �����ϱ⿡�� �ʹ� Ŀ�� ���̾ ����)
	float distance = length(spotLight.position - fragPos);
	distance *= spotLight.distanceBias;
	float attenuation = 1.0 / (distance * distance);


	// �ʹ� ��ų� ��ο� ��� ����
	ambient = clamp(ambient, 0.0f, 1.0f);
	diffuse = clamp(diffuse, 0.0f, 1.0f);


	return (ambient + diffuse + specular);
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
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(u_shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			
			shadow *= strength;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

	shadow = shadow / 9.0;

	return shadow;
}
