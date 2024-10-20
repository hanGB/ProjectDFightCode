/*
* ���� �� ���, Ű �Է� ����ü ���� ����
*/

#pragma once
#include "stdafx.h"

// ���� ȭ�� ����
// �ػ�
#define WINDOW_SIZE_X 1600
#define WINDOW_SIZE_Y 900
// �ִ� ������ ����
#define FRAME_LIMIT 60

// �ִ� ��� �ð�
#define MAX_ELAPSED_TIME 0.1f

// �⺻ ���� ��
#define OBJ_LINE_HEADER_SIZE		128
#define DEFAULT_SHININESS			32.f
#define DEFAULT_FONT_SIZE			48
#define SHADOW_RESOLUTION			2048
#define MULTI_SAMPLING_COUNT		4
#define GRASS_AMOUNT_TITLE			394141
#define GRASS_AMOUNT_FIELD			200010
#define GRASS_AMOUNT_BATTLE			90126
#define MOUSE_WHEEL_SPEED			50.f
#define STAIR_STEP_HEIGHT			0.2f
#define DEFAULT_AREA_WIDTH			24
#define DEFAULT_WIND_SPEED			0.5f
#define DEFUALT_STATE_CHANGE_WAIT_TIME 0.85f
#define DEFAULT_STAMINA_RECOVER_SPEED 30.0f
#define DEFAULT_STEMINA_AMOUNT		100.0f
#define DEFAULT_DAMAGE_TIME			0.3f
#define DEFAULT_TURN_GETTING_GAUGE_INCREASE_RATE 0.2f
#define DEFAULT_TURN_GETTING_GAUGE_INCREASE_RATE_FOR_MONSTER 0.25f
#define DEFAULT_RESPAWN_TIME			30.0f
#define MAX_POINT_LIGHTS_NUM			100
#define DEFAULT_DOWN_RECOVER_TIME		1.5f
#define DEFAULT_CONDITION_RECOVER_TIME	1.75f
#define MAX_BONE_INFLUENCE_SIZE	8
#define MAX_BONE_SIZE			128
#define DEFAULT_IDLE_TIME		5.0f
#define DEFAULT_SPECTRUM_COOL_TIME	0.1f
#define DEFAULT_SPECTRUM_MAINTAIN_TIME	0.3f
#define DEFAULT_RECOVER_SPOT_USE_TIME	1.75f
#define DEFAULT_RECOVER_SPOT_COOL_TIME	1.75f
#define MAX_MONSTER_ATTACK_PATTERN 9
#define ATTACK_PATTERN_SHORT_START 0
#define ATTACK_PATTERN_MIDDLE_START 3
#define ATTACK_PATTERN_LONG_START 6
#define DEFAULT_SUPER_ARMOR_RECOVER_TIME 2.5f
#define DEFAULT_SUPER_ARMOR_SIZE 3
#define DEFAULT_DAMAGE_COOL_TIME 0.45f
#define DEFAULT_MIN_SOUND_DISTANCE 5.0f
#define DEFAULT_MAX_SOUND_DISTANCE 50.0f
#define DEFAULT_SOUND_ATTENSION 10.0f
#define DEFAULT_SOUND_PLAY_INTERVAL 2.5f

// ����Ʈ ���� �ʿ��� �ǰ� ���� ���� ����
#define HIT_SOUND_START_INDEX_IN_DEFAULT_SOUND_MAP 100

// ���� ���迡�� �� �ð��� 5��
#define HOUR_TO_SEC_IN_GAME_WORLD	5.0f

// ��Ʋ ���
#define BATTLE_MODE_TURN 0
#define BATTLE_MODE_HYBRID 1


// �׸��� �ؽ��� ��ġ ���� ��
#define SHADOW_TEXTURE_NUMBER_START 100

// ��ī�̹ڽ� �ؽ��� ��ġ ��
#define SKYBOX_TEXTURE_NUMBER 50

// �¾��(�𷺼ų� ����Ʈ) ��ī�̶���Ʈ ����
#define SUNRISE_SKY_LIGHT_COLOR glm::vec4(0.1f, 0.0f, 0.0f, 1.0f)
#define DAYTIME_SKY_LIGHT_COLOR glm::vec4(0.78f, 0.84f, 1.0f, 1.0f)
#define SUNSET_SKY_LIGHT_COLOR glm::vec4(0.3f, 0.0f, 0.05f, 1.0f)
#define NIGHT_SKY_LIGHT_COLOR glm::vec4(0.0f, 0.08f, 0.16f, 1.0f)
#define RAINY_SKY_LIGHT_COLOR glm::vec4(0.2f, 0.32f, 0.49f, 1.0f)

