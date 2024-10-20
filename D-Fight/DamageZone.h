#pragma once
#include "GameObject.h"

class CDamageZone : public CGameObject {
public:
	CDamageZone();
	~CDamageZone();

private:
	float m_fTime = 0.0f;
	float m_fEndTime = 1.0f;
	bool m_bIsCircle;

	bool m_bIsDamageOn = false;
	bool m_bAlwaysOn = false;

	bool m_bIsGivenDamage = false;

public:
	virtual void Update(float elapsedTimeInSec, GameUpdateParameters parameters);
	virtual void Render(CRenderer* renderer);

	// 충돌 검사
	bool CheckCollision(CGameObject* object);

	void SetIsCircle(bool circle);

	void SetShowTime(float endtime);

	void SetIsDamageOn(bool on);
	bool GetIsDamageOn() const;

	bool GetIsNotVolumeCollision();
	void SetAlwaysOn(bool on);

	void SetIsGivenDamage(bool give);
	bool GetIsGivenDamage() const;
private:

};