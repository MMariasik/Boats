#ifndef BUTTON
#define BUTTON

#include<SFML/Graphics.hpp>

class button : public sf::Drawable
{
	friend class TgetCodeGUI;
	friend class TenterCodeGUI;
	friend class Tgame;

public:
	button(void (* const fnc)(), sf::Vector2f position, sf::Vector2f buttonSize, unsigned int charSize, std::string textOnButton, std::string fontPath);
	//~button();

	void mouseClicked(bool pressed, sf::Vector2i mousePos);
	bool animateHover(sf::Vector2i mousePos);

private:
	bool leftMouseButtonIsPressed{ false };
	void buttonAnimation(bool deanimate);
	bool isMouseOnMe(sf::Vector2i mousePos);
	void onClick();
	
	bool isMouseHoveringMe;
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void (* const myFunction)();
	sf::Font font;
	sf::Text text;
	sf::Vector2f originPos;
	sf::RectangleShape shape;
};

#endif // !BUTTON
