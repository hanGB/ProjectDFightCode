#include "stdafx.h"
#include <random>
#include "BossMonster.h"
#include "LandingDustEffectParticle.h"
#include "ShootingBallEffectParticle.h"
#include "BeamParticle.h"

void CBossMonster::BuildBehaviorTree()
{
	CSetBossAttackPattern* setAttackPattern
		= new CSetBossAttackPattern(this, "Set Attack Pattern");

	CPlayBossAttackPattern1* playAttackPattern1 = new CPlayBossAttackPattern1(this, "Play Boss Attack Pattern1");
	CPlayBossAttackPattern2* playAttackPattern2 = new CPlayBossAttackPattern2(this, "Play Boss Attack Pattern2");
	CPlayBossAttackPattern3* playAttackPattern3 = new CPlayBossAttackPattern3(this, "Play Boss Attack Pattern3");
	CPlayBossAttackPattern4* playAttackPattern4 = new CPlayBossAttackPattern4(this, "Play Boss Attack Pattern4");
	CPlayBossAttackPattern5* playAttackPattern5 = new CPlayBossAttackPattern5(this, "Play Boss Attack Pattern5");
	CPlayBossAttackPattern6* playAttackPattern6 = new CPlayBossAttackPattern6(this, "Play Boss Attack Pattern6");
	CPlayBossAttackPattern7* playAttackPattern7 = new CPlayBossAttackPattern7(this, "Play Boss Attack Pattern7");
	CSelectorNode* patternSelector = new CSelectorNode("Boss Select pattern");
	patternSelector->AddChild(playAttackPattern1);
	patternSelector->AddChild(playAttackPattern2);
	patternSelector->AddChild(playAttackPattern3);
	patternSelector->AddChild(playAttackPattern4);
	patternSelector->AddChild(playAttackPattern5);
	patternSelector->AddChild(playAttackPattern6);
	patternSelector->AddChild(playAttackPattern7);

	CSequenceNode* attackSequence = new CSequenceNode("Boss Attack hero");
	attackSequence->AddChild(setAttackPattern);
	attackSequence->AddChild(patternSelector);

	CBossMonsterFallowHero* fallowoHero = new CBossMonsterFallowHero(this, "Boss Fallow Hero");
	CBossChangePhase* bossChangePhase = new CBossChangePhase(this, "Boss Change Phase");

	CSequenceNode* fsequence = new CSequenceNode("Boss Find and Fallow Hero Sequence");
	fsequence->AddChild(fallowoHero);
	fsequence->AddChild(bossChangePhase);
	fsequence->AddChild(attackSequence);

	CSelectorNode* selector = new CSelectorNode("Boss Selector");
	selector->AddChild(fsequence);

	SetBehaviorTree(new CBehaviorTree("Boss object ai", selector));
}

BehaviorTreeState CBossMonsterFallowHero::Run()
{
	if (object->GetIsChangedPhase() 
		|| object->GetAnimationState() == ANI_STATE_HIT
		|| object->GetAnimationState() == ANI_STATE_HIT_LEFT
		|| object->GetAnimationState() == ANI_STATE_HIT_RIGHT) {
		return BT_SUCCESS;
	}

	glm::vec3 position = object->GetPosition();
	glm::vec3 heroPos = BackBoard()->GetHeroPosition();

	float distanceToHero
		= glm::distance(glm::vec3(position.x, 0.0f, position.z), glm::vec3(heroPos.x, 0.0f, heroPos.z));

	if (distanceToHero < 20.f) {
			return BT_SUCCESS;
	}

	if (object->GetIsAnimate()) {
		if (object->GetAnimationState() != ANI_STATE_WALK) {
			object->SetAnimationState(ANI_STATE_WALK);
		}
	}

	glm::vec3 dir = glm::normalize(glm::vec3(heroPos.x, 0.0f, heroPos.z)
		- glm::vec3(position.x, 0.0f, position.z));

	GameUpdateParameters param;
	memset(&param, 0, sizeof(param));
	param.force = glm::vec3(dir.x * 20 * object->GetMass(), 0, dir.z * 20 * object->GetMass());
	object->CGameObject::Update(BackBoard()->GetElapsedTimeInSec(), param);

	return BT_FAILURE;
}

