/*
* 게임 내 상수, 키 입력 구조체 등을 정의
*/

#pragma once
#include "stdafx.h"

// 게임 화면 설정
// 해상도
#define WINDOW_SIZE_X 1600
#define WINDOW_SIZE_Y 900
// 최대 프레임 제한
#define FRAME_LIMIT 60

// 최대 대기 시간
#define MAX_ELAPSED_TIME 0.1f

// 기본 설정 값
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

// 디폴트 사운드 맵에서 피격 사운드 시작 번지
#define HIT_SOUND_START_INDEX_IN_DEFAULT_SOUND_MAP 100

// 게임 세계에서 한 시간은 5초
#define HOUR_TO_SEC_IN_GAME_WORLD	5.0f

// 배틀 모드
#define BATTLE_MODE_TURN 0
#define BATTLE_MODE_HYBRID 1


// 그림자 텍스쳐 위치 시작 값
#define SHADOW_TEXTURE_NUMBER_START 100

// 스카이박스 텍스쳐 위치 값
#define SKYBOX_TEXTURE_NUMBER 50

// 태양빛(디렉셔널 라이트) 스카이라이트 색상
#define SUNRISE_SKY_LIGHT_COLOR glm::vec4(0.1f, 0.0f, 0.0f, 1.0f)
#define DAYTIME_SKY_LIGHT_COLOR glm::vec4(0.78f, 0.84f, 1.0f, 1.0f)
#define SUNSET_SKY_LIGHT_COLOR glm::vec4(0.3f, 0.0f, 0.05f, 1.0f)
#define NIGHT_SKY_LIGHT_COLOR glm::vec4(0.0f, 0.08f, 0.16f, 1.0f)
#define RAINY_SKY_LIGHT_COLOR glm::vec4(0.2f, 0.32f, 0.49f, 1.0f)

#define NIGHT_LIGHT_COLOR glm::vec4(0.25f, 0.25f, 0.25f, 1.0f)
#define DAYTIME_LIGHT_COLOR glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
#define RAINY_LIGHT_COLOR glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)
#define SUNRISE_SUNSET_LIGHT_COLOR glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)


// 비어있는 값
#define EMPTY_VALUE -1

// 쉐이더 코드 종류
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

// 키 아스키코드 값
#define KEY_ESC 27
#define KEY_TAB 9

// 일반 값
#define PI_VALUE 3.1415926535f
#define GRAVITY_VALUE 9.80665f

// 바운딩 박스 설정
#define BOUNDING_BOX true

// 처리된 키입력 확인용(한 번 누르면 끝인 입력)
typedef struct GameInputsProcessingEnd {
	// 사용할 키보드 키 입력 추가
	bool KeyW;
	bool KeyA;
	bool KeyS;
	bool KeyD;

	// 마우스 입력
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

// 키 입력
typedef struct GameInputs {
	// 마우스 위치
	int MouseXPos;
	int MouseYPos;

	// 마우스 입력
	bool MouseLeftButton;
	bool MouseRightButton;

	// 사용할 키보드 키 입력 추가
	bool KeyW;
	bool KeyA;
	bool KeyS;
	bool KeyD;

	bool KeySpace;
	bool KeyLeftShift;

	// 카메라 변경
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

	// 스크롤 휠 사용(휠은 처리 필요가 true, 처리 후 false)
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

// 오브젝트 업데이트 매개변수
typedef struct GameUpdateParameters {
	glm::vec3 force;
}GameUpdateParameters;
