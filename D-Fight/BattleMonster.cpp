#include "stdafx.h"
#include "BattleMonster.h"

CBattleMonster::CBattleMonster()
{
	memset(&m_backupAnimationData, 0, sizeof(m_backupAnimationData));

	CBattleMonster::BuildBehaviorTree();
}

CBattleMonster::~CBattleMonster()
{
	delete m_pBehaviorTree;
}

void CBattleMonster::Update(float elapsedTimeInSec, GameUpdateParameters parameters)
{
	if (m_pTurnBattleObject->GetAnimationState() == ANI_STATE_DEATH) {
		MatchAnimationWithTurnBattleObject();
		return;
	}

	// 턴 게이지가 hp바에 반영되도록 설정
	m_pTurnBattleObject->SetGettingTurnGauge(m_fTurnGauge);

	CGameObject::Update(elapsedTimeInSec, parameters);

	if (m_pTurnBattleObject->UpdateDownLevelForHybridBattle(elapsedTimeInSec)) {
		if (GetAnimationState() != ANI_STATE_IDLE) {
			SetAnimationState(ANI_STATE_IDLE);
		}
		// 다운 되어 있을 경우 리턴
		return;
	}

	// 상태이상 처리
	TurnBattle::SBattleVariable bv = m_pTurnBattleObject->GetBattleVariable();
	if (bv.condition != NORMAL_CONDITION) {
		bv.conditionDuration -= elapsedTimeInSec;
		if (bv.conditionDuration <= 0.0f) {
			bv.condition = NORMAL_CONDITION;
		}
		m_pTurnBattleObject->SetBattleVariable(bv);
	}
	
	UpdateSuperArmorRecoverTime(elapsedTimeInSec);
	m_pBehaviorTree->Run();

	if (IsGetTurn()) {
		m_pTurnBattleObject->InitTurn();
		SetTurnGauge(0.0f);
	}

	UpdateDamageCoolTime(elapsedTimeInSec);
}

void CBattleMonster::Render(CRenderer* renderer)
{
	CGameObject::Render(renderer);
	//RenderHPBarOnTop(renderer);
}

bool CBattleMonster::GiveDamageToHp(short damage)
{
	return false;
}

void CBattleMonster::SetAttackPattern(short pattern)
{
	m_sAttackPattern = pattern;
}

short CBattleMonster::GetAttackPattern() const
{
	return m_sAttackPattern;
}

void CBattleMonster::SetObjectManager(CObjectManager* manager)
{
	m_pObjectManager = manager;
}

CObjectManager* CBattleMonster::GetObjectManager()
{
	return m_pObjectManager;
}

void CBattleMonster::SetIsAttacking(bool attack)
{
	m_bIsAttacking = attack;
}

bool CBattleMonster::GetIsAttacking() const
{
	return m_bIsAttacking;
}

void CBattleMonster::SetTurnBattleObject(CTurnBattleObject* object)
{
	m_pTurnBattleObject = object;
}

CTurnBattleObject* CBattleMonster::GetTurnBattleObject()
{
	return m_pTurnBattleObject;
}

void CBattleMonster::InitWithTurnBattleObject()
{
	if (m_pTurnBattleObject == NULL) {
		std::cout << "몬스터를 초기화하기 위한 턴 배틀 오브젝트가 없습니다.\n";
		return;
	}
	SetModelWithBone(m_pTurnBattleObject->GetModelWithBone());
	SetAnimationDataNameMap(m_pTurnBattleObject->GetAnimationDataNameMap());
	SetTextureIndex(m_pTurnBattleObject->GetTextureIndex());
	SetBloodTextureID(m_pTurnBattleObject->GetBloodTextureID());
	SetDefaultSoundID(m_pTurnBattleObject->GetDefaultSoundID());
	SetHitSoundID(m_pTurnBattleObject->GetHitSoundID());
	SetIsAnimate(true);
	SetAnimationState(ANI_STATE_IDLE);
}

