#ifndef HANDLENETWORKING
#define HANDLENETWORKING

#include<SFML/Network.hpp>
#include<SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include"buttonFunctions.h"
#include<random>

enum class STATUSOFCONNECTION
{
	NOTHING,
	HOST,
	JOINER
};

class ThandleNetwork
{
	friend class Tgame;
	friend void fnc::inGame::backToMenu();

public:
	static STATUSOFCONNECTION status;

	ThandleNetwork();
	~ThandleNetwork();

	static int  becomeAHost();
	static void deleteHost();
	static bool becomeAJoiner(unsigned short port);

	static sf::TcpSocket socket;
private:

	static unsigned short randomAPort();

	static void waitForPlayer();

	static void receiveData();
	static void markInGameBoatsPositions(unsigned int fieldX, unsigned int fieldY, sf::Image& oImg, sf::Image& mapsImg, sf::Vector2f& mapsLeftTop);
	static void markTakenFields(unsigned int fieldX, unsigned int fieldY, sf::Image& oImg, sf::Image& mapsImg, sf::Vector2f& mapsLeftTop);

	static sf::IpAddress localIP;

	static sf::Thread waitForConnection;
	static sf::Thread receiver;

	static sf::TcpListener* listener;
};


#endif // !HANDLENETWORKING
