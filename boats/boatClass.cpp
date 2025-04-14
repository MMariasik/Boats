#include "boatClass.h"
#include"game.h"
#include"Tsounds.h"

#include<iostream>

#define NOTDEFINEDPOSITION 9999



bool boat::mouseGrabbedSomeone{ false };
boat* boat::grabbedBoat{ nullptr };

boat::boat(std::string path, sf::Vector2f pos, bool wasGrabbd) : Sprite()
{
	this->wasGrabbed = wasGrabbd;
	this->setPosition(pos);
	this->lastPos = pos;

	this->img.loadFromFile(path);
	this->txt.loadFromImage(img);
	this->setTexture(txt);

	this->size = static_cast<short>(txt.getSize().x / 50.0);
	this->orientation = orient::right;

	this->followMouse = nullptr;
	this->isTurnedToRight = true;

	this->setOrigin(25.0f, 25.0f);

	this->realSize = static_cast<sf::Vector2f>(this->img.getSize());
	
	this->whichFieldImTaking = sf::Vector2i{ NOTDEFINEDPOSITION, NOTDEFINEDPOSITION };
	this->whichFieldWasTaking = sf::Vector2i{ NOTDEFINEDPOSITION, NOTDEFINEDPOSITION };
}

boat::~boat()
{
	if (followMouse)
	{
		followMouse->terminate();
		delete followMouse;
	}
}

void boat::turn(bool left)
{ // 0 - right, 1 - down, 2 - left, 3 - up
	if (boat::grabbedBoat)
	{
		if (left)
		{
			if (--boat::grabbedBoat->orientation < 0) boat::grabbedBoat->orientation = 3;
			boat::grabbedBoat->rotate(-90.0f);
		}
		else
		{
			if (++boat::grabbedBoat->orientation > 3) boat::grabbedBoat->orientation = 0;
			boat::grabbedBoat->rotate(90.0f);
		}


		sf::Vector2f size{ static_cast<sf::Vector2f>(boat::grabbedBoat->img.getSize()) };
		if (boat::grabbedBoat->orientation == orient::down || boat::grabbedBoat->orientation == orient::up)
		{
			boat::grabbedBoat->realSize = (sf::Vector2f{ size.y, size.x });
		}
		else if (boat::grabbedBoat->orientation == orient::left || boat::grabbedBoat->orientation == orient::right)
		{
			boat::grabbedBoat->realSize = size;
		}


		if ((boat::grabbedBoat->isTurnedToRight == false && boat::grabbedBoat->orientation == orient::right) ||
			(boat::grabbedBoat->isTurnedToRight == true && boat::grabbedBoat->orientation == orient::left))
		{
			boat::grabbedBoat->isTurnedToRight = !boat::grabbedBoat->isTurnedToRight;
			boat::grabbedBoat->img.flipVertically();
			boat::grabbedBoat->txt.loadFromImage(boat::grabbedBoat->img);
			boat::grabbedBoat->setTexture(boat::grabbedBoat->txt);
		}
	}
}

bool boat::onClick(sf::RenderWindow& window, Tmap& Map)
{ // here after click, i check colision

	sf::Vector2f mousePos{ sf::Mouse::getPosition(window) };
	if (boat::mouseGrabbedSomeone && boat::grabbedBoat == this)
	{ // letting off the grabbed boat
		boat::letBoatGo(mousePos, Map);
		this->setScale(1.0f, 1.0f);
		return true;
	}
	else if (!boat::mouseGrabbedSomeone)
	{ // grabbing a boat
		if (isMouseOnMe(mousePos))
		{
			this->grabBoat(window, Map);
			this->setScale(1.1f, 1.1f);
			return true;
		}
		return false;
	}
	else
		return false;
}

void boat::grabBoat(sf::RenderWindow& window, Tmap& Map)
{
	if (!followMouse)
	{
		Tsounds::play(SOUNDTYPE::PUICKUPBOAT);
		boat::mouseGrabbedSomeone = true;
		boat::grabbedBoat = this;
		Tgame::allowClicking = false;
		followMouse = new sf::Thread{ setMousesPos, pack{ this, &window, &Map } };
		followMouse->launch();

		Tgame::resetTakenFields();
	}
}

void boat::letBoatGo(sf::Vector2f mousePos, Tmap& Map)
{
	if (boat::isOnAvaiblePlaceToPut(mousePos, Map.getMapsCollision()))
	{
		boat::putBoat();
	}
	else
	{
		boat::cancelPuttingBoat(false);
	}
}

void boat::putBoat()
{
	Tsounds::play(SOUNDTYPE::JUMPINTOWATER);

	if (boat::grabbedBoat->wasGrabbed == false)
	{
		Tgame::decreaseAmountOfAvaibleBoats(boat::grabbedBoat->size);
		Tgame::textes.update();
	}
	boat::grabbedBoat->wasGrabbed = true;

	boat::grabbedBoat->lastPos = boat::grabbedBoat->getPosition();
	boat::cancelPuttingBoat(true);

	Tgame::resetTakenFields();
}

