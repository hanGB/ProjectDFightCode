#pragma once

#include "Globals.h"
#include "TurnBattleGobals.h"

class CButton {
public:
	CButton(const wchar_t* text);
	~CButton();

private:
	// ��ư ����
	wchar_t m_Text[MAX_INFORMATION_SIZE / 2];

	// ����, ��� ����
	bool m_bIsSelected = false;
	bool m_bIsUsed = false;

	// ���콺 ���� ����
	bool m_bIsSelectedWithMouse = false;

	// ���� ���콺 ��ġ
	int m_iPrevMouseX = WINDOW_SIZE_X / 2;
	int m_iPrevMouseY = WINDOW_SIZE_Y / 2;

	bool m_bIsMousePressDown = false;

public:
	void Render(int textureId, glm::vec2 position, glm::vec2 size, float textSize, float margin);

	bool SelectedWithKeyBoard();
	bool SelectedWithMouse(int mouseX, int mouseY, float left, float top, float right, float buttom);

	bool SetIsUsed(GameInputs* inputs);

	bool GetIsUsed() const;

	void SetIsSelected(bool select);

	void SetIsMousePressDown(bool down);
};