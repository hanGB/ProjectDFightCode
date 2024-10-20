/*
* SoundPlayer
*	게임 내 소리 재생을 하는 class
* 
*   Singleton pattern으로 변경 필요 
*/
#pragma once
#include <map>

class CSoundPlayer {
private:
	CSoundPlayer();
	~CSoundPlayer();

public:
	// 싱글톤 패턴 구현 함수
	static CSoundPlayer* GetInstance();

private:
	static CSoundPlayer* m_pInstance;

	irrklang::ISoundEngine* m_pSoundEngine = NULL;

	std::map<int, irrklang::ISoundSource*> m_SoundMap;
	std::map<const char*, int> m_SoundName;

public:
	// 소리 파일 로드
	int LoadSound(const char* fileName);

	// 루프하지 않는 소리 플레이
	void PlaySound(int index, float volume);
	
	// 플레이어 위치에 따라 소리 크기가 감소하는 플레이
	void PlaySoundWithPosition(int index, glm::vec3 position, float volume = 1.0f);

	// BGM 전용 사운드 플레이(루프하는 음악)
	irrklang::ISound* SetBackGroundMusic(int index, float volume);
	void PlayAndStopBackGroundMusic(irrklang::ISound* bgm, float bPlay);
	void DeleteBackGroundMusic(irrklang::ISound* bgm);

	// 사운드 삭제 후 empty_value 리턴
	int DeleteSoundAndSetEmptyID(int index);
};