#pragma once
#include "GameState.h"
#include "GameFramework.h"
#include "TurnBattleObject.h"
#include "BehaviorTree.h"
#include "TurnBattleGobals.h"
#include "BattleMonster.h"
#include "TimeOfDaySkybox.h"
#include "FountainWaterDropParticle.h"
#include "RainDropParticle.h"
#include "GameOverState.h"
#include "GameClearState.h"
#include <random>

namespace TurnBattle {
	// Ÿ���õ� ��ų ����
	struct STargetedSkill {
		// �����ϴ���
		bool exisited;

		// Ÿ���É����
		bool targeted;

		// hp�� ǥ�� ����
		bool IsShowedHpbar;

		// ��ų �ε���
		short skillIndex;

		// ��ų ����Ʈ ���� �ð�
		float skillEffectTime;

		// ��ų ����Ʈ ������
		int skillEffectData;
	};

	struct SUsedSkillData {
		bool processing;

		STargetedSkill heroParty[MAX_HERO_PARTY_MEMBER];
		STargetedSkill enemyParty[MAX_ENEMY_PARTY_MEMBER];
	};
}

struct SBattleMapData {
	int fieldModel;
	int fieldTexture;
	int wallModel;
	int wallTexture;
	int lampModel;
	int lampTexture;
	int lampLightModel;
	glm::vec4 lightColor;
};

class CTurnBattleState : public CGameState {
public:
	CTurnBattleState();
	~CTurnBattleState();

private:
	CGameFramework* m_pGameFramework = NULL;
	CTimeOfDaySkybox* m_pTimeOfDaySkybox = NULL;

	CGameOverState* m_pGameOverState = NULL;
	CGameClearState* m_pGameClearState = NULL;


	// ��ų ���� ����
	std::map<short, TurnBattle::SSkill*> m_SkillMap;

	// ��ų ����Ʈ ��������Ʈ ���� ����
	std::map<std::string, int> m_SkillEffectSpriteMap;

	// ��ų ���� ����Ʈ ���� ����
	std::map<short, int> m_SkillSoundEffectMap;

	// ���丶Ÿ ���� ����
	std::map<short, TurnBattle::SAutomata*> m_AutomataMap;

	// �� ���� ����
	std::map<short, TurnBattle::SDeck*> m_DeckMap;

	// ���� ���� ����
	std::map<short, TurnBattle::SMonster*> m_MonsterMap;

	// ���� �м� ����(�� �� �̻� ��� �Ϸ�)
	std::map<short, bool > m_MonsterAnalyzeEndDataMap;

	// ���� �𵨸�, �ִϸ��̼� �ؽ���, ���� ���� ����
	std::map<short, std::string> m_MonsterModelMap;
	std::map<short, std::map<GameObjectAnimationState, std::string>> m_MonsterAnimationsMap;
	std::map<short, int> m_MonsterTextureMap;
	std::map<short, int> m_MonsterBloodTextureMap;
	std::map<short, int> m_MonsterDefaultSoundMap;

	std::map<int, SBattleMapData> m_BattleFieldDataMap;


	// ���ΰ� ��Ƽ ����
	std::vector<CTurnBattleObject*> m_HeroVector;

	// �� ���� ��Ƽ ����
	std::vector<CTurnBattleObject*> m_EnemyMonsterVector;
	
	// �ൿƮ���� ���� ó�� ������ ����
	bool m_bIsProcessing = false;

	// �Ϲ� ���� ��� ����
	bool m_bIsNormalAttackUsed = false;
	bool m_bIsMatterAttack = true;
	// �߰� �Ϲ� ���� Ÿ��
	bool m_bIsAdditionalNormalAttackUsed = false;
	bool m_bIsAdditionalNormalAttackTime = false;

	// ��ų�� ó���ϱ� ���� ����
	// ���ǰ� �ִ� ��ų ����
	TurnBattle::SUsedSkillData m_UsedSkillData;
	// ��ų�� ����ߴ��� ����
	bool m_bIsSkillUsed = false;
	// ���õ� ��ų �ε���
	short m_sSelectedSkillIndex;

	// ���丶Ÿ ��ȯ ����
	bool m_bIsAutomataSummoned = false;
	// ��� ��ȯ�� ���丶Ÿ ��ġ
	int m_iSummonedAutomataLocation = 0;

	// ��ų �� ���� ����
	bool m_bIsSkillDeckChanged = false;

