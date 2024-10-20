#pragma once
#include "GameObject.h"
#include "Camera.h"
#include "BehaviorTree.h"
#include "TurnBattleGobals.h"

/*
*	턴 기반 전투를 위한 기본 플레이어, 적 오브젝트
*	
*		각종 스테이터스 등을 갖고 전투 행동을 갖음
*/

namespace TurnBattle {
	// 계산용 정보(전투 중에 거의 변하지 않음)
	struct SStatus {
		// 각 수치의 최대치
		short maxHp;
		short maxSp;
		short maxTurn;

		// 속성 정보(내성)
		short attribute[9];

		// 레벨
		short level;

		// 스탯(물질 공격력, 물질 방어력, 정신 공격력, 정신 방어력, 명중률, 회피율 순)
		short stat[6];
	};

	// 전투 변수(전투 중에 변하여 업데이트 되는 정보)
	struct SBattleVariable {
		short hp;
		short sp;
		short turn;

		// 버프
		short attackBuff;
		short defenseBuff;

		// 다운 수치
		short down;

		// 검험치
		short exp;

		// 대기 회피 레벨
		short avoidLevel;

		// 상태
		short condition;
		// 상태 지속 시간
		float conditionDuration;
	};

	// 스킬 정보
	struct SSkill {
		// 속성
		short attribute;

		// 이름
		wchar_t name[MAX_NAME_SIZE];

		// 데미지
		short minDamage; 
		short maxDamage;

		// 확률
		float rate;

		// 소모 량
		short turn;
		short sp;

		// 효과
		short effect;
		float effectRate;

		// 설명
		wchar_t informationLine1[MAX_INFORMATION_SIZE];
		wchar_t informationLine2[MAX_INFORMATION_SIZE];

		// 스킬 이펙트 정보
		bool billboardEffect;
		// 스킬 이펙트 지속 시간
		float effectTime;
		// 스킬 이펙트 데이터
		int effectData;

		// 타겟
		short target;
		bool selfParty;
	};

	struct SAutomata {
		// 이름
		wchar_t name[MAX_NAME_SIZE];
		wchar_t englishName[MAX_NAME_SIZE];

		// 스탯
		SStatus status;

		// 스킬리스트
		short skillList[MAX_SKILL_COUNT];
		short skillSize;

		short attackAttribute;
	};

	struct SDeck {
		// 이름
		wchar_t name[MAX_NAME_SIZE];

		wchar_t strengh[MAX_INFORMATION_SIZE];
		wchar_t weak[MAX_INFORMATION_SIZE];
		wchar_t information[MAX_INFORMATION_SIZE];

		// 속성
		short attribute[ATTRIBUTE_COUNT];

		// 스킬리스트
		short skillList[MAX_SKILL_COUNT];
		short skillSize;

		short attackAttribute;
	};

	struct SMonster {
		// 이름
		wchar_t name[MAX_NAME_SIZE];

		// 스탯
		SStatus status;

		// 스킬리스트
		short skillList[MAX_SKILL_COUNT];
		short skillSize;

		short automata;
		short deck;

		short modelId;
		short textureId;

		short attackAttribute;
	};
}

class CTurnBattleObject : public CGameObject {
public:
	CTurnBattleObject();
	~CTurnBattleObject();

private:
	TurnBattle::SStatus m_Status;
	TurnBattle::SBattleVariable m_BattleVariable;

	// 이름
	wchar_t m_wName[MAX_NAME_SIZE];

	// 가지고 있는 스킬
	std::vector<short> m_SkillVector;
	int m_selectedSkill = 0; 

	// 가지고 있는 오토마타
	std::vector<short> m_AutomataVector;
	// 가지고 있는 스킬 덱
	std::vector<short> m_DeckVector;
	
	// 훔쳐지는 대상
	short m_sAutomataBeingStolen;
	short m_sSkillDeckBeingStolen;

	// 카메라 높이
	float m_fCameraHight = 2.0f;
	// 카메라 거리
	float m_fDistance = 10.f;

	// 대미지 표시 관련 변수
	float m_bIsShowedDamageText = false;
	float m_fShowingDamageTextTime = 0.0f;
	wchar_t m_wDamageText[MAX_NAME_SIZE];
	glm::vec4 m_vDamageTextColor = DAMAGE_FONT_COLOR_NORMAL;

