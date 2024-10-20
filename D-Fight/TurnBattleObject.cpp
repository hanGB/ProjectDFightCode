#include "stdafx.h"
#include "TurnBattleObject.h"

CTurnBattleObject::CTurnBattleObject()
{

}

CTurnBattleObject::~CTurnBattleObject()
{
}

void CTurnBattleObject::Update(float elapsedTimeInSec, GameUpdateParameters parameters)
{
	// 데미지 텍스트 표시 시간 감소
	if (m_bIsShowedDamageText) {
		m_fShowingDamageTextTime -= elapsedTimeInSec;

		// 끝나가면 점점 투명해지도록 설정
		if (m_fShowingDamageTextTime < DEFAULT_SHOW_DAMAGE_TEXT_TIME / 2.f) {
			m_vDamageTextColor =
				glm::vec4(m_vDamageTextColor.r, m_vDamageTextColor.g, m_vDamageTextColor.b,
					m_fShowingDamageTextTime / (DEFAULT_SHOW_DAMAGE_TEXT_TIME / 2.f));
		}

		if (m_fShowingDamageTextTime <= 0.0f)
			m_bIsShowedDamageText = false;
	}

	if (GetAnimator()) {
		if (!GetAnimator()->GetIsLoop()) {
			if (GetAnimator()->IsOneCycleEnd()) {
				SetAnimationState(ANI_STATE_IDLE);
			}
		}
		GetAnimator()->UpdateAnimation(elapsedTimeInSec);
	}
}

bool CTurnBattleObject::UpdateDownLevelForHybridBattle(float elapsedTimeInSec)
{
	// 행동 불가 상태(다운 수치가 있을 경우) 참을 리턴

	if (m_BattleVariable.down == 0) {
		m_fDownRecoverTime = DEFAULT_DOWN_RECOVER_TIME;
		return false;
	}

	if (m_fDownRecoverTime <= 0) {
		m_BattleVariable.down--;

		if (m_BattleVariable.down == 3) {
			// 존재하지 않는 다운 레벨 처리
			m_BattleVariable.down--;
			m_fDownRecoverTime = DEFAULT_DOWN_RECOVER_TIME;
		}
		else if (m_BattleVariable.down == DOWN_LEVEL_FALL) {
			// fall일 경우 2배의 시간 소모
			m_fDownRecoverTime = DEFAULT_DOWN_RECOVER_TIME * 2.0f;
		}
		else {
			m_fDownRecoverTime = DEFAULT_DOWN_RECOVER_TIME;
		}
	}

	m_fDownRecoverTime -= elapsedTimeInSec;
	return true;
}

TurnBattle::SStatus CTurnBattleObject::GetStatus() const
{
	return m_Status;
}

void CTurnBattleObject::SetStatus(TurnBattle::SStatus status)
{
	m_Status = status;
}

const wchar_t* CTurnBattleObject::GetName() const
{
	return m_wName;
}

void CTurnBattleObject::SetName(const wchar_t* name)
{
	wcscpy_s(m_wName, name);
}

TurnBattle::SBattleVariable CTurnBattleObject::GetBattleVariable() const
{
	return m_BattleVariable;
}

void CTurnBattleObject::SetBattleVariable(TurnBattle::SBattleVariable battleVariable)
{
	m_BattleVariable = battleVariable;
}

void CTurnBattleObject::SetIndexBingStolen(short automata, short skillDeck)
{
	m_sAutomataBeingStolen = automata;
	m_sSkillDeckBeingStolen = skillDeck;
}

void CTurnBattleObject::GetIndexBeingStolen(short* automata, short* skillDeck)
{
	*automata = m_sAutomataBeingStolen;
	*skillDeck = m_sSkillDeckBeingStolen;
}

