#include "stdafx.h"
#include "Button.h"
#include "Renderer.h"

CButton::CButton(const wchar_t* text)
{
	wcscpy_s(m_Text, text);
}

CButton::~CButton()
{
}

void CButton::Render(int textureId, glm::vec2 position, glm::vec2 size, float textSize, float margin)
{
	CRenderer *renderer = CRenderer::GetInstance();

	if (m_bIsSelected) {
		renderer->UseShader(UI_SHADER);
		renderer->RenderTexture2D(textureId,
			position, glm::vec2(0.0f, 0.0f),
			size, DEFAULT_BACKGROUND_COLOR);
	}

	renderer->UseShader(TEXT_SHADER);
	renderer->RenderText(m_Text, position.x + margin, position.y + margin, textSize, DEFAULT_TEXT_COLOR);
}

bool CButton::SelectedWithKeyBoard()
{
	m_bIsSelected = true;
	m_bIsSelectedWithMouse = false;

	return true;
}

bool CButton::SelectedWithMouse(int mouseX, int mouseY, float left, float top, float right, float buttom)
{
	if (mouseX == m_iPrevMouseX && mouseY == m_iPrevMouseY) return false;

	m_iPrevMouseX = mouseX;
	m_iPrevMouseY = mouseY;

	if (left < mouseX && mouseX < right) {
		if (buttom < mouseY && mouseY < top) {
			m_bIsSelected = true;
			m_bIsSelectedWithMouse = true;
			return true;
		}
	}

	if (m_bIsSelectedWithMouse) {
		m_bIsSelected = false;
		m_bIsSelectedWithMouse = false;
	}
	return false;
}

bool CButton::SetIsUsed(GameInputs* inputs)
{
	if (!m_bIsSelected) {
		if (!inputs->MouseLeftButton) m_bIsMousePressDown = false;
		return false;
	}

	if (inputs->IsPressedDownKeyOnce(&inputs->KeySpace, &inputs->processingEnd.KeySpace)) {
		m_bIsUsed = true;
		return true;
	}

	if (!m_bIsSelectedWithMouse) return false;

	if (inputs->MouseLeftButton) {
		m_bIsMousePressDown = true;
		return false;
	}
	else {
		if (m_bIsMousePressDown) {
			m_bIsMousePressDown = false;
			m_bIsUsed = true;
			return true;
		}
		return false;
	}
}

bool CButton::GetIsUsed() const
{
	return m_bIsUsed;
}

void CButton::SetIsSelected(bool select)
{
	m_bIsSelected = false;
	m_bIsSelectedWithMouse = false;
}

void CButton::SetIsMousePressDown(bool down)
{
	m_bIsMousePressDown = down;
}
