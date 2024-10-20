#include "stdafx.h"
#include "RecoverSpot.h"
#include "BehaviorTree.h"
#include "TurnBattleGobals.h"

CRecoverSpot::CRecoverSpot()
{
}

CRecoverSpot::~CRecoverSpot()
{
}

void CRecoverSpot::Update(float elapsedTimeInSec, GameUpdateParameters parameters)
{

}

void CRecoverSpot::Render(CRenderer* renderer)
{

}

bool CRecoverSpot::ApplyPlayerControl(float elapsedTimeInSec, GameInputs* inputs)
{
	if (m_fCoolTime > 0.0f) {
		m_bIsOverlapedHero = false;
		m_fCoolTime -= elapsedTimeInSec;
		return false;
	}
	if (m_bIsOverlapedHero) {
		if (inputs->KeyE) {
			m_fPressOnTime += elapsedTimeInSec;
			if (m_fPressOnTime >= DEFAULT_RECOVER_SPOT_USE_TIME) {
				m_fPressOnTime = 0.0f;
				m_bIsOverlapedHero = false;
				m_fCoolTime = DEFAULT_RECOVER_SPOT_COOL_TIME;
				return true;
			}
			else {
				m_bIsOverlapedHero = false;
				return false;
			}
		}
	}
	m_bIsOverlapedHero = false;
	m_fPressOnTime = 0.0f;
	return false;
}

void CRecoverSpot::SetIsOverlapedHero(bool overlap)
{
	m_bIsOverlapedHero = overlap;
}

void CRecoverSpot::RenderKeyUI(int backgroundTextureID, int x, int y)
{
	if (m_bIsOverlapedHero) {
		float textLocationX, textLocationY;

		CRenderer* renderer = CRenderer::GetInstance();

		float barLocationX, barLocationY;

		// 버튼 정보 표시 바
		renderer->UseShader(UI_SHADER);

		barLocationX = 90.f / (float)x;
		barLocationY = 42.5f / (float)y;

		renderer->RenderTexture2D(backgroundTextureID,
			glm::vec2(0.5f + barLocationX, 0.5f + barLocationY), glm::vec2(0.0f, 0.0f),
			glm::vec2(320.0f, 42.5f), DEFAULT_BACKGROUND_COLOR);

		renderer->UseShader(TEXT_SHADER);

		float addColor = m_fPressOnTime / DEFAULT_CONDITION_RECOVER_TIME / 2;

		textLocationX = 100.f / (float)x;
		textLocationY = 50.f / (float)y;
		if (m_fCoolTime <= 0.0f) {
			renderer->RenderText(L"E를 길게 눌러 회복", 0.5f + textLocationX, 0.5f + textLocationY,
				0.75f, glm::vec4(0.5f + addColor, 0.5f + addColor, 0.5f + addColor, 1.0));
		}
		else {
			wchar_t text[50];
			wsprintf(text, L"%d.%d초후에 회복 가능", (int)m_fCoolTime, (int)(m_fCoolTime * 10));
			renderer->RenderText(text, 0.5f + textLocationX, 0.5f + textLocationY,
				0.75f, glm::vec4(0.5f, 0.5f, 0.5f, 1.0));
		}
	}
}

float CRecoverSpot::GetPressOnTime() const
{
	return m_fPressOnTime;
}