BehaviorTreeState CSetBossAttackPattern::Run()
{
	if (object->GetIsChangedPhase()) {
		m_fWaitTime = 1.0f;
		m_sContinuousAttackTime = 0;
		return BT_FAILURE;
	}

	if (!object->GetIsAttacking()) {
		if (!object->GetIsSuperArmor()) {
			if (object->GetAnimationState() == ANI_STATE_HIT
				|| object->GetAnimationState() == ANI_STATE_HIT_LEFT
				|| object->GetAnimationState() == ANI_STATE_HIT_RIGHT) {
				if (!object->GetAnimator()->IsOneCycleEnd()) {
					return BT_FAILURE;
				}
			}
		}

		glm::vec3 objectPos = object->GetPosition();
		glm::vec3 heroPos = BackBoard()->GetHeroPosition();
		float distanceToHero
			= glm::distance(glm::vec3(objectPos.x, 0.0f, objectPos.z), glm::vec3(heroPos.x, 0.0f, heroPos.z));
		short enablePatterns[9];
		if (object->GetPhase() == 1) {
			enablePatterns[0] = 3; enablePatterns[1] = 3; enablePatterns[2] = 1;
			enablePatterns[3] = 1; enablePatterns[4] = 5; enablePatterns[5] = 5;
			enablePatterns[6] = 2; enablePatterns[7] = 4; enablePatterns[8] = 4;
		}
		else {
			enablePatterns[0] = 6; enablePatterns[1] = 6; enablePatterns[2] = 1;
			enablePatterns[3] = 1; enablePatterns[4] = 1; enablePatterns[5] = 5;
			enablePatterns[6] = 2; enablePatterns[7] = 7; enablePatterns[8] = 7;
		}

		short patterns[6];
		if (distanceToHero < 6.f) {
			patterns[0] = enablePatterns[ATTACK_PATTERN_SHORT_START];
			patterns[1] = enablePatterns[ATTACK_PATTERN_SHORT_START + 1];
			patterns[2] = enablePatterns[ATTACK_PATTERN_SHORT_START + 2];
			patterns[3] = enablePatterns[ATTACK_PATTERN_SHORT_START];
			patterns[4] = enablePatterns[ATTACK_PATTERN_SHORT_START + 1];
			patterns[5] = enablePatterns[ATTACK_PATTERN_SHORT_START + 2];
		}
		else if (distanceToHero > 12.f) {
			patterns[0] = enablePatterns[ATTACK_PATTERN_LONG_START];
			patterns[1] = enablePatterns[ATTACK_PATTERN_LONG_START + 1];
			patterns[2] = enablePatterns[ATTACK_PATTERN_LONG_START + 2];
			patterns[3] = enablePatterns[ATTACK_PATTERN_LONG_START];
			patterns[4] = enablePatterns[ATTACK_PATTERN_LONG_START + 1];
			patterns[5] = enablePatterns[ATTACK_PATTERN_LONG_START + 2];
		}
		else {
			patterns[0] = enablePatterns[ATTACK_PATTERN_MIDDLE_START];
			patterns[1] = enablePatterns[ATTACK_PATTERN_MIDDLE_START + 1];
			patterns[2] = enablePatterns[ATTACK_PATTERN_MIDDLE_START + 2];
			patterns[3] = enablePatterns[ATTACK_PATTERN_MIDDLE_START];
			patterns[4] = enablePatterns[ATTACK_PATTERN_MIDDLE_START + 1];
			patterns[5] = enablePatterns[ATTACK_PATTERN_MIDDLE_START + 2];
		}

		m_fWaitTime = 1.5f;

		std::random_device rd;
		std::default_random_engine dre(rd());
		std::uniform_real_distribution<float> urd(0.0f, 1.0f);
		std::uniform_int_distribution<short> uid(0, 5);

		short pattern;
		// 한번도 연속 공격을 안했을 경우 대기하지 않음
		if (m_sContinuousAttackTime == 0) {
			pattern = patterns[uid(dre)];
		}
		else {
			// 확률 계산(연속 공격 횟수가 높을수록 대기 확률 증가)
			float rate = urd(dre);
			if (rate > m_sContinuousAttackTime * 0.05f) {
				pattern = patterns[uid(dre)];
			}
			else {
				pattern = 0;
			}
		}

		// 연속 공격 횟수 저장
		if (pattern != 0) {
			m_sContinuousAttackTime++;
		}
		else {
			m_sContinuousAttackTime = 0;
		}

		object->SetAttackPattern(pattern);
		object->SetIsAttacking(true);


		if (object->GetIsAnimate()) {
			if (pattern == 0) {
				if (object->GetAnimationState() == ANI_STATE_HIT
					|| object->GetAnimationState() == ANI_STATE_HIT_LEFT
					|| object->GetAnimationState() == ANI_STATE_HIT_RIGHT) {
					if (object->GetAnimator()->IsOneCycleEnd()) {
						object->SetAnimationState(ANI_STATE_IDLE);
					}
				}
				else if (object->GetAnimationState() != ANI_STATE_IDLE) {
					object->SetAnimationState(ANI_STATE_IDLE);
				}
			}
			else if (pattern == 1) {
				if (object->GetAnimationState() != ANI_STATE_JUMP) {
					object->SetAnimationState(ANI_STATE_JUMP);
					object->GetAnimator()->SetSpeed(0.85f);
					object->GetAnimator()->SetIsLoop(false);
				}
			}
			else if (pattern == 2) {
				if (object->GetAnimationState() != ANI_STATE_WALK) {
					object->SetAnimationState(ANI_STATE_WALK);
				}
			}
			else if (pattern == 3) {
				if (object->GetAnimationState() != ANI_STATE_ATTACK) {
					object->SetAnimationState(ANI_STATE_ATTACK);
					object->GetAnimator()->SetIsLoop(false);
				}
			}
			else if (pattern == 4) {
				if (object->GetAnimationState() != ANI_STATE_IDLE) {
					object->SetAnimationState(ANI_STATE_IDLE);
				}
			}
			else if (pattern == 5) {
				if (object->GetAnimationState() != ANI_STATE_SKILL_1) {
					object->SetAnimationState(ANI_STATE_SKILL_1);
					object->GetAnimator()->SetIsLoop(false);
				}
			}
			else if (pattern == 6) {
				if (object->GetAnimationState() != ANI_STATE_ATTACK) {
					object->SetAnimationState(ANI_STATE_ATTACK);
					object->GetAnimator()->SetIsLoop(false);
				}
			}
			else if (pattern == 7) {
				if (object->GetAnimationState() != ANI_STATE_SKILL_1) {
					object->SetAnimationState(ANI_STATE_SKILL_1);
					object->GetAnimator()->SetIsLoop(false);
				}
			}
		}

		return BT_SUCCESS;
	}
	else {
		if (object->GetAnimationState() == ANI_STATE_HIT 
			|| object->GetAnimationState() == ANI_STATE_HIT_LEFT
			|| object->GetAnimationState() == ANI_STATE_HIT_RIGHT) {
			if (object->GetAnimator()->IsOneCycleEnd()) {
				object->SetAnimationState(ANI_STATE_IDLE);
			}
		}

		if (object->GetAttackPattern() == 0) {
			// 잠시 대기
			if (m_fWaitTime < 0.0f) {
				object->SetIsAttacking(false);
			}

			m_fWaitTime -= BackBoard()->GetElapsedTimeInSec();
			return BT_RUNNING;
		}
		else {
			return BT_SUCCESS;
		}
	}
}

