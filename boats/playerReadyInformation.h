#ifndef PLAYERREADYINFORMATION
#define PLAYERREADYINFORMATION

#include<SFML/Graphics.hpp>

#define fontPath "RESOURCES/FONT/Aloevera-OVoWO.ttf"

class playerReadyInformation : public sf::Drawable
{
public:
	playerReadyInformation(sf::Vector2f pos, sf::Vector2f size);

	void update();
	bool isReady;
private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	sf::RectangleShape shape;
	sf::Font font;
	sf::Text text;
};

class myReadyInformation : public sf::RectangleShape
{
public:
	myReadyInformation(sf::Vector2f pos, sf::Vector2f size);
	bool update();
	bool imReady;
};

#endif //PLAYERREADYINFORMATION