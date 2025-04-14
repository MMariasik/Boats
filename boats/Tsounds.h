#ifndef TSOUNDS
#define TSOUNDS

#define MAXSOUNDS 255

#include<SFML/Audio.hpp>
#include<vector>

enum class SOUNDTYPE
{
	BUTTONCLICK,   
	ENEMYORMEREADY,
	ENTERLETTER,
	EXPLOSION,
	HIT,
	JUMPINTOWATER,
	LOSE,
	PUICKUPBOAT,
	PLACEDATIVE,
	SUKEN,
	WIN,
	WRONGCODE,

	MUSIC //music is last sound type ALWAYS
};

class Tsounds
{
public:
	static void prepareClass();

	static void play(SOUNDTYPE type, float volume = 100.0f);
	static void changeMusicVolume(float volume);

private:
	static void playMusic(float volume = 100.0f);
	static void addSoundToBuffer(const std::string& path, int whereToPut);

	static sf::SoundBuffer buffer[int(SOUNDTYPE::MUSIC)];
	static sf::Sound sounds[MAXSOUNDS];
	static sf::Music backGroundMusic;

};

#endif // 