BehaviorTreeState CBossChangePhase::Run()
{
	if (object->GetPhase() == 1) {
		if (object->GetIsChangedPhase()) {
			if (object->GetAnimationState() == ANI_STATE_PHASE_CHANGE) {
				if (object->GetAnimator()->IsOneCycleEnd()) {
					object->SetAnimationState(ANI_STATE_IDLE);
					object->SetIsChangedPhase(false);
					object->SetPhase(2);
					return BT_FAILURE;
				}
				else {
					return BT_RUNNING;
				}
			}
			else {
				object->SetAnimationState(ANI_STATE_PHASE_CHANGE);
				object->GetAnimator()->SetIsLoop(false);
				return BT_RUNNING;
			}
		}
		else {
			return BT_SUCCESS;
		}
	}
	else {
		return BT_SUCCESS;
	}
}

// CSetAttackPattern1---------------------------------------

BehaviorTreeState CPlayBossAttackPattern1::Run()
{
	if (object->GetIsChangedPhase()
		|| object->GetAnimationState() == ANI_STATE_HIT
		|| object->GetAnimationState() == ANI_STATE_HIT_LEFT
		|| object->GetAnimationState() == ANI_STATE_HIT_RIGHT) {
		m_bIsAddForce = false;
		m_bIsSpawnedDamageZone = false;
		m_bIsOnAir = false;
		m_fSize = 1.0f;
		m_fWaitTime = 0.0f;
		object->SetIsAttacking(false);

		if (m_pDamageZone != NULL) {
			m_pDamageZone->SetLifeTime(-1.0f);
			m_pDamageZone = NULL;
		}
		return BT_SUCCESS;
	}

	if (object->GetAttackPattern() == 1) {
		if (!m_bIsAddForce) {
			// 바닥에 있을 경우 주인공 적으로 점프
			if (object->GetState() == STATE_GROUND) {
				m_bIsAddForce = true;
				m_fSize = 1.0f;

				// 주인공 있는 방향 계산
				glm::vec3 position = object->GetPosition();
				glm::vec3 heroPos = BackBoard()->GetHeroPosition();
				glm::vec3 dir = glm::normalize(glm::vec3(heroPos.x, 0.0f, heroPos.z)
					- glm::vec3(position.x, 0.0f, position.z));

				GameUpdateParameters param;
				memset(&param, 0, sizeof(param));
				param.force = glm::vec3(dir.x * 600 * object->GetMass(),
					300.f * object->GetMass(),
					dir.z * 600 * object->GetMass());

				// 60프레임으로 가정하고 업데이트하여 동일한 힘만 받도록 처리
				object->CGameObject::Update(1.0f / 60.0f, param);

				return BT_RUNNING;
			}
			object->SetIsAttacking(false);
			return BT_FAILURE;
		}
		else {
			// 한번은 공중에 있는 상태로 변경되었는지 확인
			if (object->GetState() == STATE_FALLING) {
				m_bIsOnAir = true;
			}

			if (m_bIsOnAir) {
				// 다시 바닥 상태인 지 확인
				if (object->GetState() == STATE_GROUND) {
					// 착지한 곳을 중심으로 데미지 존 생성
					if (!m_bIsSpawnedDamageZone) {
						object->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));

						int damageZoneIndex = object->GetObjectManager()->AddObject<CDamageZone>(
							glm::vec3(object->GetPosition().x, -4.0f, object->GetPosition().z),
							glm::vec3(0.0f, 0.0f, 0.0f),
							glm::vec3(1.0f, 0.1f, 1.0f),
							1.0f);
						m_pDamageZone = object->GetObjectManager()->GetGameObject<CDamageZone>(damageZoneIndex);
						m_pDamageZone->SetType(TYPE_DAMAGE_ZONE);
						m_pDamageZone->SetShowTime(1.5f);
						m_pDamageZone->SetIsCircle(true);
						m_pDamageZone->SetAlwaysOn(true);
						m_pDamageZone->SetLifeTime(1.5f);
						m_pDamageZone->SetParent(object);
						m_pDamageZone->SetVolume(glm::vec3(1.0f, 10.0f, 1.0f));

						int landingDust = object->GetObjectManager()->
							AddParticle<CLandingDustEffectParticle>(
								glm::vec3(object->GetPosition().x, -3.5f, object->GetPosition().z),
								glm::vec3(1.0f, 1.0f, 1.0f));
						object->GetObjectManager()->
							GetParticle<CLandingDustEffectParticle>(landingDust)->
							SetTextureID(object->GetDustTextureID());
						object->GetObjectManager()->
							GetParticle<CLandingDustEffectParticle>(landingDust)->
							SetColor(glm::vec4(0.8f, 0.8f, 0.5f, 0.85f));
						object->GetObjectManager()->
							GetParticle<CLandingDustEffectParticle>(landingDust)->
							SetRadius(3.0f);
						object->GetObjectManager()->
							GetParticle<CLandingDustEffectParticle>(landingDust)->SetLifeTime(2.f);
						object->GetObjectManager()->
							GetParticle<CLandingDustEffectParticle>(landingDust)->CreateVAO();

						m_bIsSpawnedDamageZone = true;

						return BT_RUNNING;
					}
					else {
						// 대미지 존이 점점 커지도록
						if (m_fSize > 6.0f) {
							m_bIsAddForce = false;
							m_bIsSpawnedDamageZone = false;
							m_bIsOnAir = false;
							m_pDamageZone->SetLifeTime(0.0f);
							object->SetIsAttacking(false);
							return BT_SUCCESS;
						}

						m_fSize += BackBoard()->GetElapsedTimeInSec() * 8.f;

						m_pDamageZone->SetSize(glm::vec3(m_fSize, 0.1f, m_fSize));

						return BT_RUNNING;
					}
				}
				else {
					return BT_RUNNING;
				}
			}
			return BT_RUNNING;
		}
	}
	return BT_FAILURE;
}

