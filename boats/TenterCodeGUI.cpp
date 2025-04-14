#include"TenterCodeGUI.h"
#include"buttonFunctions.h"

#define fontPath "RESOURCES/FONT/Aloevera-OVoWO.ttf"

TenterCodeGUI::TenterCodeGUI(sf::Vector2f position, sf::Vector2f size)
	: exitButton{ fnc::MainMenu::deleteGetCodeGUI, sf::Vector2f{position.x + size.x - 15.0f, position.y}, sf::Vector2f{ 15.0f, 15.0f }, 29, "x", fontPath },
	  enterCode{ fnc::MainMenu::enterInvitingCode, sf::Vector2f{position.x + 285.0f, position.y + 115.0f}, sf::Vector2f{60.0f, 30.0f}, 15, "ENTER\nCODE", fontPath }
{
	this->numberOfDigits = 0;

	this->font.loadFromFile(fontPath);
	
	this->backGroundRect.setPosition(position);
	this->backGroundRect.setSize(size);
	this->backGroundRect.setFillColor(sf::Color{ 0x0063c4 });
	
	this->codeBackGroundRect.setPosition(position.x + 5.0f, position.y + 60.0f);
	this->codeBackGroundRect.setSize(sf::Vector2f{ size.x - 10.0f, size.y / 3 });
	this->codeBackGroundRect.setFillColor(sf::Color::White);
	
	this->tittle.setFont(font);
	this->tittle.setPosition(position.x + 5.0f, position.y + 5.0f);
	this->tittle.setCharacterSize(40);
	this->tittle.setFillColor(sf::Color::White);
	this->tittle.setString("JOIN TO A GAME");
	
	this->descripton.setFont(font);
	this->descripton.setPosition(position.x + 35.0f, position.y + 106.0f);
	this->descripton.setCharacterSize(20);
	this->descripton.setFillColor(sf::Color::White);
	this->descripton.setString("TYPE THE CODE TO\nJOIN TO THE GAME");
	
	this->code.setFont(font);
	this->code.setPosition(position.x + 100.0f, position.y + 52.0f);
	this->code.setCharacterSize(50);
	this->code.setFillColor(sf::Color::Black);
	this->code.setString("");
	
	this->exitButton.text.setPosition(sf::Vector2f{ position.x + size.x - 16.0f, position.y - 15.0f });
	this->exitButton.shape.setFillColor(sf::Color::Red);
	
	auto pos{ this->enterCode.text.getPosition() };
	this->enterCode.text.setPosition(pos.x - 5.0f, pos.y - 12.0f);
}

TenterCodeGUI::~TenterCodeGUI()
{
}

bool TenterCodeGUI::enterDigit(char character)
{
	if (numberOfDigits < 5)
	{
		std::string str{ this->code.getString() };
		str.push_back(character);
		this->code.setString(str);
		numberOfDigits++;
		return true;
	}
	return false;
}

bool TenterCodeGUI::deleteCharacter()
{
	if (numberOfDigits > 0)
	{
		std::string str{ this->code.getString() };
		str.pop_back();
		this->code.setString(str);
		numberOfDigits--;
		return true;
	}
	return false;
}

void TenterCodeGUI::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(backGroundRect, states);
	target.draw(codeBackGroundRect, states);
	target.draw(exitButton, states);
	target.draw(enterCode, states);
	target.draw(code, states);
	target.draw(tittle, states);
	target.draw(descripton, states);
}
