#include "stdafx.h"
#include "DamageZone.h"


CDamageZone::CDamageZone()
{
	
}

CDamageZone::~CDamageZone()
{

}

void CDamageZone::Update(float elapsedTimeInSec, GameUpdateParameters parameters)
{
	if (!m_bAlwaysOn) {
		if (m_fEndTime + DEFAULT_DAMAGE_TIME < m_fTime) {
			SetLifeTime(0.0f);
		}
		else if (m_fEndTime < m_fTime) {
			m_bIsDamageOn = true;
		}

		m_fTime += elapsedTimeInSec;
	}
}

void CDamageZone::Render(CRenderer* renderer)
{
	static int m_iBoxModelID = renderer->LoadOBJModelWithIndex("./resource/obj/box.obj");
	static int m_iCylinderModelID = renderer->LoadOBJModelWithIndex("./resource/obj/cylinder.obj");

	renderer->UseShader(DAMAGE_ZONE_SHADER);

	if (m_bAlwaysOn) {
		m_fTime = m_fEndTime;
	}

	if (m_bIsCircle) {
		renderer->RenderDamageZone(m_iCylinderModelID, GetPosition(), GetRotation(), GetSize(), m_fTime / m_fEndTime);
	}
	else {
		renderer->RenderDamageZone(m_iBoxModelID, GetPosition(), GetRotation(), GetSize(), m_fTime / m_fEndTime);
	}

	renderer->UseShader(DEFAULT_SHADER);
}

bool CDamageZone::CheckCollision(CGameObject* object)
{
	if (m_bIsDamageOn || m_bAlwaysOn) {
		glm::vec3 objectPosition = object->GetPosition();
		glm::vec3 objectVolume = object->GetVolume();
		glm::vec3 position = GetPosition();
		glm::vec3 size = GetSize();

		if (m_bIsCircle) {
			glm::vec3 tempObjectPos = glm::vec3(objectPosition.x, 0.0f, objectPosition.z);
			glm::vec3 tempPos = glm::vec3(position.x, 0.0f, position.z);

			float dis = glm::distance(tempObjectPos, tempPos);

			// x,z축에 평행한 원으로 생각하여 계산
			if (dis > size.x) {
				return false;
			}
			else {
				float objectHight = objectPosition.y - objectVolume.y;
				if (objectHight > position.y + size.y) {;
					return false;
				}
				return true;
			}
		}
		else {
			return false;
		}
	}
	return false;
}

void CDamageZone::SetIsCircle(bool circle)
{
	m_bIsCircle = circle;
}

void CDamageZone::SetShowTime(float time)
{
	m_fEndTime = time;
}

void CDamageZone::SetIsDamageOn(bool on)
{
	m_bIsDamageOn = on;
}

bool CDamageZone::GetIsDamageOn() const
{
	return m_bIsDamageOn;
}


bool CDamageZone::GetIsNotVolumeCollision()
{
	if (m_bIsCircle) {
		return true;
	}
	else if (!m_bIsDamageOn) {
		return true;
	}
	return false;
}

void CDamageZone::SetAlwaysOn(bool on)
{
	m_bAlwaysOn = on;
}

void CDamageZone::SetIsGivenDamage(bool give)
{
	m_bIsGivenDamage = give;
}

bool CDamageZone::GetIsGivenDamage() const
{
	return m_bIsGivenDamage;
}
