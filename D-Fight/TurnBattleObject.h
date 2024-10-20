#pragma once
#include "GameObject.h"
#include "Camera.h"
#include "BehaviorTree.h"
#include "TurnBattleGobals.h"

/*
*	�� ��� ������ ���� �⺻ �÷��̾�, �� ������Ʈ
*	
*		���� �������ͽ� ���� ���� ���� �ൿ�� ����
*/

namespace TurnBattle {
	// ���� ����(���� �߿� ���� ������ ����)
	struct SStatus {
		// �� ��ġ�� �ִ�ġ
		short maxHp;
		short maxSp;
		short maxTurn;

		// �Ӽ� ����(����)
		short attribute[9];

		// ����
		short level;

		// ����(���� ���ݷ�, ���� ����, ���� ���ݷ�, ���� ����, ���߷�, ȸ���� ��)
		short stat[6];
	};

	// ���� ����(���� �߿� ���Ͽ� ������Ʈ �Ǵ� ����)
	struct SBattleVariable {
		short hp;
		short sp;
		short turn;

		// ����
		short attackBuff;
		short defenseBuff;

		// �ٿ� ��ġ
		short down;

		// ����ġ
		short exp;

		// ��� ȸ�� ����
		short avoidLevel;

		// ����
		short condition;
		// ���� ���� �ð�
		float conditionDuration;
	};

	// ��ų ����
	struct SSkill {
		// �Ӽ�
		short attribute;

		// �̸�
		wchar_t name[MAX_NAME_SIZE];

		// ������
		short minDamage; 
		short maxDamage;

		// Ȯ��
		float rate;

		// �Ҹ� ��
		short turn;
		short sp;

		// ȿ��
		short effect;
		float effectRate;

		// ����
		wchar_t informationLine1[MAX_INFORMATION_SIZE];
		wchar_t informationLine2[MAX_INFORMATION_SIZE];

		// ��ų ����Ʈ ����
		bool billboardEffect;
		// ��ų ����Ʈ ���� �ð�
		float effectTime;
		// ��ų ����Ʈ ������
		int effectData;

		// Ÿ��
		short target;
		bool selfParty;
	};

	struct SAutomata {
		// �̸�
		wchar_t name[MAX_NAME_SIZE];
		wchar_t englishName[MAX_NAME_SIZE];

		// ����
		SStatus status;

		// ��ų����Ʈ
		short skillList[MAX_SKILL_COUNT];
		short skillSize;

		short attackAttribute;
	};

	struct SDeck {
		// �̸�
		wchar_t name[MAX_NAME_SIZE];

		wchar_t strengh[MAX_INFORMATION_SIZE];
		wchar_t weak[MAX_INFORMATION_SIZE];
		wchar_t information[MAX_INFORMATION_SIZE];

		// �Ӽ�
		short attribute[ATTRIBUTE_COUNT];

		// ��ų����Ʈ
		short skillList[MAX_SKILL_COUNT];
		short skillSize;

		short attackAttribute;
	};

	struct SMonster {
		// �̸�
		wchar_t name[MAX_NAME_SIZE];

		// ����
		SStatus status;

		// ��ų����Ʈ
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

	// �̸�
	wchar_t m_wName[MAX_NAME_SIZE];

	// ������ �ִ� ��ų
	std::vector<short> m_SkillVector;
	int m_selectedSkill = 0; 

	// ������ �ִ� ���丶Ÿ
	std::vector<short> m_AutomataVector;
	// ������ �ִ� ��ų ��
	std::vector<short> m_DeckVector;
	
	// �������� ���
	short m_sAutomataBeingStolen;
	short m_sSkillDeckBeingStolen;

	// ī�޶� ����
	float m_fCameraHight = 2.0f;
	// ī�޶� �Ÿ�
	float m_fDistance = 10.f;

	// ����� ǥ�� ���� ����
	float m_bIsShowedDamageText = false;
	float m_fShowingDamageTextTime = 0.0f;
	wchar_t m_wDamageText[MAX_NAME_SIZE];
	glm::vec4 m_vDamageTextColor = DAMAGE_FONT_COLOR_NORMAL;

	// ���� ���Ϳ� ���� ������ ���� ����
	bool m_bIsBoss = false;

	// ���� ��� ���� ������
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

	// ���̺긮�� ��Ʋ�� �ٿ� ��ġ ������Ʈ
	bool UpdateDownLevelForHybridBattle(float elapsedTimeInSec);

	// ���� Getter, Setter
	TurnBattle::SStatus GetStatus() const;
	void SetStatus(TurnBattle::SStatus status);

	const wchar_t* GetName() const;
	void SetName(const wchar_t* name);

	TurnBattle::SBattleVariable GetBattleVariable() const;
	void SetBattleVariable(TurnBattle::SBattleVariable battleVariable);

	void SetIndexBingStolen(short automata, short skillDeck);
	void GetIndexBeingStolen(short* automata, short* skillDeck);

	// ��ġ(0, 1, 2, 3, 4)�� ����(��, ��)�� ���� ī�޶� ���� �Լ� �߰�
	void SetCameraPosAndDirectionWithNumber(CCamera* camera, int order, int partySize, bool self, bool enemy);

	// ���Ϳ� �� �߰� �Լ�
	std::vector<short> GetSkillVector();
	void GiveSkill(short skillID);
	std::vector<short> GetAutomataVector();
	void GiveAutomata(short automataID);
	std::vector<short> GetDeckVector();
	void GiveDeck(short deckID);

	// ������ Ư�� ��ġ �� ���� �Լ� 
	void RemoveDeck(int location);

	void SetAttributeAndSkillWithDeck(TurnBattle::SDeck* deck);

	// ��ų ��ȭ, �ʱ�ȭ�� �ε����� �޾� �� ��ġ �� ���� �Լ�
	void ChangeSkillAt(int index, short skillID);

	// ��ų ��� ���� ���� Ȯ��
	bool CheckEnableSkill(TurnBattle::SSkill *skill);

	// ���� hp �� ������
	void RenderHPBarOnTop(CRenderer* renderer, bool isHeroParty);

	// ���� hp �� ������
	void RenderHPBarOnTopForHybridBattle(CRenderer* renderer);

	// ������ ǥ�� �ؽ�Ʈ ������
	void RenderDamageText(CRenderer* renderer, bool isHeroParty);

	// �ٿ� ���� ǥ�� �ؽ�Ʈ ������
	void RenderDownLevelText(CRenderer* renderer, bool isHeroParty);

	void SetIsShowedDamageText(bool show);
	void SetShowingDamageTextTime(float time);
	void SetDamageText(const wchar_t* text);
	void SetDamageTextColor(glm::vec4 color);

	void SetIsBoss(bool boss);
	bool GetIsBoss() const;

	void InitBattleVariable();

	// �� ������ ����
	// �� �������� �����ϵ��� ����
	void IncreaseGettingTurnGauge(float value);
	// �� �������� ��� �ݿ�
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