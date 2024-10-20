#include "stdafx.h"
#include "ObjectManager.h"
#include "DamageZone.h"
#include "FieldMonster.h"

CObjectManager::CObjectManager()
{
	m_pRenderer = CRenderer::GetInstance();
}

CObjectManager::~CObjectManager()
{
	if (m_fWallSizes != NULL) {
		delete[] m_fWallSizes;
	}
}

void CObjectManager::DeleteAllObjectWithType(GameObjectType type)
{
	auto it = m_ObjectList.begin();
	while (it != m_ObjectList.end())
	{
		GameObjectType objectType = (*it)->GetType();
		if (objectType == type)
		{
			delete (*it);
			m_ObjectList.erase(it++);
			continue;
		}
		it++;
	}
}

void CObjectManager::DeleteAllNoInteractionObjectWithType(GameObjectType type)
{
	auto it = m_NoInteractionObjectList.begin();
	while (it != m_NoInteractionObjectList.end())
	{
		GameObjectType objectType = (*it)->GetType();
		if (objectType == type)
		{
			delete (*it);
			m_NoInteractionObjectList.erase(it++);
			continue;
		}
		it++;
	}
}

void CObjectManager::DeleteAllParticle()
{
	auto it = m_GravityParticleList.begin();
	while (it != m_GravityParticleList.end())
	{
		delete (*it);
		m_GravityParticleList.erase(it++);
		continue;

		it++;
	}
}

void CObjectManager::Update(float elapsedTimeInSec)
{
	// 쓰레기 처리
	DoGarbegeCollection(elapsedTimeInSec);

	// 충돌 처리
	CheckCollision();

	for (auto object : m_ObjectList)
	{
		GameObjectType type = object->GetType();

		if (type == TYPE_HERO)
			object->Update(elapsedTimeInSec, m_HeroUpdateParameters);
		else if (type == TYPE_FIXED || type == TYPE_FLOOR || type == TYPE_COLLISION_IGNORE_AREA
			|| type == TYPE_WEAPON || type == TYPE_CAMERA_COLLISION || type == TYPE_RECOVER_SPOT)
			continue; // 고정된 물체는 힘을 받지 않음
		else
			object->Update(elapsedTimeInSec, m_OthersUpdateParameters);
	}

	for (auto particle : m_GravityParticleList)
	{
		particle->Update(elapsedTimeInSec);
	}

	for (auto object : m_ObjectList)
	{
		object->CalculateVerticesAfterAnimation(m_pRenderer);
	}
}

void CObjectManager::Render()
{
	for (auto object : m_NoInteractionObjectList)
	{
		if (object->GetWhetherToRender())
		{
			object->Render(m_pRenderer);
			if (object->GetBorder())
			{
				m_pRenderer->UseShader(BORDER_SHADER);
				object->BorderRender(m_pRenderer);
				glClear(GL_STENCIL_BUFFER_BIT);
				m_pRenderer->UseShader(DEFAULT_SHADER);
			}
		}
	}

	for (auto object : m_ObjectList)
	{
		if (object->GetWhetherToRender())
		{
			object->Render(m_pRenderer);
			if (object->GetBorder())
			{
				m_pRenderer->UseShader(BORDER_SHADER);
				object->BorderRender(m_pRenderer);
				glClear(GL_STENCIL_BUFFER_BIT);
				m_pRenderer->UseShader(DEFAULT_SHADER);
			}
		}
	}
}

void CObjectManager::DepthRender()
{
	for (auto object : m_NoInteractionObjectList)
	{
		if (object->GetWhetherToRender())
		{
			if (object->GetIsMadeShadow()) {
				if (object->GetType() != TYPE_DAMAGE_ZONE) {
					object->DepthRender(m_pRenderer);
				}
			}
		}
	}

	for (auto object : m_ObjectList)
	{
		if (object->GetWhetherToRender())
		{
			if (object->GetIsMadeShadow()) {
				if (object->GetType() != TYPE_DAMAGE_ZONE) {
					object->DepthRender(m_pRenderer);
				}
			}
		}
	}
}

void CObjectManager::SetUpdateParameters(GameUpdateParameters heroUpdateParameters, GameUpdateParameters othersUpdateParameters)
{
	m_HeroUpdateParameters = heroUpdateParameters;
	m_OthersUpdateParameters = othersUpdateParameters;
}

void CObjectManager::BoundingBoxRender()
{
	for (auto object : m_ObjectList)
	{
		if (object->GetEnableCollision())
			object->BoundingBoxRenderer(m_pRenderer);
	}
}

void CObjectManager::RenderParticle()
{
	for (auto particle : m_GravityParticleList)
	{
		particle->Render();
	}
}