	// ��� ����
	bool m_bIsWaited = false;
	// �޽�(��� �ൿ Ÿ��) ����
	bool m_bIsRested = false;

	// ��ƿ ����
	bool m_bIsStolen = false;
	// ���丶Ÿ ��ƿ(��ƿ �ൿ Ÿ��) ����
	bool m_bIsStolenAutomata = false;

	// ���� ����
	bool m_bIsRunaway = false;
	// ���� ���� ����
	bool m_bIsRunawaySuccess = false;

	// �Ѱ��� Ű�� ��� ������ �ִ� �ð�
	float m_fKeepPressKey = 0.0f;

	// ���� �� ���� ����
	int m_iOrder = 0;
	bool m_bHeroTeam = true;
	int m_iTarget = 0;

	// �޴� ����
	int m_iAct = 0;
	int m_iMenu = 0; // 0: ����, 1: ��ų, 2: ���丶Ÿ, 3: �� ����...

	// �� ���� ǥ��
	bool m_bIsShowEnemyInformation = false;

	// ��ų ����
	int m_iSelectedSkill = 0;
	int m_iShowedSkillNum = 0;
	// ��ų ���� ǥ��
	bool m_bIsShowSkillInformation = false;

	// ���丶Ÿ ����
	int m_iSelectedAutomata = 0;
	int m_iShowedAutomataNum = 0;
	short m_sSummonedAutomata[2] = { 0, 0 };
	// ���丶Ÿ ���� ǥ��
	bool m_bIsShowAutomataInformation = false;

	// ��ų �� ����
	int m_iSelectedSkillDeck = 0;

	// �α� �ؽ�Ʈ
	wchar_t m_wLogText[30] = L"�׽�Ʈ �ؽ�Ʈ";
	// �α� ���̱�
	bool m_bShowLog = false;

	float m_fChangeAnimationTime = 0.0f;
	bool m_bIsChangedSkillDeckAnimationEnd = true;
	bool m_bIsChangedSkillDeckAnimationMovingEnd = false;

	// �ൿƮ��
	CBehaviorTree* m_pBehaviorTree = NULL;

	int m_HeroModelID = EMPTY_VALUE;
	int m_HeroTextureID = EMPTY_VALUE;
	int m_AutomataModelID = EMPTY_VALUE;
	int m_AutomataTextureID = EMPTY_VALUE;

	int m_battleBGMID = EMPTY_VALUE;

	// �ܵ� �𵨸�, �ؽ���
	int m_grassModelID = EMPTY_VALUE;
	int m_grassTextureID = EMPTY_VALUE;

	GLuint m_GrassSSBOModelMatrices;

	// BGM
	irrklang::ISound* m_pBGM = NULL;

	// UI ���ۿ� ��� �簢�� �ؽ���
	int m_WhiteRectTextureID = EMPTY_VALUE;

	// �� ���� ���� UI ���� �ؽ���
	int m_UIEnemyNameBarTextureID = EMPTY_VALUE;
	int m_UIMainMenuBarTextureID = EMPTY_VALUE;
	int m_UIMainMenuBar2TextureID = EMPTY_VALUE;
	int m_UIMouseButtonInformationTextureID = EMPTY_VALUE;
	int m_UITurnNumberTextureID = EMPTY_VALUE;
	int m_UIHpSpBarTextureID = EMPTY_VALUE;
	int m_UIKeyInformationTextureID = EMPTY_VALUE;
	int m_UILongKeyButtonTextureID = EMPTY_VALUE;
	int m_UIListBarTextureID = EMPTY_VALUE;
	int m_UIListBarWithBoxTextureID = EMPTY_VALUE;
	int m_UISmallKeyButtonTextureID = EMPTY_VALUE;
	int m_UIActBarTextureID = EMPTY_VALUE;
	int m_UICardForSkillDeckTextureID = EMPTY_VALUE;
	int m_UICardForSkillDeckFrameTextureID = EMPTY_VALUE;
	int m_UIScrollBarTextureID = EMPTY_VALUE;
	int m_UIActLogBarTextureID = EMPTY_VALUE;
	int m_UIBurnConditionTextureID = EMPTY_VALUE;
	int m_UIAnxietyConditionTextureID = EMPTY_VALUE;

	// ī�޶� ����
	bool m_bTestCameraOn = false;

