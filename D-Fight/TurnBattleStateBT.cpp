#include "stdafx.h"
#include "TurnBattleState.h"

// �ൿƮ�� ����

void CTurnBattleState::BuildBehaviorTree()
{
	// �Ϲ� ���� ���� ���
	CCheckUseNormalAttack* checkUseNormalAttack
		= new CCheckUseNormalAttack(this, "Check is normal attack used and show log");
	CMoveToTarget* moveToTarget
		= new CMoveToTarget(this, "Move to target");
	CAttackTarget* attackTarget
		= new CAttackTarget(this, "Attack target");

	CSequenceNode* normalAttackSequence = new CSequenceNode("Check and Processing normal attack");
	normalAttackSequence->AddChild(checkUseNormalAttack);
	normalAttackSequence->AddChild(moveToTarget);
	normalAttackSequence->AddChild(attackTarget);

	// ��ų ���� ���
	CCheckIsSkillUsedAndShowSkillName* checkIsSkillUsedAndShowSkillName
		= new CCheckIsSkillUsedAndShowSkillName(this, "Check is skill used and show skill Name");
	CSetSkillData* setSkillData
		= new CSetSkillData(this, "Set skill data and wait");
	CGiveSkillDamage* giveSkillDamage
		= new CGiveSkillDamage(this, "Give skill damage");

	CSequenceNode* skillSequence = new CSequenceNode("Check and Processing Skill");
	skillSequence->AddChild(checkIsSkillUsedAndShowSkillName);
	skillSequence->AddChild(setSkillData);
	skillSequence->AddChild(giveSkillDamage);

	// ���丶Ÿ ��ȯ ���� ���
	CCheckSummonedAutomaAndShowLog* checkSummonedAutomaAndShowLog
		= new CCheckSummonedAutomaAndShowLog(this, "Check is automata summoned and show log");
	CSetCameraAndVisibility* setCameraAndVisibility
		= new CSetCameraAndVisibility(this, "Set camera and wait to show automata");

	CSequenceNode* automataSequence = new CSequenceNode("Check and Show Automata");
	automataSequence->AddChild(checkSummonedAutomaAndShowLog);
	automataSequence->AddChild(setCameraAndVisibility);

	// ��ų �� ���� ���
	CCheckChangeSkillDeckAndShowLog* checkChangeSkillDeckAndShowLog
		= new CCheckChangeSkillDeckAndShowLog(this, "Check is skill deck changed and show log");
	CSetSelfCameraAndWait* setSelfCameraAndWait
		= new CSetSelfCameraAndWait(this, "Set Self Camera And Wait");

	CSequenceNode* skillDeckSequenceNode = new CSequenceNode("Check and Wait Skill Deck changed");
	skillDeckSequenceNode->AddChild(checkChangeSkillDeckAndShowLog);
	skillDeckSequenceNode->AddChild(setSelfCameraAndWait);

	// ��� �޴� ���� ���
	CCheckWaitAndShowLog* checkWaitAndShowLog
		= new CCheckWaitAndShowLog(this, "Check is waited and show log");
	CWaitSomeTime* waitSomeTime
		= new CWaitSomeTime(this, "Wait some time");

	CSequenceNode* waitSequence = new CSequenceNode("Check Wait");
	waitSequence->AddChild(checkWaitAndShowLog);
	waitSequence->AddChild(waitSomeTime);

	// ��ƿ �޴� ���� ���
	CCheckStealAndShowLog* checkStealAndShowLog
		= new CCheckStealAndShowLog(this, "Check is stolen and show log");
	CWaitAndGotoMainMenu* waitAndGotoMainMenu
		= new CWaitAndGotoMainMenu(this, "Wait and go to main menu");

	CSequenceNode* stealSequence = new CSequenceNode("Check Steal");
	stealSequence->AddChild(checkStealAndShowLog);
	stealSequence->AddChild(waitAndGotoMainMenu);

	// ���� �޴� ���� ���
	CCheckRunawayAndShowLog* checkRunawayAndShowLog
		= new CCheckRunawayAndShowLog(this, "Check is runaway and show log");
	CWaitSomeTimeForRunaway* waitSomeTimeForRunaway
		= new CWaitSomeTimeForRunaway(this, "Wait some time for runaway menu");

	CSequenceNode* runawaySequence = new CSequenceNode("Check runaway");
	runawaySequence->AddChild(checkRunawayAndShowLog);
	runawaySequence->AddChild(waitSomeTimeForRunaway);

	// ó���� �ൿ ���� ���
	CSelectorNode* playerActionSelector = new CSelectorNode("Player action select");
	playerActionSelector->AddChild(normalAttackSequence);
	playerActionSelector->AddChild(skillSequence);
	playerActionSelector->AddChild(automataSequence);
	playerActionSelector->AddChild(skillDeckSequenceNode);
	playerActionSelector->AddChild(stealSequence);
	playerActionSelector->AddChild(waitSequence);
	playerActionSelector->AddChild(runawaySequence);


	// �� �ൿ ���� ���
	CCheckIsNotHeroTurn* checkIsNotHeroTurn
		= new CCheckIsNotHeroTurn(this, "Check is not hero turn");
	CSelectEnemyAction* selectEnemyAction
		= new CSelectEnemyAction(this, "Enemy select action");

	CSequenceNode* enemySkillSequence = new CSequenceNode("Enemy skill");
	enemySkillSequence->AddChild(selectEnemyAction);
	enemySkillSequence->AddChild(skillSequence);

	CSelectorNode* enemyAttackSelector = new CSelectorNode("Enemy attack slect");
	enemyAttackSelector->AddChild(enemySkillSequence);
	enemyAttackSelector->AddChild(normalAttackSequence);

	CSequenceNode* enemyActionSequence = new CSequenceNode("Enemy action");
	enemyActionSequence->AddChild(checkIsNotHeroTurn);
	enemyActionSequence->AddChild(enemyAttackSelector);

	CSelectorNode* turnBattleSelector = new CSelectorNode("Turn Battle processing");
	turnBattleSelector->AddChild(enemyActionSequence);
	turnBattleSelector->AddChild(playerActionSelector);


	m_pBehaviorTree = new CBehaviorTree("Turn Battle Processing AI", turnBattleSelector);
}


// CCheckUseNormalAttack---------------------------------------------

BehaviorTreeState CCheckUseNormalAttack::Run()
{
	// �Ϲ� ���� ����
	if (!state->GetIsProcessing() || state->GetIsAdditionalNormalAttackUsed()) {
		if (state->GetIsNormalAttackUsed()) {

			wchar_t logText[MAX_INFORMATION_SIZE];
			if (state->GetIsMatterAttack()) {
				wsprintf(logText, L"���� ���� ���");
			}
			else {
				wsprintf(logText, L"���� ���� ���");
			}
			state->SetIsAdditionalNormalAttackUsed(false);
			state->SetShowLog(true);
			state->SetIsProcessing(true);
			state->SetLogText(logText);
			return BT_SUCCESS;
		}
		return BT_FAILURE;
	}

	return BT_FAILURE;
}

// CMoveToTarget------------------------------------------------------

BehaviorTreeState CMoveToTarget::Run()
{
	CTurnBattleObject* object = state->GetTurnBattleObject(state->GetIsHeroTurn(), state->GetOrder());
	if (object->GetIsFixed())
		return BT_SUCCESS;

	if (!m_bSetTargetPosition) {
		int order, target;
		short index;
		state->GetOrderSkillTargetAndIndex(&order, &target, &index);

		CTurnBattleObject* targetObject;
		if (state->GetIsHeroTurn()) {
			m_OrderObject = state->GetTurnBattleObject(true, order);
			targetObject = state->GetTurnBattleObject(false, target);
		}
		else {
			m_OrderObject = state->GetTurnBattleObject(false, order);
			targetObject = state->GetTurnBattleObject(true, target);
		}

		m_vTargetPosition = targetObject->GetPosition();
		m_fTargetAndUserSize = targetObject->GetVolume().z + m_OrderObject->GetVolume().z;
		m_bSetTargetPosition = true;
		if (m_OrderObject->GetIsAnimate()) {
			m_OrderObject->SetAnimationState(ANI_STATE_WALK);
		}
		return BT_RUNNING;
	}
	else {
		glm::vec3 position = m_OrderObject->GetPosition();
		float bias = 0.75f;
		if (glm::distance(glm::vec3(position.x, 0.0f, position.z),
			glm::vec3(m_vTargetPosition.x, 0.0f, m_vTargetPosition.z)) < m_fTargetAndUserSize + bias) {
			m_bSetTargetPosition = false;
			if (m_OrderObject->GetIsAnimate()) {
				m_OrderObject->SetAnimationState(ANI_STATE_IDLE);
			}
			return BT_SUCCESS;
		}

		glm::vec3 dir = glm::normalize(m_vTargetPosition - position);
		dir = glm::vec3(dir.x, 0.0f, dir.z);
		m_OrderObject->SetPosition(position + (dir * m_fSpeed * BackBoard()->GetElapsedTimeInSec()));
		glm::vec3 prevPosition = m_OrderObject->GetPosition();

		// Ÿ�� �������� ȸ��
		GameUpdateParameters param;
		memset(&param, 0, sizeof(param));
		param.force = glm::vec3(dir.x * object->GetMass(), 0.0f, dir.z * object->GetMass());

		// ������Ʈ�� ������Ʈ�� �ൿƮ���� ������Ʈ �ϱ� ������ �θ��� ������Ʈ�� ȣ��
		object->CGameObject::Update(BackBoard()->GetElapsedTimeInSec(), param);
		object->SetPosition(prevPosition);
		state->SetCameraToOrderObject();

		return BT_RUNNING;
	}
}

