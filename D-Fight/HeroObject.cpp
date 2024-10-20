#include "stdafx.h"
#include "HeroObject.h"
#include "SoundPlayer.h"

CHeroObject::CHeroObject()
{
	m_walkSoundID = CSoundPlayer::GetInstance()->LoadSound("./resource/sound/Effect/grassWalkSound.wav");
	m_attackSoundID = CSoundPlayer::GetInstance()->LoadSound("./resource/sound/Effect/attackSound.mp3");
	m_hitSoundID = CSoundPlayer::GetInstance()->LoadSound("./resource/sound/Effect/heroHitSound.wav");
	m_dashSoundID = CSoundPlayer::GetInstance()->LoadSound("./resource/sound/Effect/dashSound.wav");

	SetHitSoundID(m_hitSoundID);
}

CHeroObject::~CHeroObject()
{
	
}

void CHeroObject::Update(float elapsedTimeInSec, GameUpdateParameters parameters)
{
	// 적에게 잡힌 상태에서는 움직임 불가
	if (m_bIsHeldByEnemy) return;

	SetWeaponPosition();
	GameObjectAnimationState animationState = GetAnimationState();

	// 특정 행동을 하지 않을 경우 스테미나 회복
	if (!m_bIsJumping && animationState != ANI_STATE_RECOVER && animationState != ANI_STATE_HIT
		&& animationState != ANI_STATE_AVOID && animationState != ANI_STATE_ATTACK) {
		RecoverStemina(elapsedTimeInSec);
		if (animationState == ANI_STATE_IDLE) {
			m_fIdleTime += elapsedTimeInSec;
			if (m_fIdleTime >= DEFAULT_IDLE_TIME) {
				SetAnimationState(ANI_STATE_IDLE_LONG);
				GetAnimator()->SetSpeed(0.2f);
			}
		}
	}
	else {
		GameUpdateParameters param;
		memset(&param, 0, sizeof(param));

		float angle = GetRotation().y - 90.0f;
		glm::vec3 direction = glm::vec3(-glm::cos(glm::radians(angle)), 0.f, glm::sin(glm::radians(angle)));
		direction = glm::normalize(direction);

		param.force = direction;

		if (m_bIsJumping) {
			if (m_fJumpTime < 0.0f) {
				m_bIsJumping = false;
			}
			{
				m_fJumpTime -= elapsedTimeInSec;
				if (GetEnableCollision()) param.force.y = 20000.f;
			}
		}
		if (animationState == ANI_STATE_AVOID) {
			if (GetAnimator()->IsOneCycleEnd()) {
				SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
				SetAnimationState(ANI_STATE_IDLE);
				m_fIdleTime = 0.0f;
				SetEnableCollision(true);
			}
			else 
			{
				param.force.x *= 20000.f;
				param.force.z *= 20000.f;
			}
		}
		else if (animationState == ANI_STATE_ATTACK) {
			if (GetAnimator()->IsOneCycleEnd()) {
				m_pWeapon->SetEnableCollision(false);
				if (glm::length(GetVelocity()) == 0.0f) {
					SetAnimationState(ANI_STATE_IDLE);
					m_fIdleTime = 0.0f;
				}
				else {
					SetAnimationState(ANI_STATE_WALK);
				}
			}
		}
		else if (animationState == ANI_STATE_HIT) {
			if (GetAnimator()->IsOneCycleEnd()) {
				m_pWeapon->SetEnableCollision(false);
				if (glm::length(GetVelocity()) == 0.0f) {
					SetAnimationState(ANI_STATE_IDLE);
					m_fIdleTime = 0.0f;
				}
				else {
					SetAnimationState(ANI_STATE_WALK);
				}
			}
		}
		else if (animationState == ANI_STATE_RECOVER) {
			if (!m_bIsRecovering) {
				if (glm::length(GetVelocity()) == 0.0f) {
					SetAnimationState(ANI_STATE_IDLE);
					m_fIdleTime = 0.0f;
				}
				else {
					SetAnimationState(ANI_STATE_WALK);
				}
			}
		}

		// 60프레임으로 가정하고 업데이트하여 동일한 힘만 받도록 처리
		CGameObject::Update(1.0f / 60.0f, param);
	}

	CGameObject::Update(elapsedTimeInSec, parameters);
}