	// 보스 몬스터에 대한 설정을 위한 변수
	bool m_bIsBoss = false;

	// 턴을 얻기 위한 게이지
	float m_fShowingTurnGauge = 0.0f;
	float m_fGettingTurnGauge = 0.0f;
	bool m_bIsUpdatedTurnGaugeWhenEndTurn = false;

	short m_sAttackAttribute = UNIVERSAL;

	float m_fDownRecoverTime = 2.0f;

	short m_sMonsterId;

	bool m_bFixed = false;
	bool m_bIsSubObject = false;

	GLuint m_UIBackgroundTextureID;

public:
	virtual void Update(float elapsedTimeInSec, GameUpdateParameters parameters);

	// 하이브리드 배틀용 다운 수치 업데이트
	bool UpdateDownLevelForHybridBattle(float elapsedTimeInSec);

	// 정보 Getter, Setter
	TurnBattle::SStatus GetStatus() const;
	void SetStatus(TurnBattle::SStatus status);

	const wchar_t* GetName() const;
	void SetName(const wchar_t* name);

	TurnBattle::SBattleVariable GetBattleVariable() const;
	void SetBattleVariable(TurnBattle::SBattleVariable battleVariable);

	void SetIndexBingStolen(short automata, short skillDeck);
	void GetIndexBeingStolen(short* automata, short* skillDeck);

	// 위치(0, 1, 2, 3, 4)와 방향(앞, 뒤)을 통해 카메라 설정 함수 추가
	void SetCameraPosAndDirectionWithNumber(CCamera* camera, int order, int partySize, bool self, bool enemy);

	// 벡터에 값 추가 함수
	std::vector<short> GetSkillVector();
	void GiveSkill(short skillID);
	std::vector<short> GetAutomataVector();
	void GiveAutomata(short automataID);
	std::vector<short> GetDeckVector();
	void GiveDeck(short deckID);

	// 벡터의 특정 위치 값 삭제 함수 
	void RemoveDeck(int location);

	void SetAttributeAndSkillWithDeck(TurnBattle::SDeck* deck);

	// 스킬 강화, 초기화용 인덱스를 받아 그 위치 값 변경 함수
	void ChangeSkillAt(int index, short skillID);

	// 스킬 사용 가능 여부 확인
	bool CheckEnableSkill(TurnBattle::SSkill *skill);

	// 위에 hp 바 렌더링
	void RenderHPBarOnTop(CRenderer* renderer, bool isHeroParty);

	// 위에 hp 바 렌더링
	void RenderHPBarOnTopForHybridBattle(CRenderer* renderer);

	// 데미지 표시 텍스트 렌더링
	void RenderDamageText(CRenderer* renderer, bool isHeroParty);

	// 다운 레벨 표시 텍스트 렌더링
	void RenderDownLevelText(CRenderer* renderer, bool isHeroParty);

	void SetIsShowedDamageText(bool show);
	void SetShowingDamageTextTime(float time);
	void SetDamageText(const wchar_t* text);
	void SetDamageTextColor(glm::vec4 color);

	void SetIsBoss(bool boss);
	bool GetIsBoss() const;

	void InitBattleVariable();

	// 턴 게이지 관련
	// 턴 게이지가 증가하도록 설정
	void IncreaseGettingTurnGauge(float value);
	// 턴 게이지를 즉시 반영
	void SetGettingTurnGauge(float gauge);

	float GetGettingTurnGauge() const;
	float GetShowingTurnGauge();

	void SetIsUpdatedTurnGaugeWhenEndTurn(bool end);
	bool GetIsUpdatedTurnGaugeWhenEndTurn() const;

	void SetAttackAttribute(short attribute);
	short GetAttackAttribute() const;

	void GiveAttackDamage(CTurnBattleObject* attacker, bool hero);

	void InitTurn();

	void SetMonsterId(short id);
	short GetMonsterId() const;

	void SetIsFixed(bool fix);
	bool GetIsFixed() const;

	void SetIsSubObject(bool sub);
	bool GetIsSubObject() const;

	void RemoveAllSkill();

	void SetUIBackGround(int textureID);
	GLuint GetUIBackGround() const;


private:

};