	// ���콺 ���� ���� ����
	bool m_bMouseFixed = true;
	int m_iPrevMousePosX = WINDOW_SIZE_X / 2;
	int m_iPrevMousePosY = WINDOW_SIZE_Y / 2;

	// �ٿ�� �ڽ� �׸���
	bool m_bShowBoundingBox = false;

	// �ܵ� ��鸲�� ���� �ð� ����
	float m_fTimeForGrass = 0;

	// �α׸� �����ִ� �ð�
	float m_fShowLogTime = SHOW_LOG_TIME;

	// �� ������ ���� ���� ��ȣ
	int m_iAreaNumber = 4;

	// �� ���� ���� ��� ��
	bool m_bIsTurnBattleStartWait = false;
	float m_fStartWaitTime = 0.0f;

	// ���ΰ��� �׼� ���� �¸��� ����Ǵ��� Ȯ��
	bool m_bIsStartedWithHeroWin = true;

	// ���ΰ��� �׾� ���� ���� ����� Ȯ��
	bool m_bIsGameover = false;
	bool m_bIsGameclear = false;

	// ���̺긮�� ��� ����
	bool m_bIsTurnModeOpened = false;

	int m_iHybridBattleMonsterOrder = 0;
	CBehaviorTree* m_pBehaviorTreeInHybridBattleForEnemy = NULL;

	bool m_bIsProcessingInHybridBattleForEnemy = false;

	CRainDropParticle* m_pRainDropParticle = NULL;
	int m_waterDropTexureID = EMPTY_VALUE;

	std::vector<CGameObject*> m_vLampLightVector;

public:
	// �÷��̾��� Ű �Է� ó��
	virtual void ProcessInputs(float elapsedTimeInSec, GameInputs* inputs);

	virtual void Update(float elapsedTimeInSec);
	virtual void RenderScene();

	// ������Ʈ ���Խ�
	virtual void Enter();
	// ������Ʈ �����
	virtual void Exit();
	// ������Ʈ �ӽ� �̵�
	virtual void Pause();
	// ������Ʈ ����
	virtual void Resume();

	TurnBattle::SSkill* GetSkillData(short index);
	int GetSkillSoundEffect(short id);

	TurnBattle::SUsedSkillData GetUsedSkillData() const;
	void SetUsedSkillData(TurnBattle::SUsedSkillData usedSkillData);

	TurnBattle::SAutomata* GetAutomataData(short index);

	TurnBattle::SDeck* GetSkillDeckData(short index);

	// �ൿƮ���� ��Ʈ���ϱ� ���� getter, setter
	bool GetIsSkillUsed() const;
	void SetIsSkillUsed(bool used);

	bool GetIsNormalAttackUsed() const;
	void SetIsNormalAttackUsed(bool used);

	bool GetIsAdditionalNormalAttackUsed() const;
	void SetIsAdditionalNormalAttackUsed(bool used);

	bool GetIsAdditionalNormalAttackTime() const;
	void SetIsAdditionalNormalAttackTime(bool enable);

	bool GetIsAutomataSummoned() const;
	void SetIsAutomataSummoned(bool summoned);

	bool GetIsSkillDeckChanged() const;
	void SetIsSkillDeckChanged(bool changed);

	bool GetIsMatterAttack() const;
	void SetIsMatterAttack(bool matter);

	bool GetIsProcessing() const;
	void SetIsProcessing(bool processing);

	bool GetIsHeroTurn() const;
	void SetIsHeroTurn(bool turn);

	bool GetIsChangedSkillDeckAnimationEnd() const;
	void SetIsChangedSkillDeckAnimationEnd(bool end);

	bool GetIsWaited() const;
	void SetIsWaited(bool wait);

	bool GetIsRested() const;
	void SetIsRested(bool rest);

	bool GetIsStolen() const;
	void SetIsStolen(bool still);

	bool GetIsStolenAutomata() const;
	void SetIsStolenAutomata(bool automata);

	bool GetIsRunaway() const;
	void SetIsRunaway(bool runaway);

	void SetIsRunawaySuccess(bool success);

	int GetOrder() const;

	void GetOrderSkillTargetAndIndex(int *order, int *target, short * selectedSkillIndex);

	void GetSummonedAutomataInformation(short *index, int *location);

	void GetChangedSkillDeckInformation(short *index, int *location);

	void SetLogText(const wchar_t* logtext);