// CAttackTarget------------------------------------------------------

BehaviorTreeState CAttackTarget::Run()
{
	if (!m_bSetAnimation) {
		m_bSetAnimation = true;

		CTurnBattleObject* object;
		if (state->GetIsHeroTurn()) {
			object = state->GetTurnBattleObject(true, state->GetOrder());
		}
		else {
			object = state->GetTurnBattleObject(false, state->GetOrder());
		}
		if (object->GetIsAnimate()) {
			if (state->GetIsMatterAttack()) {
				object->SetAnimationState(ANI_STATE_ATTACK);
				CSoundPlayer::GetInstance()->PlaySound(state->GetSkillSoundEffect(0), 1.0f);
			}
			else {
				object->SetAnimationState(ANI_STATE_SKILL_1);
				CSoundPlayer::GetInstance()->PlaySound(state->GetSkillSoundEffect(1), 1.0f);
			}
			object->GetAnimator()->SetIsLoop(false);
		}

		return BT_RUNNING;
	}
	else {
		if (state->GetIsAdditionalNormalAttackTime()) {
			// �߰� ���� ��� �ð�
			m_fAdditionalAttackTime -= BackBoard()->GetElapsedTimeInSec();

			int order, target;
			short index;
			state->GetOrderSkillTargetAndIndex(&order, &target, &index);
			bool heroTurn = state->GetIsHeroTurn();

			short turn = state->GetTurnBattleObject(heroTurn, order)->GetBattleVariable().turn;

			// �߰� ���� ���� �ٽ� ��ü ����
			if (state->GetIsAdditionalNormalAttackUsed()) {
				state->SetIsAdditionalNormalAttackTime(false);
				m_bSetAnimation = false;

				return BT_SUCCESS;
			}

			// ���� ��� �ð��� �������� ����
			if (turn == 0 || m_fAdditionalAttackTime <= 0.0f) {
				m_bSetAnimation = false;

				CTurnBattleObject* object;
				if (state->GetIsHeroTurn()) {
					object = state->GetTurnBattleObject(true, state->GetOrder());
				}
				else {
					object = state->GetTurnBattleObject(false, state->GetOrder());
				}
				// �������� ����
				if (state->GetIsHeroTurn()) {
					object->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
				}
				else {
					object->SetRotation(glm::vec3(0.0f, 180.0f, 0.0f));
				}

				glm::vec3 prevPosition;
				// ���� ��ġ�� �̵�

				float height = state->GetTurnBattleObject(heroTurn, order)->GetVolume().y;
				if (heroTurn) {
					prevPosition = glm::vec3(-8.0f + 5.0f * order, -4.0f + height, 10.0f);
				}
				else {
					if (state->GetTurnBattleObject(heroTurn, order)->GetIsBoss()) {
						prevPosition = glm::vec3(-15.0f + 6.0f + 6.0f * order, -4.0f + height, -10.0f);
					}
					else if (state->GetTurnBattleObject(heroTurn, order)->GetIsSubObject()) {
						if (state->GetTurnBattleObject(heroTurn, order)->GetMonsterId() == 11) {
							prevPosition = glm::vec3(-15.0f + 6.0f + 6.0f * 0, -4.0f + height, -10.0f);
						}
						else {
							prevPosition = glm::vec3(-15.0f + 6.0f + 6.0f * 2, -4.0f + height, -10.0f);
						}
					}
					else {
						prevPosition = glm::vec3(-15.0f + 6.0f * order, -4.0f + height, -10.0f);
					}
				}
				if (state->GetTurnBattleObject(heroTurn, order)->GetIsAnimate()) {
					state->GetTurnBattleObject(heroTurn, order)->SetAnimationState(ANI_STATE_IDLE);
				}
				state->GetTurnBattleObject(heroTurn, order)->SetPosition(prevPosition);
				state->SetCameraToOrderObject();

				state->SetIsAdditionalNormalAttackTime(false);

				state->SetIsProcessing(false);
				state->SetIsNormalAttackUsed(false);
				

				return BT_SUCCESS;
			}
			return BT_RUNNING;
		}
		else {
			CTurnBattleObject* object;
			if (state->GetIsHeroTurn()) {
				object = state->GetTurnBattleObject(true, state->GetOrder());
			}
			else {
				object = state->GetTurnBattleObject(false, state->GetOrder());
			}

			if (object->GetAnimator()->IsOneCycleEnd() || !object->GetIsAnimate()) {
				// �߰� ���� �ð� �ο�
				state->SetIsAdditionalNormalAttackTime(true);
				m_fAdditionalAttackTime = 0.5f;
				ApplyAttackDamage();
				
			}
			return BT_RUNNING;
		}
	}
}

void CAttackTarget::ApplyAttackDamage()
{
	int order, target;
	short index;
	state->GetOrderSkillTargetAndIndex(&order, &target, &index);
	bool nowTurn = state->GetIsHeroTurn();

	// ������ ����
	TurnBattle::SStatus userStatus = state->GetTurnBattleObject(nowTurn, order)->GetStatus();
	TurnBattle::SBattleVariable userBV = state->GetTurnBattleObject(nowTurn, order)->GetBattleVariable();
	TurnBattle::SStatus targetStatus = state->GetTurnBattleObject(!nowTurn, target)->GetStatus();
	TurnBattle::SBattleVariable targetBV = state->GetTurnBattleObject(!nowTurn, target)->GetBattleVariable();

	wchar_t damageText[MAX_NAME_SIZE];
	glm::vec4 damageTextColor;

	TurnBattle::SBattleVariable finalBV
		= CalculateAttackDamage(state->GetIsMatterAttack(), userStatus, userBV, targetStatus, targetBV, 
			damageText, &damageTextColor);

	state->GetTurnBattleObject(!nowTurn, target)->SetDamageText(damageText);
	state->GetTurnBattleObject(!nowTurn, target)->SetDamageTextColor(damageTextColor);
	state->GetTurnBattleObject(!nowTurn, target)->SetIsShowedDamageText(true);
	state->GetTurnBattleObject(!nowTurn, target)->SetShowingDamageTextTime(DEFAULT_SHOW_DAMAGE_TEXT_TIME);
	state->GetTurnBattleObject(!nowTurn, target)->SetBattleVariable(finalBV);

	if (finalBV.hp <= 0) {
		state->GetTurnBattleObject(!nowTurn, target)->SetAnimationState(ANI_STATE_DEATH);
		state->GetTurnBattleObject(!nowTurn, target)->GetAnimator()->SetIsLoop(false);
	}
}

