#ifndef BOATCLASS
#define BOATCLASS

#include<SFML/Graphics.hpp>
#include"map.h"
// important to remember that this class will be used while configuration mode, so the implementation will be only in this area 
class boat;

struct pack
{
	boat* Boat;
	sf::RenderWindow* window;
	Tmap* Map;
};

class boat : public sf::Sprite
{
	friend class Tgame;

public:
	boat(std::string path, sf::Vector2f pos, bool wasGrabbd = false);
	~boat();

	static void turn(bool left);
	bool onClick(sf::RenderWindow& window, Tmap& Map);

	sf::Vector2i whichFieldWasTaking; // this is position to use in run mode
	sf::Vector2i whichFieldImTaking;
	bool isMouseOnMe(sf::Vector2f mousePos);

	short int size;
private:
	void grabBoat(sf::RenderWindow& window, Tmap& Map);

	static void letBoatGo(sf::Vector2f mousePos, Tmap& Map);
	static bool isOnAvaiblePlaceToPut(sf::Vector2f mousePos, sf::FloatRect map);
	static inline bool secureCheck();
	static void putBoat();
	static void cancelPuttingBoat(bool saveTakenField);
	static bool mouseGrabbedSomeone;
	static boat* grabbedBoat;
	static void setMousesPos(pack Pack);
	static void setFieldsPos(sf::Vector2f mousePos, sf::FloatRect map, bool setPos);

	sf::Thread* followMouse;
	bool wasGrabbed;

	bool isTurnedToRight;
	sf::Image img;
	sf::Texture txt;
	sf::Vector2f lastPos;
	sf::Vector2f realSize;
	short int orientation; // 0 - right, 1 - down, 2 - left, 3 - up
};


#endif // !BOATCLASS
