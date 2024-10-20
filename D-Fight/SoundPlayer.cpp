#include "stdafx.h"
#include "SoundPlayer.h"
#include "Globals.h"
#include "BehaviorTree.h"

CSoundPlayer::CSoundPlayer()
{
	m_pSoundEngine = irrklang::createIrrKlangDevice();
	if (!m_pSoundEngine)
	{
		std::cout << "사운드 엔진 초기화 실패\n";
	}
}

CSoundPlayer::~CSoundPlayer()
{
	m_pSoundEngine->drop();
}

CSoundPlayer* CSoundPlayer::GetInstance()
{
	static CSoundPlayer instance;
	return &instance;
}

int CSoundPlayer::LoadSound(const char* fileName)
{
	if (m_SoundMap.size() != 0)
	{
		auto it = m_SoundName.find(fileName);
		if (it != m_SoundName.end())
			return (*it).second;
	}

	irrklang::ISoundSource* sound = m_pSoundEngine->addSoundSourceFromFile(fileName);
	if (sound == NULL) 
	{
		std::cout << fileName << "을 열어 사운드를 생성 할 수 없습니다.\n";
		return EMPTY_VALUE;
	}

	int index = 0;
	while (1)
	{
		if (m_SoundMap.find(index) == m_SoundMap.end())
			break;

		++index;
	}
	m_SoundMap.insert(std::pair<int, irrklang::ISoundSource*>(index, sound));
	m_SoundName.insert(std::pair<const char*, int>(fileName, index));

	return index;
}

void CSoundPlayer::PlaySound(int index, float volume)
{
	auto it = m_SoundMap.find(index);
	
	if (it == m_SoundMap.end()) 
	{
		std::cout << index  << " 번호의 Sound가 없습니다.\n";
		return;
	}
	(*it).second->setDefaultVolume(volume);
	m_pSoundEngine->play2D((*it).second);
}

void CSoundPlayer::PlaySoundWithPosition(int index, glm::vec3 position, float volume)
{
	// 주인공 위치와 거리 계산
	glm::vec3 heroPos = CBlackBoard::GetInstance()->GetHeroPosition();
	float dis = glm::distance(heroPos, position);

	float rate = 1.0f;

	if (dis > DEFAULT_MAX_SOUND_DISTANCE) return;
	
	float calculateDistance = (dis - DEFAULT_MIN_SOUND_DISTANCE) / DEFAULT_SOUND_ATTENSION;
	if (dis > DEFAULT_MIN_SOUND_DISTANCE) rate = 1.0f / (1.0f + calculateDistance * calculateDistance);

	auto it = m_SoundMap.find(index);

	if (it == m_SoundMap.end())
	{
		std::cout << index << " 번호의 Sound가 없습니다.\n";
		return;
	}
	(*it).second->setDefaultVolume(volume * rate);
	m_pSoundEngine->play2D((*it).second);
}

irrklang::ISound* CSoundPlayer::SetBackGroundMusic(int index, float volume)
{
	auto it = m_SoundMap.find(index);
	(*it).second->setDefaultVolume(volume);
	irrklang::ISound* bgm = m_pSoundEngine->play2D((*it).second, true, true);

	return bgm;
}

void CSoundPlayer::DeleteBackGroundMusic(irrklang::ISound* bgm)
{
	bgm->stop();
	bgm->drop();

	bgm = NULL;
}

void CSoundPlayer::PlayAndStopBackGroundMusic(irrklang::ISound* bgm, float bPlay)
{
	if (bgm != NULL) 
	{
		if (!bPlay)
		{
			bgm->setIsPaused(true);
		}
		else
		{
			bgm->setPlayPosition(0);
			bgm->setIsPaused(false);
			bgm->setIsLooped(true);
		}
	}
}

int CSoundPlayer::DeleteSoundAndSetEmptyID(int index)
{
	if (index == EMPTY_VALUE)
		return EMPTY_VALUE;

	auto it = m_SoundMap.find(index);

	m_pSoundEngine->removeSoundSource((*it).second->getName());
	m_SoundMap.erase(index);

	return EMPTY_VALUE;
}