TurnBattle::SBattleVariable CAttackTarget::CalculateAttackDamage(bool matterAttack,
	TurnBattle::SStatus userStatus, TurnBattle::SBattleVariable userBV,
	TurnBattle::SStatus targetStatus, TurnBattle::SBattleVariable targetBV,
	wchar_t* damageText, glm::vec4* damageTextColor)
{
	// Ȯ�� ���� ���� ����
	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_real_distribution<float> urd(0.0f, 1.0f);

	TurnBattle::SBattleVariable finalBV = targetBV;

	// ���� ������ ��ų ������
	short attackDamage = SKILL_MIN_DAMAGE_BEFORE_APPLY_WEAKNESS;


	bool fail = false;

	float rate = urd(dre);

	// �������� ���� ���߷� ����
	short additionalRateStat = userStatus.stat[ACCURACY] - targetStatus.stat[AVOIDABILITY];
	float additionalRate = additionalRateStat * ADDITIONAL_RATE;

	float attackRate = 0.9f;

	// ���� Ȯ��
	if ((attackRate + additionalRate - (ADDITIONAL_AVOID_LEVEL * targetBV.avoidLevel)) > rate) {
		// ��ų ����� ���
		// ���� ������ ���
		short additionalDamageStat;
		if (matterAttack) {
			additionalDamageStat = userStatus.stat[MATTER_ATTACK] - targetStatus.stat[MATTER_DEFENSE];
		}
		// ���� ������ ���
		else {
			additionalDamageStat = userStatus.stat[MIND_ATTACK] - targetStatus.stat[MIND_DEFENSE];
		}
		// �������� ����� ���
		attackDamage = short(1 * additionalDamageStat);

		// ���� ���� �� �ּ� ������ ���� ���� ��� �ּ� �������� 
		if (attackDamage < 1)
			attackDamage = 1;

		// ���� ����
		short buff = userBV.attackBuff - targetBV.defenseBuff;
		attackDamage = (short)(attackDamage * (float)(1.0f + buff * ADDITIONAL_BUFF));

		// ���� ������ �ּ� ������ ���� ���� ��� �ּ� �������� 
		if (attackDamage < 1)
			attackDamage = 1;

		// ������ �ؽ�Ʈ ����
		if (attackDamage >= 1000) {
			wsprintf(damageText, L"%hd", attackDamage);
		}
		else if (attackDamage >= 100) {
			wsprintf(damageText, L" %hd", attackDamage);
		}
		else if (attackDamage >= 10) {
			wsprintf(damageText, L"  %hd", attackDamage);
		}
		else if (attackDamage >= 1) {
			wsprintf(damageText, L"   %hd", attackDamage);
		}
		*damageTextColor = DAMAGE_FONT_COLOR_NORMAL;
	}
	else {
		attackDamage = 0;

		// ������ �ؽ�Ʈ ����
		wsprintf(damageText, L"MISS");
		*damageTextColor = DAMAGE_FONT_COLOR_NON;
	}

	// ����
	short tempHp = finalBV.hp;
	finalBV.hp -= attackDamage;

	if (finalBV.hp < 0 && tempHp != 1) {
		// hp�� 0���� ������� hp 1 ȸ���� �ٿ���� crash��(�ʹ� ���� ������ still �۵��� ����)
		finalBV.hp = 1;
		finalBV.down = DOWN_LEVEL_CRASH;
	}
	else {
		finalBV.hp = glm::clamp(finalBV.hp, (short)0, targetStatus.maxHp);
	}

	return finalBV;
}

// CCheckIsSkillUsedAndShowSkillName ----------------------------------

BehaviorTreeState CCheckIsSkillUsedAndShowSkillName::Run()
{
	// ��ų�� ������� ���
	if (!state->GetIsProcessing()) {
		if (state->GetIsSkillUsed()) {
			int order;
			int target;
			short index;
			state->GetOrderSkillTargetAndIndex(&order, &target, &index);
			TurnBattle::SSkill* skill = state->GetSkillData(index);

			wchar_t logText[MAX_INFORMATION_SIZE];
			wsprintf(logText, L"��ų %s ���", skill->name);

			state->SetShowLog(true);
			state->SetIsProcessing(true);
			state->SetLogText(logText);
			
			CTurnBattleObject* object;

			if (state->GetIsHeroTurn()) {
				object = state->GetTurnBattleObject(true, state->GetOrder());
			}
			else {
				object = state->GetTurnBattleObject(false, state->GetOrder());
			}
			if (object->GetIsAnimate()) {
				short skillIndex;
				state->GetOrderSkillTargetAndIndex(&order, &target, &skillIndex);
				if (skillIndex <= MAXIUM_GIVE_DAMAGE_SKILL_INDEX) {
					object->SetAnimationState(ANI_STATE_SKILL_1);
					if (object->GetIsSubObject()) {
						if (state->GetOrder() == 0) {
							object->GetParent()->SetAnimationState(ANI_STATE_SKILL_RIGHT);
							object->GetParent()->GetAnimator()->SetIsLoop(false);
						}
						else {
							object->GetParent()->SetAnimationState(ANI_STATE_SKILL_LEFT);
							object->GetParent()->GetAnimator()->SetIsLoop(false);
						}
					}

				}
				else if (skillIndex < SPECIAL_SKILL_INDEX_START) {
					object->SetAnimationState(ANI_STATE_SKILL_3);
				}
				object->GetAnimator()->SetIsLoop(false);
			}

			return BT_SUCCESS;
		}
		else {
			return BT_FAILURE;
		}
	}
	return BT_FAILURE;
}

// CSetSkillData ------------------------------------------------------

BehaviorTreeState CSetSkillData::Run()
{
	if (!m_bSetSkillData) {
		SetUsedSkillData();
		m_fWaitTime = 1.0f;
		m_bSetSkillData = true;
		return BT_RUNNING;
	}
	else {
		if (m_fWaitTime <= 0.0f) {
			m_bSetSkillData = false;
			return BT_SUCCESS;
		}
		m_fWaitTime -= BackBoard()->GetElapsedTimeInSec();
		return BT_RUNNING;
	}
}

void CSetSkillData::SetUsedSkillData()
{
	// ��ų�� Ÿ�� ���� ���
	int order, target;
	short index;
	state->GetOrderSkillTargetAndIndex(&order, &target, &index);
	TurnBattle::SSkill* skill = state->GetSkillData(index);
	bool heroTurn = state->GetIsHeroTurn();

	CSoundPlayer::GetInstance()->PlaySound(state->GetSkillSoundEffect(index), 1.0f);

	// ���� ��Ƽ�� Ÿ������ �ϴ� ��ų�� ��� 
	if (skill->selfParty) {
		state->SetCameraToSelectedObject(heroTurn, order, true);
		heroTurn = (heroTurn + 1) % 2;
		target = order;
	}

	TurnBattle::SUsedSkillData usedSkillData = state->GetUsedSkillData();

	// ��ų�� ���� Ÿ�� ����
	if (std::abs(skill->target) == ONE_TARGET) {
		FillUsedSkillData(usedSkillData, heroTurn, target, index, skill->effectTime, skill->effectData);
	}
	else if (std::abs(skill->target) == ALL_TARGET) {
		int max = MAX_ENEMY_PARTY_MEMBER;
		if (!heroTurn) max = MAX_HERO_PARTY_MEMBER;
		for (int i = 0; i < max; ++i) {
			FillUsedSkillData(usedSkillData, heroTurn, i, index, skill->effectTime, skill->effectData);
		}
	}
	else if (std::abs(skill->target) == ONE_AND_ALL_TARGET) {
		int max = MAX_ENEMY_PARTY_MEMBER;
		if (!heroTurn) max = MAX_HERO_PARTY_MEMBER;
		// Ư�� ȿ�� ��ų
		TurnBattle::SSkill* effectSkill = state->GetSkillData(index + SPECIAL_SKILL_INDEX_START);
		
		for (int i = 0; i < max; ++i) {
			if (index < 70) {
				if (i == target) {
					FillUsedSkillData(usedSkillData, heroTurn, i, index, skill->effectTime, skill->effectData);
				}
				else {
					FillUsedSkillData(usedSkillData, heroTurn, i, 
						index + SPECIAL_SKILL_INDEX_START, effectSkill->effectTime, effectSkill->effectData);
				}
			}
			else {
				if (i == target) {
					FillUsedSkillData(usedSkillData, heroTurn, i, 
						index + SPECIAL_SKILL_INDEX_START, effectSkill->effectTime, effectSkill->effectData);
				}
				else {
					FillUsedSkillData(usedSkillData, heroTurn, i, index, skill->effectTime, skill->effectData);
				}
			}
		}
	}
	else if (std::abs(skill->target) == OTHER_TARGET) {
		int max = MAX_ENEMY_PARTY_MEMBER;
		if (!heroTurn) max = MAX_HERO_PARTY_MEMBER;
		for (int i = 0; i < max; ++i) {
			if (i != target) {
				FillUsedSkillData(usedSkillData, heroTurn, i, index, skill->effectTime, skill->effectData);
			}
		}
	}

	state->SetUsedSkillData(usedSkillData);
}