void CTurnBattleObject::SetCameraPosAndDirectionWithNumber(
	CCamera* camera, int order, int partySize, bool self, bool enemy)
{
	glm::vec3 pos = GetPosition();
	glm::vec3 volume = GetVolume();

	float verticalAngle = 0;
	float horizontalAngle = 0;

	// 자신을 바라볼 때(스킬 시전 등) 
	if (self)
	{
		horizontalAngle = 0;
		if (enemy)
		{
			// 적 몬스터면 반전
			horizontalAngle = PI_VALUE;
			if (m_bIsBoss) {
				camera->SetCameraPosition(
					glm::vec3(pos.x, pos.y + volume.y / 2.0f * 1.5f, pos.z + volume.z + m_fDistance / 2.0f * 1.5f));
			}
			else {
				camera->SetCameraPosition(glm::vec3(pos.x, pos.y + volume.y / 2.0f, pos.z + volume.z + m_fDistance / 2.0f));
			}
		}
		else
		{
			camera->SetCameraPosition(glm::vec3(pos.x, pos.y + volume.y / 2.0f, pos.z - volume.z - m_fDistance / 2.0f));
		}
	}
	// 적을 바라볼 때
	else
	{
		if (enemy)
		{
			verticalAngle = -PI_VALUE / 9;
			horizontalAngle =  -(PI_VALUE / (partySize * 6) * (order - (partySize / 2)));
			if (m_bIsBoss) {
				camera->SetCameraPosition(
					glm::vec3(pos.x, pos.y + volume.y + m_fCameraHight * 1.5f, pos.z - volume.z - m_fDistance * 1.5f));
			}
			else {
				camera->SetCameraPosition(
					glm::vec3(pos.x, pos.y + volume.y + m_fCameraHight, pos.z - volume.z - m_fDistance));
			}
			
		}
		else {
			verticalAngle = -PI_VALUE / 9;
			horizontalAngle = PI_VALUE + (PI_VALUE / (partySize * 6) * (order - (partySize / 2)));
			camera->SetCameraPosition(glm::vec3(pos.x, pos.y + volume.y + m_fCameraHight, pos.z + volume.z + m_fDistance));
		}
	}
	camera->SetQuarterCameraAngle(horizontalAngle, verticalAngle);
}

std::vector<short> CTurnBattleObject::GetSkillVector()
{
	return m_SkillVector;
}

void CTurnBattleObject::GiveSkill(short skillID)
{
	m_SkillVector.push_back(skillID);
}

std::vector<short> CTurnBattleObject::GetAutomataVector()
{
	return m_AutomataVector;
}

void CTurnBattleObject::GiveAutomata(short automataID)
{
	m_AutomataVector.push_back(automataID);
}

std::vector<short> CTurnBattleObject::GetDeckVector()
{
	return m_DeckVector;
}

void CTurnBattleObject::GiveDeck(short deckID)
{
	m_DeckVector.push_back(deckID);
}

void CTurnBattleObject::RemoveDeck(int location)
{
	size_t size = m_DeckVector.size();
	m_DeckVector.erase(m_DeckVector.begin() + location);
	m_DeckVector.resize(size - 1);
}

void CTurnBattleObject::SetAttributeAndSkillWithDeck(TurnBattle::SDeck* deck)
{
	memcpy(m_Status.attribute, deck->attribute, sizeof(m_Status.attribute));
	m_SkillVector.clear();

	for (int i = 0; i < deck->skillSize; ++i) {
		m_SkillVector.push_back(deck->skillList[i]);
	}

	m_sAttackAttribute = deck->attackAttribute;
}

void CTurnBattleObject::ChangeSkillAt(int index, short skillID)
{
	m_SkillVector.at(index) = skillID;
}

bool CTurnBattleObject::CheckEnableSkill(TurnBattle::SSkill* skill)
{
	if (skill->turn > m_BattleVariable.turn)
		return false;
	if (skill->sp > m_BattleVariable.sp)
		return false;
	return true;
}

