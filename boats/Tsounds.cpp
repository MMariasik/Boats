#include "Tsounds.h"
#include<iostream>

sf::SoundBuffer Tsounds::buffer[int(SOUNDTYPE::MUSIC)];
sf::Sound Tsounds::sounds[MAXSOUNDS];
sf::Music Tsounds::backGroundMusic;

void Tsounds::prepareClass()
{
	std::string sounds[int(SOUNDTYPE::MUSIC)] {
	
		"buttonClick",
		"enemyMeReady",
		"enterLetter",
		"explosion",
		"hit",
		"jumpIntoWater",
		"lose",
		"pickUpBoat",
		"placeDative",
		"suken",
		"win",
		"wrongCode"	
	};

	for (int i{ 0 }; i < int(SOUNDTYPE::MUSIC); i++)
	{
		Tsounds::addSoundToBuffer(sounds[i], i);
	}

	sf::Listener::setGlobalVolume(100);
}

void Tsounds::addSoundToBuffer(const std::string& path, int whereToPut)
{
	sf::SoundBuffer sb;

	if (!sb.loadFromFile("RESOURCES/SND/" + path + ".wav")) std::cout << "error while loading sound: " + path << std::endl;

	Tsounds::buffer[whereToPut] = sb;
}

void Tsounds::play(SOUNDTYPE type, float volume)
{
	if (type == SOUNDTYPE::MUSIC)
	{
		Tsounds::playMusic(volume);
		return;
	}

	for (int i{ 0 }; i < MAXSOUNDS; i++)
	{
		if (Tsounds::sounds[i].getStatus() == sf::Sound::Playing) { continue; } // this slot is taken
		else
		{
			Tsounds::sounds[i].resetBuffer();
			Tsounds::sounds[i].setBuffer(Tsounds::buffer[int(type)]);
			Tsounds::sounds[i].setVolume(volume);
			Tsounds::sounds[i].setLoop(false);
			Tsounds::sounds[i].setPitch(1.0f);
			Tsounds::sounds[i].setAttenuation(1.0f);
			Tsounds::sounds[i].setRelativeToListener(true);
			Tsounds::sounds[i].play();

			std::cout << "Taken sound slot: " << i << std::endl;
			break;
		}
	}

}

void Tsounds::playMusic(float volume)
{
	if (Tsounds::backGroundMusic.getStatus() == sf::Sound::Playing)
		return;

	if (!Tsounds::backGroundMusic.openFromFile("RESOURCES/SND/seaBackGround.wav"))
		std::cout << "something went wrong while loading music" << std::endl;

	Tsounds::backGroundMusic.setRelativeToListener(true);
	Tsounds::backGroundMusic.setAttenuation(1.0f);
	Tsounds::backGroundMusic.setPitch(1.0f);
	Tsounds::backGroundMusic.setLoop(true);
	Tsounds::backGroundMusic.setVolume(volume);
	
	Tsounds::backGroundMusic.play();
}

void Tsounds::changeMusicVolume(float volume)
{
	Tsounds::backGroundMusic.setVolume(volume);
}