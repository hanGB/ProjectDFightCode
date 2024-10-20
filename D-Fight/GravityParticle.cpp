#include "stdafx.h"
#include "GravityParticle.h"
#include "Renderer.h"

CGravityParticle::CGravityParticle()
{
	
}

CGravityParticle::~CGravityParticle()
{
	glDeleteBuffers(1, &m_VAO);
}

void CGravityParticle::Update(float elapsedTimeInSec)
{
	m_fTime += elapsedTimeInSec;
}

void CGravityParticle::Render()
{
	CRenderer::GetInstance()->RenderGravityParticle(m_VAO, m_iVertexCount, m_TextureID,
		m_vPosition, m_vSize, m_vColor, m_fTime - m_fDelayTime, m_bIsLoop, m_bIsApplyGravity);
}

void CGravityParticle::SetVAO(GLuint vao)
{
	m_VAO = vao;
}

void CGravityParticle::SetVertexCount(int count)
{
	m_iVertexCount = count;
}

void CGravityParticle::SetTextureID(int id)
{
	m_TextureID = id;
}

void CGravityParticle::SetPosition(glm::vec3 pos)
{
	m_vPosition = pos;
}

void CGravityParticle::SetSize(glm::vec3 size)
{
	m_vSize = size;
}

void CGravityParticle::SetColor(glm::vec4 color)
{
	m_vColor = color;
}

void CGravityParticle::SetLifeTime(float time)
{
	m_fLifeTime = time;
}

float CGravityParticle::GetLifeTime() const
{
	return m_fLifeTime;
}

void CGravityParticle::SetIndex(int index)
{
	m_Index = index;
}

int CGravityParticle::GetIndex() const
{
	return m_Index;
}

void CGravityParticle::SetIsLoop(bool loop)
{
	m_bIsLoop = loop;
}

void CGravityParticle::SetIsApplyGravity(bool gravity)
{
	m_bIsApplyGravity = gravity;
}

void CGravityParticle::SetDelayTime(float time)
{
	m_fDelayTime = time;
}

float CGravityParticle::GetDelayTime() const
{
	return m_fDelayTime;
}
