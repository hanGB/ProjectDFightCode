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
	// ���� ��ǥ�迡���� ���� ��ǥ
	vFragmentPos = vec3(u_mModelTransform * vec4(a_vVertexPosition, 1.0));

	mat3 modelTansformForNormal = mat3(transpose(inverse(u_mModelTransform)));

	// ���� ��ǥ�迡���� ��� ���� (��ĳ�� ���� �� ��ġ ���)
	vNormal = modelTansformForNormal * a_vVertexNormal;

	// �ؽ��� UV ��ǥ
	vUV = a_vVertexUV;

	vFragmentPosLightSpace = u_mLightSpaceMatrix * vec4(vFragmentPos, 1.0);

	// TBN ��� ����
	vec3 T = normalize(modelTansformForNormal * a_vVertexTangent);
	vec3 B = normalize(modelTansformForNormal * a_vVertexBitangent);
	vec3 N = normalize(modelTansformForNormal * a_vVertexNormal);
	
	mTBN = mat3(T, B, N);

	gl_Position = 
		u_mProjectionTransform * u_mViewTransform * u_mModelTransform * vec4(a_vVertexPosition, 1.0);
}