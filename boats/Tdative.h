#ifndef TDATIVE
#define TDATIVE

#include<SFML/Graphics.hpp>
#include"button.h"
#include"buttonFunctions.h"

class Tdative : public sf::Sprite
{
	friend class Tgame;
	friend class ThandleNetwork;
	friend void fnc::inGame::shoot();

public:
	Tdative(sf::RenderWindow& window);
	~Tdative();

	void update(sf::Vector2f pos);
	void onClick();

	static void followMouse(Tdative* ths);
private:
	void randomField();
	
	bool isMouseOnMe(sf::Vector2f mousePos);
	static bool isMouseOnMap(sf::Vector2f mousePos, sf::FloatRect mapsHitbox);
	inline bool isMouseOnValidField();

	void setMousePos(sf::Vector2f mousePos);
	void setFieldPos(sf::Vector2f mousePos, sf::FloatRect mapsHitbox);

	static button* shootButton;
	sf::Texture txtOfDative;
	sf::Texture txtOfExplosion;
	bool letMeDissolve;
	sf::Thread follower;
	static bool isFollowing;
	sf::Vector2u whichFieldImTaking;
	sf::RenderWindow* window;
	sf::Vector2u whichFieldWasTaking;
};

#endif // !TDATIVE