	void SetShowLog(bool show);

	CTurnBattleObject* GetTurnBattleObject(bool hero, int index);

	void SetBattleVariable(TurnBattle::SBattleVariable bv, bool hero, int index);

	void SetCameraToOrderObject();

	void SetCameraToSelectedObject(bool heroParty, int index, bool self);

	// ���� �޴� ����
	void SetMenuLocation(int location);

	// ��ų ����, Ÿ�� ���� ����
	void SetSelectedSkillAndTarget(int skill, int target);

	// ��Ƽ �ɹ� ��
	int GetPartySize(bool heroParty) const;

	// ���� ���� �� ����
	void SetAreaNumber(int areaNumber);
	void SetIsStartedWithHeroWin(bool win);

	// ���̺긮�� ���� ���� ����
	// �� ��� ������Ʈ
	void TurnModeUpdate(float elapsedTimeInSec);
	void TurnModeProcessInputs(float elapsedTimeInSec, GameInputs* inputs);
	void TurnModeGettingTurnUpdate();
	// ���̺긮�� ��Ʋ ������Ʈ ��� ó��
	bool TurnModeRemoveDeadObject();
	// �� ��� UI ������
	void TurnModeUIRender();
	// ���� �����ϰ� ���� ���� ��Ƽ �ɹ��� HP, SP �� UI ������
	void TurnModeRenderHPSPBarUI(int x, int y);
	// Ű ���� UI ������(���� �޴�)
	void TurnModeRenderKeyInformationUIForMainMenu(int x, int y);
	// �� ���� �̸� ǥ�� UI ������
	void TurnModeRenderEnemyNameBarUI(int x, int y);
	// ������ ������
	void TurnModeRenderRenderBillboards(std::vector<CBattleMonster*> monsterVector, glm::vec3 heroPos);
	void SetEnemyTurnZeroInHyBridBattleStart();
	// �� ���� �� ����� �� ��� ���� ����
	bool SetEnemyTurnStart();

	void RenderActLogInHybridBattle();

	int GetMenu() const;
	void SetMenu(int menu);

	// �� ��� ���� ����
	void SetIsTurnModeOpened(bool open);
	bool GetIsTurnModeOpened() const;

	bool CanBeUsed();
	bool IsHadPartyMamber();

	void ChangeOrder();

	// Ȥ�� Ÿ�� ��ȣ���� �� ������Ʈ ���� ���� ��� ���� ������Ʈ ����
	int GetTarget();
	void SetTarget(int target);

	int GetAutomataModelID() const;
	int GetAutomataTextureID() const;

	int GetHybridBattleMonsterOrder() const;

	void RunBTForEnemyInHybridBattle();

	void SetIsProcessingInHybridBattleForEnemy(bool processing);
	bool GetIsProcessingInHybridBattleForEnemy() const;
	
	void UpdateUsedSkillData(float elapsedTimeInSec);

	void SetTimeOfDaySkybox(CTimeOfDaySkybox* timeOfDaySkybox);
	void SetFountainWaterDropParticle(CFountainWaterDropParticle* fountainWaterDropParticle);

	// ���� ������ ���
	std::string GetMonsterModelInMonsterModelMap(short id);
	std::map<GameObjectAnimationState, std::string> GetMonsterAnimationsInMonsterAnimationsMap(short id);
	int GetMonsterTextureInMonsterTextureMap(short id);
	int GetMonsterBloodTextureInMonsterBloodTextureMap(short id);
	// ����� �ؽ��� ���̵� ����Ͽ� ����
	int GetMonsterDefaultSoundInMonsterDefaultSoundMap(short id);

	void SetSkillHitAnimation();

	bool GetIsGameover() const;
	void SetIsGameover(bool over);
	bool GetIsGameclear() const;
	void SetIsGameclear(bool clear);

	CGameOverState* GetGameOverState();
	CGameClearState* GetGameClearState();

	// ���ΰ� ���� ��
	void HeroLevelUp(short exp);

	SBattleMapData GetBattleFieldData(int number);

	GLuint GetGrassSSBOModelMatrices() const;

	bool GetIsRunawaySuccess();

	bool IsShowingLogEnd();

private:
	// �Ϲ� ������(���� �׸��� �̿�)
	virtual void NormalRender();

	// UI ������
	virtual void UIRender();

	// �ʿ� ��, �ؽ���, ���� �ε�
	virtual void LoadData();