void boat::cancelPuttingBoat(bool saveTakenField)
{
	boat::grabbedBoat->followMouse->terminate();
	delete boat::grabbedBoat->followMouse;
	boat::grabbedBoat->followMouse = nullptr;
	boat::grabbedBoat->setPosition(boat::grabbedBoat->lastPos);
	if (!saveTakenField)
	{
		boat::grabbedBoat->whichFieldImTaking = boat::grabbedBoat->whichFieldWasTaking;
		while (boat::grabbedBoat->orientation != orient::right)
		{
			boat::turn(true);
		}
	}
	else boat::grabbedBoat->whichFieldWasTaking = boat::grabbedBoat->whichFieldImTaking;
	boat::grabbedBoat->setColor(sf::Color::White);
	boat::grabbedBoat = nullptr;
	boat::mouseGrabbedSomeone = false;
}

bool boat::isOnAvaiblePlaceToPut(sf::Vector2f mousePos, sf::FloatRect field)
{
	if (field.contains(mousePos))
	{
		sf::Vector2f neededSpace{ (boat::grabbedBoat->size - 1) * Tmap::sizeOfField.x, (boat::grabbedBoat->size - 1) * Tmap::sizeOfField.y };

		switch (boat::grabbedBoat->orientation)
		{
		case orient::up:
		{
			if (secureCheck())
				for (int i{ 0 }; i < boat::grabbedBoat->size; i++)
				{
					if (Tgame::takenFields[boat::grabbedBoat->whichFieldImTaking.x][boat::grabbedBoat->whichFieldImTaking.y - i] == true)
						return false;
				}

			if (mousePos.y > field.top + neededSpace.y)
				return true;
			break;
		}
		case orient::left:
		{
			if (secureCheck())
				for (int i{ 0 }; i < boat::grabbedBoat->size; i++)
				{
					if (Tgame::takenFields[boat::grabbedBoat->whichFieldImTaking.x - i][boat::grabbedBoat->whichFieldImTaking.y] == true)
						return false;
				}

			if (mousePos.x > field.left + neededSpace.x)
				return true;
			break;
		}
		case orient::down:
		{
			if (secureCheck())
				for (int i{ 0 }; i < boat::grabbedBoat->size; i++)
				{
					if (Tgame::takenFields[boat::grabbedBoat->whichFieldImTaking.x][boat::grabbedBoat->whichFieldImTaking.y + i] == true)
						return false;
				}

			if (mousePos.y < field.top + field.height - neededSpace.y)
				return true;
			break;
		}
		case orient::right:
		{
			if (secureCheck())
				for (int i{ 0 }; i < boat::grabbedBoat->size; i++)
				{
					if (Tgame::takenFields[boat::grabbedBoat->whichFieldImTaking.x + i][boat::grabbedBoat->whichFieldImTaking.y] == true)
						return false;
				}

			if (mousePos.x < field.left + field.width - neededSpace.x)
				return true;
			break;
		}
		default:
			break;
		}
	}
	return false;
}

inline bool boat::secureCheck()
{
	if (boat::grabbedBoat->whichFieldImTaking.x == NOTDEFINEDPOSITION || boat::grabbedBoat->whichFieldImTaking.y == NOTDEFINEDPOSITION)
		return false;
	return true;
}

bool boat::isMouseOnMe(sf::Vector2f mousePos)
{
	bool direction{ this->orientation == orient::left || this->orientation == orient::down };
	sf::Vector2f pos{ this->getPosition() };
	sf::Vector2f boatPos{ pos.x - Tmap::sizeOfField.x / 2, pos.y - Tmap::sizeOfField.y / 2 };

	if (this->orientation == orient::left)
	{
		boatPos.x -= (this->size - 1) * Tmap::sizeOfField.x;
	}
	if (this->orientation == orient::up)
	{
		boatPos.y -= (this->size - 1) * Tmap::sizeOfField.y;
	}

	sf::FloatRect rectangle{ boatPos, this->realSize };

	if (rectangle.contains(mousePos))
		return true;
	return false;
}

void boat::setMousesPos(pack Pack)
{
	sf::sleep(sf::seconds(0.1f));
	Tgame::allowClicking = true;

	Pack.Boat->lastPos = Pack.Boat->getPosition();
	while (1)
	{
		if (Pack.Boat && Pack.window)
		{
			sf::Vector2f mousePos{ static_cast<sf::Vector2f>(sf::Mouse::getPosition(*Pack.window)) };

			if (boat::isOnAvaiblePlaceToPut(mousePos, Pack.Map->getMapsCollision()))
			{
				boat::setFieldsPos(mousePos, Pack.Map->getMapsCollision(), true);
				boat::grabbedBoat->setColor(sf::Color::White);
			}
			else
			{
				Pack.Boat->setPosition(mousePos);
				boat::grabbedBoat->setColor(sf::Color{ 0xfc2a2aff });
				boat::setFieldsPos(mousePos, Pack.Map->getMapsCollision(), false);
			}
		}
	}
}

void boat::setFieldsPos(sf::Vector2f mousePos, sf::FloatRect map, bool setPos)
{
	for (int i{ 0 }; i < int(map.height / Tmap::sizeOfField.y); i++)
	{
		for (int j{ 0 }; j < int(map.width / Tmap::sizeOfField.x); j++)
		{
			sf::FloatRect field{ sf::Vector2f{ map.left + i * Tmap::sizeOfField.x, map.top + j * Tmap::sizeOfField.y }, Tmap::sizeOfField };
			if (field.contains(mousePos))
			{
				boat::grabbedBoat->whichFieldImTaking = sf::Vector2i{ i, j };
				if(setPos) boat::grabbedBoat->setPosition(sf::Vector2f{ field.left + Tmap::sizeOfField.x / 1.9f, field.top + Tmap::sizeOfField.y / 2.05f + 1.0f });
			}
		}
	}
}