void CHeroObject::ProcessInputs(float elapsedTimeInSec, GameInputs* inputs)
{
	// 적에게 잡힌 상태에서는 움직임 불가
	if (m_bIsHeldByEnemy) return;

	GameUpdateParameters param;
	memset(&param, 0, sizeof(param));

	glm::vec3 force = glm::vec3(0.0f, 0.0f, 0.0f);

	float angle = m_fCameraHorizontalAngle + PI_VALUE / 2.0f;
	glm::vec3 dir = normalize(glm::vec3(-glm::cos(angle), 0.0f, glm::sin(angle)));
	glm::vec3 normal = normalize(glm::vec3(dir.z, 0.0f, -dir.x));

	// 이동
	if (inputs->KeyW) force += dir;
	if (inputs->KeyS) force -= dir;
	if (inputs->KeyA) force += normal;
	if (inputs->KeyD) force -= normal;

	GameObjectAnimationState animationState = GetAnimationState();

	// 점프
	if (inputs->KeySpace) {
		if (!m_bIsJumping) {
			if (GetState() == STATE_GROUND) {
				if (animationState != ANI_STATE_AVOID) {
					if (UseStemina(10)) {
						m_bIsJumping = true;
						m_fJumpTime = 0.3f;
					}
				}
			}
		}
	}

	// 회피
	if (inputs->KeyLeftShift) {
		if (animationState != ANI_STATE_AVOID) {
			if (UseStemina(15)) {
				if (animationState == ANI_STATE_ATTACK) {
					m_pWeapon->SetEnableCollision(false);
				}
				CSoundPlayer::GetInstance()->PlaySound(m_dashSoundID, 1.0f);
				SetAnimationState(ANI_STATE_AVOID);
				GetAnimator()->SetIsLoop(false);
				GetAnimator()->SetSpeed(1.2f);
				SetEnableCollision(false);
			}
		}
	}

	// 공격
	if (inputs->MouseLeftButton) {
		if (animationState != ANI_STATE_ATTACK && animationState != ANI_STATE_AVOID && animationState != ANI_STATE_HIT) {
			if (m_pTurnBattleObject->GetBattleVariable().condition != BURN_CONDITION) {
				SetAnimationState(ANI_STATE_ATTACK);
				CSoundPlayer::GetInstance()->PlaySound(m_attackSoundID, 1.0f);
				GetAnimator()->SetIsLoop(false);
				GetAnimator()->SetSpeed(0.5f);
				m_pWeapon->SetEnableCollision(true);
				m_pWeapon->SetMass(30.0f);
			}
		}
	}

	float horizonForceAmount = 10000.0f;
	if (animationState == ANI_STATE_ATTACK) {
		// 공격 중 일경우 속도 감소
		horizonForceAmount /= 20.0f;
	}
	else if (animationState == ANI_STATE_AVOID || animationState == ANI_STATE_HIT) {
		// 회피, 피격 중에는 조작 이동 불가
		horizonForceAmount *= 0.0f;
	}

	param.force = glm::vec3(
		force.x * horizonForceAmount,
		0.0f,
		force.z * horizonForceAmount);

	glm::vec3 prevVelocity = glm::vec3(GetVelocity().x, 0.0f, GetVelocity().z);

	CGameObject::Update(elapsedTimeInSec, param);

	if (animationState == ANI_STATE_WALK || animationState == ANI_STATE_IDLE || animationState == ANI_STATE_IDLE_LONG) {
		// 상하좌우 키 입력이 없을 경우 즉시 멈춤
		if (!inputs->KeyW && !inputs->KeyS && !inputs->KeyA && !inputs->KeyD) {
			SetVelocity(glm::vec3(0.0f, GetVelocity().y, 0.0f));

			if (GetIsAnimate()) {
				if (glm::length(prevVelocity) != 0.0f) {
					SetAnimationState(ANI_STATE_IDLE);
					m_fIdleTime = 0.0f;
				}
			}
		}
		else {
			// 애니메이션 스테이트 계산
			if (GetIsAnimate()) {
				glm::vec3 prev = prevVelocity;
				glm::vec3 now = GetVelocity();

				if (glm::length(prev) == 0.0f) {
					if (glm::length(now) != 0.0f) {
						SetAnimationState(ANI_STATE_WALK);
					}
				}
				else {
					if (glm::length(now) == 0.0f) {
						SetAnimationState(ANI_STATE_IDLE);
						m_fIdleTime = 0.0f;
					}
				}
			}

			if (m_walkSoundCoolTime <= 0.0f) {
				CSoundPlayer::GetInstance()->PlaySound(m_walkSoundID, 0.5f);
				m_walkSoundCoolTime = 0.38f;
			}
		}
	}

	if (m_walkSoundCoolTime > 0.0f) {
		m_walkSoundCoolTime -= elapsedTimeInSec;
	}
}