void CObjectManager::DoGarbegeCollection(float elapsedTimeInSec)
{
	auto it = m_ObjectList.begin();
	while (it != m_ObjectList.end())
	{
		float lifeTime = (*it)->GetLifeTime();
		if (lifeTime <= 0.f)
		{
			// 자식 제거
			auto cit = m_ObjectList.begin();
			while (cit != m_ObjectList.end())
			{
				if ((*cit)->GetParent() == (*it)) 
				{
					delete (*cit);
					m_ObjectList.erase(cit++);
					continue;
				}
				cit++;
			}
			delete (*it);
			m_ObjectList.erase(it++);	
			continue;
		}

		lifeTime -= elapsedTimeInSec;
		(*it)->SetLifeTime(lifeTime);
		it++;
	}

	it = m_NoInteractionObjectList.begin();
	while (it != m_NoInteractionObjectList.end())
	{
		float lifeTime = (*it)->GetLifeTime();
		if (lifeTime <= 0.f)
		{
			delete (*it);
			m_NoInteractionObjectList.erase(it++);
			continue;
		}

		lifeTime -= elapsedTimeInSec;
		(*it)->SetLifeTime(lifeTime);
		it++;
	}

	// 파티클
	auto itParticle = m_GravityParticleList.begin();
	while (itParticle != m_GravityParticleList.end())
	{
		float lifeTime = (*itParticle)->GetLifeTime();
		if (lifeTime <= 0.f)
		{
			delete (*itParticle);
			m_GravityParticleList.erase(itParticle++);
			continue;
		}

		lifeTime -= elapsedTimeInSec;
		(*itParticle)->SetLifeTime(lifeTime);
		itParticle++;
	}
}

void CObjectManager::DeleteChildOfObject(CGameObject* object) 
{
	auto it = m_ObjectList.begin();
	while (it != m_ObjectList.end())
	{
		if ((*it)->GetParent() == object)
		{
			delete (*it);
			m_ObjectList.erase(it++);
			continue;
		}
		it++;
	}
}

bool CObjectManager::AABBCollision(CGameObject* objectA, CGameObject* objectB)
{
	// 회전이 없는 충돌 계산

	// 위치 얻기
	glm::vec3 aPos = objectA->GetPosition();
	glm::vec3 bPos = objectB->GetPosition();

	// 크기 변환
	glm::mat4 aScaleMat = glm::mat4(1.0f); 
	aScaleMat = glm::scale(aScaleMat, objectA->GetSize());

	glm::mat4 bScaleMat = glm::mat4(1.0f);
	bScaleMat = glm::scale(bScaleMat, objectB->GetSize());

	// 부피 얻기
	glm::vec3 aVolume = aScaleMat * glm::vec4(objectA->GetVolume(), 1.0f);
	glm::vec3 bVolume = bScaleMat * glm::vec4(objectB->GetVolume(), 1.0f);

	glm::vec3 aMin = glm::vec3(
		aPos.x - (aVolume.x), 
		aPos.y - (aVolume.y),
		aPos.z - (aVolume.z));
	glm::vec3 aMax = glm::vec3(
		aPos.x + (aVolume.x),
		aPos.y + (aVolume.y),
		aPos.z + (aVolume.z));

	glm::vec3 bMin = glm::vec3(
		bPos.x - (bVolume.x),
		bPos.y - (bVolume.y),
		bPos.z - (bVolume.z));
	glm::vec3 bMax = glm::vec3(
		bPos.x + (bVolume.x),
		bPos.y + (bVolume.y),
		bPos.z + (bVolume.z));

	if (aMin.x > bMax.x) return false;
	if (aMin.y > bMax.y) return false;
	if (aMin.z > bMax.z) return false;
	if (aMax.x < bMin.x) return false;
	if (aMax.y < bMin.y) return false;
	if (aMax.z < bMin.z) return false;

	ProcessCollisionWithVolume(objectA, objectB, aVolume, bVolume);
	return true;
}

