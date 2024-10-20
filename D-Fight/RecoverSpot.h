#pragma once
#include "GameObject.h"

class CRecoverSpot : public CGameObject {
public:
	CRecoverSpot();
	~CRecoverSpot();

private:
	bool m_bIsOverlapedHero = false;
	float m_fPressOnTime = 0.0f;
	float m_fCoolTime = 0.0f;

public:
	virtual void Update(float elapsedTimeInSec, GameUpdateParameters parameters);
	virtual void Render(CRenderer* renderer);

	bool ApplyPlayerControl(float elapsedTimeInSec, GameInputs* inputs);

	void SetIsOverlapedHero(bool overlap);

	void RenderKeyUI(int backgroundTextureID, int x, int y);

	float GetPressOnTime() const;

private:
	
};