// CSetAttackPattern2---------------------------------------

BehaviorTreeState CPlayBossAttackPattern2::Run()
{
	if (object->GetIsChangedPhase()
		|| object->GetAnimationState() == ANI_STATE_HIT
		|| object->GetAnimationState() == ANI_STATE_HIT_LEFT
		|| object->GetAnimationState() == ANI_STATE_HIT_RIGHT) {
		m_vHeroDirection = glm::vec3(0.0f);
		m_bIsSpawnedDamageZone = false;
		m_fWaitTime = 1.0f;
		object->SetEnableCollision(true);
		object->SetIsAttacking(false);

		if (m_pDamageZone != NULL) {
			m_pDamageZone->SetLifeTime(-1.0f);
			m_pDamageZone = NULL;
		}
		return BT_SUCCESS;
	}

	if (object->GetAttackPattern() == 2) {
		if (object->GetState() == STATE_GROUND) {
			if (!m_bIsSpawnedDamageZone) {
				// 주인공 있는 방향 계산
				glm::vec3 position = object->GetPosition();
				glm::vec3 heroPos = BackBoard()->GetHeroPosition();
				m_vHeroDirection = glm::normalize(glm::vec3(heroPos.x, 0.0f, heroPos.z)
					- glm::vec3(position.x, 0.0f, position.z));

				// 오브젝트에 작은 힘을 주어 현재 주인공 방향으로 회전
				GameUpdateParameters param;
				memset(&param, 0, sizeof(param));
				param.force = glm::vec3(m_vHeroDirection.x * object->GetMass(),
					0.0f,
					m_vHeroDirection.z * object->GetMass());

				// 오브젝트의 업데이트는 행동트리도 업데이트 하기 때문에 부모의 업데이트를 호출
				object->CGameObject::Update(BackBoard()->GetElapsedTimeInSec(), param);

				int damageZoneIndex = object->GetObjectManager()->AddObject<CDamageZone>(
					glm::vec3(object->GetPosition().x + m_vHeroDirection.x * 5.f,
						-4.0f, object->GetPosition().z + m_vHeroDirection.z * 5.f),
					glm::vec3(0.0f, object->GetRotation().y, 0.0f),
					glm::vec3(1.5f, 0.2f, 6.0f),
					1.0f);
				m_pDamageZone = object->GetObjectManager()->GetGameObject<CDamageZone>(damageZoneIndex);
				m_pDamageZone->SetType(TYPE_DAMAGE_ZONE);
				m_pDamageZone->SetIsCircle(false);
				m_pDamageZone->SetShowTime(0.6f);
				m_pDamageZone->SetParent(object);
				m_pDamageZone->SetVolume(glm::vec3(1.f, 8.0f, 1.f));

				m_fWaitTime = 0.6f + DEFAULT_DAMAGE_TIME;

				m_bIsSpawnedDamageZone = true;
				return BT_RUNNING;
			}
			else {
				if (m_fWaitTime < 0.0f) {
					m_bIsSpawnedDamageZone = false;
					object->SetEnableCollision(true);
					object->SetIsAttacking(false);

					return BT_SUCCESS;
				}
				else if (0.3f + DEFAULT_DAMAGE_TIME > m_fWaitTime && m_fWaitTime > DEFAULT_DAMAGE_TIME) {
					GameUpdateParameters param;
					memset(&param, 0, sizeof(param));
					param.force = glm::vec3(m_vHeroDirection.x * 500 * object->GetMass(),
						0.0f,
						m_vHeroDirection.z * 500 * object->GetMass());

					// 오브젝트의 업데이트는 행동트리도 업데이트 하기 때문에 부모의 업데이트를 호출
					object->CGameObject::Update(BackBoard()->GetElapsedTimeInSec(), param);
					object->SetEnableCollision(false);
				}

				m_fWaitTime -= BackBoard()->GetElapsedTimeInSec();
				return BT_RUNNING;
			}
		}
		return BT_FAILURE;
	}
	return BT_FAILURE;
}

// CSetAttackPattern3---------------------------------------