bool CObjectManager::AABBCollisionWithOnlyRightRotattion(CGameObject* objectA, CGameObject* objectB)
{
	// 위치 얻기
	glm::vec3 aPos = objectA->GetPosition();
	glm::vec3 bPos = objectB->GetPosition();

	// 크기 변환
	glm::mat4 aScaleMat = glm::mat4(1.0f);
	aScaleMat = glm::scale(aScaleMat, objectA->GetSize());

	glm::mat4 bScaleMat = glm::mat4(1.0f);
	bScaleMat = glm::scale(bScaleMat, objectB->GetSize());

	// 회전 행렬
	glm::quat aQuat
		= glm::quat(glm::vec3(
			glm::radians(objectA->GetRotation().x), 
			glm::radians(objectA->GetRotation().y), 
			glm::radians(objectA->GetRotation().z)));
	glm::mat4 aRotMat = glm::toMat4(aQuat);

	// 회전 행렬
	glm::quat bQuat
		= glm::quat(glm::vec3(
			glm::radians(objectB->GetRotation().x),
			glm::radians(objectB->GetRotation().y),
			glm::radians(objectB->GetRotation().z)));
	glm::mat4 bRotMat = glm::toMat4(bQuat);

	// 부피 얻기
	glm::vec3 aVolume = aRotMat * aScaleMat * glm::vec4(objectA->GetVolume(), 1.0f);
	glm::vec3 bVolume = bRotMat * bScaleMat * glm::vec4(objectB->GetVolume(), 1.0f);

	// 절대값으로 변경
	aVolume = glm::vec3(abs(aVolume.x), abs(aVolume.y), abs(aVolume.z));
	bVolume = glm::vec3(abs(bVolume.x), abs(bVolume.y), abs(bVolume.z));

	glm::vec3 aMin = glm::vec3(
		aPos.x - (aVolume.x),
		aPos.y - (aVolume.y),
		aPos.z - (aVolume.z));
	glm::vec3 aMax = glm::vec3(
		aPos.x + (aVolume.x),
		aPos.y + (aVolume.y),
		aPos.z + (aVolume.z));

	glm::vec3 bMin = glm::vec3(
		bPos.x - (bVolume.x),
		bPos.y - (bVolume.y),
		bPos.z - (bVolume.z));
	glm::vec3 bMax = glm::vec3(
		bPos.x + (bVolume.x),
		bPos.y + (bVolume.y),
		bPos.z + (bVolume.z));

	if (aMin.x > bMax.x) return false;
	if (aMin.y > bMax.y) return false;
	if (aMin.z > bMax.z) return false;
	if (aMax.x < bMin.x) return false;
	if (aMax.y < bMin.y) return false;
	if (aMax.z < bMin.z) return false;

	ProcessCollisionWithVolume(objectA, objectB, aVolume, bVolume);
	return true;
}