#define NIGHT_LIGHT_COLOR glm::vec4(0.25f, 0.25f, 0.25f, 1.0f)
#define DAYTIME_LIGHT_COLOR glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
#define RAINY_LIGHT_COLOR glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)
#define SUNRISE_SUNSET_LIGHT_COLOR glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)


// ����ִ� ��
#define EMPTY_VALUE -1

// ���̴� �ڵ� ����
#define DEFAULT_SHADER				0
#define TEXT_SHADER					1
#define UI_SHADER					2
#define BILLBOARD_SHADER			3
#define BILLBOARD_SPRITE_SHADER		4
#define SKYBOX_SHADER				5
#define DEPTH_SHADER				6
#define BOUNDING_BOX_SHADER			7
#define BORDER_SHADER				8
#define TOTAL_SCENE_SHADER			9
#define INSTANCE_SHADER				10
#define BILLBOARD_TEXT_SHADER		11
#define DAMAGE_ZONE_SHADER			12
#define WATER_SHADER				13
#define GRAVITY_PARTICLE_SHADER		14
#define DISPLAY_FXAA_SHADER			15
#define Blur_SHADER					16

// Ű �ƽ�Ű�ڵ� ��
#define KEY_ESC 27
#define KEY_TAB 9

// �Ϲ� ��
#define PI_VALUE 3.1415926535f
#define GRAVITY_VALUE 9.80665f

// �ٿ�� �ڽ� ����
#define BOUNDING_BOX true

// ó���� Ű�Է� Ȯ�ο�(�� �� ������ ���� �Է�)
typedef struct GameInputsProcessingEnd {
	// ����� Ű���� Ű �Է� �߰�
	bool KeyW;
	bool KeyA;
	bool KeyS;
	bool KeyD;

	// ���콺 �Է�
	bool MouseLeftButton;
	bool MouseRightButton;

	bool KeySpace;
	bool KeyLeftShift;

	bool KeyC;

	bool KeyE;
	bool KeyF;

	bool KeyQ;
	bool KeyR;
	bool KeyT;

	bool KeyPlus;
	bool KeyMinus;

	bool KeyESC;
	bool KeyTab;
	bool KeyLeftCtrl;
	bool KeyB;

	bool KeyArrowUp;
	bool KeyArrowDown;
	bool KeyArrowLeft;
	bool KeyArrowRight;

	bool KeyF2;

	bool KeyZ;
	bool KeyX;
}GameInputsProcessingEnd;

// Ű �Է�
typedef struct GameInputs {
	// ���콺 ��ġ
	int MouseXPos;
	int MouseYPos;

	// ���콺 �Է�
	bool MouseLeftButton;
	bool MouseRightButton;

	// ����� Ű���� Ű �Է� �߰�
	bool KeyW;
	bool KeyA;
	bool KeyS;
	bool KeyD;

	bool KeySpace;
	bool KeyLeftShift;

	// ī�޶� ����
	bool KeyC;

	bool KeyE;
	bool KeyF;

	bool KeyQ;
	bool KeyR;
	bool KeyT;

	bool KeyPlus;
	bool KeyMinus;

	bool KeyESC;
	bool KeyTab;
	bool KeyLeftCtrl;
	bool KeyB;

	bool KeyArrowUp;
	bool KeyArrowDown;
	bool KeyArrowLeft;
	bool KeyArrowRight;

	bool KeyF2;

	bool KeyZ;
	bool KeyX;

	// ��ũ�� �� ���(���� ó�� �ʿ䰡 true, ó�� �� false)
	bool MouseWheelUp;
	bool MouseWheelDown;

	GameInputsProcessingEnd processingEnd;

	void processKey(bool *key, bool* processingEndKey, bool state) {
		if (*key != state)
			*processingEndKey = true;
		*key = state;
	}

	bool IsPressedDownKeyOnce(bool* key, bool* processingEndKey) {
		if (*processingEndKey == false) {
			return false;
		}
		if (*key == false) {
			return false;
		}

		*processingEndKey = false;
		return true;
	}

}GameInputs;

// ������Ʈ ������Ʈ �Ű�����
typedef struct GameUpdateParameters {
	glm::vec3 force;
}GameUpdateParameters;
