#version 330 core

layout(location = 0) in vec3 a_vVertexPosition;
layout(location = 1) in vec2 a_vVertexUV;
layout(location = 2) in vec3 a_vVertexNormal;
layout(location = 3) in vec3 a_vVertexTangent;
layout(location = 4) in vec3 a_vVertexBitangent;

out vec3 vFragmentPos;
out vec3 vNormal;
out vec2 vUV;
out vec4 vFragmentPosLightSpace;
out mat3 mTBN;

uniform mat4 u_mModelTransform;
uniform mat4 u_mViewTransform;	
uniform mat4 u_mProjectionTransform;
uniform mat4 u_mLightSpaceMatrix;


void main()
{
	// 월드 좌표계에서의 정점 좌표
	vFragmentPos = vec3(u_mModelTransform * vec4(a_vVertexPosition, 1.0));

	mat3 modelTansformForNormal = mat3(transpose(inverse(u_mModelTransform)));

	// 월드 좌표계에서의 노멀 벡터 (스캐일 사용시 역 전치 사용)
	vNormal = modelTansformForNormal * a_vVertexNormal;

	// 텍스쳐 UV 좌표
	vUV = a_vVertexUV;

	vFragmentPosLightSpace = u_mLightSpaceMatrix * vec4(vFragmentPos, 1.0);

	// TBN 행렬 생성
	vec3 T = normalize(modelTansformForNormal * a_vVertexTangent);
	vec3 B = normalize(modelTansformForNormal * a_vVertexBitangent);
	vec3 N = normalize(modelTansformForNormal * a_vVertexNormal);
	
	mTBN = mat3(T, B, N);

	gl_Position = 
		u_mProjectionTransform * u_mViewTransform * u_mModelTransform * vec4(a_vVertexPosition, 1.0);
}