void CSetSkillData::FillUsedSkillData(TurnBattle::SUsedSkillData& usedSkillData, 
	bool heroTurn, int index, short skillIndex, float effectTime, int effectData)
{
	if (heroTurn) {
		if (usedSkillData.enemyParty[index].exisited) {
			usedSkillData.enemyParty[index].targeted = true;
			usedSkillData.enemyParty[index].IsShowedHpbar = true;
			usedSkillData.enemyParty[index].skillIndex = skillIndex;
			usedSkillData.enemyParty[index].skillEffectTime = effectTime;
			usedSkillData.enemyParty[index].skillEffectData = effectData;
		}
	}
	else {
		if (usedSkillData.heroParty[index].exisited) {
			usedSkillData.heroParty[index].targeted = true;
			usedSkillData.heroParty[index].IsShowedHpbar = true;
			usedSkillData.heroParty[index].skillIndex = skillIndex;
			usedSkillData.heroParty[index].skillEffectTime = effectTime;
			usedSkillData.heroParty[index].skillEffectData = effectData;
		}
	}
}

// CGiveSkillDamage ----------------------------------------------

BehaviorTreeState CGiveSkillDamage::Run()
{
	if (!m_bGiveDamageEnd) {
		// ��ų ������ ����
		ApplySkillDamage();
		// Ÿ���� ����
		TargetingEndInUsedSkillData();

		m_bGiveDamageEnd = true;
		m_fWaitTime = 0.75f;

		return BT_RUNNING;
	}
	else {
		if (m_fWaitTime < 0.0f) {
			// hp�� ǥ�� ����
			ShowHpBarEndInUsedSkillData();
			m_bGiveDamageEnd = false;
			state->SetIsProcessing(false);
			state->SetIsSkillUsed(false);

			CTurnBattleObject* object;
			if (state->GetIsHeroTurn()) {
				object = state->GetTurnBattleObject(true, state->GetOrder());
			}
			else {
				object = state->GetTurnBattleObject(false, state->GetOrder());
			}
			if (object->GetIsAnimate()) {
				object->SetAnimationState(ANI_STATE_IDLE);
			}

			return BT_SUCCESS;
		}

		m_fWaitTime -= BackBoard()->GetElapsedTimeInSec();
		return BT_RUNNING;
	}
}

void CGiveSkillDamage::ApplySkillDamage()
{
	TurnBattle::SUsedSkillData usedSkillData = state->GetUsedSkillData();

	int userIndex = state->GetOrder();
	bool heroTurn = state->GetIsHeroTurn();
	TurnBattle::SStatus userStatus = state->GetTurnBattleObject(heroTurn, userIndex)->GetStatus();
	TurnBattle::SBattleVariable userBV = state->GetTurnBattleObject(heroTurn, userIndex)->GetBattleVariable();

	for (int i = 0; i < MAX_ENEMY_PARTY_MEMBER; ++i) {
		if (usedSkillData.enemyParty[i].exisited) {
			if (usedSkillData.enemyParty[i].targeted) {
				short skillIndex = usedSkillData.enemyParty[i].skillIndex;
				TurnBattle::SSkill* skill = state->GetSkillData(skillIndex);
				TurnBattle::SStatus targetStatus = state->GetTurnBattleObject(false, i)->GetStatus();
				TurnBattle::SBattleVariable targetBV = 
					state->GetTurnBattleObject(false, i)->GetBattleVariable();

				wchar_t damageText[MAX_NAME_SIZE];
				glm::vec4 damageTextColor;
				TurnBattle::SBattleVariable finalBV;
				if (skillIndex <= MAXIUM_GIVE_DAMAGE_SKILL_INDEX) {
					finalBV = CalculateSkillDamage(skill, userStatus, userBV, targetStatus, targetBV,
						damageText, &damageTextColor);
				}
				else if (skillIndex < SPECIAL_SKILL_INDEX_START){
					finalBV = CalculateSkillEffect(skill, userStatus, userBV, targetStatus, targetBV,
						damageText, &damageTextColor);
				}
				else {
					finalBV = CalculateSkillSpecial(skill, userStatus, userBV, targetStatus, targetBV,
						damageText, &damageTextColor);
				}

				state->GetTurnBattleObject(false, i)->SetDamageText(damageText);
				state->GetTurnBattleObject(false, i)->SetDamageTextColor(damageTextColor);
				state->GetTurnBattleObject(false, i)->SetIsShowedDamageText(true);
				state->GetTurnBattleObject(false, i)->SetShowingDamageTextTime(DEFAULT_SHOW_DAMAGE_TEXT_TIME);
				state->GetTurnBattleObject(false, i)->SetBattleVariable(finalBV);

				if (finalBV.hp <= 0) {
					state->GetTurnBattleObject(false, i)->SetAnimationState(ANI_STATE_DEATH);
					state->GetTurnBattleObject(false, i)->GetAnimator()->SetIsLoop(false);
				}
			}
		}
	}
	for (int i = 0; i < MAX_HERO_PARTY_MEMBER; ++i) {
		if (usedSkillData.heroParty[i].exisited) {
			if (usedSkillData.heroParty[i].targeted) {
				short skillIndex = usedSkillData.heroParty[i].skillIndex;
				TurnBattle::SSkill* skill = state->GetSkillData(skillIndex);
				TurnBattle::SStatus targetStatus = state->GetTurnBattleObject(true, i)->GetStatus();
				TurnBattle::SBattleVariable targetBV =
					state->GetTurnBattleObject(true, i)->GetBattleVariable();
				
				wchar_t damageText[MAX_NAME_SIZE];
				glm::vec4 damageTextColor;

				TurnBattle::SBattleVariable finalBV;
				if (skillIndex <= MAXIUM_GIVE_DAMAGE_SKILL_INDEX) {
					finalBV = CalculateSkillDamage(skill, userStatus, userBV, targetStatus, targetBV,
						damageText, &damageTextColor);
				}
				else if (skillIndex < SPECIAL_SKILL_INDEX_START) {
					finalBV = CalculateSkillEffect(skill, userStatus, userBV, targetStatus, targetBV,
						damageText, &damageTextColor);
				}
				else {
					finalBV = CalculateSkillSpecial(skill, userStatus, userBV, targetStatus, targetBV,
						damageText, &damageTextColor);
				}
				state->GetTurnBattleObject(true, i)->SetDamageText(damageText);
				state->GetTurnBattleObject(true, i)->SetDamageTextColor(damageTextColor);
				state->GetTurnBattleObject(true, i)->SetIsShowedDamageText(true);
				state->GetTurnBattleObject(true, i)->SetShowingDamageTextTime(DEFAULT_SHOW_DAMAGE_TEXT_TIME);
				state->GetTurnBattleObject(true, i)->SetBattleVariable(finalBV);

				if (finalBV.hp <= 0) {
					state->GetTurnBattleObject(true, i)->SetAnimationState(ANI_STATE_DEATH);
					state->GetTurnBattleObject(true, i)->GetAnimator()->SetIsLoop(false);
				}
			}
		}
	}

	state->SetUsedSkillData(usedSkillData);
}

