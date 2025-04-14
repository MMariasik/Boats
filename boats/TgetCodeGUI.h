#ifndef TGETCODEGUI
#define TGETCODEGUI

#include<SFML/Graphics.hpp>
#include"button.h"
#include"buttonFunctions.h"

class TgetCodeGUI : public sf::Drawable
{
	friend void fnc::MainMenu::copyCodeToClipBoard();
public:
	TgetCodeGUI(sf::Vector2f position, sf::Vector2f size);
	~TgetCodeGUI();

	button exitButton;
	button copyCodeToClipBoard;
private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	sf::Font font;
	sf::RectangleShape backGroundRect;
	sf::RectangleShape codeBackGroundRect;
	sf::Text tittle;
	static std::string codee;
	sf::Text code;
	sf::Text descripton;
};

#endif // !TGETCODEGUI