	// �ʿ� ���� ��, �ؽ��� �����
	void LoadMonsterData();

	// �ʿ� ���� �ʵ� ��, �ؽ��� �����
	void LoadBattleFieldData();

	// �ʿ� ��, �ؽ���, ���� �ε� UI ���ۿ�
	void LoadUIData();

	// ó�� State�� ����������� ������Ʈ �Ŵ����� ������Ʈ �߰�
	virtual void AddObjectToManager();
	
	// ���� �ʵ� ����
	void MakeBattleField(int areaNumber);

	// ���ΰ� ���� �ʱ�ȭ
	void InitHeroStatus();

	// �� ���� �� �� ���� �ʱ�ȭ
	void InitTurnBattle();
	void InsertMonsterToEnemyVector(bool isWin, int areaNumber);
	void InsertMonsterToEnemyVectorBoss(bool isWin, int areaNumber);

	// ���� ������ �ʱ�ȭ�� ����
	void InitVariableAfterBattle();

	void MakeGrassInstance();

	void RenderBillboards();

	// �� ���� �޴��� ��ǲ ó��
	void ProcessInputsInTurnBattleMenu(float elapsedTimeInSec, GameInputs* inputs);
	void ProcessInputsADMove(float elapsedTimeInSec, GameInputs* inputs);
	void ProcessInputsForEachMenuSelect(float elapsedTimeInSec, GameInputs* inputs);
	// �� �޴� �� ��ǲ ó��
	void ProcessInputsForEachMenuProcessing(float elapsedTimeInSec, GameInputs* inputs);
	void ProcessInputsInMainMenu(float elapsedTimeInSec, GameInputs* inputs);
	void ProcessInputsInSkillMenu(float elapsedTimeInSec, GameInputs* inputs);
	void ProcessInputsInAutomataMenu(float elapsedTimeInSec, GameInputs* inputs);

	// Ű ���������� ���� ��� ���
	bool IsKeyResultWaitedEnough(float time);

	// �� �޴��� ����
	void ExecuteInMenu();

	// ��ų ���
	void UseSkill();

	// ���丶Ÿ ��ȯ
	void SummonAutomata();

	// ��ų �� ����
	void ChangeSkillDeck();

	// �Ϲ� ���� ���
	void UseNormalAttack(bool matter, bool additional);

	// ��� �ൿ
	void WaitAction(bool rest);

	// ��ƿ �ൿ
	void StealAction(bool automata);

	// �� ���� �̸� ǥ�� UI ������
	void RenderEnemyNameBarUI(int x, int y);
	// ���� �޴� UI ������
	void RenderMainMenuUI(int x, int y);

	// Ű ���� UI ������(���� �޴�)
	void RenderKeyInformationUIForMainMenu(int x, int y);
	// Ű ���� UI ������(�ൿ �޴�)
	void RenderKeyInformationUIForActMenu(int x, int y);
	// Ű ���� UI ������(��ų �޴�)
	void RenderKeyInformationUIForSkillMenu(int x, int y);
	// Ű ���� UI ������(���丶Ÿ �޴�)
	void RenderKeyInformationUIForAutomataMenu(int x, int y);
	// Ű ���� UI ������(�� ���� �޴�)
	void RenderKeyInformationUIForDeckChangeMenu(int x, int y);
	// Ű ���� UI ������(��ƿ, ��� �޴�)
	void RenderKeyInformationUIForStillAndWait(int x, int y);

	// HP, SP �� UI ������
	void RenderHPSPBarUI(int x, int y);
	// ��ų ��� UI ������
	void RenderSkillListUI(int x, int y);
	// ���丶Ÿ ��� UI ������
	void RenderAutomataListUI(int x, int y);
	// space, shift Ű ǥ��
	void RenderSpaceAndShiftButtonUI(int x, int y);
	// shift Ű ǥ��
	void RenderShiftButtonUI(int x, int y);
	// ���� �� ǥ��
	void RenderTurns(int x, int y);
	// ��ų �� ���� UI
	void RenderChangeSkillDeckUI(int x, int y);
	// ��ų �� ������
	void RenderSkillDeck(int x, int y, float locationX, float locationY, int index);
	// ��ƿ �޴� ������
	void RenderStealMenuUI(int x, int y);
	// ��� �޴� ������
	void RenderWaitMenuUI(int x, int y);
	// �ൿ �α� ǥ�� UI ������
	void RenderActLogUI(int x, int y);
	// ��ų �� ���� �� UI �ִϸ��̼�
	void RenderChangingSkillDeckUIAnimation(int x, int y);