TurnBattle::SBattleVariable CGiveSkillDamage::CalculateSkillDamage(TurnBattle::SSkill* skill,
	TurnBattle::SStatus userStatus, TurnBattle::SBattleVariable userBV,
	TurnBattle::SStatus targetStatus, TurnBattle::SBattleVariable targetBV,
	wchar_t* damageText, glm::vec4* damageTextColor)
{
	// Ȯ�� ���� ���� ����
	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_real_distribution<float> urd(0.0f, 1.0f);

	TurnBattle::SBattleVariable finalBV = targetBV;

	// ���� ������ ��ų ������
	short skillDamage = SKILL_MIN_DAMAGE_BEFORE_APPLY_WEAKNESS;
	short downLevel = targetBV.down;
	
	short skillAttribute = skill->attribute;
	// ���ɼӼ��� �ƴ� ��� Ÿ���� �Ӽ� ���� ���� �̿�
	if (skillAttribute != UNIVERSAL) {
		short targetAttributeWeakness = targetStatus.attribute[skillAttribute];
		
		bool fail = false;
		if (skillAttribute <= EVIL) {
			// ������ ��ȿ�� �ƴ� ��� ����� ���
			if (targetAttributeWeakness != NON) {
				// ��ų�� �����ߴ��� ���
				float rate = urd(dre);
				
				// �������� ���� ���߷� ����
				short additionalRateStat = userStatus.stat[ACCURACY] - targetStatus.stat[AVOIDABILITY];
				float additionalRate = additionalRateStat * ADDITIONAL_RATE;
				
				float skillRate = skill->rate;

				// ���� Ȯ��
				if ((skillRate + additionalRate - (ADDITIONAL_AVOID_LEVEL * targetBV.avoidLevel)) > rate) {
					// ��ų ����� ���
					// ���� ������ ���
					short additionalDamageStat;
					if (skillAttribute <= WIND) {
						additionalDamageStat = userStatus.stat[MATTER_ATTACK] - targetStatus.stat[MATTER_DEFENSE];
					}
					// ���� ������ ���
					else {
						additionalDamageStat = userStatus.stat[MIND_ATTACK] - targetStatus.stat[MIND_DEFENSE];
					}
					// �������� ����� ���
					std::uniform_int_distribution<short> uid(skill->minDamage, skill->maxDamage);
					skillDamage = uid(dre);
					skillDamage = short(skillDamage * (1.0f + ADDITIONAL_DAMAGE * additionalDamageStat));

					// ���� ���� �� �ּ� ������ ���� ���� ��� �ּ� �������� 
					if (skillDamage < SKILL_MIN_DAMAGE_BEFORE_APPLY_WEAKNESS)
						skillDamage = SKILL_MIN_DAMAGE_BEFORE_APPLY_WEAKNESS;

					// ���� ����
					short buff = userBV.attackBuff - targetBV.defenseBuff;
					skillDamage = (short)(skillDamage * (float)(1.0f + buff * ADDITIONAL_BUFF));

					// ���� ������ �ּ� ������ ���� ���� ��� �ּ� �������� 
					if (skillDamage < SKILL_MIN_DAMAGE_BEFORE_APPLY_WEAKNESS)
						skillDamage = SKILL_MIN_DAMAGE_BEFORE_APPLY_WEAKNESS;
				}
				else {
					fail = true;
					skillDamage = 0;
					*damageTextColor = DAMAGE_FONT_COLOR_NON;
					wsprintf(damageText, L"MISS");
				}
			}
			else {
				wsprintf(damageText, L"NON");
			}

			// ���� ���н� �ѱ�
			if (!fail) {
				// ���� ����
				if (targetAttributeWeakness == NON) {
					skillDamage = short(skillDamage * 0.0f);
					*damageTextColor = DAMAGE_FONT_COLOR_NON;
				}
				else if (targetAttributeWeakness == HALF) {
					skillDamage = short(skillDamage * 0.5f);
					*damageTextColor = DAMAGE_FONT_COLOR_HALF;
				}
				else if (targetAttributeWeakness == NORMAL) {
					skillDamage = short(skillDamage * 1.0f);
					*damageTextColor = DAMAGE_FONT_COLOR_NORMAL;
				}
				else if (targetAttributeWeakness == WEAK) {
					skillDamage = short(skillDamage * 1.5f);
					*damageTextColor = DAMAGE_FONT_COLOR_WEAK;
					if (downLevel == DOWN_LEVEL_DOWN1) downLevel = DOWN_LEVEL_DOWN2;
					else  downLevel = DOWN_LEVEL_DOWN1;
				}
				else if (targetAttributeWeakness == AWFUL) {
					skillDamage = short(skillDamage * 2.0f);
					*damageTextColor = DAMAGE_FONT_COLOR_AWFUL;
					downLevel = DOWN_LEVEL_FALL;
				}
			}
		}
		// ��, ��� �Ӽ��� ���� ������� ���� ���
		else {
			// ȿ�� Ȯ�� ���
			float effectRate = urd(dre);

			float skillEffectRate = skill->effectRate;

			// ���� ����
			if (targetAttributeWeakness == NON) {
				*damageTextColor = DAMAGE_FONT_COLOR_NORMAL;
				skillEffectRate = 0.0f;
			}
			else if (targetAttributeWeakness == HALF) {
				*damageTextColor = DAMAGE_FONT_COLOR_NORMAL;
				skillEffectRate = skillEffectRate * 0.5f;
			}
			else if (targetAttributeWeakness == NORMAL) {
				*damageTextColor = DAMAGE_FONT_COLOR_NORMAL;
				skillEffectRate = skillEffectRate;
			}
			else if (targetAttributeWeakness == WEAK) {
				skillEffectRate = skillEffectRate + 0.25f;
				*damageTextColor = DAMAGE_FONT_COLOR_WEAK;
				if (downLevel == DOWN_LEVEL_DOWN1) downLevel = DOWN_LEVEL_DOWN2;
				else  downLevel = DOWN_LEVEL_DOWN1;
			}
			else if (targetAttributeWeakness == AWFUL) {
				skillEffectRate = skillEffectRate + 0.5f;
				*damageTextColor = DAMAGE_FONT_COLOR_AWFUL;
				downLevel = DOWN_LEVEL_FALL;
			}

			skillDamage = skill->minDamage;
			if (skillEffectRate > effectRate) {
				if (skillAttribute == LIGHT) {
					skillDamage = (short)(skillDamage * 3.0f);
				}
				else if (skillAttribute == DARK) {
					skillDamage = (short)(skillDamage * 4.0f);
				}
			}
		}
	}
	else {
		// ���� �Ӽ� ���
	}

	// ��ų Ư�� ȿ�� ó��
	if (skill->effect != NO_SKILL_EFFECT) {
		if (finalBV.condition == NORMAL_CONDITION) {
			float effectRate = urd(dre);
			float skillEffectRate = skill->effectRate;
			if (effectRate <= skillEffectRate) {
				if (skill->effect == BURN_SKILL_EFFECT) {
					finalBV.condition = BURN_CONDITION;
					finalBV.conditionDuration = DEFAULT_CONDITION_RECOVER_TIME;
				}
				else if (skill->effect == DROWNING_SKILL_EFFECT) {
					finalBV.condition = BURN_CONDITION;
					finalBV.conditionDuration = DEFAULT_CONDITION_RECOVER_TIME;
				}
				else if (skill->effect == DROWNING_SKILL_EFFECT) {
					skillDamage += skill->minDamage;
					downLevel = DOWN_LEVEL_CRASH;
				}
			}
		}
	}

	// ����
	if (skillDamage >= 1000) {
		wsprintf(damageText, L"%hd", skillDamage);
	}
	else if (skillDamage >= 100) {
		wsprintf(damageText, L" %hd", skillDamage);
	}
	else if (skillDamage >= 10) {
		wsprintf(damageText, L"  %hd", skillDamage);
	}
	else if (skillDamage >= 1) {
		wsprintf(damageText, L"   %hd", skillDamage);
	}
	short tempHp = finalBV.hp;
	finalBV.hp -= skillDamage;

	if (finalBV.hp < 0 && tempHp != 1) {
		// hp�� 0���� ������� hp 1 ȸ���� �ٿ���� crash��(�ʹ� ���� ������ still �۵��� ����)
		finalBV.hp = 1;
		finalBV.down = DOWN_LEVEL_CRASH;
	}
	else {
		finalBV.hp = glm::clamp(finalBV.hp, (short)0, targetStatus.maxHp);
		finalBV.down = downLevel;
	}

	return finalBV;
}