void CBattleMonster::SetIsDead(bool dead)
{
	m_bIsDead = dead;
}

bool CBattleMonster::GetIsDead() const
{
	return m_bIsDead;
}

float CBattleMonster::GetTurnGauge() const
{
	return m_fTurnGauge;
}

void CBattleMonster::SetTurnGauge(float gauge)
{
	m_fTurnGauge = gauge;
}

bool CBattleMonster::IsGetTurn()
{
	if (m_fTurnGauge >= 1.0f) {
		return true;
	}
	return false;
}

void CBattleMonster::AddTurnGauge(float value)
{
	m_fTurnGauge += value;
}

void CBattleMonster::UseSkillCameraSetting(CCamera* camera)
{
	// 주인공 방향으로 수평각 계산
	glm::vec3 pos = GetPosition();
	glm::vec3 heroPos = CBlackBoard::GetInstance()->GetHeroPosition();

	glm::vec3 dir = heroPos - pos;
	dir = glm::normalize(glm::vec3(dir.x, 0.0f, dir.z));

	glm::mat4 rotationMat = 
		glm::toMat4(glm::quat(glm::vec3(glm::radians(0.0), glm::radians(-90.0f), glm::radians(0.0f))));

	dir = rotationMat * glm::vec4(dir, 1.0f);

	float horizontalAngle = glm::atan(dir.z, -dir.x) - PI_VALUE / 2.0f;

	float verticalAngle = -PI_VALUE / 8.0f;

	// 계산된 각도로 필요 벡터 계산 뒤 적용
	glm::vec3 direction = glm::vec3(
		glm::cos(verticalAngle) * glm::sin(horizontalAngle),
		glm::sin(verticalAngle),
		glm::cos(verticalAngle) * glm::cos(horizontalAngle));

	glm::vec3 right = glm::vec3(
		glm::sin(horizontalAngle - PI_VALUE / 2.0f),
		0.0f,
		glm::cos(horizontalAngle - PI_VALUE / 2.0f));

	glm::vec3 up = glm::cross(right, direction);

	float distance = glm::distance(pos, heroPos);
	if (distance < 10.0f) distance = 10.0f; 

	float height = GetVolume().y * GetSize().y;
	camera->SetCamera((pos + heroPos) / 2.0f 
		- glm::normalize(dir) * distance / 1.5f + glm::vec3(0.0f, height + 2.5f, 0.0f),
		direction, right, up, 60.0f);

	// 주인공 방향으로 회전
	GameUpdateParameters param;
	memset(&param, 0, sizeof(param));
	
	dir = heroPos - pos;
	dir = glm::normalize(glm::vec3(dir.x, 0.0f, dir.z));

	glm::vec3 force = glm::vec3(0.0f, 0.0f, 0.0f);
	param.force = glm::vec3(
		dir.x,
		0.0f,
		dir.z);

	CGameObject::Update(1.0f / (float)FRAME_LIMIT, param);
	
	// 혹시 모를 버그를 수정하기 위해 원래 위치로 설정
	SetPosition(pos);
}

void CBattleMonster::SetSelectedSkill(int skillIndex)
{
	m_iSelectSkill = skillIndex;
}

int CBattleMonster::GetSelectedSkill() const
{
	return m_iSelectSkill;
}

void CBattleMonster::SetDustTextureID(int id)
{
	m_DustTextureID = id;
}

int CBattleMonster::GetDustTextureID() const
{
	return m_DustTextureID;
}

void CBattleMonster::SetBallTextureID(int id)
{
	m_BallTextureID = id;
}

int CBattleMonster::GetBallTextureID() const
{
	return m_BallTextureID;
}

void CBattleMonster::SetAttackPatternList(short* pattern)
{
	for (short i = 0; i < MAX_MONSTER_ATTACK_PATTERN; ++i) {
		m_AttackPatternList[i] = pattern[i];
	}
}