	// ��ų �� ���� ǥ�� UI ������
	void RenderSkillInformationUI(int x, int y);
	// ���丶Ÿ �� ���� ǥ�� UI ������
	void RenderAutomataInformationUI(int x, int y);
	// �� �� ���� ǥ�� UI ������
	void RenderEnemyInformationUI(int x, int y);

	// ���� ������Ʈ ����
	bool RemoveDeadObject();

	// ���� ������ ����
	void SetMonsterData(short id, CTurnBattleObject* object);

	// ��ų ������ �о����
	void ReadSkillData(const char* fileName);
	// ���丶Ÿ ������ �о����
	void ReadAutomataData(const char* fileName);
	// �� ������ �о����
	void ReadDeckData(const char* fileName);
	// ���� ������ �о����
	void ReadMonsterData(const char* fileName);

	void UpdateDirectionalLightToTime(float elapsedTimeInSec);
	void SetDirectionalLightToTime();

	void BuildBehaviorTree();

	// ���̺긮�� ���� ���� ����
	void TurnModeProcessInputsForEachMenuProcessing(float elapsedTimeInSec, GameInputs* inputs);
	void TurnModeProcessInputsInMainMenu(float elapsedTimeInSec, GameInputs* inputs);
	void TurnModeProcessInputsADMove(float elapsedTimeInSec, GameInputs* inputs);
	void BuildBehaviorTreeInHybridBattleForEnemy();
};

// �Ϲ� ���� ��� ����
class CCheckUseNormalAttack : public CLeafNode {
public:
	CCheckUseNormalAttack(CTurnBattleState* state, std::string name) : state(state) {
		SetName(name);
	}
	~CCheckUseNormalAttack() { }

private:
	CTurnBattleState* state;

public:
	virtual BehaviorTreeState Run();

};

class CMoveToTarget : public CLeafNode {
public:
	CMoveToTarget(CTurnBattleState* state, std::string name) : state(state) {
		SetName(name);
	}
	~CMoveToTarget() { }

private:
	// ���� �� ������Ʈ
	CTurnBattleObject* m_OrderObject;
	// Ÿ�� ��ġ
	glm::vec3 m_vTargetPosition;
	float m_fTargetAndUserSize = 2.0f;

	bool m_bSetTargetPosition = false;
	CTurnBattleState* state;
	float m_fSpeed = 10.f;

public:
	virtual BehaviorTreeState Run();

};

class CAttackTarget : public CLeafNode {
public:
	CAttackTarget(CTurnBattleState* state, std::string name) : state(state) {
		SetName(name);
	}
	~CAttackTarget() { }

private:
	CTurnBattleState* state;
	bool m_bSetAnimation = false;
	float m_fAdditionalAttackTime = 0.5f;

public:
	virtual BehaviorTreeState Run();

private:
	void ApplyAttackDamage();

	TurnBattle::SBattleVariable CalculateAttackDamage(bool matterAttack,
		TurnBattle::SStatus userStatus, TurnBattle::SBattleVariable userBV,
		TurnBattle::SStatus targetStatus, TurnBattle::SBattleVariable targetBV, 
		wchar_t* damageText, glm::vec4* damageTextColor);
};

// ��ų ��� ����

class CCheckIsSkillUsedAndShowSkillName : public CLeafNode {
public:
	CCheckIsSkillUsedAndShowSkillName(CTurnBattleState* state, std::string name) : state(state) {
		SetName(name);
	}
	~CCheckIsSkillUsedAndShowSkillName() { }

private:
	CTurnBattleState* state;

public:
	virtual BehaviorTreeState Run();
};


class CSetSkillData : public CLeafNode {
public:
	CSetSkillData(CTurnBattleState* state, std::string name) : state(state) {
		SetName(name);
	}
	~CSetSkillData() { }

private:
	CTurnBattleState* state;
	bool m_bSetSkillData = false;
	float m_fWaitTime = 0.0f;

public:
	virtual BehaviorTreeState Run();

	void FillUsedSkillData(TurnBattle::SUsedSkillData& usedSkillData,
		bool heroTurn, int index, short skillIndex, float effectTime, int effectData);

private:
	void SetUsedSkillData();

	
};

