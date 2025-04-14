#include "Tdative.h"
#include"game.h"
#include"Tsounds.h"
#include"handleNetworking.h"
#include<iostream>

bool Tdative::isFollowing{ false };
button* Tdative::shootButton{ nullptr };

Tdative::Tdative(sf::RenderWindow& ww) : Sprite(), follower{ followMouse, this },
						whichFieldImTaking{ NOTDEFINEDPOSITION, NOTDEFINEDPOSITION }, window(&ww),
	whichFieldWasTaking{ NOTDEFINEDPOSITION, NOTDEFINEDPOSITION }
{
	this->txtOfDative.loadFromFile("RESOURCES/IMG/dative.png");
	this->txtOfExplosion.loadFromFile("RESOURCES/IMG/explosion.png");

	this->setPosition(0, 0);
	this->setOrigin(txtOfDative.getSize().x/2.0f, txtOfDative.getSize().y/2.0f);
	this->setTexture(txtOfDative);
	this->letMeDissolve = false;
}

Tdative::~Tdative()
{
	follower.terminate();
}

void Tdative::update(sf::Vector2f pos)
{
	if (Tgame::status == GAMESTATUS::RUNTIME && Tgame::myRound == false)
	{
		this->setPosition(pos);
	}
}

void Tdative::onClick()
{
	if ((Tgame::status == GAMESTATUS::NOTHING || Tgame::status == GAMESTATUS::RUNTIME) && Tgame::myRound == true)
	{
		sf::Vector2f mousePos{ sf::Mouse::getPosition(*this->window) };

		if (!Tdative::isFollowing)
		{
			if (isMouseOnMe(mousePos))
			{
				this->setScale(1.2f, 1.2f);
				follower.launch();
				Tdative::isFollowing = true;
				Tgame::allowClicking = false;
				
				if(Tdative::shootButton)
					for (std::vector<button*>::iterator it = Tgame::Buttons.begin(); it != Tgame::Buttons.end(); it++)
						if (*it == Tdative::shootButton)
						{
							Tgame::Buttons.erase(it);
							delete Tdative::shootButton;
							Tdative::shootButton = nullptr;
							break;
						}
			}
		}
		else
		{
			Tsounds::play(SOUNDTYPE::PLACEDATIVE);

			if(!Tdative::shootButton) 
				Tdative::shootButton = Tgame::createButton(fnc::inGame::shoot, sf::Vector2f{ 635.0f, 505.0f }, sf::Vector2f{ 210.0f, 55.0f }, 30, "    SHOOT", fontPath);

			this->setScale(1.0f, 1.0f);

			follower.terminate();

			this->randomField();

			Tdative::isFollowing = false;

			// here random position if not defined yet
			sf::FloatRect field{ sf::Vector2f{ Tgame::enemiesMap.getMapsCollision().left + this->whichFieldWasTaking.x * Tmap::sizeOfField.x,
				Tgame::enemiesMap.getMapsCollision().top + this->whichFieldWasTaking.y * Tmap::sizeOfField.y},
				Tmap::sizeOfField };

			this->setPosition(sf::Vector2f{ field.left + Tmap::sizeOfField.x / 1.9f - 1.0f, field.top + Tmap::sizeOfField.y / 2.05f + 1.0f});

			sf::Packet packet;
			packet.clear();
			packet << TYPEOFSENTDATA::DATIVEPOS << this->getPosition().x << this->getPosition().y;
			Tgame::handleNetwork.socket.send(packet);
		}
	}
}

bool Tdative::isMouseOnMe(sf::Vector2f mousePos)
{
	sf::FloatRect hitbox{ sf::Vector2f{ this->getPosition().x - Tmap::sizeOfField.x / 2, this->getPosition().y - Tmap::sizeOfField.y / 2 }, sf::Vector2f{txtOfDative.getSize()}};
	if (hitbox.contains(mousePos))
		return true;
	return false;
}

bool Tdative::isMouseOnMap(sf::Vector2f mousePos, sf::FloatRect mapsHitbox)
{
	if (mapsHitbox.contains(mousePos))
		return true;
	return false;
}

bool Tdative::isMouseOnValidField()
{
	return !(Tgame::takenFields[this->whichFieldImTaking.x][this->whichFieldImTaking.y]);
}

void Tdative::setMousePos(sf::Vector2f mousePos)
{
	this->setPosition(mousePos);
}

void Tdative::setFieldPos(sf::Vector2f mousePos, sf::FloatRect mapsHitbox)
{
	for (unsigned int i{ 0 }; i < unsigned int(mapsHitbox.height / Tmap::sizeOfField.y); i++)
	{
		for (unsigned int j{ 0 }; j < unsigned int(mapsHitbox.width / Tmap::sizeOfField.x); j++)
		{
			sf::FloatRect field{ sf::Vector2f{ mapsHitbox.left + i * Tmap::sizeOfField.x, mapsHitbox.top + j * Tmap::sizeOfField.y }, Tmap::sizeOfField };
			if (field.contains(mousePos))
			{
				this->whichFieldImTaking = sf::Vector2u{ i, j };
				if (this->isMouseOnValidField())
				{
					this->setPosition(sf::Vector2f{ field.left + Tmap::sizeOfField.x / 1.9f - 1.0f, field.top + Tmap::sizeOfField.y / 2.05f + 1.0f }); //TODO
					this->whichFieldWasTaking = this->whichFieldImTaking;
				}
				else this->setMousePos(mousePos);
			}
		}
	}
}

void Tdative::followMouse(Tdative* ths)
{
	sf::sleep(sf::seconds(0.1f));
	Tgame::allowClicking = true;

	while (1)
	{
		sf::Vector2f mousePos{ sf::Mouse::getPosition(*ths->window) };

		if (Tdative::isMouseOnMap(mousePos, Tgame::enemiesMap.getMapsCollision()))
		{
			ths->setFieldPos(mousePos, Tgame::enemiesMap.getMapsCollision());
		}
		else
		{
			ths->setMousePos(mousePos);
		}
	}
}

void Tdative::randomField()
{
	if (this->whichFieldWasTaking.x == NOTDEFINEDPOSITION || this->whichFieldWasTaking.y == NOTDEFINEDPOSITION)
	{
		static std::random_device dev;
		static std::mt19937 rng(dev());
		static std::uniform_int_distribution<std::mt19937::result_type> dist6(0, 9); // sf::Vector2i{ int(dist6(rng)), int(dist6(rng)) };

		sf::Vector2u randomedField;
		do
		{
			randomedField = { unsigned int(dist6(rng)), unsigned int(dist6(rng)) };

		} while (Tgame::takenFields[randomedField.x][randomedField.y] == true);

		this->whichFieldWasTaking = this->whichFieldImTaking = randomedField;
	}
}
