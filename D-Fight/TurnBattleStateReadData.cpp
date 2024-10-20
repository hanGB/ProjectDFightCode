#include "stdafx.h"
#include "TurnBattleState.h"

void CTurnBattleState::ReadSkillData(const char* fileName)
{
	// 유니코트로 변경하기 위해 로컬의 언어 설정으로 변경
	setlocale(LC_ALL, "");

	FILE* file;
	fopen_s(&file, fileName, "r");
	if (file == NULL)
	{
		std::cout << fileName << " 을 열 수 없습니다.\n";
		return;
	}
	for (int i = 0; i < SKILL_SIZE; ++i)
	{
		TurnBattle::SSkill* skill = new TurnBattle::SSkill;
		short id;

		char name[MAX_NAME_SIZE];
		char informationLine1[MAX_INFORMATION_SIZE];
		char informationLine2[MAX_INFORMATION_SIZE];
		char skillEffectSprite[MAX_INFORMATION_SIZE / 2];

		short selfParty;
		fscanf_s(file, "%hd %hd %[^\t] %hd %hd %hd %hd %f %hd %f %hd %hd %[^\t] %[^\t]  %[^\t]",
			&id, &skill->attribute, &name, MAX_NAME_SIZE, &skill->turn, &skill->minDamage, &skill->maxDamage, &skill->sp, &skill->rate,
			&skill->effect, &skill->effectRate, &skill->target, &selfParty,
			&informationLine1, MAX_INFORMATION_SIZE, &informationLine2, MAX_INFORMATION_SIZE,
			&skillEffectSprite, MAX_INFORMATION_SIZE / 2);
		skill->selfParty = (bool)selfParty;

		skill->billboardEffect = true;
		skill->effectTime = 1.0f;		// 시간은 1로 동일하며 계산시 프레임 타임에 값을 곱해 속도로 조절

		// char* -> wchar_t* 변경
		size_t count = 0;
		mbstowcs_s(&count, skill->name, name, MAX_NAME_SIZE);
		count = 0;		
		mbstowcs_s(&count, skill->informationLine1, informationLine1, MAX_INFORMATION_SIZE);
		count = 0;
		mbstowcs_s(&count, skill->informationLine2, informationLine2, MAX_INFORMATION_SIZE);

		auto effectSprite = m_SkillEffectSpriteMap.find(skillEffectSprite);
		if (effectSprite != m_SkillEffectSpriteMap.end()) {
			skill->effectData = effectSprite->second;
		}
		else {
			int skillSheet = Renderer()->ReadPNGTextureFile(skillEffectSprite);
			skill->effectData = skillSheet;
			m_SkillEffectSpriteMap.insert(std::pair<std::string, int>((const char*)skillEffectSprite, skillSheet));
		}
		m_SkillMap.insert(std::pair<short, TurnBattle::SSkill*>(id, skill));
	}

	fclose(file);

	// 스킬 사운드 이펙트
	{
		int igSound = SoundPlayer()->LoadSound("./resource/sound/Effect/skill/igSound.wav");
		m_SkillSoundEffectMap.insert(std::pair<short, int>(10, igSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(11, igSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(12, igSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(13, igSound));
		int aqSound = SoundPlayer()->LoadSound("./resource/sound/Effect/skill/aqSound.wav");
		m_SkillSoundEffectMap.insert(std::pair<short, int>(20, aqSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(21, aqSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(22, aqSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(23, aqSound));
		int terSound = SoundPlayer()->LoadSound("./resource/sound/Effect/skill/terSound.wav");
		m_SkillSoundEffectMap.insert(std::pair<short, int>(30, terSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(31, terSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(32, terSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(33, terSound));
		int ventSound = SoundPlayer()->LoadSound("./resource/sound/Effect/skill/ventSound.wav");
		m_SkillSoundEffectMap.insert(std::pair<short, int>(40, ventSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(41, ventSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(42, ventSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(43, ventSound));
		int perSound = SoundPlayer()->LoadSound("./resource/sound/Effect/skill/perSound.wav");
		m_SkillSoundEffectMap.insert(std::pair<short, int>(50, perSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(51, perSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(52, perSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(53, perSound));
		int boniSound = SoundPlayer()->LoadSound("./resource/sound/Effect/skill/boniSound.wav");
		m_SkillSoundEffectMap.insert(std::pair<short, int>(60, boniSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(61, boniSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(62, boniSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(63, boniSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(2060, boniSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(2061, boniSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(2062, boniSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(2063, boniSound));
		int malSound = SoundPlayer()->LoadSound("./resource/sound/Effect/skill/malSound.mp3");
		m_SkillSoundEffectMap.insert(std::pair<short, int>(70, malSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(71, malSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(72, malSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(73, malSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(2070, malSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(2071, malSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(2072, malSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(2073, malSound));
		int luxSound = SoundPlayer()->LoadSound("./resource/sound/Effect/skill/luxSound.wav");
		m_SkillSoundEffectMap.insert(std::pair<short, int>(80, luxSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(81, luxSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(82, luxSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(83, luxSound));
		int noxSound = SoundPlayer()->LoadSound("./resource/sound/Effect/skill/noxSound.wav");
		m_SkillSoundEffectMap.insert(std::pair<short, int>(90, noxSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(91, noxSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(92, noxSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(93, noxSound));
		int fatSound = SoundPlayer()->LoadSound("./resource/sound/Effect/skill/fatSound.wav");
		m_SkillSoundEffectMap.insert(std::pair<short, int>(1000, fatSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(1001, fatSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(1002, fatSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(1003, fatSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(1004, fatSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(1010, fatSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(1011, fatSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(1012, fatSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(1013, fatSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(1014, fatSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(1024, fatSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(1025, fatSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(1030, fatSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(1031, fatSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(1032, fatSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(1040, fatSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(1041, fatSound));
		m_SkillSoundEffectMap.insert(std::pair<short, int>(1042, fatSound));

		m_SkillSoundEffectMap.insert(std::pair<short, int>(990, malSound));

		int attackSound = SoundPlayer()->LoadSound("./resource/sound/Effect/attackSound.mp3");
		m_SkillSoundEffectMap.insert(std::pair<short, int>(0, attackSound));
		int shotSound = SoundPlayer()->LoadSound("./resource/sound/Effect/shotSound.wav");
		m_SkillSoundEffectMap.insert(std::pair<short, int>(1, shotSound));
	}
}

void CTurnBattleState::ReadAutomataData(const char* fileName)
{
	// 유니코트로 변경하기 위해 로컬의 언어 설정으로 변경
	setlocale(LC_ALL, "");

	FILE* file;
	fopen_s(&file, fileName, "r");
	if (file == NULL)
	{
		std::cout << fileName << " 을 열 수 없습니다.\n";
		return;
	}
	for (int i = 0; i < AUTOMATA_SIZE; ++i)
	{
		TurnBattle::SAutomata* automata = new TurnBattle::SAutomata;
		short id;

		char name[MAX_NAME_SIZE];
		char englishName[MAX_NAME_SIZE];
		char skillList[MAX_INFORMATION_SIZE];

		fscanf_s(file, "%hd %hd %[^\t] %[^\t] %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd%hd %hd %hd %hd %hd %hd %[^\t]",
			&id, &automata->attackAttribute, &name, MAX_NAME_SIZE, &englishName, MAX_NAME_SIZE,
			&automata->status.level, &automata->status.maxHp, &automata->status.maxSp, &automata->status.maxTurn,
			&automata->status.attribute[0], &automata->status.attribute[1], &automata->status.attribute[2],
			&automata->status.attribute[3], &automata->status.attribute[4], &automata->status.attribute[5],
			&automata->status.attribute[6], &automata->status.attribute[7], &automata->status.attribute[8],
			&automata->status.stat[0], &automata->status.stat[1], &automata->status.stat[2],
			&automata->status.stat[3], &automata->status.stat[4], &automata->status.stat[5],
			&skillList, MAX_INFORMATION_SIZE);

		// char* -> wchar_t* 변경
		size_t count = 0;
		mbstowcs_s(&count, automata->name, name, MAX_NAME_SIZE);
		count = 0;
		mbstowcs_s(&count, automata->englishName, englishName, MAX_NAME_SIZE);

		// 스킬 데이터 문자열을 스킬 데이터로 변경
		int index = 0;
		char skillID[5];
		int skillIDSize = 0;
		short skillSize = 0;
		while (1) {
			if (skillList[index] == ' ' || skillList[index] == '\0') {
				// 스킬 아이디 문자열을 실제 아이디 값(short)으로 변경
				short id = 0;
				for (int i = 0; i < skillIDSize; ++i) {
					id += (short)((skillID[i] - '0') * pow(10, skillIDSize - 1 - i));
				}
				automata->skillList[skillSize] = id;
				skillIDSize = 0;
				skillSize++;

				if (skillList[index] == '\0') {
					automata->skillSize = skillSize;
					break;
				}
			}
			else {
				skillID[skillIDSize] = skillList[index];
				skillIDSize++;
			}
			skillList[index];
			index++;
		}
		m_AutomataMap.insert(std::pair<short, TurnBattle::SAutomata*>(id, automata));
	}

	fclose(file);
}

void CTurnBattleState::ReadDeckData(const char* fileName)
{
	// 유니코트로 변경하기 위해 로컬의 언어 설정으로 변경
	setlocale(LC_ALL, "");

	FILE* file;
	fopen_s(&file, fileName, "r");
	if (file == NULL)
	{
		std::cout << fileName << " 을 열 수 없습니다.\n";
		return;
	}
	for (int i = 0; i < AUTOMATA_SIZE; ++i)
	{
		TurnBattle::SDeck* deck = new TurnBattle::SDeck;
		short id;

		char name[MAX_NAME_SIZE];
		char strengh[MAX_INFORMATION_SIZE];
		char weak[MAX_INFORMATION_SIZE];
		char information[MAX_INFORMATION_SIZE];
		char skillList[MAX_INFORMATION_SIZE];

		fscanf_s(file, "%hd %hd %[^\t] %[^\t] %[^\t] %[^\t] %hd %hd %hd %hd %hd %hd %hd %hd %hd %[^\t]",
			&id, &deck->attackAttribute, &name, MAX_NAME_SIZE, &strengh, MAX_NAME_SIZE, &weak, MAX_NAME_SIZE, &information, MAX_NAME_SIZE,
			&deck->attribute[0], &deck->attribute[1], &deck->attribute[2],
			&deck->attribute[3], &deck->attribute[4], &deck->attribute[5],
			&deck->attribute[6], &deck->attribute[7], &deck->attribute[8],
			&skillList, MAX_INFORMATION_SIZE);

		// char* -> wchar_t* 변경
		size_t count = 0;
		mbstowcs_s(&count, deck->name, name, MAX_NAME_SIZE);
		count = 0;
		mbstowcs_s(&count, deck->strengh, strengh, MAX_INFORMATION_SIZE);
		count = 0;
		mbstowcs_s(&count, deck->weak, weak, MAX_INFORMATION_SIZE);
		count = 0;
		mbstowcs_s(&count, deck->information, information, MAX_INFORMATION_SIZE);

		// 스킬 데이터 문자열을 스킬 데이터로 변경
		int index = 0;
		char skillID[5];
		int skillIDSize = 0;
		short skillSize = 0;
		while (1) {
			if (skillList[index] == ' ' || skillList[index] == '\0') {
				// 스킬 아이디 문자열을 실제 아이디 값(short)으로 변경
				short id = 0;
				for (int i = 0; i < skillIDSize; ++i) {
					id += (short)((skillID[i] - '0') * pow(10, skillIDSize - 1 - i));
				}
				deck->skillList[skillSize] = id;
				skillIDSize = 0;
				skillSize++;

				if (skillList[index] == '\0') {
					deck->skillSize = skillSize;
					break;
				}
			}
			else {
				skillID[skillIDSize] = skillList[index];
				skillIDSize++;
			}
			skillList[index];
			index++;
		}
		m_DeckMap.insert(std::pair<short, TurnBattle::SDeck*>(id, deck));
	}

	fclose(file);
}

void CTurnBattleState::ReadMonsterData(const char* fileName)
{
	// 유니코트로 변경하기 위해 로컬의 언어 설정으로 변경
	setlocale(LC_ALL, "");

	FILE* file;
	fopen_s(&file, fileName, "r");
	if (file == NULL)
	{
		std::cout << fileName << " 을 열 수 없습니다.\n";
		return;
	}
	for (int i = 0; i < MONSTER_SIZE; ++i)
	{
		TurnBattle::SMonster* monster = new TurnBattle::SMonster;
		short id;

		char name[MAX_NAME_SIZE];
		char skillList[MAX_INFORMATION_SIZE];

		fscanf_s(file, "%hd %hd %[^\t] %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd%hd %hd %hd %hd %hd %hd %[^\t] %hd %hd %hd %hd",
			&id, &monster->attackAttribute, &name, MAX_NAME_SIZE,
			&monster->status.level, &monster->status.maxHp, &monster->status.maxSp, &monster->status.maxTurn,
			&monster->status.attribute[0], &monster->status.attribute[1], &monster->status.attribute[2],
			&monster->status.attribute[3], &monster->status.attribute[4], &monster->status.attribute[5],
			&monster->status.attribute[6], &monster->status.attribute[7], &monster->status.attribute[8],
			&monster->status.stat[0], &monster->status.stat[1], &monster->status.stat[2],
			&monster->status.stat[3], &monster->status.stat[4], &monster->status.stat[5],
			&skillList, MAX_INFORMATION_SIZE, &monster->automata, &monster->deck,
			&monster->modelId, &monster->textureId);

		// char* -> wchar_t* 변경
		size_t count = 0;
		mbstowcs_s(&count, monster->name, name, MAX_NAME_SIZE);

		// 스킬 데이터 문자열을 스킬 데이터로 변경
		int index = 0;
		char skillID[5];
		int skillIDSize = 0;
		short skillSize = 0;
		while (1) {
			if (skillList[index] == ' ' || skillList[index] == '\0') {
				// 스킬 아이디 문자열을 실제 아이디 값(short)으로 변경
				short id = 0;
				for (int i = 0; i < skillIDSize; ++i) {
					id += (short)((skillID[i] - '0') * pow(10, skillIDSize - 1 - i));
				}
				monster->skillList[skillSize] = id;
				skillIDSize = 0;
				skillSize++;

				if (skillList[index] == '\0') {
					monster->skillSize = skillSize;
					break;
				}
			}
			else {
				skillID[skillIDSize] = skillList[index];
				skillIDSize++;
			}
			skillList[index];
			index++;
		}	
		m_MonsterMap.insert(std::pair<short, TurnBattle::SMonster*>(id, monster));
		m_MonsterAnalyzeEndDataMap.insert(std::pair<short, bool>(id, false));

	}

	fclose(file);
}

void CTurnBattleState::LoadMonsterData()
{
	// 모델
	{
		Renderer()->LoadModelWithBone("./resource/dae/monster/slime/slime_sliding.dae", "slime");
		m_MonsterModelMap.insert(std::pair<short, std::string>(1, "slime"));
		Renderer()->LoadModelWithBone("./resource/dae/monster/centipede/centipede_walk.dae", "centipede");
		m_MonsterModelMap.insert(std::pair<short, std::string>(2, "centipede"));
		Renderer()->LoadModelWithBone("./resource/dae/monster/golem/golem_walk.dae", "golem");
		m_MonsterModelMap.insert(std::pair<short, std::string>(3, "golem"));	
		Renderer()->LoadModelWithBone("./resource/dae/monster/boss/boss_wait.dae", "boss");
		m_MonsterModelMap.insert(std::pair<short, std::string>(4, "boss"));
		m_MonsterModelMap.insert(std::pair<short, std::string>(5, "boss"));
	}
	// 애니메이션
	{
		Renderer()->LoadAnimation("./resource/dae/monster/slime/slime_damaged.dae", "slime", "slime_damage");
		Renderer()->LoadAnimation("./resource/dae/monster/slime/slime_death.dae", "slime", "slime_death");
		Renderer()->LoadAnimation("./resource/dae/monster/slime/slime_melee.dae", "slime", "slime_melee");
		Renderer()->LoadAnimation("./resource/dae/monster/slime/slime_sliding.dae", "slime", "slime_walk");
		Renderer()->LoadAnimation("./resource/dae/monster/slime/slime_wait.dae", "slime", "slime_wait");
		Renderer()->LoadAnimation("./resource/dae/monster/slime/slime_jump.dae", "slime", "slime_skill");
		Renderer()->LoadAnimation("./resource/dae/monster/centipede/centipede_damaged.dae", "centipede", "centipede_damage");
		Renderer()->LoadAnimation("./resource/dae/monster/centipede/centipede_death.dae", "centipede", "centipede_death");
		Renderer()->LoadAnimation("./resource/dae/monster/centipede/centipede_hitting.dae", "centipede", "centipede_attack");
		Renderer()->LoadAnimation("./resource/dae/monster/centipede/centipede_walk.dae", "centipede", "centipede_walk");
		Renderer()->LoadAnimation("./resource/dae/monster/centipede/centipede_wait.dae", "centipede", "centipede_wait");
		Renderer()->LoadAnimation("./resource/dae/monster/centipede/centipede_shoot.dae", "centipede", "centipede_skill");
		Renderer()->LoadAnimation("./resource/dae/monster/centipede/centipede_hold.dae", "centipede", "centipede_hold");
		Renderer()->LoadAnimation("./resource/dae/monster/golem/golem_damage.dae", "golem", "golem_damage");
		Renderer()->LoadAnimation("./resource/dae/monster/golem/golem_death.dae", "golem", "golem_death");
		Renderer()->LoadAnimation("./resource/dae/monster/golem/golem_smash.dae", "golem", "golem_smash");
		Renderer()->LoadAnimation("./resource/dae/monster/golem/golem_walk.dae", "golem", "golem_walk");
		Renderer()->LoadAnimation("./resource/dae/monster/golem/golem_wait.dae", "golem", "golem_wait");
		Renderer()->LoadAnimation("./resource/dae/monster/golem/golem_skill.dae", "golem", "golem_skill");
		Renderer()->LoadAnimation("./resource/dae/monster/boss/boss_Mdamage.dae", "boss", "boss_damage");
		Renderer()->LoadAnimation("./resource/dae/monster/boss/boss_death.dae", "boss", "boss_death");
		Renderer()->LoadAnimation("./resource/dae/monster/boss/boss_triplebite.dae", "boss", "boss_bite");
		Renderer()->LoadAnimation("./resource/dae/monster/boss/boss_walking.dae", "boss", "boss_walk");
		Renderer()->LoadAnimation("./resource/dae/monster/boss/boss_wait.dae", "boss", "boss_wait");
		Renderer()->LoadAnimation("./resource/dae/monster/boss/boss_Mbreath.dae", "boss", "boss_skill");
		Renderer()->LoadAnimation("./resource/dae/monster/boss/boss_jump.dae", "boss", "boss_jump");
		Renderer()->LoadAnimation("./resource/dae/monster/boss/boss_Lbreath.dae", "boss", "boss_skill_left");
		Renderer()->LoadAnimation("./resource/dae/monster/boss/boss_Ldamage.dae", "boss", "boss_damage_left");
		Renderer()->LoadAnimation("./resource/dae/monster/boss/boss_Rbreath.dae", "boss", "boss_skill_right");
		Renderer()->LoadAnimation("./resource/dae/monster/boss/boss_Rdamage.dae", "boss", "boss_damage_right");
		Renderer()->LoadAnimation("./resource/dae/monster/boss/boss_phasechange.dae", "boss", "boss_phase_change");
		Renderer()->LoadAnimation("./resource/dae/monster/boss/boss_MdamageII.dae", "boss", "boss_damage_II");
		Renderer()->LoadAnimation("./resource/dae/monster/boss/boss_death.dae", "boss", "boss_death_II");
		Renderer()->LoadAnimation("./resource/dae/monster/boss/boss_biteII.dae", "boss", "boss_bite_II");
		Renderer()->LoadAnimation("./resource/dae/monster/boss/boss_walkingII.dae", "boss", "boss_walk_II");
		Renderer()->LoadAnimation("./resource/dae/monster/boss/boss_waitII.dae", "boss", "boss_wait_II");
		Renderer()->LoadAnimation("./resource/dae/monster/boss/boss_MbreathII.dae", "boss", "boss_skill_II");
		Renderer()->LoadAnimation("./resource/dae/monster/boss/boss_jumpII.dae", "boss", "boss_jump_II");
	}
	{
		std::map<GameObjectAnimationState, std::string> animationDatas;
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_IDLE, "slime_wait"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_WALK, "slime_walk"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_ATTACK, "slime_melee"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_HIT, "slime_damage"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_SKILL_1, "slime_skill"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_SKILL_2, "slime_skill"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_SKILL_3, "slime_skill"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_DEATH, "slime_death"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_JUMP, "slime_skill"));
		m_MonsterAnimationsMap.insert(std::pair<short, std::map<GameObjectAnimationState, std::string>>(1, animationDatas));
	}
	{
		std::map<GameObjectAnimationState, std::string> animationDatas;
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_IDLE, "centipede_wait"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_WALK, "centipede_walk"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_ATTACK, "centipede_attack"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_HIT, "centipede_damage"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_SKILL_1, "centipede_skill"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_SKILL_2, "centipede_skill"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_SKILL_3, "centipede_skill"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_DEATH, "centipede_death"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_HOLD, "centipede_hold"));
		m_MonsterAnimationsMap.insert(std::pair<short, std::map<GameObjectAnimationState, std::string>>(2, animationDatas));
	}
	{
		std::map<GameObjectAnimationState, std::string> animationDatas;
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_IDLE, "golem_wait"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_WALK, "golem_walk"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_ATTACK, "golem_smash"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_HIT, "golem_damage"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_SKILL_1, "golem_skill"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_SKILL_2, "golem_skill"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_SKILL_3, "golem_skill"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_JUMP, "golem_walk"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_DEATH, "golem_death"));
		m_MonsterAnimationsMap.insert(std::pair<short, std::map<GameObjectAnimationState, std::string>>(3, animationDatas));
	}
	{
		std::map<GameObjectAnimationState, std::string> animationDatas;
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_IDLE, "boss_wait"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_WALK, "boss_walk"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_ATTACK, "boss_bite"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_HIT, "boss_damage"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_SKILL_1, "boss_skill"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_SKILL_2, "boss_skill"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_SKILL_3, "boss_skill"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_DEATH, "boss_death"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_JUMP, "boss_jump"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_HIT_LEFT, "boss_damage_left"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_HIT_RIGHT, "boss_damage_right"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_PHASE_CHANGE, "boss_phase_change"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_SKILL_RIGHT, "boss_skill_right"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_SKILL_LEFT, "boss_skill_left"));
		m_MonsterAnimationsMap.insert(std::pair<short, std::map<GameObjectAnimationState, std::string>>(4, animationDatas));
	}
	{
		std::map<GameObjectAnimationState, std::string> animationDatas;
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_IDLE, "boss_wait_II"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_WALK, "boss_walk_II"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_ATTACK, "boss_bite_II"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_HIT, "boss_damage_II"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_SKILL_1, "boss_skill_II"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_SKILL_2, "boss_skill_II"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_SKILL_3, "boss_skill_II"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_DEATH, "boss_death_II"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_JUMP, "boss_jump_II"));
		animationDatas.insert(std::pair<GameObjectAnimationState, std::string>(ANI_STATE_PHASE_CHANGE, "boss_phase_change"));
		m_MonsterAnimationsMap.insert(std::pair<short, std::map<GameObjectAnimationState, std::string>>(5, animationDatas));
	}

	// 텍스쳐
	{
		char allZeroTexture[] = "./resource/texture/AllZeroTexture.jpg";
		char allOneTexture[] = "./resource/texture/AllOneTexture.jpg";
		char planNormalTexture[] = "./resource/texture/planNormalTexture.jpg";

		// 슬라임
		m_MonsterTextureMap.insert(std::pair<short, int>(1,
			Renderer()->LoadPNGTexture(allOneTexture, "./resource/texture/monsterParty/slime/fire.png",
				allOneTexture, DEFAULT_SHININESS, planNormalTexture)));
		m_MonsterTextureMap.insert(std::pair<short, int>(2,
			Renderer()->LoadPNGTexture(allOneTexture, "./resource/texture/monsterParty/slime/water.png",
				allOneTexture, DEFAULT_SHININESS, planNormalTexture)));
		m_MonsterTextureMap.insert(std::pair<short, int>(3,
			Renderer()->LoadPNGTexture(allOneTexture, "./resource/texture/monsterParty/slime/ground.png",
				allOneTexture, DEFAULT_SHININESS, planNormalTexture)));
		m_MonsterTextureMap.insert(std::pair<short, int>(4,
			Renderer()->LoadPNGTexture(allOneTexture, "./resource/texture/monsterParty/slime/wind.png",
				allOneTexture, DEFAULT_SHININESS, planNormalTexture)));
		m_MonsterTextureMap.insert(std::pair<short, int>(5,
			Renderer()->LoadPNGTexture(allOneTexture, "./resource/texture/monsterParty/centipede/centipede_color_b.png",
				allZeroTexture, DEFAULT_SHININESS, "./resource/texture/monsterParty/centipede/centipede_normal.png")));
		m_MonsterTextureMap.insert(std::pair<short, int>(6,
			Renderer()->LoadPNGTexture(allOneTexture, "./resource/texture/monsterParty/centipede/centipede_color_w.png",
				allZeroTexture, DEFAULT_SHININESS, "./resource/texture/monsterParty/centipede/centipede_normal.png")));
		m_MonsterTextureMap.insert(std::pair<short, int>(7,
			Renderer()->LoadPNGTexture(allOneTexture, "./resource/texture/monsterParty/centipede/centipede_color.png",
				allZeroTexture, DEFAULT_SHININESS, "./resource/texture/monsterParty/centipede/centipede_normal.png")));
		m_MonsterTextureMap.insert(std::pair<short, int>(8,
			Renderer()->LoadPNGTexture(allOneTexture, "./resource/texture/monsterParty/golem/golem.jpg",
				allZeroTexture, DEFAULT_SHININESS, planNormalTexture)));
		m_MonsterTextureMap.insert(std::pair<short, int>(9,
			Renderer()->LoadPNGTexture(allOneTexture, "./resource/texture/monsterParty/golem/golemBlack.jpg",
				allZeroTexture, DEFAULT_SHININESS, planNormalTexture)));
		m_MonsterTextureMap.insert(std::pair<short, int>(10,
			Renderer()->LoadPNGTexture(allOneTexture, "./resource/texture/monsterParty/boss/kb_uv.png",
				allZeroTexture, DEFAULT_SHININESS, planNormalTexture)));
	}
	{
		m_MonsterBloodTextureMap.insert(std::pair<short, int>(1,
			Renderer()->ReadPNGTextureFile("./resource/texture/monsterParty/slime/fireSub.png")));
		m_MonsterBloodTextureMap.insert(std::pair<short, int>(2,
			Renderer()->ReadPNGTextureFile("./resource/texture/monsterParty/slime/waterSub.png")));
		m_MonsterBloodTextureMap.insert(std::pair<short, int>(3,
			Renderer()->ReadPNGTextureFile("./resource/texture/monsterParty/slime/groundSub.png")));
		m_MonsterBloodTextureMap.insert(std::pair<short, int>(4,
			Renderer()->ReadPNGTextureFile("./resource/texture/monsterParty/slime/windSub.png")));
		m_MonsterBloodTextureMap.insert(std::pair<short, int>(5,
			Renderer()->ReadPNGTextureFile("./resource/texture/monsterParty/centipede/blood.png")));
		m_MonsterBloodTextureMap.insert(std::pair<short, int>(6,
			Renderer()->ReadPNGTextureFile("./resource/texture/monsterParty/centipede/blood.png")));
		m_MonsterBloodTextureMap.insert(std::pair<short, int>(7,
			Renderer()->ReadPNGTextureFile("./resource/texture/monsterParty/centipede/blood.png")));
		m_MonsterBloodTextureMap.insert(std::pair<short, int>(8,
			Renderer()->ReadPNGTextureFile("./resource/texture/monsterParty/golem/rockParticle.png")));
		m_MonsterBloodTextureMap.insert(std::pair<short, int>(9,
			Renderer()->ReadPNGTextureFile("./resource/texture/monsterParty/golem/rockParticleBlack.png")));
		m_MonsterBloodTextureMap.insert(std::pair<short, int>(10,
			Renderer()->ReadPNGTextureFile("./resource/texture/monsterParty/boss/blood.png")));
	}

	// 사운드 (텍스쳐 아이디와 동일하게 각 아이디 설정)
	{
		
		int slimeSoundId = SoundPlayer()->LoadSound("./resource/sound/Effect/monster/slimeSound.wav");
		m_MonsterDefaultSoundMap.insert(std::pair<short, int>(1, slimeSoundId));
		m_MonsterDefaultSoundMap.insert(std::pair<short, int>(2, slimeSoundId));
		m_MonsterDefaultSoundMap.insert(std::pair<short, int>(3, slimeSoundId));
		m_MonsterDefaultSoundMap.insert(std::pair<short, int>(4, slimeSoundId));
		int centipedeSoundId = SoundPlayer()->LoadSound("./resource/sound/Effect/monster/centipedeSound.wav");
		m_MonsterDefaultSoundMap.insert(std::pair<short, int>(5, centipedeSoundId));
		m_MonsterDefaultSoundMap.insert(std::pair<short, int>(6, centipedeSoundId));
		m_MonsterDefaultSoundMap.insert(std::pair<short, int>(7, centipedeSoundId));
		int golemSoundId = SoundPlayer()->LoadSound("./resource/sound/Effect/monster/golemSound.wav");
		m_MonsterDefaultSoundMap.insert(std::pair<short, int>(8, golemSoundId));
		m_MonsterDefaultSoundMap.insert(std::pair<short, int>(9, golemSoundId));
		int bossSoundId = SoundPlayer()->LoadSound("./resource/sound/Effect/monster/bossSound.wav");
		m_MonsterDefaultSoundMap.insert(std::pair<short, int>(10, bossSoundId));
	}
	// 피격 사운드 (텍스쳐 아이디 + HIT_SOUND_START_INDEX_IN_DEFAULT_SOUND_MAP으로 각 아이디 설정)
	{
		int slimeHitSoundId = SoundPlayer()->LoadSound("./resource/sound/Effect/monster/slimeHitSound.wav");
		m_MonsterDefaultSoundMap.insert(std::pair<short, int>(HIT_SOUND_START_INDEX_IN_DEFAULT_SOUND_MAP + 1, slimeHitSoundId));
		m_MonsterDefaultSoundMap.insert(std::pair<short, int>(HIT_SOUND_START_INDEX_IN_DEFAULT_SOUND_MAP + 2, slimeHitSoundId));
		m_MonsterDefaultSoundMap.insert(std::pair<short, int>(HIT_SOUND_START_INDEX_IN_DEFAULT_SOUND_MAP + 3, slimeHitSoundId));
		m_MonsterDefaultSoundMap.insert(std::pair<short, int>(HIT_SOUND_START_INDEX_IN_DEFAULT_SOUND_MAP + 4, slimeHitSoundId));
		int centipedeHitSoundId = SoundPlayer()->LoadSound("./resource/sound/Effect/monster/centipedeHitSound.wav");
		m_MonsterDefaultSoundMap.insert(std::pair<short, int>(HIT_SOUND_START_INDEX_IN_DEFAULT_SOUND_MAP + 5, centipedeHitSoundId));
		m_MonsterDefaultSoundMap.insert(std::pair<short, int>(HIT_SOUND_START_INDEX_IN_DEFAULT_SOUND_MAP + 6, centipedeHitSoundId));
		m_MonsterDefaultSoundMap.insert(std::pair<short, int>(HIT_SOUND_START_INDEX_IN_DEFAULT_SOUND_MAP + 7, centipedeHitSoundId));
		int golemHitSoundId = SoundPlayer()->LoadSound("./resource/sound/Effect/monster/golemHitSound.wav");
		m_MonsterDefaultSoundMap.insert(std::pair<short, int>(HIT_SOUND_START_INDEX_IN_DEFAULT_SOUND_MAP + 8, golemHitSoundId));
		m_MonsterDefaultSoundMap.insert(std::pair<short, int>(HIT_SOUND_START_INDEX_IN_DEFAULT_SOUND_MAP + 9, golemHitSoundId));
		int bossHitSoundId = SoundPlayer()->LoadSound("./resource/sound/Effect/monster/bossHitSound.wav");
		m_MonsterDefaultSoundMap.insert(std::pair<short, int>(HIT_SOUND_START_INDEX_IN_DEFAULT_SOUND_MAP + 10, bossHitSoundId));
	}
}

void CTurnBattleState::LoadBattleFieldData()
{
	SBattleMapData data;
	data.fieldModel = Renderer()->LoadOBJModelWithIndex("./resource/obj/world/battleField.obj");
	data.fieldTexture = Renderer()->LoadPNGTexture(
		"./resource/texture/world/Brick/t_brick_floor_002_ao_2k.jpg",
		"./resource/texture/world/Brick/t_brick_floor_002_diffuse_2k.jpg",
		"./resource/texture/AllZeroTexture.jpg", DEFAULT_SHININESS / 8.f,
		"./resource/texture/world/Brick/t_brick_floor_002_nor_gl_2k.jpg");
	data.lampModel = Renderer()->LoadOBJModelWithIndex("./resource/obj/world/lamp.obj");
	data.lampTexture = Renderer()->LoadPNGTexture("./resource/texture/AllOneTexture.jpg",
		"./resource/texture/world/lamp.png",
		"./resource/texture/AllOneTexture.jpg", DEFAULT_SHININESS,
		"./resource/texture/planNormalTexture.jpg");
	data.lampLightModel = Renderer()->LoadOBJModelWithIndex("./resource/obj/world/lampLight.obj");
	data.lightColor = glm::vec4(10.0f, 10.0f, 0.75f, 1.0f);
	data.wallModel = Renderer()->LoadOBJModelWithIndex("./resource/obj/world/wall/brickWall.obj");
	data.wallTexture = Renderer()->LoadPNGTexture("./resource/texture/AllOneTexture.jpg",
		"./resource/texture/world/Brick/brickWallColor.png",
		"./resource/texture/AllZeroTexture.jpg", DEFAULT_SHININESS / 4.0f,
		"./resource/texture/world/Brick/brickWallNormal.png");
	m_BattleFieldDataMap.insert(std::pair<int, SBattleMapData>(1, data));
	data.fieldTexture = Renderer()->LoadPNGTexture(
		"./resource/texture/AllOneTexture.jpg",
		"./resource/texture/world/Marble/Marble012_2K_Color.jpg",
		"./resource/texture/AllOneTexture.jpg", DEFAULT_SHININESS,
		"./resource/texture/world/Marble/Marble012_2K_NormalGL.jpg");
	data.wallModel = Renderer()->LoadOBJModelWithIndex("./resource/obj/world/wall/marbleWall.obj");
	data.wallTexture = Renderer()->LoadPNGTexture("./resource/texture/AllOneTexture.jpg",
		"./resource/texture/world/Marble/marbleWallColor.png",
		"./resource/texture/AllOneTexture.jpg", DEFAULT_SHININESS,
		"./resource/texture/planNormalTexture.jpg");
	m_BattleFieldDataMap.insert(std::pair<int, SBattleMapData>(2, data));
	data.fieldTexture = Renderer()->LoadPNGTexture(
		"./resource/texture/world/Desert/sandstone_cracks_ao_2k.jpg",
		"./resource/texture/world/Desert/sandstone_cracks_diff_2k.jpg",
		"./resource/texture/AllZeroTexture.jpg", DEFAULT_SHININESS / 8.f,
		"./resource/texture/world/Desert/sandstone_cracks_nor_gl_2k.jpg");
	data.wallModel = Renderer()->LoadOBJModelWithIndex("./resource/obj/world/wall/sandWall.obj");
	data.wallTexture = Renderer()->LoadPNGTexture("./resource/texture/AllOneTexture.jpg",
		"./resource/texture/world/Desert/sandWallColor.jpg",
		"./resource/texture/AllZeroTexture.jpg", DEFAULT_SHININESS / 4.0f,
		"./resource/texture/world/Desert/sandWallNormal.png");
	m_BattleFieldDataMap.insert(std::pair<int, SBattleMapData>(3, data));
	data.fieldTexture = Renderer()->LoadPNGTexture(
		"./resource/texture/world/Grass/Ground037_2K_AmbientOcclusion.jpg",
		"./resource/texture/world/Grass/Ground037_2K_Color.jpg",
		"./resource/texture/AllZeroTexture.jpg", DEFAULT_SHININESS / 8.f,
		"./resource/texture/world/Grass/Ground037_2K_Normal.jpg");
	data.wallModel = Renderer()->LoadOBJModelWithIndex("./resource/obj/world/wall/leafWall.obj");
	data.wallTexture = Renderer()->LoadPNGTexture("./resource/texture/AllOneTexture.jpg",
		"./resource/texture/world/Grass/leafWallColor.png",
		"./resource/texture/AllZeroTexture.jpg", DEFAULT_SHININESS / 4.0f,
		"./resource/texture/world/Grass/leafWallNormal.png");
	m_BattleFieldDataMap.insert(std::pair<int, SBattleMapData>(4, data));
}
