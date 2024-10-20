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
	// 타켓팅된 스킬 정보
	struct STargetedSkill {
		// 존재하는지
		bool exisited;

		// 타켓팅됬는지
		bool targeted;

		// hp바 표시 여부
		bool IsShowedHpbar;

		// 스킬 인덱스
		short skillIndex;

		// 스킬 이펙트 지속 시간
		float skillEffectTime;

		// 스킬 이펙트 데이터
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


	// 스킬 정보 저장
	std::map<short, TurnBattle::SSkill*> m_SkillMap;

	// 스킬 이펙트 스프라이트 정보 저장
	std::map<std::string, int> m_SkillEffectSpriteMap;

	// 스킬 사운드 이펙트 정보 저장
	std::map<short, int> m_SkillSoundEffectMap;

	// 오토마타 정보 저장
	std::map<short, TurnBattle::SAutomata*> m_AutomataMap;

	// 덱 정보 저장
	std::map<short, TurnBattle::SDeck*> m_DeckMap;

	// 몬스터 정보 저장
	std::map<short, TurnBattle::SMonster*> m_MonsterMap;

	// 몬스터 분석 여부(한 번 이상 사냥 완료)
	std::map<short, bool > m_MonsterAnalyzeEndDataMap;

	// 몬스터 모델링, 애니메이션 텍스쳐, 사운드 정보 저장
	std::map<short, std::string> m_MonsterModelMap;
	std::map<short, std::map<GameObjectAnimationState, std::string>> m_MonsterAnimationsMap;
	std::map<short, int> m_MonsterTextureMap;
	std::map<short, int> m_MonsterBloodTextureMap;
	std::map<short, int> m_MonsterDefaultSoundMap;

	std::map<int, SBattleMapData> m_BattleFieldDataMap;


	// 주인공 파티 정보
	std::vector<CTurnBattleObject*> m_HeroVector;

	// 적 몬스터 파티 정보
	std::vector<CTurnBattleObject*> m_EnemyMonsterVector;
	
	// 행동트리가 무언가 처리 중인지 여부
	bool m_bIsProcessing = false;

	// 일반 공격 사용 여부
	bool m_bIsNormalAttackUsed = false;
	bool m_bIsMatterAttack = true;
	// 추가 일반 공격 타임
	bool m_bIsAdditionalNormalAttackUsed = false;
	bool m_bIsAdditionalNormalAttackTime = false;

	// 스킬을 처리하기 위한 변수
	// 사용되고 있는 스킬 정보
	TurnBattle::SUsedSkillData m_UsedSkillData;
	// 스킬을 사용했는지 여부
	bool m_bIsSkillUsed = false;
	// 선택된 스킬 인덱스
	short m_sSelectedSkillIndex;

	// 오토마타 소환 여부
	bool m_bIsAutomataSummoned = false;
	// 방금 소환한 오토마타 위치
	int m_iSummonedAutomataLocation = 0;

	// 스킬 덱 변경 여부
	bool m_bIsSkillDeckChanged = false;

	// 대기 여부
	bool m_bIsWaited = false;
	// 휴식(대기 행동 타입) 여부
	bool m_bIsRested = false;

	// 스틸 여부
	bool m_bIsStolen = false;
	// 오토마타 스틸(스틸 행동 타입) 여부
	bool m_bIsStolenAutomata = false;

	// 도주 여부
	bool m_bIsRunaway = false;
	// 도주 성공 여부
	bool m_bIsRunawaySuccess = false;

	// 한가지 키를 계속 누르고 있는 시간
	float m_fKeepPressKey = 0.0f;

	// 각종 턴 조작 정보
	int m_iOrder = 0;
	bool m_bHeroTeam = true;
	int m_iTarget = 0;

	// 메뉴 조작
	int m_iAct = 0;
	int m_iMenu = 0; // 0: 메인, 1: 스킬, 2: 오토마타, 3: 덱 변경...

	// 적 정보 표시
	bool m_bIsShowEnemyInformation = false;

	// 스킬 선택
	int m_iSelectedSkill = 0;
	int m_iShowedSkillNum = 0;
	// 스킬 정보 표시
	bool m_bIsShowSkillInformation = false;

	// 오토마타 선택
	int m_iSelectedAutomata = 0;
	int m_iShowedAutomataNum = 0;
	short m_sSummonedAutomata[2] = { 0, 0 };
	// 오토마타 정보 표시
	bool m_bIsShowAutomataInformation = false;

	// 스킬 덱 선택
	int m_iSelectedSkillDeck = 0;

	// 로그 텍스트
	wchar_t m_wLogText[30] = L"테스트 텍스트";
	// 로그 보이기
	bool m_bShowLog = false;

	float m_fChangeAnimationTime = 0.0f;
	bool m_bIsChangedSkillDeckAnimationEnd = true;
	bool m_bIsChangedSkillDeckAnimationMovingEnd = false;

	// 행동트리
	CBehaviorTree* m_pBehaviorTree = NULL;

	int m_HeroModelID = EMPTY_VALUE;
	int m_HeroTextureID = EMPTY_VALUE;
	int m_AutomataModelID = EMPTY_VALUE;
	int m_AutomataTextureID = EMPTY_VALUE;

	int m_battleBGMID = EMPTY_VALUE;

	// 잔디 모델링, 텍스쳐
	int m_grassModelID = EMPTY_VALUE;
	int m_grassTextureID = EMPTY_VALUE;

	GLuint m_GrassSSBOModelMatrices;

	// BGM
	irrklang::ISound* m_pBGM = NULL;

	// UI 제작용 흰색 사각형 텍스쳐
	int m_WhiteRectTextureID = EMPTY_VALUE;

	// 턴 전투 메인 UI 관련 텍스쳐
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

	// 카메라 변경
	bool m_bTestCameraOn = false;

	// 마우스 고정 설정 변수
	bool m_bMouseFixed = true;
	int m_iPrevMousePosX = WINDOW_SIZE_X / 2;
	int m_iPrevMousePosY = WINDOW_SIZE_Y / 2;

	// 바운딩 박스 그리기
	bool m_bShowBoundingBox = false;

	// 잔디 흔들림을 위한 시간 변수
	float m_fTimeForGrass = 0;

	// 로그를 보여주는 시간
	float m_fShowLogTime = SHOW_LOG_TIME;

	// 적 생성을 위한 지역 번호
	int m_iAreaNumber = 4;

	// 턴 전투 시작 대기 중
	bool m_bIsTurnBattleStartWait = false;
	float m_fStartWaitTime = 0.0f;

	// 주인공의 액션 전투 승리로 실행되는지 확인
	bool m_bIsStartedWithHeroWin = true;

	// 주인공이 죽어 게임 오버 됬는지 확인
	bool m_bIsGameover = false;
	bool m_bIsGameclear = false;

	// 하이브리드 모드 관련
	bool m_bIsTurnModeOpened = false;

	int m_iHybridBattleMonsterOrder = 0;
	CBehaviorTree* m_pBehaviorTreeInHybridBattleForEnemy = NULL;

	bool m_bIsProcessingInHybridBattleForEnemy = false;

	CRainDropParticle* m_pRainDropParticle = NULL;
	int m_waterDropTexureID = EMPTY_VALUE;

	std::vector<CGameObject*> m_vLampLightVector;

public:
	// 플레이어의 키 입력 처리
	virtual void ProcessInputs(float elapsedTimeInSec, GameInputs* inputs);

	virtual void Update(float elapsedTimeInSec);
	virtual void RenderScene();

	// 스테이트 진입시
	virtual void Enter();
	// 스테이트 종료시
	virtual void Exit();
	// 스테이트 임시 이동
	virtual void Pause();
	// 스테이트 복귀
	virtual void Resume();

	TurnBattle::SSkill* GetSkillData(short index);
	int GetSkillSoundEffect(short id);

	TurnBattle::SUsedSkillData GetUsedSkillData() const;
	void SetUsedSkillData(TurnBattle::SUsedSkillData usedSkillData);

	TurnBattle::SAutomata* GetAutomataData(short index);

	TurnBattle::SDeck* GetSkillDeckData(short index);

	// 행동트리를 컨트롤하기 위한 getter, setter
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

	// 현재 메뉴 변경
	void SetMenuLocation(int location);

	// 스킬 선택, 타겟 선택 설정
	void SetSelectedSkillAndTarget(int skill, int target);

	// 파티 맴버 수
	int GetPartySize(bool heroParty) const;

	// 전투 진입 전 설정
	void SetAreaNumber(int areaNumber);
	void SetIsStartedWithHeroWin(bool win);

	// 하이브리드 전투 관련 정보
	// 턴 모드 업데이트
	void TurnModeUpdate(float elapsedTimeInSec);
	void TurnModeProcessInputs(float elapsedTimeInSec, GameInputs* inputs);
	void TurnModeGettingTurnUpdate();
	// 하이브리드 배틀 오브젝트 사망 처리
	bool TurnModeRemoveDeadObject();
	// 턴 모드 UI 렌더링
	void TurnModeUIRender();
	// 현재 조작하고 있지 않은 파티 맴버의 HP, SP 바 UI 렌더링
	void TurnModeRenderHPSPBarUI(int x, int y);
	// 키 설명 UI 렌더링(메인 메뉴)
	void TurnModeRenderKeyInformationUIForMainMenu(int x, int y);
	// 적 몬스터 이름 표시 UI 렌더링
	void TurnModeRenderEnemyNameBarUI(int x, int y);
	// 빌보드 렌더링
	void TurnModeRenderRenderBillboards(std::vector<CBattleMonster*> monsterVector, glm::vec3 heroPos);
	void SetEnemyTurnZeroInHyBridBattleStart();
	// 적 몬스터 턴 존재시 턴 사용 시작 설정
	bool SetEnemyTurnStart();

	void RenderActLogInHybridBattle();

	int GetMenu() const;
	void SetMenu(int menu);

	// 턴 모드 오픈 여부
	void SetIsTurnModeOpened(bool open);
	bool GetIsTurnModeOpened() const;

	bool CanBeUsed();
	bool IsHadPartyMamber();

	void ChangeOrder();

	// 혹시 타겟 번호보다 적 오브젝트 수가 적을 경우 끝에 오브젝트 선택
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

	// 몬스터 데이터 얻기
	std::string GetMonsterModelInMonsterModelMap(short id);
	std::map<GameObjectAnimationState, std::string> GetMonsterAnimationsInMonsterAnimationsMap(short id);
	int GetMonsterTextureInMonsterTextureMap(short id);
	int GetMonsterBloodTextureInMonsterBloodTextureMap(short id);
	// 사운드는 텍스쳐 아이디를 사용하여 얻음
	int GetMonsterDefaultSoundInMonsterDefaultSoundMap(short id);

	void SetSkillHitAnimation();

	bool GetIsGameover() const;
	void SetIsGameover(bool over);
	bool GetIsGameclear() const;
	void SetIsGameclear(bool clear);

	CGameOverState* GetGameOverState();
	CGameClearState* GetGameClearState();

	// 주인공 레벨 업
	void HeroLevelUp(short exp);

	SBattleMapData GetBattleFieldData(int number);

	GLuint GetGrassSSBOModelMatrices() const;

	bool GetIsRunawaySuccess();

	bool IsShowingLogEnd();

private:
	// 일반 렌더러(계산된 그림자 이용)
	virtual void NormalRender();

	// UI 렌더링
	virtual void UIRender();

	// 필요 모델, 텍스쳐, 사운드 로드
	virtual void LoadData();

	// 필요 몬스터 모델, 텍스쳐 저장용
	void LoadMonsterData();

	// 필요 전투 필드 모델, 텍스쳐 저장용
	void LoadBattleFieldData();

	// 필요 모델, 텍스쳐, 사운드 로드 UI 제작용
	void LoadUIData();

	// 처음 State가 만들어졌을때 오브젝트 매니저에 오브젝트 추가
	virtual void AddObjectToManager();
	
	// 전투 필드 제작
	void MakeBattleField(int areaNumber);

	// 주인공 스텟 초기화
	void InitHeroStatus();

	// 적 생성 등 턴 전투 초기화
	void InitTurnBattle();
	void InsertMonsterToEnemyVector(bool isWin, int areaNumber);
	void InsertMonsterToEnemyVectorBoss(bool isWin, int areaNumber);

	// 전투 끝나고 초기화할 내용
	void InitVariableAfterBattle();

	void MakeGrassInstance();

	void RenderBillboards();

	// 턴 전투 메뉴안 인풋 처리
	void ProcessInputsInTurnBattleMenu(float elapsedTimeInSec, GameInputs* inputs);
	void ProcessInputsADMove(float elapsedTimeInSec, GameInputs* inputs);
	void ProcessInputsForEachMenuSelect(float elapsedTimeInSec, GameInputs* inputs);
	// 각 메뉴 안 인풋 처리
	void ProcessInputsForEachMenuProcessing(float elapsedTimeInSec, GameInputs* inputs);
	void ProcessInputsInMainMenu(float elapsedTimeInSec, GameInputs* inputs);
	void ProcessInputsInSkillMenu(float elapsedTimeInSec, GameInputs* inputs);
	void ProcessInputsInAutomataMenu(float elapsedTimeInSec, GameInputs* inputs);

	// 키 연속적으로 누를 경우 대기
	bool IsKeyResultWaitedEnough(float time);

	// 각 메뉴안 실행
	void ExecuteInMenu();

	// 스킬 사용
	void UseSkill();

	// 오토마타 소환
	void SummonAutomata();

	// 스킬 덱 변경
	void ChangeSkillDeck();

	// 일반 공격 사용
	void UseNormalAttack(bool matter, bool additional);

	// 대기 행동
	void WaitAction(bool rest);

	// 스틸 행동
	void StealAction(bool automata);

	// 적 몬스터 이름 표시 UI 렌더링
	void RenderEnemyNameBarUI(int x, int y);
	// 메인 메뉴 UI 렌더링
	void RenderMainMenuUI(int x, int y);

	// 키 설명 UI 렌더링(메인 메뉴)
	void RenderKeyInformationUIForMainMenu(int x, int y);
	// 키 설명 UI 렌더링(행동 메뉴)
	void RenderKeyInformationUIForActMenu(int x, int y);
	// 키 설명 UI 렌더링(스킬 메뉴)
	void RenderKeyInformationUIForSkillMenu(int x, int y);
	// 키 설명 UI 렌더링(오토마타 메뉴)
	void RenderKeyInformationUIForAutomataMenu(int x, int y);
	// 키 설명 UI 렌더링(덱 변경 메뉴)
	void RenderKeyInformationUIForDeckChangeMenu(int x, int y);
	// 키 설명 UI 렌더링(스틸, 대기 메뉴)
	void RenderKeyInformationUIForStillAndWait(int x, int y);

	// HP, SP 바 UI 렌더링
	void RenderHPSPBarUI(int x, int y);
	// 스킬 목록 UI 렌더링
	void RenderSkillListUI(int x, int y);
	// 오토마타 목록 UI 렌더링
	void RenderAutomataListUI(int x, int y);
	// space, shift 키 표시
	void RenderSpaceAndShiftButtonUI(int x, int y);
	// shift 키 표시
	void RenderShiftButtonUI(int x, int y);
	// 남은 턴 표시
	void RenderTurns(int x, int y);
	// 스킬 덱 변경 UI
	void RenderChangeSkillDeckUI(int x, int y);
	// 스킬 덱 렌더링
	void RenderSkillDeck(int x, int y, float locationX, float locationY, int index);
	// 스틸 메뉴 렌더링
	void RenderStealMenuUI(int x, int y);
	// 대기 메뉴 렌더링
	void RenderWaitMenuUI(int x, int y);
	// 행동 로그 표시 UI 렌더링
	void RenderActLogUI(int x, int y);
	// 스킬 덱 변경 중 UI 애니메이션
	void RenderChangingSkillDeckUIAnimation(int x, int y);

	// 스킬 상세 정보 표시 UI 렌더링
	void RenderSkillInformationUI(int x, int y);
	// 오토마타 상세 정보 표시 UI 렌더링
	void RenderAutomataInformationUI(int x, int y);
	// 적 상세 정보 표시 UI 렌더링
	void RenderEnemyInformationUI(int x, int y);

	// 죽은 오브젝트 제거
	bool RemoveDeadObject();

	// 보스 페이즈 변경
	void SetMonsterData(short id, CTurnBattleObject* object);

	// 스킬 데이터 읽어오기
	void ReadSkillData(const char* fileName);
	// 오토마타 데이터 읽어오기
	void ReadAutomataData(const char* fileName);
	// 덱 데이터 읽어오기
	void ReadDeckData(const char* fileName);
	// 몬스터 데이터 읽어오기
	void ReadMonsterData(const char* fileName);

	void UpdateDirectionalLightToTime(float elapsedTimeInSec);
	void SetDirectionalLightToTime();

	void BuildBehaviorTree();

	// 하이브리드 전투 관련 정보
	void TurnModeProcessInputsForEachMenuProcessing(float elapsedTimeInSec, GameInputs* inputs);
	void TurnModeProcessInputsInMainMenu(float elapsedTimeInSec, GameInputs* inputs);
	void TurnModeProcessInputsADMove(float elapsedTimeInSec, GameInputs* inputs);
	void BuildBehaviorTreeInHybridBattleForEnemy();
};