void CHeroObject::SpectrumUpdater(float elapsedTimeInSec, CObjectManager* objectManager)
{
	if (GetIsAnimate()) {
		if (GetAnimationState() == ANI_STATE_ATTACK || GetAnimationState() == ANI_STATE_AVOID) {
			if (GetAnimationState() == ANI_STATE_ATTACK) {
				if (GetAnimator()->GetAnimationCurrentTime() >= 0.5f) {
					return;
				}
			}
			if (m_fSpectrumCoolTime < 0.f) {
				m_fSpectrumCoolTime = DEFAULT_SPECTRUM_COOL_TIME;
				int spectrumID = objectManager->AddObject<CGameObject>(
					GetPosition(),
					GetRotation(),
					GetSize(),
					GetMass());
				CGameObject* spectrumObject = objectManager->GetGameObject<CGameObject>(spectrumID);
				spectrumObject->SetTextureIndex(GetTextureIndex());
				spectrumObject->SetIsCullingFace(GetIsCullingFace());
				spectrumObject->SetType(TYPE_FIXED);
				spectrumObject->SetState(STATE_GROUND);
				spectrumObject->SetEnableCollision(false);
				spectrumObject->SetIsAnimate(true);
				spectrumObject->SetModelWithBone("player");
				spectrumObject->SetIsMadeShadow(false);
				spectrumObject->SetBorder(true);
				spectrumObject->SetAnimationDataNameMap(GetAnimationDataNameMap());
				spectrumObject->SetAnimationState(GetAnimationState());
				spectrumObject->SetColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
				spectrumObject->SetLifeTime(DEFAULT_SPECTRUM_MAINTAIN_TIME);
				spectrumObject->GetAnimator()->SetAnimationCurrentTime(GetAnimator()->GetAnimationCurrentTime());
				spectrumObject->GetAnimator()->UpdateAnimation(0.0f);
				spectrumObject->GetAnimator()->SetIsStop(true);
				spectrumObject->SetBorderColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.75f));
			}
			m_fSpectrumCoolTime -= elapsedTimeInSec;
		}
		else {
			m_fSpectrumCoolTime = DEFAULT_SPECTRUM_COOL_TIME;
		}
	}
}

void CHeroObject::SetCameraPosWithHeroPos(CCamera* camera)
{
	glm::vec3 heroPos = GetPosition();
	
	// 거리 계산
	if (m_fCameraDistance < m_fAngleChangeStartDistance) {	
		float distanceRate = m_fCameraDistance / m_fAngleChangeStartDistance;
		float additionalZDistance = 3.8f * (1.0f - distanceRate);
		float cameraZDistance = m_fCameraDistance * distanceRate + additionalZDistance;

		camera->SetCameraPosition(glm::vec3(heroPos.x, heroPos.y + m_fCameraDistance, heroPos.z + cameraZDistance));
	}
	else {
		camera->SetCameraPosition(glm::vec3(heroPos.x, heroPos.y + m_fCameraDistance, heroPos.z + m_fCameraDistance));
	}
}

void CHeroObject::SetScrollWheel(CCamera* camera, float elapsedTimeInSec, bool wheelUp)
{
	if (wheelUp)
		m_fCameraDistance -= (elapsedTimeInSec * MOUSE_WHEEL_SPEED);
	else
		m_fCameraDistance += (elapsedTimeInSec * MOUSE_WHEEL_SPEED);

	// 최대 최소 거리 제한
	m_fCameraDistance = glm::clamp(m_fCameraDistance, m_fMinDistance, m_fMaxDistance);

	//camera->SetQuarterCameraAngleWithDistance(m_fCameraDistance, m_fMinDistance, m_fAngleChangeStartDistance);
}

