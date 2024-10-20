#pragma once

// 모든 설정 완료 후 반드시 CreateVAO() 함수를 호출할 것!!!

class CGravityParticle {
public:
	CGravityParticle();
	~CGravityParticle();

private:
	GLuint m_VAO;
	int m_iVertexCount;

	int m_TextureID;
	
	glm::vec3 m_vPosition;
	glm::vec3 m_vSize;
	glm::vec4 m_vColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	float m_fTime = 0.f;
	float m_fLifeTime = 10000000.f;
	float m_fDelayTime = 0.f;

	int m_Index;

	bool m_bIsLoop = true;
	bool m_bIsApplyGravity = true;

public:
	void Update(float elapsedTimeInSec);
	void Render();

	void SetVAO(GLuint vao);
	void SetVertexCount(int count);

	void SetTextureID(int id);

	void SetPosition(glm::vec3 pos);
	void SetSize(glm::vec3 size);
	void SetColor(glm::vec4 color);

	void SetLifeTime(float time);
	float GetLifeTime() const;

	void SetIndex(int index);
	int GetIndex() const;
	
	void SetIsLoop(bool loop);
	void SetIsApplyGravity(bool gravity);

	void SetDelayTime(float time);
	float GetDelayTime() const;

	virtual void CreateVAO() = 0;

private:
	
};