// 일반 공격 사용 관련
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
	// 현재 턴 오브젝트
	CTurnBattleObject* m_OrderObject;
	// 타겟 위치
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

// 스킬 사용 관련

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
	
	// 대미지 부여 스킬
	TurnBattle::SBattleVariable CalculateSkillDamage(TurnBattle::SSkill* skill,
		TurnBattle::SStatus userStatus, TurnBattle::SBattleVariable userBV,
		TurnBattle::SStatus targetStatus, TurnBattle::SBattleVariable targetBV,
		wchar_t* damageText, glm::vec4* damageTextColor);

	// 효과 부여 스킬(회복, 버프 디버프)
	TurnBattle::SBattleVariable CalculateSkillEffect(TurnBattle::SSkill* skill,
		TurnBattle::SStatus userStatus, TurnBattle::SBattleVariable userBV,
		TurnBattle::SStatus targetStatus, TurnBattle::SBattleVariable targetBV,
		wchar_t* damageText, glm::vec4* damageTextColor);

	// 버프 디버프 스킬
	TurnBattle::SBattleVariable CalculateBuffDeBuffSkill(TurnBattle::SSkill* skill, 
		TurnBattle::SBattleVariable targetBV, bool* alreadyMax);
	// 회복 스킬
	TurnBattle::SBattleVariable CalculateRecoverSkill(TurnBattle::SSkill* skill,
		TurnBattle::SStatus targetStatus, TurnBattle::SBattleVariable targetBV);

	// 특수 스킬(sp 감소 등)
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

// 오토마타 소환 관련
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

// 스킬 덱 변경 관련
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

// 대기 관련
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

// 스틸 관련
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

// 도주 관련
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

// 적의 행동 관련
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

// 하이브리드 배틀 안 적의 행동 선택

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


// 스킬 사용 관련

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
