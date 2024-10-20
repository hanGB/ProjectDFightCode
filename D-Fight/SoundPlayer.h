/*
* SoundPlayer
*	���� �� �Ҹ� ����� �ϴ� class
* 
*   Singleton pattern���� ���� �ʿ� 
*/
#pragma once
#include <map>

class CSoundPlayer {
private:
	CSoundPlayer();
	~CSoundPlayer();

public:
	// �̱��� ���� ���� �Լ�
	static CSoundPlayer* GetInstance();

private:
	static CSoundPlayer* m_pInstance;

	irrklang::ISoundEngine* m_pSoundEngine = NULL;

	std::map<int, irrklang::ISoundSource*> m_SoundMap;
	std::map<const char*, int> m_SoundName;

public:
	// �Ҹ� ���� �ε�
	int LoadSound(const char* fileName);

	// �������� �ʴ� �Ҹ� �÷���
	void PlaySound(int index, float volume);
	
	// �÷��̾� ��ġ�� ���� �Ҹ� ũ�Ⱑ �����ϴ� �÷���
	void PlaySoundWithPosition(int index, glm::vec3 position, float volume = 1.0f);

	// BGM ���� ���� �÷���(�����ϴ� ����)
	irrklang::ISound* SetBackGroundMusic(int index, float volume);
	void PlayAndStopBackGroundMusic(irrklang::ISound* bgm, float bPlay);
	void DeleteBackGroundMusic(irrklang::ISound* bgm);

	// ���� ���� �� empty_value ����
	int DeleteSoundAndSetEmptyID(int index);
};