TurnBattle::SBattleVariable CGiveSkillDamage::CalculateSkillEffect(TurnBattle::SSkill* skill, 
	TurnBattle::SStatus userStatus, TurnBattle::SBattleVariable userBV, 
	TurnBattle::SStatus targetStatus, TurnBattle::SBattleVariable targetBV, 
	wchar_t* damageText, glm::vec4* damageTextColor)
{
	TurnBattle::SBattleVariable finalBV = targetBV;

	// maxDamage�� ��ų ȿ�� ������ ���

	if (RUIN <= skill->attribute && skill->attribute <= EVIL) {
		bool alreadyMax;

		finalBV = CalculateBuffDeBuffSkill(skill, finalBV, &alreadyMax);

		short skillDamage = skill->minDamage;
		if (skill->maxDamage == ATTACK_BUFF) {
			wsprintf(damageText, L" +%hd", skillDamage);
			*damageTextColor = DAMAGE_FONT_COLOR_ATTACK_BUF;
		}
		else if (skill->maxDamage == DEFENSE_BUFF) {
			wsprintf(damageText, L" +%hd", skillDamage);
			*damageTextColor = DAMAGE_FONT_COLOR_DEFENSE_BUF;
		}
		else if (skill->maxDamage == ATTACK_DEFENSE_BUFF) {
			wsprintf(damageText, L"+%hd +%hd", skillDamage, skillDamage);
			*damageTextColor = DAMAGE_FONT_COLOR_ATTACK_DEFENSE_BUF;
		}
		else if (skill->maxDamage == ATTACK_DEBUFF) {
			wsprintf(damageText, L" -%hd", skillDamage);
			*damageTextColor = DAMAGE_FONT_COLOR_ATTACK_BUF;
		}
		else if (skill->maxDamage == DEFENSE_DEBUFF) {
			wsprintf(damageText, L" -%hd", skillDamage);
			*damageTextColor = DAMAGE_FONT_COLOR_DEFENSE_BUF;
		}
		else if (skill->maxDamage == ATTACK_DEFENSE_DEBUFF) {
			wsprintf(damageText, L"-%hd -%hd", skillDamage, skillDamage);
			*damageTextColor = DAMAGE_FONT_COLOR_ATTACK_DEFENSE_BUF;
		}
		else if (skill->maxDamage == REMOVE_BUFF_DEBUFF) {
			wsprintf(damageText, L"ZERO");
			*damageTextColor = DAMAGE_FONT_COLOR_REMOVE_BUF;
		}

		if (alreadyMax) {
			wsprintf(damageText, L" +-0");
		}
	}
	else if (skill->attribute == UNIVERSAL) {
		finalBV = CalculateRecoverSkill(skill, targetStatus, finalBV);

		if (skill->maxDamage == RECOVER_HP) {
			short skillDamage = skill->minDamage;
			if (skillDamage >= 1000) {
				wsprintf(damageText, L"%hd", skillDamage);
			}
			else if (skillDamage >= 100) {
				wsprintf(damageText, L" %hd", skillDamage);
			}
			else if (skillDamage >= 10) {
				wsprintf(damageText, L"  %hd", skillDamage);
			}
			else if (skillDamage >= 1) {
				wsprintf(damageText, L"   %hd", skillDamage);
			}
			*damageTextColor = DAMAGE_FONT_COLOR_RECOVERY;
		}
	}

	return finalBV;
}

TurnBattle::SBattleVariable CGiveSkillDamage::CalculateBuffDeBuffSkill(
	TurnBattle::SSkill* skill, TurnBattle::SBattleVariable targetBV, bool* alreadyMax)
{
	*alreadyMax = false;

	if (skill->maxDamage == ATTACK_BUFF) {
		if (targetBV.attackBuff == MAX_BUFF_SIZE) {
			*alreadyMax = true;
		}
		else {
			targetBV.attackBuff += skill->minDamage;
		}
	}
	else if (skill->maxDamage == DEFENSE_BUFF) {
		if (targetBV.defenseBuff == MAX_BUFF_SIZE) {
			*alreadyMax = true;
		}
		else {
			targetBV.defenseBuff += skill->minDamage;
		}
	}
	else if (skill->maxDamage == ATTACK_DEFENSE_BUFF) {
		if (targetBV.attackBuff == MAX_BUFF_SIZE && targetBV.defenseBuff == MAX_BUFF_SIZE) {
			*alreadyMax = true;
		}
		else {
			targetBV.attackBuff += skill->minDamage;
			targetBV.defenseBuff += skill->minDamage;
		}
	}
	else if (skill->maxDamage == ATTACK_DEBUFF) {
		if (targetBV.attackBuff == -MAX_BUFF_SIZE) {
			*alreadyMax = true;
		}
		else {
			targetBV.attackBuff -= skill->minDamage;
		}
	}
	else if (skill->maxDamage == DEFENSE_DEBUFF) {
		if (targetBV.defenseBuff == -MAX_BUFF_SIZE) {
			*alreadyMax = true;
		}
		else {
			targetBV.defenseBuff -= skill->minDamage;
		}
	}
	else if (skill->maxDamage == ATTACK_DEFENSE_DEBUFF) {
		if (targetBV.attackBuff == -MAX_BUFF_SIZE && targetBV.defenseBuff == -MAX_BUFF_SIZE) {
			*alreadyMax = true;
		}
		else {
			targetBV.attackBuff -= skill->minDamage;
			targetBV.defenseBuff -= skill->minDamage;
		}
	}
	else if (skill->maxDamage == REMOVE_BUFF_DEBUFF) {
		targetBV.attackBuff = skill->minDamage;
		targetBV.defenseBuff = skill->minDamage;
	}

	targetBV.attackBuff = glm::clamp(targetBV.attackBuff, (short)-MAX_BUFF_SIZE, (short)MAX_BUFF_SIZE);
	targetBV.defenseBuff = glm::clamp(targetBV.defenseBuff, (short)-MAX_BUFF_SIZE, (short)MAX_BUFF_SIZE);

	return targetBV;
}

TurnBattle::SBattleVariable CGiveSkillDamage::CalculateRecoverSkill(TurnBattle::SSkill* skill, 
	TurnBattle::SStatus targetStatus, TurnBattle::SBattleVariable targetBV)
{
	targetBV.hp += skill->minDamage;
	targetBV.hp = glm::clamp(targetBV.hp, (short)0, targetStatus.maxHp);

	return targetBV;
}

TurnBattle::SBattleVariable CGiveSkillDamage::CalculateSkillSpecial(TurnBattle::SSkill* skill, 
	TurnBattle::SStatus userStatus, TurnBattle::SBattleVariable userBV, 
	TurnBattle::SStatus targetStatus, TurnBattle::SBattleVariable targetBV, 
	wchar_t* damageText, glm::vec4* damageTextColor)
{
	TurnBattle::SBattleVariable finalBV = targetBV;

	if (GOOD <= skill->attribute && skill->attribute <= EVIL) {
		finalBV = CalculateDecreaseSpSkill(skill, targetBV, damageText, damageTextColor);
	}

	return finalBV;
}

TurnBattle::SBattleVariable CGiveSkillDamage::CalculateDecreaseSpSkill(TurnBattle::SSkill* skill, 
	TurnBattle::SBattleVariable targetBV, 
	wchar_t* damageText, glm::vec4* damageTextColor)
{
	// Ȯ�� ���� ���� ����
	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_real_distribution<float> urd(0.0f, 1.0f);

	if (urd(dre) <= skill->rate) {

		std::uniform_int_distribution<short> uid(skill->minDamage, skill->maxDamage);

		short spAmount = uid(dre);
		targetBV.sp -= spAmount;
		if (spAmount >= 1000) {
			wsprintf(damageText, L"%hd", spAmount);
		}
		else if (spAmount >= 100) {
			wsprintf(damageText, L" %hd", spAmount);
		}
		else if (spAmount >= 10) {
			wsprintf(damageText, L"  %hd", spAmount);
		}
		else if (spAmount >= 1) {
			wsprintf(damageText, L"   %hd", spAmount);
		}

		if (targetBV.sp <= 0) {
			targetBV.sp = 0;
			targetBV.down = DOWN_LEVEL_CRASH;
		}
		*damageTextColor = DAMAGE_FONT_COLOR_SP;
	}
	else {
		*damageTextColor = DAMAGE_FONT_COLOR_NON;
		wsprintf(damageText, L"MISS");
	}

	return targetBV;
}

void CGiveSkillDamage::TargetingEndInUsedSkillData()
{
	TurnBattle::SUsedSkillData usedSkillData = state->GetUsedSkillData();

	for (int i = 0; i < MAX_ENEMY_PARTY_MEMBER; ++i) {
		if (usedSkillData.enemyParty[i].exisited) {
			usedSkillData.enemyParty[i].targeted = false;
		}
	}
	for (int i = 0; i < MAX_HERO_PARTY_MEMBER; ++i) {
		if (usedSkillData.heroParty[i].exisited) {
			usedSkillData.heroParty[i].targeted = false;
		}
	}

	state->SetUsedSkillData(usedSkillData);
}

void CGiveSkillDamage::ShowHpBarEndInUsedSkillData()
{
	TurnBattle::SUsedSkillData usedSkillData = state->GetUsedSkillData();
	bool heroTurn = state->GetIsHeroTurn();

	for (int i = 0; i < MAX_ENEMY_PARTY_MEMBER; ++i) {
		if (usedSkillData.enemyParty[i].exisited) {
			usedSkillData.enemyParty[i].IsShowedHpbar = false;
		}
	}
	for (int i = 0; i < MAX_HERO_PARTY_MEMBER; ++i) {
		if (usedSkillData.heroParty[i].exisited) {
			usedSkillData.heroParty[i].IsShowedHpbar = false;
		}
	}

	state->SetUsedSkillData(usedSkillData);
}

// CCheckSummonedAutomaAndShowLog --------------------------------