void CHeroObject::ControlBackViewCamera(CCamera* camera, float elapsedTimeInSec, GameInputs* inputs, bool mouse)
{
	// 마우스 이동에 의한 카메라 회전 계산
	if (mouse)
	{
		int x, y;
		CRenderer::GetInstance()->GetDisplayResolution(&x, &y);

		m_fCameraHorizontalAngle +=
			m_fCameraSpeed * elapsedTimeInSec * float(x / 2 - inputs->MouseXPos);

		m_fCameraVerticalAngle +=
			m_fCameraSpeed / 2 * elapsedTimeInSec * float(y / 2 - inputs->MouseYPos);

		// 수직 각 제한
		m_fCameraVerticalAngle = glm::clamp(m_fCameraVerticalAngle, -PI_VALUE / 4.0f, PI_VALUE / 4.0f);
		m_fCameraHorizontalAngle = glm::mod(m_fCameraHorizontalAngle, 2.0f * PI_VALUE);
	}

	CalculateBackViewCamera(camera);
}

void CHeroObject::CalculateBackViewCamera(CCamera* camera)
{
	// 계산된 각도로 필요 벡터 계산 뒤 적용
	glm::vec3 direction = glm::vec3(
		glm::cos(m_fCameraVerticalAngle) * glm::sin(m_fCameraHorizontalAngle),
		glm::sin(m_fCameraVerticalAngle),
		glm::cos(m_fCameraVerticalAngle) * glm::cos(m_fCameraHorizontalAngle));

	glm::vec3 right = glm::vec3(
		glm::sin(m_fCameraHorizontalAngle - PI_VALUE / 2.0f),
		0.0f,
		glm::cos(m_fCameraHorizontalAngle - PI_VALUE / 2.0f));
	
	glm::vec3 up = glm::cross(right, direction);

	// 이전에 계산되어서 충돌처리된 카메라 충돌체의 위치로 계산된 결과 적용
	camera->SetCamera(m_pCameraCollisionObject->GetPosition(), m_vPrevDirection, m_vPrevRight, m_vPrevUp, 60.0f);

	// 카메라 충돌체에 계산된 결과 적용
	m_pCameraCollisionObject->SetPosition(GetPosition() - glm::normalize(direction) * m_fCameraDistance);
	m_pCameraCollisionObject->SetVelocity(-glm::normalize(direction));

	m_vPrevDirection = direction;
	m_vPrevRight = right;
	m_vPrevUp = up;
}

void CHeroObject::ControlBackViewCameraInTurnMode(CCamera* camera,
	float elapsedTimeInSec, GameInputs* inputs, float horizontalAngle)
{
	// 수직 각 제한
	m_fCameraVerticalAngle = -PI_VALUE / 6.0f;

	// 계산된 각도로 필요 벡터 계산 뒤 적용
	glm::vec3 direction = glm::vec3(
		glm::cos(m_fCameraVerticalAngle) * glm::sin(horizontalAngle),
		glm::sin(m_fCameraVerticalAngle),
		glm::cos(m_fCameraVerticalAngle) * glm::cos(horizontalAngle));

	glm::vec3 right = glm::vec3(
		glm::sin(horizontalAngle - PI_VALUE / 2.0f),
		0.0f,
		glm::cos(horizontalAngle - PI_VALUE / 2.0f));

	glm::vec3 up = glm::cross(right, direction);

	camera->SetCamera(GetPosition() - glm::normalize(direction) * m_fCameraDistance, direction, right, up, 60.0f);

	// 카메라에 맞추어 주인공 회전 및 무기 위치 설정
	GameUpdateParameters param;
	memset(&param, 0, sizeof(param));

	glm::vec3 force = glm::vec3(0.0f, 0.0f, 0.0f);
	force += direction;
	param.force = glm::vec3(
		force.x,
		0.0f,
		force.z);

	CGameObject::Update(0.f, param);
	SetWeaponPosition();
}

