/*
* Object Manager
*	제 2번으로 호출되는 클래스
*
*	게임내 오브젝트 관리
*/

#pragma once
#include <list>
#include "GameObject.h"
#include "GravityParticle.h"
#include "Renderer.h"
#include "TestObject.h"

class CObjectManager
{
public:
	CObjectManager();
	~CObjectManager();

private:
	std::list<CGameObject*> m_ObjectList;
	std::list<CGameObject*> m_NoInteractionObjectList;
	std::list<CGravityParticle*> m_GravityParticleList;
	CRenderer* m_pRenderer = NULL;

	// 업데이트 매개변수
	GameUpdateParameters m_HeroUpdateParameters;
	GameUpdateParameters m_OthersUpdateParameters;

	// 원형 발판 
	bool m_bFloor = false;
	float m_fFloorHeight = -4.0f;
	float m_fDiameter = 250.0f;
	float m_fFWallHeight = 5.0f;
	float m_fWallThickness = 3.0f;

	float* m_fWallSizes = NULL;
	int m_iWallCount = 0;

	bool m_bIsEnabledCollionWithEnemies = false;

	int m_iAtiveArea = 0;

public:
	// 오브젝트 추가
	template<class T>
	int AddObject(glm::vec3 position, glm::vec3 rotation, glm::vec3 size, float mass)
	{
		T* object = new T();
		object->SetPosition(position);
		object->SetRotation(rotation);
		object->SetSize(size);
		object->SetMass(mass);
		object->SetIndex((int)m_ObjectList.size());

		m_ObjectList.push_back(object);

		return object->GetIndex();
	}

	// 오브젝트 추가
	template<class T>
	int AddObject(T* object)
	{
		m_ObjectList.push_back(object);

		return object->GetIndex();
	}

	// 오브젝트 얻기
	template<class T>
	T* GetGameObject(int index)
	{
		auto it = m_ObjectList.cbegin();

		for (int i = 0; i < index; ++i)
		{
			++it;
		}

		return dynamic_cast<T*>(*it);
	}

	// 상호작용 하지 않는 오브젝트 추가
	template<class T>
	int AddNoInteractionObject(glm::vec3 position, glm::vec3 rotation, glm::vec3 size, float mass)
	{
		T* object = new T();
		object->SetPosition(position);
		object->SetRotation(rotation);
		object->SetSize(size);
		object->SetMass(mass);
		object->SetIndex((int)m_NoInteractionObjectList.size());

		m_NoInteractionObjectList.push_back(object);

		return object->GetIndex();
	}

	// 상호작용 하지 않는 오브젝트 얻기
	template<class T>
	T* GetNoInteractionGameObject(int index)
	{
		auto it = m_NoInteractionObjectList.cbegin();

		for (int i = 0; i < index; ++i)
		{
			++it;
		}

		return dynamic_cast<T*>(*it);
	}

	// 파티클 추가
	template<class T>
	int AddParticle(glm::vec3 position, glm::vec3 size)
	{
		T* particle = new T();
		particle->SetPosition(position);
		particle->SetSize(size);
		particle->SetIndex((int)m_GravityParticleList.size());

		m_GravityParticleList.push_back(particle);

		return particle->GetIndex();
	}

	// 오브젝트 추가
	template<class T>
	int AddParticle(T* particle)
	{
		m_GravityParticleList.push_back(particle);

		return particle->GetIndex();
	}


	// 파티클 얻기
	template<class T>
	T* GetParticle(int index)
	{
		auto it = m_GravityParticleList.cbegin();

		for (int i = 0; i < index; ++i)
		{
			++it;
		}

		return dynamic_cast<T*>(*it);
	}

	// 특정 타입 오브젝트 삭제
	void DeleteAllObjectWithType(GameObjectType type);
	void DeleteAllNoInteractionObjectWithType(GameObjectType type);
	void DeleteAllParticle();

	void Update(float elapsedTimeInSec);
	virtual void Render();
	virtual void DepthRender();
	void BoundingBoxRender();
	void RenderParticle();

	// 오브젝트 업데이트 매개변수 받기
	void SetUpdateParameters(GameUpdateParameters heroUpdateParameters, GameUpdateParameters othersUpdateParameters);

	// 충돌 체크
	void CheckCollision();

	// 쓰레기 처리(필요 없는 오브젝트 제거)
	void DoGarbegeCollection(float elapsedTimeInSec);

	// 오브젝트의 자식 제거
	void DeleteChildOfObject(CGameObject* object);

	std::list<CGameObject*> GetNoInteractionObjectList();
	void SetNoInteractionObjectList(std::list<CGameObject*> list);

	void SetWallSizes(float* sizes, int count);

	void SetIsEnabledCollionWithEnemies(bool enable);
	
	// 볼륨, 위치 등의 데이터가 필요없는 충돌 처리
	virtual void ProcessCollisionWithOtherData(CGameObject* objectA, CGameObject* objectB);

	void SetAtiveArea(int area);

private:
	// 회전되지 않은 물체끼리의 충돌
	bool AABBCollision(CGameObject* objectA, CGameObject* objectB);
	// 직각 회전된 물체끼리의 충돌
	bool AABBCollisionWithOnlyRightRotattion(CGameObject* objectA, CGameObject* objectB);
	// 회전된 물체와의 충돌
	bool OBBCollision(CGameObject* objectA, CGameObject* objectB);

	void ProcessCollisionWithVolume(CGameObject* objectA, CGameObject* objectB, glm::vec3 volumeA, glm::vec3 volumeB);

	// 고정된 B와 위치를 맞춤
	void AdjustPositionFixedA(CGameObject* objectA, CGameObject* objectB, glm::vec3 volumeA, glm::vec3 volumeB);

	// 캡슐형 충돌체 물체간의 충돌
	bool CapsuleCollision(CGameObject* objectA, CGameObject* objectB);

	// 박스와 캡슐형 충돌체 물체간의 충돌
	bool BoxAndCapsuleCollision(CGameObject* boxObject, CGameObject* capsuleObject);

	// 원형 바닥과의 충돌 처리
	void CircleFloorCollision(CGameObject* object, float height, float diameter);

	// 원형 벽과의 충돌 처리
	bool CircleWallCollision(CGameObject* object, glm::vec3 realVolume, float distance,
		float wallBottom, float wallHeight, float diameter, float thickness);

	// 오직 직각 회전으로 이루어진 회전인지 검사
	bool CheckOnlyRightAngleRotation(glm::vec3 rotation);
};