BehaviorTreeState CCheckSummonedAutomaAndShowLog::Run()
{
	// ���丶Ÿ�� ��ȯ ���� ���
	if (!state->GetIsProcessing()) {
		if (state->GetIsAutomataSummoned()) {
			short index;
			int location;
			state->GetSummonedAutomataInformation(&index, &location);
			TurnBattle::SAutomata* automata = state->GetAutomataData(index);

			wchar_t logText[MAX_INFORMATION_SIZE];
			wsprintf(logText, L"���丶Ÿ %s ��ȯ", automata->name);

			state->SetShowLog(true);
			state->SetIsProcessing(true);
			state->SetLogText(logText);

			if (state->GetTurnBattleObject(true, state->GetOrder())->GetIsAnimate()) {
				state->GetTurnBattleObject(true, state->GetOrder())->SetAnimationState(ANI_STATE_SKILL_1);
				state->GetTurnBattleObject(true, state->GetOrder())->GetAnimator()->SetIsLoop(false);
			}
			return BT_SUCCESS;
		}
		else {
			return BT_FAILURE;
		}
	}
	return BT_FAILURE;
}

// CSetCameraAndVisibility --------------------------------

BehaviorTreeState CSetCameraAndVisibility::Run()
{
	if (!m_bSetEnd) {
		short index;
		int location;
		state->GetSummonedAutomataInformation(&index, &location);
		m_iSummonedAutomata = location + 1;
		m_fShowTime = 1.0f;
		m_bSetVisibilityEnd = false;
		m_bSetEnd = true;
		state->SetCameraToSelectedObject(true, m_iSummonedAutomata, true);
		return BT_RUNNING;
	}
	else {
		if (m_fShowTime < 0.0f) {
			state->SetIsAutomataSummoned(false);
			state->SetIsProcessing(false);
			m_bSetEnd = false;
			return BT_SUCCESS;
		}
		else if (!m_bSetVisibilityEnd && m_fShowTime < 0.5f) {
			// ������ ��ȯ�� ���丶Ÿ�� ���̵��� ����
			m_bSetVisibilityEnd = true;
			state->GetTurnBattleObject(true, m_iSummonedAutomata)->SetWhetherToRender(true);

			if (state->GetTurnBattleObject(true, state->GetOrder())->GetIsAnimate()) {
				state->GetTurnBattleObject(true, state->GetOrder())->SetAnimationState(ANI_STATE_IDLE);
			}
		}

		m_fShowTime -= BackBoard()->GetElapsedTimeInSec();
		return BT_RUNNING;
	}
}

// CCheckChangeSkillDeckAndShowLog --------------------------------

BehaviorTreeState CCheckChangeSkillDeckAndShowLog::Run()
{
	// ��ų ���� ���� ���� ���
	if (!state->GetIsProcessing()) {
		if (state->GetIsSkillDeckChanged()) {
			short index;
			int location;
			state->GetChangedSkillDeckInformation(&index, &location);
			TurnBattle::SDeck* deck = state->GetSkillDeckData(index);

			wchar_t logText[MAX_INFORMATION_SIZE];
			wsprintf(logText, L"��ų �� %s ����", deck->name);

			state->SetShowLog(true);
			state->SetIsProcessing(true);
			state->SetLogText(logText);
			return BT_SUCCESS;
		}
		else {
			return BT_FAILURE;
		}
	}
	return BT_FAILURE;
}

// CSetSelfCameraAndWait --------------------------------

BehaviorTreeState CSetSelfCameraAndWait::Run()
{
	if (!m_bSetEnd) {
		state->SetCameraToSelectedObject(true, 0, true);
		// ��ų �� ���� �ִϸ��̼� �۵�
		state->SetIsChangedSkillDeckAnimationEnd(false);
		m_bSetEnd = true;
		return BT_RUNNING;
	}
	else {
		if (state->GetIsChangedSkillDeckAnimationEnd()) {
			state->SetIsSkillDeckChanged(false);
			state->SetIsProcessing(false);
			state->SetMenu(MAIN_MENU);
			m_bSetEnd = false;
			return BT_SUCCESS;
		}
		return BT_RUNNING;
	}
}

// CCheckWaitAndShowLog --------------------------------

BehaviorTreeState CCheckWaitAndShowLog::Run()
{
	// ��⸦ ����������
	if (!state->GetIsProcessing()) {
		if (state->GetIsWaited()) {
			bool rest = state->GetIsRested();
			bool heroTurn = state->GetIsHeroTurn();
			int order = state->GetOrder();

			short turn = state->GetTurnBattleObject(heroTurn, order)->GetBattleVariable().turn;
			wchar_t logText[MAX_INFORMATION_SIZE];
			if (rest) {
				wsprintf(logText, L"�޽�: hp, sp %hd ����", turn);
			}
			else {
				wsprintf(logText, L"���: ȸ���� %hd%% ����", turn);
			}
			
			if (state->GetTurnBattleObject(heroTurn, order)->GetIsAnimate()) {
				state->GetTurnBattleObject(heroTurn, order)->SetAnimationState(ANI_STATE_IDLE_LONG);
				state->GetTurnBattleObject(heroTurn, order)->GetAnimator()->SetSpeed(0.2f);
			}

			state->SetShowLog(true);
			state->SetIsProcessing(true);
			state->SetLogText(logText);
			return BT_SUCCESS;
		}
		else {
			return BT_FAILURE;
		}
	}
	return BT_FAILURE;
}

// CWaitSomeTime ----------------------------------------

BehaviorTreeState CWaitSomeTime::Run()
{
	if (!m_bSetEnd) {
		m_fWaitTime = 1.0f;
		m_bSetEnd = true;
		return BT_RUNNING;
	}
	else {
		if (m_fWaitTime < 0.0f) {
			//�� ����
			bool heroTurn = state->GetIsHeroTurn();
			int order = state->GetOrder();
			TurnBattle::SBattleVariable bv = state->GetTurnBattleObject(heroTurn, order)->GetBattleVariable();
			bv.turn = 0;
			state->GetTurnBattleObject(heroTurn, order)->SetBattleVariable(bv);

			state->SetIsWaited(false);
			state->SetIsProcessing(false);
			m_bSetEnd = false;

			if (state->GetTurnBattleObject(heroTurn, order)->GetIsAnimate()) {
				state->GetTurnBattleObject(heroTurn, order)->SetAnimationState(ANI_STATE_IDLE);
			}

			return BT_SUCCESS;
		}

		m_fWaitTime -= BackBoard()->GetElapsedTimeInSec();
		return BT_RUNNING;
	}
}

// CCheckStealAndShowLog --------------------------------

BehaviorTreeState CCheckStealAndShowLog::Run()
{
	// ��ƿ�� ����������
	if (!state->GetIsProcessing()) {
		if (state->GetIsStolen()) {
			bool stolenAutomata = state->GetIsStolenAutomata();
			int order, target;
			short temp;
			state->GetOrderSkillTargetAndIndex(&order, &target, &temp);

			short downLevel = state->GetTurnBattleObject(false, target)->GetBattleVariable().down;
			wchar_t logText[MAX_INFORMATION_SIZE];

			if (state->GetTurnBattleObject(true, order)->GetIsAnimate()) {
				state->GetTurnBattleObject(true, order)->SetAnimationState(ANI_STATE_SKILL_2);
				state->GetTurnBattleObject(true, order)->GetAnimator()->SetIsLoop(false);
			}

			// �ٿ� ������ 2���� ���� ��� ���� ���ɼ� ����
			if (downLevel >= DOWN_LEVEL_DOWN2) {
				short automataBeingStolen, skillDeckBingStolen;
				state->GetTurnBattleObject(false, target)->GetIndexBeingStolen(&automataBeingStolen, &skillDeckBingStolen);

				if (stolenAutomata) {
					bool haveA = false;
					int automataCount = (int)state->GetTurnBattleObject(true, order)->GetAutomataVector().size();
					for (int i = 0; i < automataCount; ++i) {
						if (state->GetTurnBattleObject(true, order)->GetAutomataVector().at(i) == automataBeingStolen) {
							haveA = true;
							break;
						}
					}
					if (haveA) {
						wsprintf(logText, L"����: �����ϴ� ���丶Ÿ");
					}
					else {
						wsprintf(logText, L"%s ��ƿ", state->GetAutomataData(automataBeingStolen)->name);
						state->GetTurnBattleObject(true, order)->GiveAutomata(automataBeingStolen);
					}
				}	
				else {
					bool haveD = false;
					int deckCount = (int)state->GetTurnBattleObject(true, order)->GetDeckVector().size();
					if (deckCount == 5) {
						wsprintf(logText, L"����: �� �ڸ� ����");
					}
					else {
						for (int i = 0; i < deckCount; ++i) {
							if (state->GetTurnBattleObject(true, order)->GetDeckVector().at(i) == skillDeckBingStolen) {
								haveD = true;
								break;
							}
						}
						if (haveD) {
							wsprintf(logText, L"����: �����ϴ� ��ų ��");
						}
						else {
							wsprintf(logText, L"%s ��ƿ", state->GetSkillDeckData(skillDeckBingStolen)->name);
							state->GetTurnBattleObject(true, order)->GiveDeck(skillDeckBingStolen);
						}
					}	
				}
			}
			else {
				wsprintf(logText, L"��ƿ ����: �� �ٿ� ���� ����");
			}

			state->SetShowLog(true);
			state->SetIsProcessing(true);
			state->SetLogText(logText);
			return BT_SUCCESS;
		}
		else {
			return BT_FAILURE;
		}
	}
	return BT_FAILURE;
}

