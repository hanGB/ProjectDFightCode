#version 330 core

in vec3 vFragmentPos;
in vec3 vNormal;
in vec2 vUV;
in vec4 vFragmentPosLightSpace;
in mat3 mTBN;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

// 활성화된 지역 번호
uniform int u_iActiveArea;
const int c_AREA_WIDTH = 24;

// 카메라 위치
uniform vec3 u_vViewPos;
uniform bool u_bSpotLightOn;

// 색상
uniform vec4 u_vColor;

// 광원 여부(광원처리를 하지 않음)
uniform bool u_bIsLightSource;


// 텍스쳐 재질 정보 구조체
struct Material {
	sampler2D ambient;
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
	sampler2D normal;
};
uniform Material u_material;

// 디렉셔널 라이트(무한히 먼 거리의 광원에서 뿜어져 나오는 빛, 태양)
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

// 디렉셔널 라이트 그림자 계산
uniform sampler2D u_shadowMap;
float ShadowCalculationForDirectionalLight(float strength, float bias);


// 포인트 라이트(모든 방향으로 뿜어져 나오는 빛, 전구)
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
uniform int u_bIsPointLightOn[100];			// uniform은 bool값의 배열을 정확하게 받을 수 없음으로 int로 대신 사용
uniform int u_iNumoOfPointLights;
vec4 CalculatePointLight(PointLight pointLight, vec3 normalVector, vec3 fragPos, vec3 viewDir, vec3 pos);