BehaviorTreeState CPlayBossAttackPattern3::Run()
{
	if (object->GetIsChangedPhase() 
		|| object->GetAnimationState() == ANI_STATE_HIT
		|| object->GetAnimationState() == ANI_STATE_HIT_LEFT
		|| object->GetAnimationState() == ANI_STATE_HIT_RIGHT) {
		m_bIsSpawnedDamageZone = false;
		m_fWaitTime = 1.0f;
		object->SetIsAttacking(false);

		if (m_pDamageZone1 != NULL) {
			m_pDamageZone1->SetLifeTime(-1.0f);
			m_pDamageZone1 = NULL;
		}
		if (m_pDamageZone2 != NULL) {
			m_pDamageZone2->SetLifeTime(-1.0f);
			m_pDamageZone2 = NULL;
		}
		if (m_pDamageZone3 != NULL) {
			m_pDamageZone3->SetLifeTime(-1.0f);
			m_pDamageZone3 = NULL;
		}
		return BT_SUCCESS;
	}

	if (object->GetAttackPattern() == 3) {
		if (object->GetState() == STATE_GROUND) {
			if (!m_bIsSpawnedDamageZone) {
				// 주인공 있는 방향 계산
				glm::vec3 position = object->GetPosition(); position.y = 0;
				glm::vec3 heroPos = BackBoard()->GetHeroPosition(); heroPos.y = 0;
				glm::vec3 dir = glm::normalize(heroPos - position);

				// 오브젝트에 작은 힘을 주어 현재 주인공 방향으로 회전
				GameUpdateParameters param;
				memset(&param, 0, sizeof(param));
				param.force = glm::vec3(dir.x * object->GetMass(), 0.0f, dir.z * object->GetMass());

				// 오브젝트의 업데이트는 행동트리도 업데이트 하기 때문에 부모의 업데이트를 호출
				object->CGameObject::Update(BackBoard()->GetElapsedTimeInSec(), param);

				object->SetPosition(position);
				object->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));

				glm::vec3 rotation = object->GetRotation();
				glm::quat modelQuat
					= glm::quat(glm::vec3(glm::radians(rotation)));
				glm::mat4 rotationMat = glm::toMat4(modelQuat);

				int damageZoneIndex = object->GetObjectManager()->AddObject<CDamageZone>(
					glm::vec3(position + glm::vec3(rotationMat * glm::vec4(0.0f, -4.f, -4.5f, 1.0f))),
					glm::vec3(0.0f, rotation.y, 0.0f),
					glm::vec3(1.5f, 0.1f, 1.5f),
					1.0f);
				m_pDamageZone1 = object->GetObjectManager()->GetGameObject<CDamageZone>(damageZoneIndex);
				m_pDamageZone1->SetType(TYPE_DAMAGE_ZONE);
				m_pDamageZone1->SetIsCircle(false);
				m_pDamageZone1->SetShowTime(0.3f);
				m_pDamageZone1->SetParent(object);
				m_pDamageZone1->SetVolume(glm::vec3(1.0f, 10.0f, 1.0f));

				damageZoneIndex = object->GetObjectManager()->AddObject<CDamageZone>(
					glm::vec3(position + glm::vec3(rotationMat * glm::vec4(-2.75f, -4.f, -4.5f, 1.0f))),
					glm::vec3(0.0f, rotation.y + 30.0f, 0.0f),
					glm::vec3(1.5f, 0.1f, 1.5f),
					1.0f);
				m_pDamageZone2 = object->GetObjectManager()->GetGameObject<CDamageZone>(damageZoneIndex);
				m_pDamageZone2->SetType(TYPE_DAMAGE_ZONE);
				m_pDamageZone2->SetIsCircle(false);
				m_pDamageZone2->SetShowTime(0.5f);
				m_pDamageZone2->SetParent(object);
				m_pDamageZone2->SetVolume(glm::vec3(1.0f, 10.0f, 1.0f));

				damageZoneIndex = object->GetObjectManager()->AddObject<CDamageZone>(
					glm::vec3(position + glm::vec3(rotationMat * glm::vec4(2.75f, -4.f, -4.5f, 1.0f))),
					glm::vec3(0.0f, rotation.y - 30.0f, 0.0f),
					glm::vec3(1.5f, 0.1f, 1.5f),
					1.0f);
				m_pDamageZone3 = object->GetObjectManager()->GetGameObject<CDamageZone>(damageZoneIndex);
				m_pDamageZone3->SetType(TYPE_DAMAGE_ZONE);
				m_pDamageZone3->SetIsCircle(false);
				m_pDamageZone3->SetShowTime(0.5f);
				m_pDamageZone3->SetParent(object);
				m_pDamageZone3->SetVolume(glm::vec3(1.0f, 10.0f, 1.0f));


				m_fWaitTime = 0.5f + DEFAULT_DAMAGE_TIME;

				m_bIsSpawnedDamageZone = true;
				return BT_RUNNING;
			}
			else {
				if (m_fWaitTime < 0.0f) {
					m_bIsSpawnedDamageZone = false;
					object->SetIsAttacking(false);

					return BT_SUCCESS;
				}

				m_fWaitTime -= BackBoard()->GetElapsedTimeInSec();
				return BT_RUNNING;
			}
		}
		return BT_FAILURE;
	}
	return BT_FAILURE;
}

