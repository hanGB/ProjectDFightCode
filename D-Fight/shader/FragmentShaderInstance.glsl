#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 vFragmentPos;
in vec2 vUV;
in vec4 vFragmentPosLightSpace;
in vec2 vVisible;

uniform sampler2D u_texture;

// 카메라 위치
uniform vec3 u_vViewPos;
uniform bool u_bSpotLightOn;

// 가상의 노멀 벡터
vec3 c_vNormalVector = vec3(0.f, 1.f, 0.f);

// 디렉셔널 라이트(무한히 먼 거리의 광원에서 뿜어져 나오는 빛, 태양), 인스턴스에서는 수직 벡터를 노멀 사용
struct DirectionalLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 skylight;

	float shadowStrength;
};
uniform DirectionalLight u_dirLight;
vec3 CalculateDirectionalLight(DirectionalLight dirLight, bool visible);

// 디렉셔널 라이트 그림자 계산
uniform sampler2D u_shadowMap;
float ShadowCalculationForDirectionalLight(float strength, float bias);


// 포인트 라이트(모든 방향으로 뿜어져 나오는 빛, 전구), 인스턴스에서는 앰비언트 조명만 이용하여 계산
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

// 스포트 라이트(한 점에서 원뿔 모양으로 뿜어져 나오는 빛, 손전등)
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
	
	// 디렉셔널 라이트 계산
	result += CalculateDirectionalLight(u_dirLight, visible);

	// 포인트 라이트 계산
	if (visible)
		result += CalculatePointLight(u_pointLight, vFragmentPos, u_pointLight.position);

	for (int i = 0; i < u_iNumoOfPointLights; ++i) {
		if (u_bIsPointLightOn[i] && visible) {
			result += CalculatePointLight(u_pointLight, vFragmentPos, u_vPointLightPositions[i]);
		}
	}

	if (u_bSpotLightOn && visible)
		result += CalculateSpotLight(u_spotLight, vFragmentPos);

	// 최종 출력
	FragColor = vec4(result, 1.0) * texture(u_texture, vUV);
	
	// 깊이 처리
	float near = 0.1f, far = 500.0f;
	float ndc = gl_FragCoord.z * 2.0 - 1.0;	
	float linearDepth = (2.0 * near * far) / (far + near - ndc * (far - near)) / far;
	
	// 최종 출력
	FragColor = vec4(vec3(FragColor.rgb * (1.0f - linearDepth) + vec3(u_dirLight.skylight) * linearDepth), 1.0f);

	// 밝은 부분만 따로 저장
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

	// 전역 조명 계산: Ambient light
	vec3 ambient = dirLight.ambient;

	// 산란 반사 조명 계산: Diffuse light
	// 노멀 벡터과 조명 방향과의 내적으로 강도 조절
	float diffuseColor = max( dot(c_vNormalVector, lightDir), 0.f );
	vec3 diffuse = diffuseColor * dirLight.diffuse;

	// 표면 깊이보다 깊이가 작아지도록 바이어스 추가
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

	// 광원에 의한 범위 밖 그림자 적용 제거
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
	// 빛의 방향 계산
	vec3 lightDir = normalize(lightPos - fragPos);

	// 조명과의 거리
	float distance = length(lightPos - fragPos);
	
	// 감쇠 계산
	float attenuation = 1.0 / ( 
		pointLight.constant 
		+ pointLight.linear * distance 
		+ pointLight.quadratic * distance * distance );


	// 전역 조명 계산: Ambient light
	vec3 ambient = pointLight.ambient;
	float diffuseColor = max( dot(c_vNormalVector, lightDir), 0.f );
	vec3 diffuse = diffuseColor * pointLight.diffuse;
	
	ambient  *= attenuation;
	diffuse  *= attenuation;

	return ambient + diffuse;
}

vec3 CalculateSpotLight(SpotLight spotLight, vec3 fragPos)
{
	// 시선 방향, 표면의 법선의 정규화
	vec3 viewDir = normalize(u_vViewPos - fragPos);

	// 표면으로가는 빛, 표면의 법선의 정규화
	vec3 lightDir = normalize(spotLight.position - fragPos);

	// 기본 조명 오소 계산
	// 전역 조명 계산: Ambient light
	vec3 ambient = spotLight.ambient;

	// 산란 반사 조명 계산: Diffuse light
	// 노멀 벡터과 조명 방향과의 내적으로 강도 조절
	float diffuseColor = max( dot(c_vNormalVector, lightDir), 0.f );
	vec3 diffuse = diffuseColor * spotLight.diffuse;


	// 부드러운 가장자리를 위한 계산
	float theta = dot(lightDir, normalize(-spotLight.direction));
	float epsilon = spotLight.cutOff - spotLight.outherCutOff;
	float intensity = clamp((theta - spotLight.outherCutOff) / epsilon, 0.0, 1.0);

	// 디퓨즈에 빛 세기 적용
	diffuse *= intensity;


	// 빛 감쇠 계산(실제 감쇠 효과를 적용하기에는 너무 커서 바이어스 적용)
	float distance = length(spotLight.position - fragPos);
	distance *= spotLight.distanceBias;
	float attenuation = 1.0 / (distance * distance);

	// 모든 항에 빛 감쇠 적용
	ambient *= attenuation;
	diffuse *= attenuation;

	// 너무 밝거나 어두운 경우 조절
	ambient = clamp(ambient, 0.0f, 1.0f);
	diffuse = clamp(diffuse, 0.0f, 1.0f);

	return (ambient + diffuse);
}