class CGiveSkillDamage : public CLeafNode {
public:
	CGiveSkillDamage(CTurnBattleState* state, std::string name) : state(state) {
		SetName(name);
	}
	~CGiveSkillDamage() { }

private:
	CTurnBattleState* state;
	bool m_bGiveDamageEnd = false;
	float m_fWaitTime = 0.75f;

public:
	virtual BehaviorTreeState Run();

	void TargetingEndInUsedSkillData();
	
	// ����� �ο� ��ų
	TurnBattle::SBattleVariable CalculateSkillDamage(TurnBattle::SSkill* skill,
		TurnBattle::SStatus userStatus, TurnBattle::SBattleVariable userBV,
		TurnBattle::SStatus targetStatus, TurnBattle::SBattleVariable targetBV,
		wchar_t* damageText, glm::vec4* damageTextColor);

	// ȿ�� �ο� ��ų(ȸ��, ���� �����)
	TurnBattle::SBattleVariable CalculateSkillEffect(TurnBattle::SSkill* skill,
		TurnBattle::SStatus userStatus, TurnBattle::SBattleVariable userBV,
		TurnBattle::SStatus targetStatus, TurnBattle::SBattleVariable targetBV,
		wchar_t* damageText, glm::vec4* damageTextColor);

	// ���� ����� ��ų
	TurnBattle::SBattleVariable CalculateBuffDeBuffSkill(TurnBattle::SSkill* skill, 
		TurnBattle::SBattleVariable targetBV, bool* alreadyMax);
	// ȸ�� ��ų
	TurnBattle::SBattleVariable CalculateRecoverSkill(TurnBattle::SSkill* skill,
		TurnBattle::SStatus targetStatus, TurnBattle::SBattleVariable targetBV);

	// Ư�� ��ų(sp ���� ��)
	TurnBattle::SBattleVariable CalculateSkillSpecial(TurnBattle::SSkill* skill,
		TurnBattle::SStatus userStatus, TurnBattle::SBattleVariable userBV,
		TurnBattle::SStatus targetStatus, TurnBattle::SBattleVariable targetBV,
		wchar_t* damageText, glm::vec4* damageTextColor);

	TurnBattle::SBattleVariable CalculateDecreaseSpSkill(TurnBattle::SSkill* skill,
		TurnBattle::SBattleVariable targetBV, wchar_t* damageText, glm::vec4* damageTextColor);

private:
	void ApplySkillDamage();
	
	
	void ShowHpBarEndInUsedSkillData();
};

// ���丶Ÿ ��ȯ ����
class CCheckSummonedAutomaAndShowLog : public CLeafNode {
public:
	CCheckSummonedAutomaAndShowLog(CTurnBattleState* state, std::string name) : state(state) {
		SetName(name);
	}
	~CCheckSummonedAutomaAndShowLog() { }

private:
	CTurnBattleState* state;

public:
	virtual BehaviorTreeState Run();

};

class CSetCameraAndVisibility : public CLeafNode {
public:
	CSetCameraAndVisibility(CTurnBattleState* state, std::string name) : state(state) {
		SetName(name);
	}
	~CSetCameraAndVisibility() { }

private:
	CTurnBattleState* state;
	float m_fShowTime = 0.0f;
	bool m_bSetEnd = false;
	bool m_bSetVisibilityEnd = false;

	int m_iSummonedAutomata;

public:
	virtual BehaviorTreeState Run();

};

// ��ų �� ���� ����
class CCheckChangeSkillDeckAndShowLog : public CLeafNode {
public:
	CCheckChangeSkillDeckAndShowLog(CTurnBattleState* state, std::string name) : state(state) {
		SetName(name);
	}
	~CCheckChangeSkillDeckAndShowLog() { }

private:
	CTurnBattleState* state;

public:
	virtual BehaviorTreeState Run();

};

class CSetSelfCameraAndWait : public CLeafNode {
public:
	CSetSelfCameraAndWait(CTurnBattleState* state, std::string name) : state(state) {
		SetName(name);
	}
	~CSetSelfCameraAndWait() { }

private:
	CTurnBattleState* state;
	bool m_bSetEnd = false;

public:
	virtual BehaviorTreeState Run();

};