BehaviorTreeState CPlayBossAttackPattern4::Run()
{
	if (object->GetIsChangedPhase()
		|| object->GetAnimationState() == ANI_STATE_HIT
		|| object->GetAnimationState() == ANI_STATE_HIT_LEFT
		|| object->GetAnimationState() == ANI_STATE_HIT_RIGHT) {
		m_bIsSpawnedDamageZone = false;
		object->SetIsAttacking(false);
		m_bIsRightEnd = false;
		m_bIsLeftEnd = false;

		return BT_SUCCESS;
	}

	if (object->GetAttackPattern() == 4) {
		if (!m_bIsSpawnedDamageZone) {
			if (!m_bIsRightEnd) {
				glm::vec3 heroPos = BackBoard()->GetHeroPosition();
				glm::vec3 position = object->GetPosition();

				glm::vec3 dir = glm::normalize(glm::vec3(heroPos.x, 0.0f, heroPos.z)
					- glm::vec3(position.x, 0.0f, position.z));

				// 오브젝트에 작은 힘을 주어 현재 주인공 방향으로 회전
				GameUpdateParameters param;
				memset(&param, 0, sizeof(param));
				param.force = glm::vec3(dir.x * object->GetMass(), 0.0f, dir.z * object->GetMass());

				// 주인공 위치에 원형 대미지 존 생성
				int damageZoneIndex = object->GetObjectManager()->AddObject<CDamageZone>(
					glm::vec3(heroPos.x, -4.0f, heroPos.z),
					glm::vec3(0.0f, 0.0f, 0.0f),
					glm::vec3(1.0f, 0.1f, 1.0f),
					1.0f);
				m_pDamageZone = object->GetObjectManager()->GetGameObject<CDamageZone>(damageZoneIndex);
				m_pDamageZone->SetType(TYPE_DAMAGE_ZONE);
				m_pDamageZone->SetIsCircle(true);
				m_pDamageZone->SetShowTime(0.7f);
				m_pDamageZone->SetParent(object);
				m_pDamageZone->SetVolume(glm::vec3(1.0f, 10.0f, 1.0f));

				// 오브젝트의 업데이트는 행동트리도 업데이트 하기 때문에 부모의 업데이트를 호출
				object->CGameObject::Update(BackBoard()->GetElapsedTimeInSec(), param);

				object->SetPosition(position);
				object->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));

				int shootingBall = object->GetObjectManager()->
					AddParticle<CShootingBallEffectParticle>(
						object->GetPosition(),
						glm::vec3(1.0f, 1.0f, 1.0f));
				object->GetObjectManager()->
					GetParticle<CShootingBallEffectParticle>(shootingBall)->
					SetDirection(glm::vec3(heroPos.x, -4.0f, heroPos.z)
						- glm::vec3(position.x, position.y, position.z));
				object->GetObjectManager()->
					GetParticle<CShootingBallEffectParticle>(shootingBall)->
					SetTextureID(object->GetBallTextureID());
				object->GetObjectManager()->
					GetParticle<CShootingBallEffectParticle>(shootingBall)->
					SetColor(glm::vec4(3.0f, 0.8f, 0.0f, 1.0f));
				object->GetObjectManager()->
					GetParticle<CShootingBallEffectParticle>(shootingBall)->SetLifeTime(1.5f);
				object->GetObjectManager()->
					GetParticle<CShootingBallEffectParticle>(shootingBall)->SetIsLoop(false);
				object->GetObjectManager()->
					GetParticle<CShootingBallEffectParticle>(shootingBall)->SetDelayTime(0.3f);
				object->GetObjectManager()->
					GetParticle<CShootingBallEffectParticle>(shootingBall)->CreateVAO();

				m_bIsSpawnedDamageZone = true;
				m_bIsRightEnd = true;

				object->SetAnimationState(ANI_STATE_SKILL_RIGHT);
				object->GetAnimator()->SetIsLoop(false);

				return BT_RUNNING;
			}
			else if (!m_bIsLeftEnd) {
				glm::vec3 heroPos = BackBoard()->GetHeroPosition();
				glm::vec3 position = object->GetPosition();

				glm::vec3 dir = glm::normalize(glm::vec3(heroPos.x, 0.0f, heroPos.z)
					- glm::vec3(position.x, 0.0f, position.z));

				// 오브젝트에 작은 힘을 주어 현재 주인공 방향으로 회전
				GameUpdateParameters param;
				memset(&param, 0, sizeof(param));
				param.force = glm::vec3(dir.x * object->GetMass(), 0.0f, dir.z * object->GetMass());

				// 오브젝트의 업데이트는 행동트리도 업데이트 하기 때문에 부모의 업데이트를 호출
				object->CGameObject::Update(BackBoard()->GetElapsedTimeInSec(), param);

				object->SetPosition(position);
				object->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));

				// 주인공 위치에 원형 대미지 존 생성
				int damageZoneIndex = object->GetObjectManager()->AddObject<CDamageZone>(
					glm::vec3(heroPos.x, -4.0f, heroPos.z),
					glm::vec3(0.0f, 0.0f, 0.0f),
					glm::vec3(1.0f, 0.1f, 1.0f),
					1.0f);
				m_pDamageZone = object->GetObjectManager()->GetGameObject<CDamageZone>(damageZoneIndex);
				m_pDamageZone->SetType(TYPE_DAMAGE_ZONE);
				m_pDamageZone->SetIsCircle(true);
				m_pDamageZone->SetShowTime(0.7f);
				m_pDamageZone->SetParent(object);
				m_pDamageZone->SetVolume(glm::vec3(1.0f, 10.0f, 1.0f));

				m_bIsSpawnedDamageZone = true;
				m_bIsLeftEnd = true;

				object->SetAnimationState(ANI_STATE_SKILL_LEFT);
				object->GetAnimator()->SetIsLoop(false);

				int shootingBall = object->GetObjectManager()->
					AddParticle<CShootingBallEffectParticle>(
						object->GetPosition(),
						glm::vec3(1.0f, 1.0f, 1.0f));
				object->GetObjectManager()->
					GetParticle<CShootingBallEffectParticle>(shootingBall)->
					SetDirection(glm::vec3(heroPos.x, -4.0f, heroPos.z)
						- glm::vec3(position.x, position.y, position.z));
				object->GetObjectManager()->
					GetParticle<CShootingBallEffectParticle>(shootingBall)->
					SetTextureID(object->GetBallTextureID());
				object->GetObjectManager()->
					GetParticle<CShootingBallEffectParticle>(shootingBall)->
					SetColor(glm::vec4(3.0f, 0.8f, 0.0f, 1.0f));
				object->GetObjectManager()->
					GetParticle<CShootingBallEffectParticle>(shootingBall)->SetLifeTime(1.5f);
				object->GetObjectManager()->
					GetParticle<CShootingBallEffectParticle>(shootingBall)->SetIsLoop(false);
				object->GetObjectManager()->
					GetParticle<CShootingBallEffectParticle>(shootingBall)->SetDelayTime(0.3f);
				object->GetObjectManager()->
					GetParticle<CShootingBallEffectParticle>(shootingBall)->CreateVAO();

				return BT_RUNNING;
			}		
		}
		else {
			if (object->GetAnimator()->IsOneCycleEnd()) {
				if (m_bIsLeftEnd) {
					m_bIsSpawnedDamageZone = false;
					m_bIsRightEnd = false;
					m_bIsLeftEnd = false;
					object->SetIsAttacking(false);
					return BT_SUCCESS;

				}
				else if (m_bIsRightEnd) {
					m_bIsSpawnedDamageZone = false;
					return BT_RUNNING;
				}
			}
			return BT_RUNNING;
		}
	}
	return BT_FAILURE;
}

