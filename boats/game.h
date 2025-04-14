#ifndef GAME
#define GAME

#include "boatClass.h"
#include"button.h"
#include"buttonFunctions.h"
#include"Ttimer.h"
#include"playerReadyInformation.h"
#include"TbackGround.h"
#include"TavaibleBoatsText.h"
#include"handleNetworking.h"
#include"TgetCodeGUI.h"
#include"TenterCodeGUI.h"
#include"Tdative.h"

#define NOTDEFINEDPOSITION 9999

enum class GAMESTATUS
{
	NOTHING,
	MAINMENU,
	CONFIG,
	RUNTIME
};

enum orient
{
	right = 0,
	down = 1,
	left = 2,
	up = 3
};

namespace TYPEOFSENTDATA
{
	constexpr short IMREADY = 0;
	constexpr short DATIVEPOS = 1;
	constexpr short SHOOT = 2;
	constexpr short SHOOTFEEDBACK = 3;
	constexpr short SURRENDER = 4;
	constexpr short SURRENDERFEEDBACK = 5;
}

struct boatsPositions
{
	bool shooted;

	bool taken;
	orient boatsOrientationIfTaken;
	sf::Vector2i startingPos;
	short size;
	bool isBoatTurnedToRight;
};

// GAME MAIN CLASS
class Tgame : public sf::Drawable
{
	friend class TavaibleBoatsText;
	friend class boat;
	friend class ThandleNetwork;
	friend class Tdative;

	friend void fnc::MainMenu::joinGame();
	friend void fnc::MainMenu::hostGame(); 
	friend void fnc::MainMenu::deleteGetCodeGUI();
	friend void fnc::MainMenu::enterInvitingCode();
	friend void fnc::MainMenu::how2Play();

	friend void fnc::Config::imReady();

	friend void fnc::inGame::shoot();
	friend void fnc::inGame::backToMenu();
	friend void fnc::inGame::surrender();

	friend int main(); // only for debug

public:

	static GAMESTATUS status;

	Tgame();
	~Tgame();

	void onClick(sf::RenderWindow& window, sf::Vector2i mousePos, bool mouseIsPressed);
	void onKeyPressed(sf::Keyboard::Key key);
	void turnBoat(bool left);
	void update(sf::RenderWindow& window);

private:
	static void resetTakenFields();
	static void markField( sf::Vector2i which);
	static void decreaseAmountOfAvaibleBoats(short int size);

	static bool takenFields[10][10];
	static boatsPositions inGameBoatsPositions[10][10];

	static void prepareDative();
	static void animateShoot();
	static void endRound(short int MissHitOrSuken);
	static bool myRound;

	static void prepareMainMenu();
	static void prepareGameConfig();
	static void prepareGame(sf::RenderWindow& window);

	static void cleanAfterConfig();

	static button* createButton(void (* const fnc)(), sf::Vector2f position, sf::Vector2f buttonSize, unsigned int charSize, std::string textOnButton, std::string pathToFont);
	static void createBoat(std::string path, sf::Vector2f pos);
	static void createReadyInformation(sf::Vector2f pos, sf::Vector2f size);

	static void createGetCodeGUI();
	static void createInviteCodeGUI();

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void drawTimer(sf::RenderTarget& target, sf::RenderStates states) const;
	void drawButtons(sf::RenderTarget& target, sf::RenderStates states) const;
	void drawBoats(sf::RenderTarget& target, sf::RenderStates states) const;
	void drawMapsAndTextes(sf::RenderTarget& target, sf::RenderStates states) const;
	void drawAdditionals(sf::RenderTarget& target, sf::RenderStates states) const;

	static void clearObjectsToDraw();
	void countDown(const sf::Time& timeElapsed, const sf::Time& time, sf::RenderWindow& window);
	void randomNotPlacedBoatsPositions();

	static void enterInvitingCode();

	static bool allowClicking;

	static int enemiesBoatsLeftCounter;
	static int mineLeftBoatsCounter;

	static Tmap myMap;
	static Tmap enemiesMap;
	static Ttimer timer;
	TbackGround bg;
	std::pair<sf::Cursor, bool> mouseCursorIsArrow; // sf::Cursor -> mouseCursor | bool -> isArrow

	static std::vector<button*> Buttons;
	static std::vector<boat*> Boats;
	static int avaibleBoats[4];
	static TavaibleBoatsText textes;
	static sf::Text boatsLeft;
	static std::vector<sf::Drawable*>AdditionalElements;
	static playerReadyInformation* isEnemyReady;
	static ThandleNetwork handleNetwork;
	static myReadyInformation* AmIReady;
	static Tdative* dative;

	static TgetCodeGUI* getInvitingCodeGui;
	static TenterCodeGUI* enterInvitingCodeGui;
};

#endif