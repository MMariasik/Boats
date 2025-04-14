#ifndef TENDERCODEGUI
#define TENDERCODEGUI

#include<SFML/Graphics.hpp>
#include"button.h"


class TenterCodeGUI : public sf::Drawable
{
	friend class Tgame;
public:
	TenterCodeGUI(sf::Vector2f position, sf::Vector2f size);
	~TenterCodeGUI();

	bool enterDigit(char character);
	bool deleteCharacter();
	
	button exitButton;
	button enterCode;
private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	sf::Font font;
	sf::RectangleShape backGroundRect;
	sf::RectangleShape codeBackGroundRect;
	unsigned short numberOfDigits;
	sf::Text tittle;
	sf::Text code;
	sf::Text descripton;
};

#endif // !TENDERCODEGUI