BehaviorTreeState CPlayBossAttackPattern5::Run()
{
	if (object->GetIsChangedPhase()
		|| object->GetAnimationState() == ANI_STATE_HIT
		|| object->GetAnimationState() == ANI_STATE_HIT_LEFT
		|| object->GetAnimationState() == ANI_STATE_HIT_RIGHT) {
		m_bIsSpawnedDamageZone = false;
		object->SetIsAttacking(false);

		return BT_SUCCESS;
	}
	if (object->GetAttackPattern() == 5) {
		if (!m_bIsSpawnedDamageZone) {
				glm::vec3 heroPos = BackBoard()->GetHeroPosition();
				glm::vec3 position = object->GetPosition();

				glm::vec3 dir = glm::normalize(glm::vec3(heroPos.x, 0.0f, heroPos.z)
					- glm::vec3(position.x, 0.0f, position.z));

				// 오브젝트에 작은 힘을 주어 현재 주인공 방향으로 회전
				GameUpdateParameters param;
				memset(&param, 0, sizeof(param));
				param.force = glm::vec3(dir.x * object->GetMass(), 0.0f, dir.z * object->GetMass());

				// 오브젝트의 업데이트는 행동트리도 업데이트 하기 때문에 부모의 업데이트를 호출
				object->CGameObject::Update(BackBoard()->GetElapsedTimeInSec(), param);

				object->SetPosition(position);
				object->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));

				// 가운데 머리 앞에 직사각형 대미지 존 생성
				int damageZoneIndex = object->GetObjectManager()->AddObject<CDamageZone>(
					glm::vec3(object->GetPosition().x + dir.x * 8.f,
						-4.0f, object->GetPosition().z + dir.z * 8.f),
					glm::vec3(0.0f, object->GetRotation().y, 0.0f),
					glm::vec3(1.f, 0.2f, 4.f),
					1.0f);
				m_pDamageZone = object->GetObjectManager()->GetGameObject<CDamageZone>(damageZoneIndex);
				m_pDamageZone->SetType(TYPE_DAMAGE_ZONE);
				m_pDamageZone->SetIsCircle(false);
				m_pDamageZone->SetShowTime(0.5f);
				m_pDamageZone->SetParent(object);
				m_pDamageZone->SetVolume(glm::vec3(1.f, 8.0f, 1.f));

				int beam = object->GetObjectManager()->
					AddParticle<CBeamParticle>(
						glm::vec3(object->GetPosition().x + dir.x * 4.f,
							object->GetPosition().y + 0.5f, object->GetPosition().z + dir.z * 4.f),
						glm::vec3(1.0f, 1.0f, 1.0f));
				object->GetObjectManager()->
					GetParticle<CBeamParticle>(beam)->
					SetRotation(object->GetRotation());
				object->GetObjectManager()->
					GetParticle<CBeamParticle>(beam)->
					SetTextureID(object->GetBallTextureID());
				object->GetObjectManager()->
					GetParticle<CBeamParticle>(beam)->
					SetColor(glm::vec4(3.0f, 0.8f, 0.0f, 1.0f));
				object->GetObjectManager()->
					GetParticle<CBeamParticle>(beam)->SetLifeTime(1.3f);
				object->GetObjectManager()->
					GetParticle<CBeamParticle>(beam)->SetIsLoop(false);
				object->GetObjectManager()->
					GetParticle<CBeamParticle>(beam)->SetDelayTime(0.3f);
				object->GetObjectManager()->
					GetParticle<CBeamParticle>(beam)->CreateVAO();

				m_bIsSpawnedDamageZone = true;

			return BT_RUNNING;
		}
		else {
			if (object->GetAnimator()->IsOneCycleEnd()) {
					m_bIsSpawnedDamageZone = false;
					object->SetIsAttacking(false);
					return BT_SUCCESS;
			}
			return BT_RUNNING;
		}
	}
	return BT_FAILURE;
}