void CTurnBattleObject::RenderHPBarOnTop(CRenderer* renderer, bool isHeroParty)
{
	glDisable(GL_DEPTH_TEST);

	glm::vec3 position = GetPosition();
	glm::vec3 volume = GetVolume();

	float gap = 1.0f;
	float gapX = 0.0f;
	float multiply = 1.0f;

	glm::vec3 location = glm::vec3(position.x + gapX, position.y + volume.y + gap, position.z);

	// 흰색으로 채우기
	renderer->RenderTexture3D(m_UIBackgroundTextureID, location, glm::vec2(1.0f * multiply, 0.1f * multiply),
		glm::vec4(1.0f, 1.0f, 1.0f, 0.25f));

	float hpRate = (float)m_BattleVariable.hp / (float)m_Status.maxHp;

	// 붉은색으로 채우기
	if (isHeroParty) {
		renderer->RenderTexture3D(m_UIBackgroundTextureID, glm::vec3(location.x + (1.0f - hpRate), location.y, location.z),
			glm::vec2(hpRate * multiply, 0.1f * multiply), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	}
	else {
		renderer->RenderTexture3D(m_UIBackgroundTextureID, glm::vec3(location.x - (1.0f - hpRate), location.y, location.z),
			glm::vec2(hpRate * multiply, 0.1f * multiply), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	}

	glEnable(GL_DEPTH_TEST);
}

void CTurnBattleObject::RenderHPBarOnTopForHybridBattle(CRenderer* renderer)
{
	glDisable(GL_DEPTH_TEST);

	glm::vec3 position = GetPosition();
	glm::vec3 volume = GetVolume();

	float gap = 1.0f;
	float gapX = 0.0f;
	float multiply = 1.0f;
	if (m_bIsBoss) {
		gap = 0.0f;
		gapX = -4.0f;
		multiply = 2.f;
	}

	glm::vec3 location = glm::vec3(position.x + gapX, position.y + volume.y + gap, position.z);

	float hpRate = (float)m_BattleVariable.hp / (float)m_Status.maxHp;

	renderer->RenderTexture3D(m_UIBackgroundTextureID, glm::vec3(location.x - (1.0f - hpRate), location.y, location.z),
			glm::vec2(hpRate * multiply, 0.1f * multiply), glm::vec4(0.2f + m_fShowingTurnGauge / 1.25f, 0.0f, 0.0f, 1.0f));

	glEnable(GL_DEPTH_TEST);
}

void CTurnBattleObject::RenderDamageText(CRenderer* renderer, bool isHeroParty)
{
	if (m_bIsShowedDamageText) {
		glDisable(GL_DEPTH_TEST);

		glm::vec3 position = GetPosition();
		glm::vec3 volume = GetVolume();

		float gapX = 0.25f;
		float gapY = 0.35f;
		float multiply = 1.0f;
		if (isHeroParty) {
			gapX = -0.5f;
		}
		if (m_bIsBoss) {
			gapY = -1.0f;
			gapX = -3.5f;
			multiply = 2.f;
		}

		glm::vec3 location = glm::vec3(position.x + gapX, position.y + volume.y + gapY, position.z);

		renderer->RenderText3D(m_wDamageText, 1.5f * multiply, location, m_vDamageTextColor);

		glEnable(GL_DEPTH_TEST);
	}
}

void CTurnBattleObject::RenderDownLevelText(CRenderer* renderer, bool isHeroParty)
{
	short downLevel = m_BattleVariable.down;
	if (downLevel != DOWN_LEVEL_NON) {
		glDisable(GL_DEPTH_TEST);

		glm::vec3 position = GetPosition();
		glm::vec3 volume = GetVolume();
		
		float gapX = -1.0f;
		float gapY = 1.0f;
		float multiply = 1.0f;

		if (isHeroParty)
			gapX = 1.0f;

		if (m_bIsBoss) {
			gapY = -0.0f;
			gapX = -6.0f;
			multiply = 2.f;
		}

		glm::vec3 location = glm::vec3(position.x + gapX, position.y + volume.y + gapY, position.z);

		if (downLevel == DOWN_LEVEL_DOWN1) {
			renderer->RenderText3D(L"DOWN1", 1.0f * multiply, location, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		}
		else if (downLevel == DOWN_LEVEL_DOWN2) {
			renderer->RenderText3D(L"DOWN2", 1.0f * multiply, location, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		}
		else if (downLevel == DOWN_LEVEL_FALL) {
			renderer->RenderText3D(L"FALL", 1.0f * multiply, location, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		}
		else if (downLevel == DOWN_LEVEL_CRASH) {
			renderer->RenderText3D(L"CRASH", 1.0f * multiply, location, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		}

		glEnable(GL_DEPTH_TEST);
	}
}

void CTurnBattleObject::SetIsShowedDamageText(bool show)
{
	m_bIsShowedDamageText = show;
}

void CTurnBattleObject::SetShowingDamageTextTime(float time)
{
	m_fShowingDamageTextTime = time;
}

void CTurnBattleObject::SetDamageText(const wchar_t* text)
{
	wsprintf(m_wDamageText, L"%s", text);
}

void CTurnBattleObject::SetDamageTextColor(glm::vec4 color)
{
	m_vDamageTextColor = color;
}

void CTurnBattleObject::SetIsBoss(bool big)
{
	m_bIsBoss = big;
}

bool CTurnBattleObject::GetIsBoss() const
{
	return m_bIsBoss;
}

void CTurnBattleObject::InitBattleVariable()
{
	m_BattleVariable.attackBuff = 0;
	m_BattleVariable.defenseBuff = 0;
	m_BattleVariable.down = 0;
	m_BattleVariable.avoidLevel = 0;
	m_BattleVariable.exp = 0;
	m_BattleVariable.hp = m_Status.maxHp;
	m_BattleVariable.sp = m_Status.maxSp;
	m_BattleVariable.turn = m_Status.maxTurn;
	m_BattleVariable.condition = NORMAL_CONDITION;
	m_BattleVariable.conditionDuration = 0.f;
}

void CTurnBattleObject::IncreaseGettingTurnGauge(float value)
{
	// 값이 0보다 작은 경우는 턴 모두 소모한 후 밖에 없음으로 이미 업데이트가 되었으면 넣지 않음
	if (value < 0.0f) {
		if (m_bIsUpdatedTurnGaugeWhenEndTurn) {
			return;
		}
		m_fGettingTurnGauge = 0.0f;
		m_fShowingTurnGauge = 0.0f;
		m_bIsUpdatedTurnGaugeWhenEndTurn = true;
	}
	else {
		m_fGettingTurnGauge += value;
		m_fGettingTurnGauge = glm::clamp(m_fGettingTurnGauge, 0.0f, 1.0f);
	}	
}

float CTurnBattleObject::GetGettingTurnGauge() const
{
	return m_fGettingTurnGauge;
}

void CTurnBattleObject::SetGettingTurnGauge(float gauge)
{
	m_fGettingTurnGauge = gauge;
	m_fShowingTurnGauge = gauge;
}

float CTurnBattleObject::GetShowingTurnGauge()
{
	if (m_fGettingTurnGauge > m_fShowingTurnGauge) {
		m_fShowingTurnGauge += CBlackBoard::GetInstance()->GetElapsedTimeInSec() / 1.5f;
	}
	else if (m_fGettingTurnGauge < m_fShowingTurnGauge){
		m_fShowingTurnGauge = m_fGettingTurnGauge;
	}

	return m_fShowingTurnGauge;
}

void CTurnBattleObject::SetIsUpdatedTurnGaugeWhenEndTurn(bool end)
{
	m_bIsUpdatedTurnGaugeWhenEndTurn = end;
}

bool CTurnBattleObject::GetIsUpdatedTurnGaugeWhenEndTurn() const
{
	return m_bIsUpdatedTurnGaugeWhenEndTurn;
}

void CTurnBattleObject::SetAttackAttribute(short attribute)
{
	m_sAttackAttribute = attribute;
}

short CTurnBattleObject::GetAttackAttribute() const
{
	return m_sAttackAttribute;
}

void CTurnBattleObject::GiveAttackDamage(CTurnBattleObject* attacker, bool hero)
{
	// 데미지 적용
	TurnBattle::SStatus attackerStatus = attacker->GetStatus();
	TurnBattle::SBattleVariable attackerBV = attacker->GetBattleVariable();
	short attribute = attacker->GetAttackAttribute();

	wchar_t damageText[MAX_NAME_SIZE];
	glm::vec4 damageTextColor;

	// 스탯으로 대미지 계산 (방어 스텟은 일부만 적용)
	short additionalDamageStat;
	if (attribute == UNIVERSAL || (LIGHT <= attribute && attribute <= DARK)) {
		additionalDamageStat = short((attackerStatus.stat[MATTER_ATTACK] + attackerStatus.stat[MIND_ATTACK]) / 2.0f)
			- short((attackerStatus.stat[MATTER_DEFENSE] + attackerStatus.stat[MIND_DEFENSE]) / 2.0f 
				* DEFAULT_APPLY_DEFENSE_STAT_RATE);
	}
	else if (FIRE <= attribute && attribute <= WIND) {
		additionalDamageStat = attackerStatus.stat[MATTER_ATTACK] 
			- short(attackerStatus.stat[MATTER_DEFENSE] * DEFAULT_APPLY_DEFENSE_STAT_RATE);
	}
	else if (RUIN <= attribute && attribute <= EVIL) {
		additionalDamageStat = attackerStatus.stat[MIND_ATTACK] 
			- short(attackerStatus.stat[MIND_DEFENSE] * DEFAULT_APPLY_DEFENSE_STAT_RATE);
	}
	
	short attackDamage = short(DEFAULT_ENEMY_ATTACK_DAMAGE * additionalDamageStat);

	if (hero) {
		attackDamage = short(DEFAULT_HERO_ATTACK_DAMAGE * additionalDamageStat);
	}

	// 스탯 적용 후 최소 데미지 보다 적을 경우 최소 데미지로 
	if (attackDamage < 1)
		attackDamage = 1;

	// 버프 적용
	short buff = attackerBV.attackBuff - m_BattleVariable.defenseBuff;
	attackDamage = (short)(attackDamage * (float)(1.0f + buff * ADDITIONAL_BUFF));

	// 버프 적용후 최소 데미지 보다 적을 경우 최소 데미지로 
	if (attackDamage < SKILL_MIN_DAMAGE_BEFORE_APPLY_WEAKNESS)
		attackDamage = SKILL_MIN_DAMAGE_BEFORE_APPLY_WEAKNESS;

	// 내성 적용
	damageTextColor = DAMAGE_FONT_COLOR_NORMAL;
	if (attribute != UNIVERSAL) {
		if (m_Status.attribute[attribute] >= WEAK) {
			damageTextColor = DAMAGE_FONT_COLOR_WEAK;
			attackDamage = (short)(attackDamage * 1.5);
		}
		else if (m_Status.attribute[attribute] <= HALF) {
			damageTextColor = DAMAGE_FONT_COLOR_HALF;
			attackDamage = (short)(attackDamage * 0.5);
		}
	}

	// 데미지 텍스트 적용
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
	
	short tempHp = m_BattleVariable.hp;
	m_BattleVariable.hp -= attackDamage;

	if (m_BattleVariable.hp < 0 && tempHp != 1) {
		// hp가 0보다 작을경우 hp 1 회복후 다운레벨을 crash로(너무 약한 적에게 still 작동을 위함)
		m_BattleVariable.hp = 1;
		m_BattleVariable.down = DOWN_LEVEL_CRASH;
	}
	else {
		m_BattleVariable.hp = glm::clamp(m_BattleVariable.hp, (short)0, m_Status.maxHp);
	}

	SetDamageText(damageText);
	SetDamageTextColor(damageTextColor);
	SetIsShowedDamageText(true);
	SetShowingDamageTextTime(DEFAULT_SHOW_DAMAGE_TEXT_TIME);
}

void CTurnBattleObject::InitTurn()
{
	m_BattleVariable.turn = m_Status.maxTurn;
}

void CTurnBattleObject::SetMonsterId(short id)
{
	m_sMonsterId = id;
}

short CTurnBattleObject::GetMonsterId() const
{
	return m_sMonsterId;
}

void CTurnBattleObject::SetIsFixed(bool fix)
{
	m_bFixed = fix;
}

bool CTurnBattleObject::GetIsFixed() const
{
	return m_bFixed;
}

void CTurnBattleObject::SetIsSubObject(bool sub)
{
	m_bIsSubObject = sub;
}

bool CTurnBattleObject::GetIsSubObject() const
{
	return m_bIsSubObject;
}

void CTurnBattleObject::RemoveAllSkill()
{
	m_SkillVector.clear();
	m_SkillVector.resize(0);
}

void CTurnBattleObject::SetUIBackGround(int textureID)
{
	m_UIBackgroundTextureID = textureID;
}

GLuint CTurnBattleObject::GetUIBackGround() const
{
	return m_UIBackgroundTextureID;
}