void CHeroObject::ControlLookAtCameraInTurnMode(CCamera* camera, float elapsedTimeInSec, GameInputs* inputs,
	float horizontalAngle, glm::vec3 lookPosition)
{
	// 수직 각 제한
	m_fCameraVerticalAngle = -PI_VALUE / 8.0f;

	// 계산된 각도로 필요 벡터 계산 뒤 적용
	glm::vec3 direction = glm::vec3(
		glm::cos(m_fCameraVerticalAngle) * glm::sin(horizontalAngle),
		glm::sin(m_fCameraVerticalAngle),
		glm::cos(m_fCameraVerticalAngle) * glm::cos(horizontalAngle));

	glm::vec3 right = glm::vec3(
		glm::sin(horizontalAngle - PI_VALUE / 2.0f),
		0.0f,
		glm::cos(horizontalAngle - PI_VALUE / 2.0f));

	glm::vec3 up = glm::cross(right, direction);

	camera->SetCamera(lookPosition - glm::normalize(direction) * m_fCameraDistance, direction, right, up, 45.0f);
}

void CHeroObject::SetFrontViewCameraInTurnMode(CCamera* camera, float horizontalAngle)
{
	// 원래 각도에 180도 더해서 정반대로 항하도록 변경
	float frontAngle = horizontalAngle + PI_VALUE;

	// 계산된 각도로 필요 벡터 계산 뒤 적용
	glm::vec3 direction = glm::vec3(
		glm::cos(m_fCameraVerticalAngle) * glm::sin(frontAngle),
		glm::sin(m_fCameraVerticalAngle),
		glm::cos(m_fCameraVerticalAngle) * glm::cos(frontAngle));

	glm::vec3 right = glm::vec3(
		glm::sin(frontAngle - PI_VALUE / 2.0f),
		0.0f,
		glm::cos(frontAngle - PI_VALUE / 2.0f));

	glm::vec3 up = glm::cross(right, direction);

	camera->SetCamera(GetPosition() - glm::normalize(direction) * m_fCameraDistance, direction, right, up, 45.0f);
}

void CHeroObject::SetTurnBattleObject(CTurnBattleObject* object)
{
	m_pTurnBattleObject = object;
	SetModelIndex(object->GetModelIndex());
	SetTextureIndex(object->GetTextureIndex());
}

CTurnBattleObject* CHeroObject::GetTurnBattleObject()
{
	return m_pTurnBattleObject;
}

void CHeroObject::InitStemina()
{
	m_sLevel = m_pTurnBattleObject->GetStatus().level;
	m_fMaxStemina =
		(float)m_pTurnBattleObject->GetBattleVariable().sp / (float)m_pTurnBattleObject->GetStatus().maxSp
		* DEFAULT_STEMINA_AMOUNT * (1.0f + ((float)m_sLevel / (float)100));

	m_fStemina = m_fMaxStemina;
}

void CHeroObject::GetStemina(float* stemina, float* maxStemina)
{
	*stemina = m_fStemina;
	*maxStemina = m_fMaxStemina;
}

bool CHeroObject::UseStemina(float amount)
{
	if (m_fStemina >= amount) {
		m_fStemina -= amount;
		return true;
	}
	return false;
}

void CHeroObject::SetWeaponObject(CGameObject* object)
{
	m_pWeapon = object;
}

void CHeroObject::GetCameraAngle(float* horizontalAngle, float* verticalAngle)
{
	*horizontalAngle = m_fCameraHorizontalAngle;
	*verticalAngle = m_fCameraVerticalAngle;
}

void CHeroObject::SetCameraAngle(float horizontalAngle, float verticalAngle)
{
	m_fCameraHorizontalAngle = horizontalAngle;
	m_fCameraVerticalAngle = verticalAngle;
}

void CHeroObject::RecoverStemina(float elapsedTimeInSec)
{
	// 스테미나 회복
	m_fStemina += (1.0f + ((float)m_sLevel / (float)100)) * DEFAULT_STAMINA_RECOVER_SPEED * elapsedTimeInSec;
	if (m_fStemina > m_fMaxStemina) {
		m_fStemina = m_fMaxStemina;
	}
}