BehaviorTreeState CPlayBossAttackPattern6::Run()
{
	if (object->GetIsChangedPhase()
		|| object->GetAnimationState() == ANI_STATE_HIT
		|| object->GetAnimationState() == ANI_STATE_HIT_LEFT
		|| object->GetAnimationState() == ANI_STATE_HIT_RIGHT) {
		m_bIsSpawnedDamageZone = false;
		m_fWaitTime = 1.0f;
		object->SetIsAttacking(false);

		if (m_pDamageZone != NULL) {
			m_pDamageZone->SetLifeTime(-1.0f);
			m_pDamageZone = NULL;
		}
		return BT_SUCCESS;
	}

	if (object->GetAttackPattern() == 6) {
		if (object->GetState() == STATE_GROUND) {
			if (!m_bIsSpawnedDamageZone) {
				// 주인공 있는 방향 계산
				glm::vec3 position = object->GetPosition(); position.y = 0;
				glm::vec3 heroPos = BackBoard()->GetHeroPosition(); heroPos.y = 0;
				glm::vec3 dir = glm::normalize(heroPos - position);

				// 오브젝트에 작은 힘을 주어 현재 주인공 방향으로 회전
				GameUpdateParameters param;
				memset(&param, 0, sizeof(param));
				param.force = glm::vec3(dir.x * object->GetMass(), 0.0f, dir.z * object->GetMass());

				// 오브젝트의 업데이트는 행동트리도 업데이트 하기 때문에 부모의 업데이트를 호출
				object->CGameObject::Update(BackBoard()->GetElapsedTimeInSec(), param);

				object->SetPosition(position);
				object->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));

				glm::vec3 rotation = object->GetRotation();
				glm::quat modelQuat
					= glm::quat(glm::vec3(glm::radians(rotation)));
				glm::mat4 rotationMat = glm::toMat4(modelQuat);

				int damageZoneIndex = object->GetObjectManager()->AddObject<CDamageZone>(
					glm::vec3(position + glm::vec3(rotationMat * glm::vec4(0.0f, -4.f, -4.f, 1.0f))),
					glm::vec3(0.0f, rotation.y, 0.0f),
					glm::vec3(1.5f, 0.1f, 1.5f),
					1.0f);
				m_pDamageZone = object->GetObjectManager()->GetGameObject<CDamageZone>(damageZoneIndex);
				m_pDamageZone->SetType(TYPE_DAMAGE_ZONE);
				m_pDamageZone->SetIsCircle(false);
				m_pDamageZone->SetShowTime(0.5f);
				m_pDamageZone->SetParent(object);
				m_pDamageZone->SetVolume(glm::vec3(1.0f, 10.0f, 1.0f));

				m_fWaitTime = 0.5f + DEFAULT_DAMAGE_TIME;

				m_bIsSpawnedDamageZone = true;
				return BT_RUNNING;
			}
			else {
				if (m_fWaitTime < 0.0f) {
					m_bIsSpawnedDamageZone = false;
					object->SetIsAttacking(false);

					return BT_SUCCESS;
				}

				m_fWaitTime -= BackBoard()->GetElapsedTimeInSec();
				return BT_RUNNING;
			}
		}
		return BT_FAILURE;
	}
	return BT_FAILURE;
}

BehaviorTreeState CPlayBossAttackPattern7::Run()
{
	if (object->GetIsChangedPhase()
		|| object->GetAnimationState() == ANI_STATE_HIT
		|| object->GetAnimationState() == ANI_STATE_HIT_LEFT
		|| object->GetAnimationState() == ANI_STATE_HIT_RIGHT) {
		m_bIsSpawnedDamageZone = false;
		object->SetIsAttacking(false);

		return BT_SUCCESS;
	}

	if (object->GetAttackPattern() == 7) {
		if (!m_bIsSpawnedDamageZone) {
			glm::vec3 heroPos = BackBoard()->GetHeroPosition();
			glm::vec3 position = object->GetPosition();

			glm::vec3 dir = glm::normalize(glm::vec3(heroPos.x, 0.0f, heroPos.z)
				- glm::vec3(position.x, 0.0f, position.z));

			// 오브젝트에 작은 힘을 주어 현재 주인공 방향으로 회전
			GameUpdateParameters param;
			memset(&param, 0, sizeof(param));
			param.force = glm::vec3(dir.x * object->GetMass(), 0.0f, dir.z * object->GetMass());

			// 주인공 위치에 원형 대미지 존 생성
			int damageZoneIndex = object->GetObjectManager()->AddObject<CDamageZone>(
				glm::vec3(heroPos.x, -4.0f, heroPos.z),
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(1.0f, 0.1f, 1.0f),
				1.0f);
			m_pDamageZone = object->GetObjectManager()->GetGameObject<CDamageZone>(damageZoneIndex);
			m_pDamageZone->SetType(TYPE_DAMAGE_ZONE);
			m_pDamageZone->SetIsCircle(true);
			m_pDamageZone->SetShowTime(0.7f);
			m_pDamageZone->SetParent(object);
			m_pDamageZone->SetVolume(glm::vec3(1.0f, 10.0f, 1.0f));

			// 오브젝트의 업데이트는 행동트리도 업데이트 하기 때문에 부모의 업데이트를 호출
			object->CGameObject::Update(BackBoard()->GetElapsedTimeInSec(), param);

			object->SetPosition(position);
			object->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));

			int shootingBall = object->GetObjectManager()->
				AddParticle<CShootingBallEffectParticle>(
					object->GetPosition(),
					glm::vec3(1.0f, 1.0f, 1.0f));
			object->GetObjectManager()->
				GetParticle<CShootingBallEffectParticle>(shootingBall)->
				SetDirection(glm::vec3(heroPos.x, -4.0f, heroPos.z)
					- glm::vec3(position.x, position.y, position.z));
			object->GetObjectManager()->
				GetParticle<CShootingBallEffectParticle>(shootingBall)->
				SetTextureID(object->GetBallTextureID());
			object->GetObjectManager()->
				GetParticle<CShootingBallEffectParticle>(shootingBall)->
				SetColor(glm::vec4(3.0f, 0.8f, 0.0f, 1.0f));
			object->GetObjectManager()->
				GetParticle<CShootingBallEffectParticle>(shootingBall)->SetLifeTime(1.5f);
			object->GetObjectManager()->
				GetParticle<CShootingBallEffectParticle>(shootingBall)->SetIsLoop(false);
			object->GetObjectManager()->
				GetParticle<CShootingBallEffectParticle>(shootingBall)->SetDelayTime(0.3f);
			object->GetObjectManager()->
				GetParticle<CShootingBallEffectParticle>(shootingBall)->CreateVAO();

			m_bIsSpawnedDamageZone = true;

			return BT_RUNNING;
		}
		else {
			if (object->GetAnimator()->IsOneCycleEnd()) {
				m_bIsSpawnedDamageZone = false;
				object->SetIsAttacking(false);
				return BT_SUCCESS;
			}
			return BT_RUNNING;
		}
	}
	return BT_FAILURE;
}
