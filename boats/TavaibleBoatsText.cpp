#include"TavaibleBoatsText.h"
#include "game.h"

TavaibleBoatsText::TavaibleBoatsText(std::string pathToFont, sf::Vector2f pos) : font{}, first{}, second{}, third{}, fourth{}, position{}
{
	font.loadFromFile(pathToFont);

	first.setFont(font);
	first.setFillColor(sf::Color::Black);
	first.setCharacterSize(55);
	first.setPosition(pos);

	second.setFont(font);
	second.setFillColor(sf::Color::Black);
	second.setCharacterSize(55);
	second.setPosition(pos.x, pos.y + 70.0f);

	third.setFont(font);
	third.setFillColor(sf::Color::Black);
	third.setCharacterSize(55);
	third.setPosition(pos.x, pos.y + 140.0f);

	fourth.setFont(font);
	fourth.setFillColor(sf::Color::Black);
	fourth.setCharacterSize(55);
	fourth.setPosition(pos.x, pos.y + 210.0f);

}

void TavaibleBoatsText::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(first, states);
	target.draw(second, states);
	target.draw(third, states);
	target.draw(fourth, states);
}

void TavaibleBoatsText::update()
{
	first.setString(std::to_string(Tgame::avaibleBoats[0]));
	second.setString(std::to_string(Tgame::avaibleBoats[1]));
	third.setString(std::to_string(Tgame::avaibleBoats[2]));
	fourth.setString(std::to_string(Tgame::avaibleBoats[3]));
}