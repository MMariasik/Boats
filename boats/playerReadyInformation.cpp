#include"playerReadyInformation.h"
#include"Tsounds.h"

playerReadyInformation::playerReadyInformation(sf::Vector2f pos, sf::Vector2f size)
{
	this->isReady = false;

	this->font.loadFromFile(fontPath);

	this->shape.setPosition(pos);
	this->shape.setSize(size);
	this->shape.setFillColor(sf::Color::Red);

	this->text.setPosition(pos.x + 0.0f, pos.y - 40.0f);
	this->text.setFont(font);
	this->text.setFillColor(sf::Color::Black);
	this->text.setString(" ENEMY IS\nNOT READY");
	this->text.setCharacterSize(35);
}

void playerReadyInformation::update()
{
	if (this->isReady)
	{
		this->text.setString(" ENEMY IS\nNOT READY");
		this->shape.setFillColor(sf::Color::Red);
		this->isReady = false;
	}
	else
	{
		this->text.setString(" ENEMY IS\n   READY");
		this->shape.setFillColor(sf::Color::Green);
		this->isReady = true;

		Tsounds::play(SOUNDTYPE::ENEMYORMEREADY);
	}
}

void playerReadyInformation::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(shape, states);
	target.draw(text, states);
}



myReadyInformation::myReadyInformation(sf::Vector2f pos, sf::Vector2f size)
{
	imReady = false;
	this->setFillColor(sf::Color::Red);
	this->setPosition(pos);
	this->setSize(size);
}

bool myReadyInformation::update()
{
	if (imReady)
	{
		imReady = false;
		this->setFillColor(sf::Color::Red);
	}
	else
	{
		imReady = true;
		this->setFillColor(sf::Color::Green);
	}
	return imReady;
}