// CWaitAndGotoMainMenu --------------------------------

BehaviorTreeState CWaitAndGotoMainMenu::Run()
{
	if (!m_bSetEnd) {
		m_fWaitTime = 1.0f;
		m_bSetEnd = true;
		return BT_RUNNING;
	}
	else {
		if (m_fWaitTime < 0.0f) {
			// ���� �޴��� �̵�
			state->SetMenuLocation(MAIN_MENU);

			// �� ����
			bool heroTurn = state->GetIsHeroTurn();
			int order = state->GetOrder();
			TurnBattle::SBattleVariable bv = state->GetTurnBattleObject(heroTurn, order)->GetBattleVariable();
			bv.turn--;
			state->GetTurnBattleObject(heroTurn, order)->SetBattleVariable(bv);

			if (state->GetTurnBattleObject(true, order)->GetIsAnimate()) {
				state->GetTurnBattleObject(true, order)->SetAnimationState(ANI_STATE_IDLE);
			}

			state->SetIsStolen(false);
			state->SetIsProcessing(false);
			m_bSetEnd = false;
			return BT_SUCCESS;
		}

		m_fWaitTime -= BackBoard()->GetElapsedTimeInSec();
		return BT_RUNNING;
	}
}

// CCheckRunawayAndShowLog --------------------------------

BehaviorTreeState CCheckRunawayAndShowLog::Run()
{
	// ���ָ� ����������
	if (!state->GetIsProcessing()) {
		if (state->GetIsRunaway()) {
			std::random_device rd;
			std::default_random_engine dre(rd());
			std::uniform_real_distribution<float> urd(0.0f, 1.0f);

			wchar_t logText[MAX_INFORMATION_SIZE];
			if (urd(dre) <= DEFAULT_RUNAWAY_RATE) {
				wsprintf(logText, L"���� ����!!!");
				state->SetIsRunawaySuccess(true);
			}
			else {
				wsprintf(logText, L"���� ����");
			}
			state->SetShowLog(true);
			state->SetIsProcessing(true);
			state->SetLogText(logText);
			return BT_SUCCESS;
		}
		else {
			return BT_FAILURE;
		}
	}
	return BT_FAILURE;
}

// CWaitSomeTimeForRunaway --------------------------------

BehaviorTreeState CWaitSomeTimeForRunaway::Run()
{
	if (!m_bSetEnd) {
		m_fWaitTime = 1.0f;
		m_bSetEnd = true;
		return BT_RUNNING;
	}
	else {
		if (m_fWaitTime < 0.0f) {
			// ���� �޴��� �̵�
			state->SetMenuLocation(MAIN_MENU);

			// �� ����
			bool heroTurn = state->GetIsHeroTurn();
			int order = state->GetOrder();
			TurnBattle::SBattleVariable bv = state->GetTurnBattleObject(heroTurn, order)->GetBattleVariable();
			bv.turn = 0;
			state->GetTurnBattleObject(heroTurn, order)->SetBattleVariable(bv);

			state->SetIsRunaway(false);
			state->SetIsProcessing(false);
			m_bSetEnd = false;
			return BT_SUCCESS;
		}

		m_fWaitTime -= BackBoard()->GetElapsedTimeInSec();
		return BT_RUNNING;
	}
}

// CCheckIsNotHeroTurn --------------------------------

BehaviorTreeState CCheckIsNotHeroTurn::Run()
{
	if (state->GetIsHeroTurn()) {
		return BT_FAILURE;
	}
	else {
		return BT_SUCCESS;
	}
}

// CSelectEnmeyAction --------------------------------

BehaviorTreeState CSelectEnemyAction::Run()
{
	CTurnBattleObject* object = state->GetTurnBattleObject(false, state->GetOrder());
	TurnBattle::SBattleVariable bv = object->GetBattleVariable();
	short sp = bv.sp;
	short turn = bv.turn;
	short usableSkillList[MAX_SKILL_COUNT];
	int skillSize = 0;

	// ���� ���� ��� �ൿ�� �������� �ʰ� ���� ����
	if (bv.turn <= 0) {
		return BT_FAILURE;
	}

	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_real_distribution<float> urd(0.0f, 1.0f);

	// Ÿ�� ����
	int targetedObject = 0;
	int partySize = state->GetPartySize(true);
	if (partySize != 1) {
		std::uniform_int_distribution<int> uidTarget(0, partySize - 1);
		targetedObject = uidTarget(dre);
	}

	float UseSkilRate = 0.85f;
	float rate = urd(dre);
	if (object->GetIsFixed()) {
		rate = 0.0f;
	}

	int condition = object->GetBattleVariable().condition;

	if (condition == ANXIETY_CONDITION && condition != BURN_CONDITION) {
		state->SetIsNormalAttackUsed(true);
		state->SetIsMatterAttack(true);
		bv.turn -= 1;

		state->SetSelectedSkillAndTarget(0, targetedObject);
		object->SetBattleVariable(bv);

		return BT_FAILURE;
	}

	if (UseSkilRate < rate) {
		state->SetIsNormalAttackUsed(true);
		// �ڿ� �Ҹ� �� Ÿ�� ����
		bv.turn -= 1;
		if (rate > 0.925f) {
			state->SetIsMatterAttack(true);
		}
		else {
			if (bv.sp == 0) {
				state->SetIsMatterAttack(true);
			}
			else {
				state->SetIsMatterAttack(false);
				bv.sp -= 1;
			}
		}
		state->SetSelectedSkillAndTarget(0, targetedObject);
		object->SetBattleVariable(bv);

		return BT_FAILURE;
	}

	// ��� ������ ��ų ��ġ ����
	for (int i = 0; i < (int)object->GetSkillVector().size(); ++i) {
		TurnBattle::SSkill* skill = state->GetSkillData(object->GetSkillVector().at(i));
		if (skill->turn <= turn) {
			if (skill->sp <= sp) {
				usableSkillList[skillSize] = object->GetSkillVector().at(i);
				skillSize++;
			}
		}
	}

	if (skillSize == 0 && object->GetIsSubObject()) {
		bv.turn -= bv.turn;
		object->SetBattleVariable(bv);

		return BT_FAILURE;
	}

	if (skillSize == 0) {
		state->SetIsNormalAttackUsed(true);
		// �ڿ� �Ҹ� �� Ÿ�� ����
		bv.turn -= 1;
		if (rate > 0.40f) {
			state->SetIsMatterAttack(true);
		}
		else {
			if (bv.sp == 0) {
				state->SetIsMatterAttack(true);
			}
			else {
				state->SetIsMatterAttack(false);
				bv.sp -= 1;
			}
		}
		state->SetSelectedSkillAndTarget(0, targetedObject);
		object->SetBattleVariable(bv);

		return BT_FAILURE;
	}
	else {
		int selectedSkill = 0;
		if (skillSize != 1) {
			std::uniform_int_distribution<int> uidSkill(0, skillSize - 1);
			selectedSkill = uidSkill(dre);
		}

		// ��ų �ڿ� �Ҹ�
		bv.turn -= state->GetSkillData(usableSkillList[selectedSkill])->turn;
		bv.sp -= state->GetSkillData(usableSkillList[selectedSkill])->sp;
		object->SetBattleVariable(bv);

		// ���� ��ų, Ÿ��, ��ų ��� ����
		state->SetSelectedSkillAndTarget(usableSkillList[selectedSkill], targetedObject);
		state->SetIsSkillUsed(true);
		return BT_SUCCESS;
	}
}
