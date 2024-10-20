/*
* 턴 전투에서만 사용하는 상수 등을 정의
*/

#pragma once
#include "stdafx.h"

// 기본 설정 값
#define MAX_NAME_SIZE			30
#define MAX_INFORMATION_SIZE	200
#define AVERAGE_LETTER_SIZE		45
#define SKILL_SIZE				66
#define AUTOMATA_SIZE			10
#define MONSTER_SIZE			13
#define SKILL_EFFECT_FRAME		5
#define MAX_SKILL_COUNT			9
#define ATTRIBUTE_COUNT			9
#define MAX_HERO_PARTY_MEMBER	3
#define MAX_ENEMY_PARTY_MEMBER	5
#define SHOW_LOG_TIME			0.83f
#define DEFAULT_TEXT_COLOR glm::vec3(1.0f, 1.0f, 1.0f)
#define DEFAULT_DISABLE_TEXT_COLOR glm::vec3(0.5f, 0.5f, 0.5f)
#define DEFAULT_BACKGROUND_COLOR glm::vec4(0.2f, 0.3f, 0.4f, 0.5f)
#define DEFAULT_RUNAWAY_RATE	0.38f
#define DEFAULT_SHOW_DAMAGE_TEXT_TIME 0.5f
#define DEFAULT_MOVE_IN_MENU_SPEED 4.0f
#define DEFAULT_AUTOMATA_SUMMON_EFFECT_TIME 0.85f
#define DEFULAT_SKILL_EFFECT_TIME_LENGTH 0.75f

#define MAX_BUFF_SIZE 9

#define SPECIAL_SKILL_INDEX_START 2000

// 일반 전투에서 공격의 대미지
#define DEFAULT_HERO_ATTACK_DAMAGE			1
#define DEFAULT_ENEMY_ATTACK_DAMAGE			1.5
#define DEFAULT_APPLY_DEFENSE_STAT_RATE	0.65f


// 스킬 데미지 폰트 색상
#define DAMAGE_FONT_COLOR_NON		glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)
#define DAMAGE_FONT_COLOR_NORMAL	glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
#define DAMAGE_FONT_COLOR_WEAK		glm::vec4(1.0f, 1.0f, 0.5f, 1.0f)
#define DAMAGE_FONT_COLOR_AWFUL		glm::vec4(1.0f, 0.5f, 0.5f, 1.0f)
#define DAMAGE_FONT_COLOR_HALF		glm::vec4(0.5f, 0.0f, 1.0f, 1.0f)
#define DAMAGE_FONT_COLOR_RECOVERY	glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)

#define DAMAGE_FONT_COLOR_ATTACK_BUF	glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)
#define DAMAGE_FONT_COLOR_DEFENSE_BUF	glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)
#define DAMAGE_FONT_COLOR_ATTACK_DEFENSE_BUF	glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)
#define DAMAGE_FONT_COLOR_REMOVE_BUF	glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)

#define DAMAGE_FONT_COLOR_SP	glm::vec4(0.0f, 0.5f, 1.0f, 1.0f)

// 레벨 업을 위한 몬스터 수(동 레벨)
#define NEED_TO_LEVEL_UP_MONSTER_AMOUNT 3

// 턴제 전투 관련 정보
// 총 속성 수, 스탯 수
#define ATTRIBUTE_SIZE 9
#define STAT_SIZE 6

// 최대 hp, sp
#define MAX_HP_SP 999
// 레벨 업 시 증가하는 hp, sp량
#define UP_HP_SP 20

// 속성 인덱스별 정보
#define FIRE 0
#define WATER 1
#define GROUND 2
#define WIND 3

#define RUIN 4
#define GOOD 5
#define EVIL 6

#define LIGHT 7
#define DARK 8

#define UNIVERSAL -1

// 스탯 인덱스별 정보
#define MATTER_ATTACK 0
#define MATTER_DEFENSE 1
#define MIND_ATTACK 2
#define MIND_DEFENSE 3
#define ACCURACY 4
#define AVOIDABILITY 5

// 메뉴 번호 정보
#define MAIN_MENU 0
#define SKILL_MENU 1
#define AUTOMATA_MENU 2
#define CHANGE_DECK_MENU 3
#define STEAL_MENU 4
#define WAIT_MENU 5

// 행동 번호 정보
#define NO_ACT_MENU 0
#define ACT_CHANGE_DECK 1
#define ACT_STILL 2
#define ACT_WAIT 3
#define ACT_RUNAWAY 4

// 스킬 타겟 정보
#define ONE_TARGET 1
#define ALL_TARGET 2
#define ONE_AND_ALL_TARGET 3
#define OTHER_TARGET 4

// 내성 정보
#define AWFUL	2
#define WEAK	1
#define NORMAL	0
#define HALF	-1
#define NON		-2

// 각 스텟, 버프당 증가률
#define ADDITIONAL_DAMAGE 0.01f
#define ADDITIONAL_RATE 0.003f
#define ADDITIONAL_BUFF 0.1f
#define ADDITIONAL_AVOID_LEVEL 0.01f

// 스킬 최소 대미지(내성 적용 전)
#define SKILL_MIN_DAMAGE_BEFORE_APPLY_WEAKNESS 2

// 다운 수준
#define DOWN_LEVEL_NON		0
#define DOWN_LEVEL_DOWN1	1
#define DOWN_LEVEL_DOWN2	2
#define DOWN_LEVEL_FALL		4
#define DOWN_LEVEL_CRASH	5

// 최대 대미지 부여 스킬 인덱스
#define MAXIUM_GIVE_DAMAGE_SKILL_INDEX 999

// 효과 정보
#define ATTACK_BUFF				1
#define DEFENSE_BUFF			2
#define ATTACK_DEFENSE_BUFF	3
#define ATTACK_DEBUFF		   -1
#define DEFENSE_DEBUFF		   -2
#define ATTACK_DEFENSE_DEBUFF -3
#define REMOVE_BUFF_DEBUFF		0
#define RECOVER_HP				5

// 상태 이상
#define NORMAL_CONDITION	0
#define BURN_CONDITION		1
#define ANXIETY_CONDITION	2

#define RECOVER_CONDITION_TIME 3.f
#define RECOVER_CONDITION_AMOUNT 1.5f

// 스킬 효과
#define NO_SKILL_EFFECT			0
#define BURN_SKILL_EFFECT		1
#define ANXIETY_SKILL_EFFECT	2
#define DROWNING_SKILL_EFFECT	3