short* CBattleMonster::GetAttackPatternList()
{
	return m_AttackPatternList;
}

void CBattleMonster::SetHero(CHeroObject* hero)
{
	m_pHero = hero;
}

CHeroObject* CBattleMonster::GetHero()
{
	return m_pHero;
}

void CBattleMonster::SetBehaviorTree(CBehaviorTree* behaviorTree)
{
	if (m_pBehaviorTree != NULL) {
		delete m_pBehaviorTree;
	}
	m_pBehaviorTree = behaviorTree;
}

CBehaviorTree* CBattleMonster::GetBehaviorTree() const
{
	return m_pBehaviorTree;
}

bool CBattleMonster::GetIsSuperArmor() const
{
	return m_bIsSuperArmor;
}

void CBattleMonster::SetIsSuperArmor(bool armor)
{
	m_bIsSuperArmor = armor;
}

void CBattleMonster::UpdateSuperArmorGauge(short value)
{
	if (!m_bIsSuperArmor) return;

	m_sSuperArmorGauge -= value;

	if (m_sSuperArmorGauge < 0) {
		SetIsSuperArmor(false);
		m_sSuperArmorGauge = DEFAULT_SUPER_ARMOR_SIZE;
	}
}

void CBattleMonster::SetSuperArmorGauge(short value)
{
	m_sSuperArmorGauge = value;
}

void CBattleMonster::UpdateSuperArmorRecoverTime(float elapsedTimeInSec)
{
	if (m_bIsSuperArmor) return;

	m_fSuperArmorRecoverTime -= elapsedTimeInSec;

	if (m_fSuperArmorRecoverTime < 0.0f) {
		SetIsSuperArmor(true);
		m_fSuperArmorRecoverTime = DEFAULT_SUPER_ARMOR_RECOVER_TIME;
	}
}

void CBattleMonster::SetSpeed(float speed)
{
	m_fSpeed = speed;
}

float CBattleMonster::GetSpeed() const
{
	return m_fSpeed;
}

void CBattleMonster::BackupAnimation()
{
	m_backupAnimationData.state = GetAnimationState();
	m_backupAnimationData.isLoop = GetAnimator()->GetIsLoop();
	m_backupAnimationData.isStop = GetAnimator()->GetIsStop();
	m_backupAnimationData.speed = GetAnimator()->GetSpeed();
	m_backupAnimationData.time = GetAnimator()->GetAnimationCurrentTime();
}

void CBattleMonster::RestoreAnimation()
{
	SetAnimationState(m_backupAnimationData.state);
	GetAnimator()->SetIsLoop(m_backupAnimationData.isLoop);
	GetAnimator()->SetIsStop(m_backupAnimationData.isStop);
	GetAnimator()->SetSpeed(m_backupAnimationData.speed);
	GetAnimator()->SetAnimationCurrentTime(m_backupAnimationData.time);
}

bool CBattleMonster::IsCanAppliedDamage()
{
	if (m_fDamageCoolTime > 0.0f) return false;

	m_fDamageCoolTime = DEFAULT_DAMAGE_COOL_TIME;
	return true;
}

void CBattleMonster::UpdateDamageCoolTime(float elapsedTimeInSec)
{
	m_fDamageCoolTime -= elapsedTimeInSec;
	if (m_fDamageCoolTime <= 0.0f)
		m_fDamageCoolTime = 0.0f;
}

void CBattleMonster::MatchAnimationWithTurnBattleObject()
{
	if (m_pTurnBattleObject) {
		SetAnimationState(m_pTurnBattleObject->GetAnimationState());

		CAnimator* animator = m_pTurnBattleObject->GetAnimator();
		GetAnimator()->SetAnimationCurrentTime(animator->GetAnimationCurrentTime());
		GetAnimator()->SetIsLoop(animator->GetIsLoop());
		GetAnimator()->SetSpeed(animator->GetSpeed());
	}
}
