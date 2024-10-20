#version 330 core

in vec3 vFragmentPos;
in vec3 vNormal;
in vec2 vUV;
in vec4 vFragmentPosLightSpace;
in mat3 mTBN;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

// Ȱ��ȭ�� ���� ��ȣ
uniform int u_iActiveArea;
const int c_AREA_WIDTH = 24;

// ī�޶� ��ġ
uniform vec3 u_vViewPos;
uniform bool u_bSpotLightOn;

// ����
uniform vec4 u_vColor;

// ���� ����(����ó���� ���� ����)
uniform bool u_bIsLightSource;


// �ؽ��� ���� ���� ����ü
struct Material {
	sampler2D ambient;
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
	sampler2D normal;
};
uniform Material u_material;

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
vec4 CalculateDirectionalLight(DirectionalLight dirLight, vec3 normalVector, vec3 viewDir, bool visible);

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
	vec3 pos = vFragmentPos; pos.y = 0;
	float len = length(pos);

	bool visible = true;
	if (len <=  c_AREA_WIDTH * 5) {	
		if (u_iActiveArea == 1) {
			if (len > c_AREA_WIDTH * 2) {
				visible = false;
			}
		}
		else if (u_iActiveArea != 0) {
			if (c_AREA_WIDTH * u_iActiveArea > len || len > c_AREA_WIDTH * (u_iActiveArea + 1)) {
				visible = false;
			}
		}
	}

	// ������ ��� �ٸ� ������ ���� ����
	if (u_bIsLightSource) {
		
		if (visible) FragColor = u_vColor;
		else discard;

		// ���� �κи� ���� ����
		float brightness = dot(FragColor.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
		if (brightness > 1.0f) {
			BrightColor = vec4(FragColor.rgb, 1.0f);
		}
		else {
			BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
		}

		return;
	}

	if (texture(u_material.diffuse, vUV).a < 0.1f) 
		discard;		// ��ǻ����� ���İ��� 0�� ����� ��� �����׸�Ʈ ���

	// �ü� ����, ǥ���� ������ ����ȭ
	vec3 viewDir = normalize(u_vViewPos - vFragmentPos);
	vec3 normalVector = texture(u_material.normal, vUV).rgb * 2.0 - 1.0;
	normalVector = normalize(mTBN * normalVector);

	vec4 result = vec4(0.0);
	
	// �𷺼ų� ����Ʈ ���
	result += CalculateDirectionalLight(u_dirLight, normalVector, viewDir, visible);

	// ����Ʈ ����Ʈ ���
	if (visible)
		result += CalculatePointLight(u_pointLight, normalVector, vFragmentPos, viewDir, u_pointLight.position);

	for (int i = 0; i < u_iNumoOfPointLights; ++i) {
		if (u_bIsPointLightOn[i] != 0 && visible) {
			result += CalculatePointLight(u_pointLight, normalVector, vFragmentPos, viewDir, u_vPointLightPositions[i]);
		}
	}

	// ����Ʈ ����Ʈ ���
	if (u_bSpotLightOn && visible)
		result += CalculateSpotLight(u_spotLight, normalVector, vFragmentPos, viewDir);	
	
	// ���� ó��
	float near = 0.1f, far = 500.0f;
	float ndc = gl_FragCoord.z * 2.0 - 1.0;	
	float linearDepth = (2.0 * near * far) / (far + near - ndc * (far - near)) / far;

	// ���� ���
	FragColor = vec4(vec3((result.rgb * u_vColor.rgb) * (1.0f - linearDepth) + vec3(u_dirLight.skylight) * linearDepth),
				1.0f * u_vColor.a);

	// ���� �κи� ���� ����
	float brightness = dot(FragColor.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
	if (brightness > 1.0f) {
		BrightColor = vec4(FragColor.rgb, 1.0f);
	}
	else {
		BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
}

vec4 CalculateDirectionalLight(DirectionalLight dirLight, vec3 normalVector, vec3 viewDir, bool visible)
{
	vec3 lightDir = normalize(-dirLight.direction);
	vec3 halfWayDir = normalize(lightDir + viewDir);		// Blinn-Phong�� ���� ���

	// ���� ���� ���: Ambient light
	vec4 ambient = dirLight.ambient * texture(u_material.ambient, vUV);

	// ��� �ݻ� ���� ���: Diffuse light
	// ��� ���Ͱ� ���� ������� �������� ���� ����
	float diffuseColor = max( dot(normalVector, lightDir), 0.f );
	vec4 diffuse = diffuseColor * dirLight.diffuse * texture(u_material.diffuse, vUV);


	// �ſ� �ݻ� ����(���ݻ� ����) ���: Specular light
	// �ݻ� ����
	// vec3 reflectDir = reflect(-lightDir, normalVector);
	// �ü� ����� �ݻ� ������� �������� ���� ����, ��¦�� ��� ������ ���̶���Ʈ ����
	float specularColor = pow( max( dot(normalVector, halfWayDir), 0.f ), u_material.shininess);
	vec4 specular =  specularColor * dirLight.specular * texture(u_material.specular, vUV);	

	// ǥ�� ���̺��� ���̰� �۾������� ���̾ �߰�
	float bias = 0.005 * tan(acos(dot(normalVector, lightDir)));
	bias = clamp(bias, 0.0, 0.075);

	float shadow = 0.9f;

	if (visible) shadow = ShadowCalculationForDirectionalLight(dirLight.shadowStrength, bias);

	if (shadow > 1.0f) {
		return ambient;
	}
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
	vec4 ambient = pointLight.ambient * texture(u_material.ambient, vUV);

	// ��� �ݻ� ���� ���: Diffuse light
	// ��� ���Ͱ� ���� ������� �������� ���� ����
	float diffuseColor = max( dot(normalVector, lightDir), 0.f );
	vec4 diffuse = diffuseColor * pointLight.diffuse * texture(u_material.diffuse, vUV);

	// �ſ� �ݻ� ����(���ݻ� ����) ���: Specular light
	// �ݻ� ����
	// vec3 reflectDir = reflect(-lightDir, normalVector);
	// �ü� ����� �ݻ� ������� �������� ���� ����, ��¦�� ��� ������ ���̶���Ʈ ����
	float specularColor = pow( max( dot(normalVector, halfWayDir), 0.f ), u_material.shininess);
	vec4 specular = specularColor * pointLight.specular * texture(u_material.specular, vUV);

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
	vec4 ambient = spotLight.ambient * texture(u_material.ambient, vUV);

	// ��� �ݻ� ���� ���: Diffuse light
	// ��� ���Ͱ� ���� ������� �������� ���� ����
	float diffuseColor = max( dot(normalVector, lightDir), 0.f );
	vec4 diffuse = diffuseColor * spotLight.diffuse * texture(u_material.diffuse, vUV);

	// �ſ� �ݻ� ����(���ݻ� ����) ���: Specular light
	// �ü� ����� �ݻ� ������� �������� ���� ����, ��¦�� ��� ������ ���̶���Ʈ ����
	float specularColor = pow( max( dot(normalVector, halfWayDir), 0.f ), u_material.shininess);
	vec4 specular = specularColor * spotLight.specular * texture(u_material.specular, vUV);


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