bool CObjectManager::OBBCollision(CGameObject* objectA, CGameObject* objectB)
{
	// 두 물체의 위치 얻기
	glm::vec3 aPos = objectA->GetPosition();
	glm::vec3 bPos = objectB->GetPosition();

	// 두 물체의 중심 사이의 거리 계산
	glm::vec3 centerDis = aPos - bPos;

	// 크기 변환
	glm::mat4 aScaleMat = glm::mat4(1.0f);
	aScaleMat = glm::scale(aScaleMat, objectA->GetSize());
	glm::mat4 bScaleMat = glm::mat4(1.0f);
	bScaleMat = glm::scale(bScaleMat, objectB->GetSize());

	// 회전 행렬
	glm::quat aQuat
		= glm::quat(glm::vec3(
			glm::radians(objectA->GetRotation().x),
			glm::radians(objectA->GetRotation().y),
			glm::radians(objectA->GetRotation().z)));
	glm::mat4 aRotMat = glm::toMat4(aQuat);

	// 회전 행렬
	glm::quat bQuat
		= glm::quat(glm::vec3(
			glm::radians(objectB->GetRotation().x),
			glm::radians(objectB->GetRotation().y),
			glm::radians(objectB->GetRotation().z)));
	glm::mat4 bRotMat = glm::toMat4(bQuat);

	// 부피 얻기
	glm::vec3 aVolume = aRotMat * aScaleMat * glm::vec4(objectA->GetVolume(), 1.0f);
	glm::vec3 bVolume = bRotMat * bScaleMat * glm::vec4(objectB->GetVolume(), 1.0f);

	// 절대값으로 변경
	aVolume = glm::vec3(abs(aVolume.x), abs(aVolume.y), abs(aVolume.z));
	bVolume = glm::vec3(abs(bVolume.x), abs(bVolume.y), abs(bVolume.z));

	glm::vec3 aLength = glm::vec3(aVolume.x, aVolume.y, aVolume.z);
	glm::vec3 bLength = glm::vec3(bVolume.x, bVolume.y, bVolume.z);

	glm::vec3 aAxisDir[3];
	glm::vec3 bAxisDir[3];

	// 각 축의 방향의 노말라이즈 값 계산
	aAxisDir[0] = glm::normalize(aRotMat * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	aAxisDir[1] = glm::normalize(aRotMat * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	aAxisDir[2] = glm::normalize(aRotMat * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

	bAxisDir[0] = glm::normalize(bRotMat * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	bAxisDir[1] = glm::normalize(bRotMat * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	bAxisDir[2] = glm::normalize(bRotMat * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

	float c[3][3];
	float absC[3][3];
	float d[3];
	float r0, r1, r;
	const float cutOff = 0.999999f;
	bool existsParallelPair = false;


	for (int i = 0; i < 3; ++i)
	{
		c[0][i] = glm::dot(aAxisDir[0], bAxisDir[i]);
		absC[0][i] = std::abs(c[0][i]);
		if (absC[0][i] > cutOff) existsParallelPair = true;
	}
	d[0] = glm::dot(centerDis, aAxisDir[0]);
	r = std::abs(d[0]);
	r0 = aLength.x;
	r1 = bLength.x * absC[0][0] + bLength.y * absC[0][1] + bLength.z * absC[0][2];
	if (r > r0 + r1) return false;

	for (int i = 0; i < 3; ++i)
	{
		c[1][i] = glm::dot(aAxisDir[1], bAxisDir[i]);
		absC[1][i] = std::abs(c[1][i]);
		if (absC[1][i] > cutOff) existsParallelPair = true;
	}
	d[1] = glm::dot(centerDis, aAxisDir[1]);
	r = std::abs(d[1]);
	r0 = aLength.y;
	r1 = bLength.x * absC[1][0] + bLength.y * absC[1][1] + bLength.z * absC[1][2];
	if (r > r0 + r1) return false;

	for (int i = 0; i < 3; ++i)
	{
		c[2][i] = glm::dot(aAxisDir[2], bAxisDir[i]);
		absC[2][i] = std::abs(c[2][i]);
		if (absC[2][i] > cutOff) existsParallelPair = true;
	}
	d[2] = glm::dot(centerDis, aAxisDir[2]);
	r = std::abs(d[2]);
	r0 = aLength.z;
	r1 = bLength.x * absC[2][0] + bLength.y * absC[2][1] + bLength.z * absC[2][2];
	if (r > r0 + r1) return false;


	r = std::abs(glm::dot(centerDis, bAxisDir[0]));
	r0 = aLength.x * absC[0][0] + aLength.y * absC[1][0] + aLength.z * absC[2][0];
	r1 = bLength.x;
	if (r > r0 + r1) return false;

	r = std::abs(glm::dot(centerDis, bAxisDir[1]));
	r0 = aLength.x * absC[0][1] + aLength.y * absC[1][1] + aLength.z * absC[2][1];
	r1 = bLength.y;
	if (r > r0 + r1) return false;

	r = std::abs(glm::dot(centerDis, bAxisDir[2]));
	r0 = aLength.x * absC[0][2] + aLength.y * absC[1][2] + aLength.z * absC[2][2];
	r1 = bLength.z;
	if (r > r0 + r1) return false;

	if (existsParallelPair)
	{
		ProcessCollisionWithVolume(objectA, objectB, aVolume, bVolume);
		return true;
	}

	r = std::abs(d[2] * c[1][0] - d[1] * c[2][0]);
	r0 = aLength.y * absC[2][0] + aLength.z * absC[1][0];
	r1 = bLength.y * absC[0][2] + bLength.z * absC[0][1];
	if (r > r0 + r1) return false;

	r = std::abs(d[2] * c[1][1] - d[1] * c[2][1]);
	r0 = aLength.y * absC[2][1] + aLength.z * absC[1][1];
	r1 = bLength.x * absC[0][2] + bLength.z * absC[0][0];
	if (r > r0 + r1) return false;

	r = std::abs(d[2] * c[1][2] - d[1] * c[2][2]);
	r0 = aLength.y * absC[2][2] + aLength.z * absC[1][2];
	r1 = bLength.x * absC[0][1] + bLength.y * absC[0][0];
	if (r > r0 + r1) return false;

	r = std::abs(d[0] * c[2][0] - d[2] * c[0][0]);
	r0 = aLength.x * absC[2][0] + aLength.z * absC[0][0];
	r1 = bLength.y * absC[1][2] + bLength.z * absC[1][1];
	if (r > r0 + r1) return false;

	r = std::abs(d[0] * c[2][1] - d[2] * c[0][1]);
	r0 = aLength.x * absC[2][1] + aLength.z * absC[0][1];
	r1 = bLength.x * absC[1][2] + bLength.z * absC[1][0];
	if (r > r0 + r1) return false;

	r = std::abs(d[0] * c[2][2] - d[2] * c[0][2]);
	r0 = aLength.x * absC[2][2] + aLength.z * absC[0][2];
	r1 = bLength.x * absC[1][1] + bLength.y * absC[1][0];
	if (r > r0 + r1) return false;

	r = std::abs(d[1] * c[0][0] - d[0] * c[1][0]);
	r0 = aLength.x * absC[1][0] + aLength.y * absC[0][0];
	r1 = bLength.y * absC[2][2] + bLength.z * absC[2][1];
	if (r > r0 + r1) return false;

	r = std::abs(d[1] * c[0][1] - d[0] * c[1][1]);
	r0 = aLength.x * absC[1][1] + aLength.y * absC[0][1];
	r1 = bLength.x * absC[2][2] + bLength.z * absC[2][0];
	if (r > r0 + r1) return false;

	r = std::abs(d[1] * c[0][2] - d[0] * c[1][2]);
	r0 = aLength.x * absC[1][2] + aLength.y * absC[0][2];
	r1 = bLength.x * absC[2][1] + bLength.y * absC[2][0];
	if (r > r0 + r1) return false;

	ProcessCollisionWithVolume(objectA, objectB, aVolume, bVolume);
	return true;
}

void CObjectManager::CheckCollision()
{
	// 원형 바닥과 충돌
	for (auto object : m_ObjectList)
	{
		CircleFloorCollision(object, m_fFloorHeight, m_fDiameter);
	}

	// 오브젝트 끼리 충돌
	if (m_ObjectList.size() > 1)
	{
		//std::cout << "- 충돌 처리 시작\n";
		for (auto objectA = m_ObjectList.begin(); objectA != m_ObjectList.end(); ++objectA)
		{
			if (!(*objectA)->GetEnableCollision())
				continue;

			std::list<CGameObject*>::iterator objectB = objectA;
			objectB++;

			bool noRotation = false;
			if ((*objectA)->GetRotation() == glm::vec3(0.0f, 0.0f, 0.0f)
				|| (*objectA)->GetRotation() == glm::vec3(0.0f, 180.0f, 0.0f))
				noRotation = true;

			bool rightRotation = CheckOnlyRightAngleRotation((*objectA)->GetRotation());

			bool AisBoxCollision = (*objectA)->GetIsBoxCollision();

			for (objectB; objectB != m_ObjectList.end(); ++objectB)
			{
				if (!(*objectB)->GetEnableCollision())
					continue;

				if (*objectB == *objectA)
					continue;

				if ((*objectA)->GetParent() == (*objectB)) {
					continue;
				}
				else if ((*objectB)->GetParent() == (*objectA)) {
					continue;
				}

				bool BisBoxCollision = (*objectB)->GetIsBoxCollision();

				bool isCollision = false;
				bool isNotVolumeCollision = false;
				if ((*objectA)->GetType() == TYPE_DAMAGE_ZONE && (*objectB)->GetType() == TYPE_HERO) {
					isNotVolumeCollision = dynamic_cast<CDamageZone*>(*objectA)->GetIsNotVolumeCollision();
					isCollision = dynamic_cast<CDamageZone*>(*objectA)->CheckCollision(*objectB);
				}
				else if ((*objectB)->GetType() == TYPE_DAMAGE_ZONE && (*objectA)->GetType() == TYPE_HERO) {
					isNotVolumeCollision = dynamic_cast<CDamageZone*>(*objectB)->GetIsNotVolumeCollision();
					isCollision = dynamic_cast<CDamageZone*>(*objectB)->CheckCollision(*objectA);
				}

				if (!isNotVolumeCollision) {
					if (AisBoxCollision && BisBoxCollision) {
						if (noRotation
							&& ((*objectB)->GetRotation() == glm::vec3(0.0f, 0.0f, 0.0f)
								|| (*objectB)->GetRotation() == glm::vec3(0.0f, 180.0f, 0.0f))) {
							isCollision = AABBCollision(*objectA, *objectB);
						}
						else if (rightRotation && CheckOnlyRightAngleRotation((*objectB)->GetRotation())) {
							isCollision = AABBCollisionWithOnlyRightRotattion(*objectA, *objectB);
						}
						else {
							isCollision = OBBCollision(*objectA, *objectB);
						}
					}
					else if (!AisBoxCollision && !BisBoxCollision) {
						isCollision = CapsuleCollision(*objectA, *objectB);
					}
					else {
						if (AisBoxCollision) {
							isCollision = BoxAndCapsuleCollision(*objectA, *objectB);
						}
						else {
							isCollision = BoxAndCapsuleCollision(*objectB, *objectA);
						}
					}
				}

				// 충돌되었을 경우(부피, 위치 등의 정보가 필요없는 계산용)
				if (isCollision) {
					ProcessCollisionWithOtherData(*objectA, *objectB);
				}
			}
		}
	}

	for (auto object : m_ObjectList)
	{
		if (object->GetEnableCollision())
		{
			if (!object->GetCollisioned())
			{
				object->SetState(STATE_FALLING);
			}
			object->SetCollisioned(false);
		}
	}
}

void CObjectManager::ProcessCollisionWithVolume(
	CGameObject* objectA, CGameObject* objectB, glm::vec3 volumeA, glm::vec3 volumeB)
{
	GameObjectType typeA = objectA->GetType();
	GameObjectType typeB = objectB->GetType();

	if ((typeA == TYPE_FIXED && (typeB == TYPE_HERO || typeB == TYPE_MOVABLE 
		|| typeB == TYPE_BULLET || typeB == TYPE_ENEMY))
		|| (typeB == TYPE_FIXED && (typeA == TYPE_HERO || typeA == TYPE_MOVABLE 
			|| typeA == TYPE_BULLET || typeB == TYPE_ENEMY))) {
		if (objectA->GetType() == TYPE_FIXED) {
			AdjustPositionFixedA(objectA, objectB, volumeA, volumeB);
		}
		else {
			AdjustPositionFixedA(objectB, objectA, volumeB, volumeA);
		}
	}
	else if ((typeA == TYPE_MOVABLE && typeB == TYPE_HERO)
		|| (typeA == TYPE_HERO && typeB == TYPE_MOVABLE)) {
		// 주인공이 고정된 물체와 충돌된 걸로 처리
		if (typeA == TYPE_MOVABLE) {
			AdjustPositionFixedA(objectA, objectB, volumeA, volumeB);
		}
		else {
			AdjustPositionFixedA(objectB, objectA, volumeB, volumeA);
		}
	}
	else if ((typeA == TYPE_ENEMY && typeB == TYPE_HERO)
		|| (typeA == TYPE_HERO && typeB == TYPE_ENEMY)) {
		// 주인공이 고정된 물체와 충돌된 걸로 처리
		if (typeA == TYPE_ENEMY) {
			AdjustPositionFixedA(objectA, objectB, volumeA, volumeB);
		}
		else {
			AdjustPositionFixedA(objectB, objectA, volumeB, volumeA);
		}
	}
	else if ((typeA == TYPE_ENEMY && typeB == TYPE_ENEMY)) {
		if (m_bIsEnabledCollionWithEnemies) {
			// 움직이는 물체와 움직이는 물체의 충돌은 무게가 더 큰 쪽을 고정된 물체로 가정하고 처리
			if (objectA->GetMass() >= objectB->GetMass()) {
				AdjustPositionFixedA(objectA, objectB, volumeA, volumeB);
			}
			else {
				AdjustPositionFixedA(objectB, objectA, volumeB, volumeA);
			}
		}
	}
}

void CObjectManager::AdjustPositionFixedA(CGameObject* objectA, CGameObject* objectB, glm::vec3 volumeA, glm::vec3 volumeB)
{
	// 두 물체의 위치 얻기
	glm::vec3 aPos = objectA->GetPosition();
	glm::vec3 bPos = objectB->GetPosition();

	bool isStairObject = false;
	// 오브젝트의 맨 아래보다 고정된 오브젝트의 위가 낮은 경우
	if ((bPos.y - volumeB.y) > (aPos.y + volumeA.y - STAIR_STEP_HEIGHT))
		isStairObject = true;

	bool isDiagonalFalling = false;
	// 고정된 오브젝트의 절반보다 아래 있을 경우 대각선으로 옆으로 판정
	if (bPos.y - volumeB.y < aPos.y)
		isDiagonalFalling = true;

	// 떨어지고 있던 중이거나 바닥에 붙어서 이동 중이고 계단처럼 올라갈 수 있으면
	if ((objectB->GetState() == STATE_FALLING && !isDiagonalFalling) 
		|| (objectB->GetState() == STATE_GROUND && isStairObject))
	{
		// 고정된 오브젝트 위로 위치 변경
		glm::vec3 pos = glm::vec3(bPos.x, bPos.y
			+ (aPos.y + (volumeA.y)) - (bPos.y - (volumeB.y)), bPos.z);
		objectB->SetState(STATE_GROUND);
		objectB->SetPosition(pos);
		glm::vec3 velocity = objectB->GetVelocity();
		objectB->SetVelocity(glm::vec3(velocity.x, 0, velocity.z));
		objectB->SetCollisioned(true);
	}
	// 대각선으로 떨어지고 있거나 바닥에 붙어서 이동 중이고 계단처럼 올라갈 수 없으면
	else if ((objectB->GetState() == STATE_FALLING && isDiagonalFalling) ||
		objectB->GetState() == STATE_GROUND && !isStairObject)
	{
		glm::vec3 velocity = objectB->GetVelocity();
		// 고정된 오브젝트 옆으로 위치 변경
		if ((aPos.z + volumeA.z - volumeA.z * 0.1) > bPos.z && bPos.z > (aPos.z - volumeA.z + volumeA.z * 0.1)) {
			glm::vec3 pos;
			if (bPos.x >= aPos.x)
				pos = glm::vec3(aPos.x + volumeA.x + volumeB.x,
					bPos.y, bPos.z);
			else
				pos = glm::vec3((aPos.x - volumeA.x - volumeB.x),
					bPos.y, bPos.z);

			objectB->SetPosition(pos);
			glm::vec3 velocity = objectB->GetVelocity();
			objectB->SetVelocity(glm::vec3(0, velocity.y, velocity.z));
		}	
		else {
			glm::vec3 pos;
			if (bPos.z >= aPos.z)
				pos = glm::vec3(bPos.x, bPos.y,
					aPos.z + volumeA.z + volumeB.z);
			else
				pos = glm::vec3(bPos.x, bPos.y,
					aPos.z - volumeA.z - volumeB.z);

			objectB->SetPosition(pos);
			glm::vec3 velocity = objectB->GetVelocity();
			objectB->SetVelocity(glm::vec3(velocity.x, velocity.y, 0));
		}
	}
}

bool CObjectManager::CapsuleCollision(CGameObject* objectA, CGameObject* objectB)
{
	// 충돌체 정보 얻기
	float radiusA, radiusB, heightA, heightB;
	objectA->GetRadiusAndHeight(&radiusA, &heightA);
	objectB->GetRadiusAndHeight(&radiusB, &heightB);

	// 두 물체의 위치 얻기
	glm::vec3 aPos = objectA->GetPosition();
	glm::vec3 bPos = objectB->GetPosition();

	// 두 물체 사이의 거리
	float disXZ = glm::distance(glm::vec3(aPos.x, 0.0f, aPos.z), glm::vec3(bPos.x, 0.0f, bPos.z));
	float disY = glm::distance(glm::vec3(0.0f, aPos.y, 0.0f), glm::vec3(0.0f, bPos.y, 0.0f));
	
	if (disXZ >= (radiusA + radiusB)) return false;
	if (disY >= heightA + heightB) return false;

	GameObjectType typeA = objectA->GetType();
	GameObjectType typeB = objectB->GetType();

	glm::vec3 aVolume = objectA->GetVolume();
	glm::vec3 bVolume = objectB->GetVolume();

	ProcessCollisionWithVolume(objectA, objectB, aVolume, bVolume);
	return true;
}

bool CObjectManager::BoxAndCapsuleCollision(CGameObject* boxObject, CGameObject* capsuleObject)
{
	// 캡슐 충돌체 정보 얻기
	float radius, height;
	capsuleObject->GetRadiusAndHeight(&radius, &height);

	// 두 물체의 위치 얻기
	glm::vec3 aPos = boxObject->GetPosition();
	glm::vec3 bPos = capsuleObject->GetPosition();

	// 박스 충돌체 정보 얻기
	// 크기 변환
	glm::mat4 scaleMat = glm::mat4(1.0f);
	scaleMat = glm::scale(scaleMat, boxObject->GetSize());
	// 회전 행렬
	glm::quat qt
		= glm::quat(glm::vec3(
			glm::radians(boxObject->GetRotation().x),
			glm::radians(boxObject->GetRotation().y),
			glm::radians(boxObject->GetRotation().z)));
	glm::mat4 rotMat = glm::toMat4(qt);
	// 부피 얻기
	glm::vec3 volume = rotMat * scaleMat * glm::vec4(boxObject->GetVolume(), 1.0f);
	// 절대값으로 변경
	volume = glm::vec3(abs(volume.x), abs(volume.y), abs(volume.z));

	// 두 물체 사이의 거리
	float disXZ = glm::distance(glm::vec3(aPos.x, 0.0f, aPos.z), glm::vec3(bPos.x, 0.0f, bPos.z));
	float disY = glm::distance(glm::vec3(0.0f, aPos.y, 0.0f), glm::vec3(0.0f, bPos.y, 0.0f));

	if (disXZ >= radius + volume.x) return false;
	if (disXZ >= radius + volume.z) return false;
	if (disY >= height + volume.y) return false;

	glm::vec3 boxVolume = boxObject->GetVolume();
	glm::vec3 capsuleVolume = capsuleObject->GetVolume();

	ProcessCollisionWithVolume(boxObject, capsuleObject, boxVolume, capsuleVolume);
	return true;
}

void CObjectManager::CircleFloorCollision(CGameObject* object, float height, float diameter)
{
	// 필요 데이트 얻기
	glm::vec3 position = object->GetPosition();
	glm::vec3 size = object->GetSize();
	glm::vec3 rotation = object->GetRotation();
	glm::vec3 volume = object->GetVolume();
	glm::vec3 velocity = object->GetVelocity();

	if (velocity == glm::vec3(0.0f, 0.0f, 0.0f))
		return;

	// 크기 계산
	if (size != glm::vec3(1.0f, 1.0f, 1.0f)) 
	{
		glm::mat4 scaleMat = glm::mat4(1.0f);
		scaleMat = glm::scale(scaleMat, size);

		volume = scaleMat * glm::vec4(volume, 1.0f);
	}
	// 회전 계산
	if (rotation != glm::vec3(0.0f, 0.0f, 0.0f))
	{
		glm::quat modelQuat
			= glm::quat(glm::vec3(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z)));
		glm::mat4 rotationMat = glm::toMat4(modelQuat);

		volume = rotationMat * glm::vec4(volume, 1.0f);
	}

	// 중심과의 거리 계산
	float distance = glm::distance(glm::vec3(position.x, height, position.z), glm::vec3(0.0f, height, 0.0f));

	// 거리가 반지름보다 작을 경우 원형 발판 안
	if (distance < (diameter / 2.0f)) {
		// 바닥 높이보다 낮을 경우 충돌(바닥 위로 이동)
		if (position.y - (volume.y) <= height)
		{
			position = glm::vec3(position.x, (volume.y) + height, position.z);
			velocity = glm::vec3(velocity.x, 0.0f, velocity.z);
			object->SetState(STATE_GROUND);
			object->SetCollisioned(true);
		}
	}

	// 계산 값 적용
	object->SetPosition(position);
	object->SetVelocity(velocity);

	// 주인공 오브젝트일 경우 충돌 무시면 벽 충돌 처리하지 않음
	if (object->GetType() == TYPE_HERO) {
		if (object->GetIsIgnoredCollision()) {
			object->SetIsIgnoredCollision(false);
			return;
		}
	}

	// 아웃 라인
	if (CircleWallCollision(object, volume, distance, height,
		m_fFWallHeight, 260.0f, 20.0f)) return;

	for (int i = 0; i < m_iWallCount; ++i) {
		if (CircleWallCollision(object, volume, distance, height,
			m_fFWallHeight, m_fWallSizes[i], m_fWallThickness)) return;
	}
}

bool CObjectManager::CircleWallCollision(CGameObject* object, glm::vec3 realVolume, float distance,
	float wallBottom, float wallHeight, float diameter, float thickness)
{
	glm::vec3 position = object->GetPosition();
	glm::vec3 velocity = object->GetVelocity();
	
	// 벽 길이보다 높거나 낮은 곳에 존재
	if (position.y > wallHeight + wallBottom || position.y < wallBottom) {
		return false;
	}

	float halfDiameter = diameter / 2.0f;
	float halfThinkness = thickness / 2.0f;

	// 중심과의 거리가 벽의 반지름 + 두께 사이에 없을 경우 
	if (!((halfDiameter + halfThinkness) > distance && distance > (halfDiameter - halfThinkness))) {
		return false;
	}
	// 이미 속도 0일경우 계산하지 않음
	if (velocity.x == 0.0f && velocity.z == 0.0f) {
		return true;
	}

	float moveSize = (halfDiameter + halfThinkness) - distance;
	if (moveSize > halfThinkness) {
		moveSize = distance - (halfDiameter - halfThinkness);
	}

	// 이동 방향 벡터의 반대 normalize
	glm::vec3 dir = glm::normalize(glm::vec3(-velocity.x, velocity.y, -velocity.z));

	position = glm::vec3(position.x + dir.x * moveSize, position.y, position.z + dir.z * moveSize);
	velocity = glm::vec3(0.0f, velocity.y, 0.0f);

	// 계산 값 적용
	object->SetPosition(position);
	object->SetVelocity(velocity);

	return true;
}

bool CObjectManager::CheckOnlyRightAngleRotation(glm::vec3 rotation)
{
	// 각도가 0, 90, 180, 270도중 하나인지 검사
	if (!(rotation.x == 90.0f || rotation.x == 180.0f || 
		rotation.x == 270.0f || rotation.x == 0.0f || 
		rotation.x == -90.0f || rotation.x == -180.0f ||
		rotation.x == -270.0f))
		return false;

	if (!(rotation.y == 90.0f || rotation.y == 180.0f ||
		rotation.y == 270.0f || rotation.y == 0.0f ||
		rotation.y == -90.0f || rotation.y == -180.0f ||
		rotation.y == -270.0f))
		return false;

	if (!(rotation.z== 90.0f || rotation.z == 180.0f ||
		rotation.z == 270.0f || rotation.z == 0.0f ||
		rotation.z == -90.0f || rotation.z == -180.0f ||
		rotation.z == -270.0f))
		return false;

	return true;
}

void CObjectManager::ProcessCollisionWithOtherData(CGameObject* objectA, CGameObject* objectB)
{
	GameObjectType typeA = objectA->GetType();
	GameObjectType typeB = objectB->GetType();

	if ((typeA == TYPE_COLLISION_IGNORE_AREA && typeB == TYPE_HERO)
		|| (typeA == TYPE_HERO && typeB == TYPE_COLLISION_IGNORE_AREA)) {
		if (typeA == TYPE_HERO) {
			objectA->SetIsIgnoredCollision(true);
		}
		else {
			objectB->SetIsIgnoredCollision(true);
		}
	}
}

void CObjectManager::SetAtiveArea(int area)
{
	// 전과 같을 경우 리턴
	if (m_iAtiveArea == area) return;

	m_iAtiveArea = area;
	for (auto object : m_ObjectList) {
		if (object->GetType() == TYPE_ENEMY) {
			if (dynamic_cast<CFieldMonster*>(object) != nullptr) {
				if (dynamic_cast<CFieldMonster*>(object)->GetAreaNumber() == m_iAtiveArea || m_iAtiveArea == 0) {
					object->SetWhetherToRender(true);
					object->SetIsMadeShadow(true);
				}
				else {
					object->SetWhetherToRender(false);
					object->SetIsMadeShadow(false);
				}
			}
		}
	}
}

std::list<CGameObject*> CObjectManager::GetNoInteractionObjectList()
{
	return m_NoInteractionObjectList;
}

void CObjectManager::SetNoInteractionObjectList(std::list<CGameObject*> list)
{
	m_NoInteractionObjectList = list;
}


void CObjectManager::SetWallSizes(float* sizes, int count)
{
	if (m_fWallSizes != NULL) {
		delete[] m_fWallSizes;
	}

	m_fWallSizes = sizes;
	m_iWallCount = count;
}

void CObjectManager::SetIsEnabledCollionWithEnemies(bool enable)
{
	m_bIsEnabledCollionWithEnemies = enable;
}
