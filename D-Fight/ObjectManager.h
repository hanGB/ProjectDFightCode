/*
* Object Manager
*	�� 2������ ȣ��Ǵ� Ŭ����
*
*	���ӳ� ������Ʈ ����
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

	// ������Ʈ �Ű�����
	GameUpdateParameters m_HeroUpdateParameters;
	GameUpdateParameters m_OthersUpdateParameters;

	// ���� ���� 
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
	// ������Ʈ �߰�
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

	// ������Ʈ �߰�
	template<class T>
	int AddObject(T* object)
	{
		m_ObjectList.push_back(object);

		return object->GetIndex();
	}

	// ������Ʈ ���
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

	// ��ȣ�ۿ� ���� �ʴ� ������Ʈ �߰�
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

	// ��ȣ�ۿ� ���� �ʴ� ������Ʈ ���
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

	// ��ƼŬ �߰�
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

	// ������Ʈ �߰�
	template<class T>
	int AddParticle(T* particle)
	{
		m_GravityParticleList.push_back(particle);

		return particle->GetIndex();
	}


	// ��ƼŬ ���
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

	// Ư�� Ÿ�� ������Ʈ ����
	void DeleteAllObjectWithType(GameObjectType type);
	void DeleteAllNoInteractionObjectWithType(GameObjectType type);
	void DeleteAllParticle();

	void Update(float elapsedTimeInSec);
	virtual void Render();
	virtual void DepthRender();
	void BoundingBoxRender();
	void RenderParticle();

	// ������Ʈ ������Ʈ �Ű����� �ޱ�
	void SetUpdateParameters(GameUpdateParameters heroUpdateParameters, GameUpdateParameters othersUpdateParameters);

	// �浹 üũ
	void CheckCollision();

	// ������ ó��(�ʿ� ���� ������Ʈ ����)
	void DoGarbegeCollection(float elapsedTimeInSec);

	// ������Ʈ�� �ڽ� ����
	void DeleteChildOfObject(CGameObject* object);

	std::list<CGameObject*> GetNoInteractionObjectList();
	void SetNoInteractionObjectList(std::list<CGameObject*> list);

	void SetWallSizes(float* sizes, int count);

	void SetIsEnabledCollionWithEnemies(bool enable);
	
	// ����, ��ġ ���� �����Ͱ� �ʿ���� �浹 ó��
	virtual void ProcessCollisionWithOtherData(CGameObject* objectA, CGameObject* objectB);

	void SetAtiveArea(int area);

private:
	// ȸ������ ���� ��ü������ �浹
	bool AABBCollision(CGameObject* objectA, CGameObject* objectB);
	// ���� ȸ���� ��ü������ �浹
	bool AABBCollisionWithOnlyRightRotattion(CGameObject* objectA, CGameObject* objectB);
	// ȸ���� ��ü���� �浹
	bool OBBCollision(CGameObject* objectA, CGameObject* objectB);

	void ProcessCollisionWithVolume(CGameObject* objectA, CGameObject* objectB, glm::vec3 volumeA, glm::vec3 volumeB);

	// ������ B�� ��ġ�� ����
	void AdjustPositionFixedA(CGameObject* objectA, CGameObject* objectB, glm::vec3 volumeA, glm::vec3 volumeB);

	// ĸ���� �浹ü ��ü���� �浹
	bool CapsuleCollision(CGameObject* objectA, CGameObject* objectB);

	// �ڽ��� ĸ���� �浹ü ��ü���� �浹
	bool BoxAndCapsuleCollision(CGameObject* boxObject, CGameObject* capsuleObject);

	// ���� �ٴڰ��� �浹 ó��
	void CircleFloorCollision(CGameObject* object, float height, float diameter);

	// ���� ������ �浹 ó��
	bool CircleWallCollision(CGameObject* object, glm::vec3 realVolume, float distance,
		float wallBottom, float wallHeight, float diameter, float thickness);

	// ���� ���� ȸ������ �̷���� ȸ������ �˻�
	bool CheckOnlyRightAngleRotation(glm::vec3 rotation);
};