// ��� ����
class CCheckWaitAndShowLog : public CLeafNode {
public:
	CCheckWaitAndShowLog(CTurnBattleState* state, std::string name) : state(state) {
		SetName(name);
	}
	~CCheckWaitAndShowLog() { }

private:
	CTurnBattleState* state;

public:
	virtual BehaviorTreeState Run();

};

class CWaitSomeTime : public CLeafNode {
public:
	CWaitSomeTime(CTurnBattleState* state, std::string name) : state(state) {
		SetName(name);
	}
	~CWaitSomeTime() { }

private:
	CTurnBattleState* state;
	float m_fWaitTime = 0.0f;
	bool m_bSetEnd = false;

public:
	virtual BehaviorTreeState Run();

};

// ��ƿ ����
class CCheckStealAndShowLog : public CLeafNode {
public:
	CCheckStealAndShowLog(CTurnBattleState* state, std::string name) : state(state) {
		SetName(name);
	}
	~CCheckStealAndShowLog() { }

private:
	CTurnBattleState* state;

public:
	virtual BehaviorTreeState Run();

};

class CWaitAndGotoMainMenu : public CLeafNode {
public:
	CWaitAndGotoMainMenu(CTurnBattleState* state, std::string name) : state(state) {
		SetName(name);
	}
	~CWaitAndGotoMainMenu() { }

private:
	CTurnBattleState* state;
	float m_fWaitTime = 0.0f;
	bool m_bSetEnd = false;

public:
	virtual BehaviorTreeState Run();

};

// ���� ����
class CCheckRunawayAndShowLog : public CLeafNode {
public:
	CCheckRunawayAndShowLog(CTurnBattleState* state, std::string name) : state(state) {
		SetName(name);
	}
	~CCheckRunawayAndShowLog() { }

private:
	CTurnBattleState* state;

public:
	virtual BehaviorTreeState Run();

};

class CWaitSomeTimeForRunaway : public CLeafNode {
public:
	CWaitSomeTimeForRunaway(CTurnBattleState* state, std::string name) : state(state) {
		SetName(name);
	}
	~CWaitSomeTimeForRunaway() { }

private:
	CTurnBattleState* state;
	float m_fWaitTime = 0.0f;
	bool m_bSetEnd = false;

public:
	virtual BehaviorTreeState Run();

};

// ���� �ൿ ����
class CCheckIsNotHeroTurn : public CLeafNode {
public:
	CCheckIsNotHeroTurn(CTurnBattleState* state, std::string name) : state(state) {
		SetName(name);
	}
	~CCheckIsNotHeroTurn() { }

private:
	CTurnBattleState* state;

public:
	virtual BehaviorTreeState Run();
};

class CSelectEnemyAction : public CLeafNode {
public:
	CSelectEnemyAction(CTurnBattleState* state, std::string name) : state(state) {
		SetName(name);
	}
	~CSelectEnemyAction() { }

private:
	CTurnBattleState* state;

public:
	virtual BehaviorTreeState Run();
};

// ���̺긮�� ��Ʋ �� ���� �ൿ ����

class CSelectEnemyActionInHybridBattle : public CLeafNode {
public:
	CSelectEnemyActionInHybridBattle(CTurnBattleState* state, std::string name) : state(state) {
		SetName(name);
	}
	~CSelectEnemyActionInHybridBattle() { }

private:
	CTurnBattleState* state;

public:
	virtual BehaviorTreeState Run();
};


// ��ų ��� ����

class CSetSkillDataInHybridBattleForEnemy : public CLeafNode {
public:
	CSetSkillDataInHybridBattleForEnemy(CTurnBattleState* state, std::string name) : state(state) {
		SetName(name);
	}
	~CSetSkillDataInHybridBattleForEnemy() { }

private:
	CTurnBattleState* state;
	bool m_bSetSkillData = false;
	float m_fWaitTime = 0.0f;

public:
	virtual BehaviorTreeState Run();

private:
	void SetUsedSkillData();
};

class CGiveSkillDamageInHybridBattleForEnemy : public CLeafNode {
public:
	CGiveSkillDamageInHybridBattleForEnemy(CTurnBattleState* state, std::string name) : state(state) {
		SetName(name);
	}
	~CGiveSkillDamageInHybridBattleForEnemy() { }

private:
	CTurnBattleState* state;
	bool m_bGiveDamageEnd = false;
	float m_fWaitTime = 0.75f;

public:
	virtual BehaviorTreeState Run();

private:
	void ApplySkillDamage();
};
