#include "stdafx.h"
#include "FieldMonster.h"
#include "SoundPlayer.h"

CFieldMonster::CFieldMonster()
{
	CFieldMonster::BuildBehaviorTree();
}

CFieldMonster::~CFieldMonster()
{
	delete m_pBehaviorTree;
}

void CFieldMonster::Update(float elapsedTimeInSec, GameUpdateParameters parameters)
{
	// 죽었을 경우 업데이트를 하지 않음
	if (m_bIsDead) {
		if (m_fRespawnTime <= 0.0f) {
			m_bIsDead = false;
			m_fDamageCoolTime = 0.0f;
			SetEnableCollision(true);
			return;
		}

		m_fRespawnTime -= elapsedTimeInSec;
		return;
	}

	if (!m_bIsRigidity) {
		m_pBehaviorTree->Run();
	}
	else {
		if (m_fRigidityTime <= 0.0f) {
			m_bIsRigidity = false;
			SetAnimationState(ANI_STATE_WALK);
		}
		m_fRigidityTime -= elapsedTimeInSec;
	}


	if (GetIsAnimate()) {
		if (GetAnimationState() == ANI_STATE_WALK) {
			if (m_fPlayDefaultSoundInterval < 0.0f) {
				m_fPlayDefaultSoundInterval = DEFAULT_SOUND_PLAY_INTERVAL;
				CSoundPlayer::GetInstance()->PlaySoundWithPosition(GetDefaultSoundID(), GetPosition());
			}
			m_fPlayDefaultSoundInterval -= elapsedTimeInSec;
		}
	}

	m_fDamageCoolTime -= elapsedTimeInSec;
	if (m_fDamageCoolTime <= 0.0f)
		m_fDamageCoolTime = 0.0f;

	CGameObject::Update(elapsedTimeInSec, parameters);
}

void CFieldMonster::Render(CRenderer* renderer)
{
	// 죽었을 경우 렌더링 하지 않음
	if (m_bIsDead) {
		return;
	}

	CGameObject::Render(renderer);
	RenderHPBarOnTop(renderer);
}

void CFieldMonster::RenderHPBarOnTop(CRenderer* renderer)
{
	static GLuint s_WhiteRectTextureID = renderer->ReadPNGTextureFile("./resource/ui/whiteRect.png");

	glm::vec3 position = GetPosition();
	glm::vec3 heroPosition = CBlackBoard::GetInstance()->GetHeroPosition();

	position.y = 0;
	heroPosition.y = 0;
	
	float dis = 15.0f;
	if (m_iAreaNumber == 1) {
		dis = 25.0f;
	}

	if (glm::distance(position, heroPosition) < dis) {
		renderer->UseShader(BILLBOARD_SHADER);

		float gap = 3.5f;

		position = GetPosition();
		glm::vec3 location = glm::vec3(position.x, position.y + gap, position.z);

		// 배경으로 채우기
		renderer->RenderTexture3D(s_WhiteRectTextureID, 
			glm::vec3(location.x, location.y - 0.13f, location.z),
			glm::vec2(1.0f, 0.03f), glm::vec4(1.0f, 1.0f, 1.0f, 0.25f));

		float hpRate = (float)m_sHp / (float)m_sMaxHp; 
		renderer->RenderTexture3D(s_WhiteRectTextureID, glm::vec3(location.x - (1.0f - hpRate), location.y, location.z),
			glm::vec2(hpRate, 0.1f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

		renderer->UseShader(DEFAULT_SHADER);
	}
}

float CFieldMonster::GetAngle() const
{
	return m_fAngle;
}

void CFieldMonster::SetAngle(float angle)
{
	m_fAngle = angle;
}

float CFieldMonster::GetAngleSpeed() const
{
	return m_fAngleSpeed;
}

void CFieldMonster::SetAngleSpeed(float speed)
{
	m_fAngleSpeed = speed;
}

int CFieldMonster::GetAreaNumber() const
{
	return m_iAreaNumber;
}

void CFieldMonster::SetAreaNumbe(int number)
{
	m_iAreaNumber = number;
	m_sMaxHp = 100 + (4 - number) * 30;
	m_sHp = m_sMaxHp;
}

bool CFieldMonster::GiveDamageToHp(short damage)
{
	m_sHp -= damage;

	// 죽었을 경우 true 리턴
	if (m_sHp <= 0) {
		m_sHp = 0;
		return true;
	}
	SetRigidity(true);
	return false;
}

bool CFieldMonster::GetIsDeviatedTrack() const
{
	return m_bIsDeviatedTrack;
}

void CFieldMonster::SetIsDeviatedTrack(bool deviate)
{
	m_bIsDeviatedTrack = deviate;
}

bool CFieldMonster::GetIsClockwise() const
{
	return m_bIsClockwise;
}

void CFieldMonster::SetIsClockwise(bool clockwise)
{
	m_bIsClockwise = clockwise;
}

void CFieldMonster::GetScoutRange(float* start, float* end)
{
	*start = m_fScoutRangeStart;
	*end = m_fScoutRangeEnd;
}

void CFieldMonster::SetScoutRange(float start, float end)
{
	m_fScoutRangeStart = start;
	m_fScoutRangeEnd = end;
}

void CFieldMonster::SetRigidity(bool rigidity)
{
	m_bIsRigidity = rigidity;

	if (m_fRigidityTime <= 0.3f) {
		SetAnimationState(ANI_STATE_HIT);
		GetAnimator()->SetIsLoop(false);
	}
	m_fRigidityTime = 1.0f;
}

bool CFieldMonster::GetIsRigidity() const
{
	return m_bIsRigidity;
}

void CFieldMonster::SetIsDead(bool dead)
{
	m_bIsDead = true;
	m_fRespawnTime = DEFAULT_RESPAWN_TIME;
	m_sHp = 100 + (4 - m_iAreaNumber) * 30;
	SetEnableCollision(false);
}

void CFieldMonster::SetBehaviorTree(CBehaviorTree* behaviorTree)
{
	m_pBehaviorTree = behaviorTree;
}

CBehaviorTree* CFieldMonster::GetBehaviorTree() const
{
	return m_pBehaviorTree;
}

void CFieldMonster::SetSpeed(float speed)
{
	m_fSpeed = speed;
}

float CFieldMonster::GetSpeed() const
{
	return m_fSpeed;
}

bool CFieldMonster::IsCanAppliedDamage()
{
	if (m_fDamageCoolTime > 0.0f) return false;

	m_fDamageCoolTime = DEFAULT_DAMAGE_COOL_TIME;
	return true;
}
