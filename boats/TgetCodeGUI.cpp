#include"TgetCodeGUI.h"
#include"buttonFunctions.h"
#include"handleNetworking.h"

#define fontPath "RESOURCES/FONT/Aloevera-OVoWO.ttf"

std::string TgetCodeGUI::codee{ TgetCodeGUI::codee };

TgetCodeGUI::TgetCodeGUI(sf::Vector2f position, sf::Vector2f size)
	: exitButton{ fnc::MainMenu::deleteGetCodeGUI, sf::Vector2f{position.x + size.x - 15.0f, position.y}, sf::Vector2f{ 15.0f, 15.0f }, 29, "x", fontPath },
	  copyCodeToClipBoard{ fnc::MainMenu::copyCodeToClipBoard, sf::Vector2f{position.x + 285.0f, position.y + 115.0f}, sf::Vector2f{60.0f, 30.0f}, 15, "COPY\nCODE", fontPath}
{
	this->font.loadFromFile(fontPath);

	this->backGroundRect.setPosition(position);
	this->backGroundRect.setSize(size);
	this->backGroundRect.setFillColor(sf::Color{ 0x0063c4 });

	this->codeBackGroundRect.setPosition(position.x + 5.0f, position.y + 60.0f);
	this->codeBackGroundRect.setSize(sf::Vector2f{ size.x - 10.0f, size.y / 3 });
	this->codeBackGroundRect.setFillColor(sf::Color::White);

	this->tittle.setFont(font);
	this->tittle.setPosition(position.x + 25.0f, position.y + 5.0f);
	this->tittle.setCharacterSize(40);
	this->tittle.setFillColor(sf::Color::White);
	this->tittle.setString("INVITE PLAYER");

	this->descripton.setFont(font);
	this->descripton.setPosition(position.x + 5.0f, position.y + 106.0f);
	this->descripton.setCharacterSize(20);
	this->descripton.setFillColor(sf::Color::White);
	this->descripton.setString("   USE THIS CODE TO\nINVITE OTHER PLAYERS");

	this->code.setFont(font);
	this->code.setPosition(position.x + 100.0f, position.y + 52.0f);
	this->code.setCharacterSize(50);
	this->code.setFillColor(sf::Color::Black);
	TgetCodeGUI::codee = std::to_string(ThandleNetwork::becomeAHost());
	this->code.setString(codee);

	this->exitButton.text.setPosition(sf::Vector2f{ position.x + size.x - 16.0f, position.y - 15.0f });
	this->exitButton.shape.setFillColor(sf::Color::Red);

	auto pos{ this->copyCodeToClipBoard.text.getPosition() };
	this->copyCodeToClipBoard.text.setPosition(pos.x - 2.0f, pos.y - 12.0f);
}

TgetCodeGUI::~TgetCodeGUI()
{
	ThandleNetwork::deleteHost();
}

void TgetCodeGUI::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(backGroundRect, states);
	target.draw(codeBackGroundRect, states);
	target.draw(exitButton, states);
	target.draw(copyCodeToClipBoard, states);
	target.draw(code, states);
	target.draw(tittle, states);
	target.draw(descripton, states);
}