void CHeroObject::SetWeaponPosition()
{
	if (GetAnimationState() == ANI_STATE_ATTACK) {

		glm::quat modelQuat
			= glm::quat(glm::vec3(glm::radians(GetRotation().x), glm::radians(GetRotation().y), glm::radians(GetRotation().z)));
		glm::mat4 rotationMat = glm::toMat4(modelQuat);

		float radius = 4.5f;
		float time = GetAnimator()->GetAnimationCurrentTime() * 2 - 0.15f;
		if (time < 0 || time > 1.0f) {
			m_pWeapon->SetEnableCollision(false);
		}
		else if (0.0f <= time && time < 0.5f) {
			m_pWeapon->SetEnableCollision(true);
		}
		time = glm::clamp(time, 0.0f, 1.0f);

		float timeToRadianAngle = PI_VALUE * time;
		glm::vec3 pos = glm::vec3(0.0f, glm::sin(timeToRadianAngle), glm::cos(timeToRadianAngle));
		pos = glm::normalize(pos);

		glm::vec3 finalPos = rotationMat * glm::vec4(pos * glm::vec3(radius, radius, radius), 1.0f);

		m_pWeapon->SetPosition(glm::vec3(finalPos.x, finalPos.y - 1.0f, finalPos.z));
		m_pWeapon->StickToParent();
	}
}

CGameObject* CHeroObject::GetWeaponObject()
{
	return m_pWeapon;
}

void CHeroObject::SetCameraCollisionObject(CGameObject* object)
{
	m_pCameraCollisionObject = object;
	InitCameraCollisionObject();
}

CGameObject* CHeroObject::GetCameraCollisionObject()
{
	return m_pCameraCollisionObject;
}

void CHeroObject::InitCameraCollisionObject()
{
	glm::vec3 direction = glm::vec3(
		glm::cos(m_fCameraVerticalAngle) * glm::sin(m_fCameraHorizontalAngle),
		glm::sin(m_fCameraVerticalAngle),
		glm::cos(m_fCameraVerticalAngle) * glm::cos(m_fCameraHorizontalAngle));

	glm::vec3 right = glm::vec3(
		glm::sin(m_fCameraHorizontalAngle - PI_VALUE / 2.0f),
		0.0f,
		glm::cos(m_fCameraHorizontalAngle - PI_VALUE / 2.0f));

	glm::vec3 up = glm::cross(right, direction);

	m_vPrevDirection = direction;
	m_vPrevRight = right;
	m_vPrevUp = up;

	m_pCameraCollisionObject->SetPosition(GetPosition() - glm::normalize(direction) * m_fCameraDistance);
	m_pCameraCollisionObject->SetVelocity(-glm::normalize(direction));
}

void CHeroObject::MatchAnimationWithTurnBattleObject()
{
	if (m_pTurnBattleObject) {
		if (GetIsAnimate()) {
			SetAnimationState(m_pTurnBattleObject->GetAnimationState());

			CAnimator* animator = m_pTurnBattleObject->GetAnimator();
			GetAnimator()->SetAnimationCurrentTime(animator->GetAnimationCurrentTime());
			GetAnimator()->SetIsLoop(animator->GetIsLoop());
			GetAnimator()->SetSpeed(animator->GetSpeed());
		}
	}
}

void CHeroObject::SetRecoverAnimation(float time)
{
	if (time <= 0.0f) {
		if (m_fRecoverCoolTime > 0.0f) {
			float color = DEFAULT_RECOVER_SPOT_USE_TIME - (DEFAULT_RECOVER_SPOT_COOL_TIME - m_fRecoverCoolTime);
			SetBorderColor(glm::vec4(0.0f, color, color, color));
			m_fRecoverCoolTime -= CBlackBoard::GetInstance()->GetElapsedTimeInSec() * 2;
		}
		else {
			SetBorder(false);
			SetBorderColor(glm::vec4(1.0f));
		}
		m_bIsRecovering = false;
		return;
	}
	SetAnimationState(ANI_STATE_RECOVER);
	GetAnimator()->SetAnimationCurrentTime(time / 2.0f);
	GetAnimator()->SetIsLoop(false);
	SetBorder(true);
	SetBorderColor(glm::vec4(0.0f, time, time, time));
	m_bIsRecovering = true;
}

void CHeroObject::SetRecoverCoolTime()
{
	m_fRecoverCoolTime = DEFAULT_RECOVER_SPOT_COOL_TIME;
}

void CHeroObject::SetIsHeldByEnemy(bool hold)
{
	m_bIsHeldByEnemy = hold;
	SetEnableCollision(!hold);
	SetState(STATE_GROUND);
	GetAnimator()->SetIsStop(hold);
}