// 스포트 라이트(한 점에서 원뿔 모양으로 뿜어져 나오는 빛, 손전등)
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

	// 광원일 경우 다른 계산없이 색상만 적용
	if (u_bIsLightSource) {
		
		if (visible) FragColor = u_vColor;
		else discard;

		// 밝은 부분만 따로 저장
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
		discard;		// 디퓨즈맵의 알파값이 0에 가까울 경우 프레그먼트 폐기

	// 시선 방향, 표면의 법선의 정규화
	vec3 viewDir = normalize(u_vViewPos - vFragmentPos);
	vec3 normalVector = texture(u_material.normal, vUV).rgb * 2.0 - 1.0;
	normalVector = normalize(mTBN * normalVector);

	vec4 result = vec4(0.0);
	
	// 디렉셔널 라이트 계산
	result += CalculateDirectionalLight(u_dirLight, normalVector, viewDir, visible);

	// 포인트 라이트 계산
	if (visible)
		result += CalculatePointLight(u_pointLight, normalVector, vFragmentPos, viewDir, u_pointLight.position);

	for (int i = 0; i < u_iNumoOfPointLights; ++i) {
		if (u_bIsPointLightOn[i] != 0 && visible) {
			result += CalculatePointLight(u_pointLight, normalVector, vFragmentPos, viewDir, u_vPointLightPositions[i]);
		}
	}

	// 스포트 라이트 계산
	if (u_bSpotLightOn && visible)
		result += CalculateSpotLight(u_spotLight, normalVector, vFragmentPos, viewDir);	
	
	// 깊이 처리
	float near = 0.1f, far = 500.0f;
	float ndc = gl_FragCoord.z * 2.0 - 1.0;	
	float linearDepth = (2.0 * near * far) / (far + near - ndc * (far - near)) / far;

	// 최종 출력
	FragColor = vec4(vec3((result.rgb * u_vColor.rgb) * (1.0f - linearDepth) + vec3(u_dirLight.skylight) * linearDepth),
				1.0f * u_vColor.a);

	// 밝은 부분만 따로 저장
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
	vec3 halfWayDir = normalize(lightDir + viewDir);		// Blinn-Phong을 위한 계산

	// 전역 조명 계산: Ambient light
	vec4 ambient = dirLight.ambient * texture(u_material.ambient, vUV);

	// 산란 반사 조명 계산: Diffuse light
	// 노멀 벡터과 조명 방향과의 내적으로 강도 조절
	float diffuseColor = max( dot(normalVector, lightDir), 0.f );
	vec4 diffuse = diffuseColor * dirLight.diffuse * texture(u_material.diffuse, vUV);


	// 거울 반사 조명(정반사 조명) 계산: Specular light
	// 반사 방향
	// vec3 reflectDir = reflect(-lightDir, normalVector);
	// 시선 방향과 반사 방향과의 내적으로 강도 조절, 반짝임 계수 값으로 하이라이트 생성
	float specularColor = pow( max( dot(normalVector, halfWayDir), 0.f ), u_material.shininess);
	vec4 specular =  specularColor * dirLight.specular * texture(u_material.specular, vUV);	

	// 표면 깊이보다 깊이가 작아지도록 바이어스 추가
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
	// 표면으로가는 빛, 표면의 법선의 정규화
	vec3 lightDir = normalize(pos - fragPos);
	vec3 halfWayDir = normalize(lightDir + viewDir);

	// 조명과의 거리
	float distance = length(pos - fragPos);
	
	// 감쇠 계산
	float attenuation = 1.0 / ( 
		pointLight.constant 
		+ pointLight.linear * distance 
		+ pointLight.quadratic * distance * distance );


	// 전역 조명 계산: Ambient light
	vec4 ambient = pointLight.ambient * texture(u_material.ambient, vUV);

	// 산란 반사 조명 계산: Diffuse light
	// 노멀 벡터과 조명 방향과의 내적으로 강도 조절
	float diffuseColor = max( dot(normalVector, lightDir), 0.f );
	vec4 diffuse = diffuseColor * pointLight.diffuse * texture(u_material.diffuse, vUV);

	// 거울 반사 조명(정반사 조명) 계산: Specular light
	// 반사 방향
	// vec3 reflectDir = reflect(-lightDir, normalVector);
	// 시선 방향과 반사 방향과의 내적으로 강도 조절, 반짝임 계수 값으로 하이라이트 생성
	float specularColor = pow( max( dot(normalVector, halfWayDir), 0.f ), u_material.shininess);
	vec4 specular = specularColor * pointLight.specular * texture(u_material.specular, vUV);

	ambient  *= attenuation;
	diffuse	 *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec4 CalculateSpotLight(SpotLight spotLight, vec3 normalVector, vec3 fragPos, vec3 viewDir)
{
	// 표면으로가는 빛, 표면의 법선의 정규화
	vec3 lightDir = normalize(spotLight.position - fragPos);
	vec3 halfWayDir = normalize(lightDir + viewDir);

	// 기본 조명 오소 계산
	// 전역 조명 계산: Ambient light
	vec4 ambient = spotLight.ambient * texture(u_material.ambient, vUV);

	// 산란 반사 조명 계산: Diffuse light
	// 노멀 벡터과 조명 방향과의 내적으로 강도 조절
	float diffuseColor = max( dot(normalVector, lightDir), 0.f );
	vec4 diffuse = diffuseColor * spotLight.diffuse * texture(u_material.diffuse, vUV);

	// 거울 반사 조명(정반사 조명) 계산: Specular light
	// 시선 방향과 반사 방향과의 내적으로 강도 조절, 반짝임 계수 값으로 하이라이트 생성
	float specularColor = pow( max( dot(normalVector, halfWayDir), 0.f ), u_material.shininess);
	vec4 specular = specularColor * spotLight.specular * texture(u_material.specular, vUV);


	// 부드러운 가장자리를 위한 계산
	float theta = dot(lightDir, normalize(-spotLight.direction));
	float epsilon = spotLight.cutOff - spotLight.outherCutOff;
	float intensity = clamp((theta - spotLight.outherCutOff) / epsilon, 0.0, 1.0);

	// 디퓨즈와 스페큘러에 빛의 세기 적용
	diffuse *= intensity;
	specular *= intensity;


	// 빛 감쇠 계산(실제 감쇠 효과를 적용하기에는 너무 커서 바이어스 적용)
	float distance = length(spotLight.position - fragPos);
	distance *= spotLight.distanceBias;
	float attenuation = 1.0 / (distance * distance);


	// 너무 밝거나 어두운 경우 조절
	ambient = clamp(ambient, 0.0f, 1.0f);
	diffuse = clamp(diffuse, 0.0f, 1.0f);


	return (ambient + diffuse + specular);
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
