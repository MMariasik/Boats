#include"buttonFunctions.h"
#include"game.h"
#include<iostream>

#include<SFML/Window/Clipboard.hpp>


//using namespace fnc;

void fnc::MainMenu::joinGame()
{
	Tgame::clearObjectsToDraw();
	Tgame::createInviteCodeGUI();
	//Tgame::prepareGameConfig();//for now
}

void fnc::MainMenu::hostGame()
{
	Tgame::clearObjectsToDraw();
	Tgame::createGetCodeGUI();
}

void fnc::MainMenu::settings()
{

}

void fnc::MainMenu::how2Play()
{
	Tgame::clearObjectsToDraw();

	static sf::Font font; 
	font.loadFromFile(fontPath);

	sf::Text* txt{ new sf::Text{} };
	txt->setFont(font);
	txt->setCharacterSize(40);
	txt->setFillColor(sf::Color::Black);
	txt->setPosition(sf::Vector2f{ 10.0f, 250.0f });
	txt->setString("here will be how to play explanation");

	Tgame::AdditionalElements.push_back(txt);

	// other buttons, to change page, TODO
	Tgame::createButton(nullptr, sf::Vector2f{ 480.0f, 530.0f }, sf::Vector2f{ 60.0f, 50.0f }, 15, "LAST\nPAGE", fontPath);
	Tgame::createButton(nullptr, sf::Vector2f{ 550.0f, 530.0f }, sf::Vector2f{ 60.0f, 50.0f }, 15, "NEXT\nPAGE", fontPath);

	Tgame::createButton(Tgame::prepareMainMenu, sf::Vector2f{ 650.0f, 530.0f }, sf::Vector2f{ 215.0f, 50.0f }, 25, "BACK TO MENU", fontPath);
}

void fnc::MainMenu::deleteGetCodeGUI()
{
	sf::sleep(sf::seconds(0.1f));
	if (Tgame::getInvitingCodeGui)
	{
		delete Tgame::getInvitingCodeGui;
		Tgame::getInvitingCodeGui = nullptr;
	}
	if (Tgame::enterInvitingCodeGui)
	{
		delete Tgame::enterInvitingCodeGui;
		Tgame::enterInvitingCodeGui = nullptr;
	}

	Tgame::prepareMainMenu();
}

void fnc::MainMenu::copyCodeToClipBoard()
{
	sf::Clipboard::setString(TgetCodeGUI::codee);
}

void fnc::MainMenu::enterInvitingCode()
{
	Tgame::enterInvitingCode();
}


void fnc::Config::imReady()
{
	Tgame::AmIReady->update();

	sf::Packet packet;
	packet << TYPEOFSENTDATA::IMREADY;

	while (Tgame::handleNetwork.socket.send(packet) != sf::Socket::Done)
	{
		std::cout << "DEBUG: something went wrong with sending imReady information" << std::endl;
	}

}

void fnc::inGame::shoot()
{
	sf::Packet packet;
	packet << TYPEOFSENTDATA::SHOOT << Tgame::dative->whichFieldWasTaking.x << Tgame::dative->whichFieldWasTaking.y;
	while (ThandleNetwork::socket.send(packet) != sf::Socket::Done)
	{
		std::cout << "DEBUG something went wrong while sending endofround" << std::endl;
	}

	Tgame::animateShoot();
}

void fnc::inGame::surrender()
{// todo now
	sf::Packet packet;
	packet.clear();
	packet << TYPEOFSENTDATA::SURRENDER;
	while (ThandleNetwork::socket.send(packet) != sf::Socket::Done)
	{
		std::cout << "something went wrong with surrender info" << std::endl;
	}
}

void fnc::inGame::backToMenu()
{
	for (int i{ 0 }; i < 10; i++)
		for (int j{ 0 }; j < 10; j++)
			Tgame::takenFields[i][j] = false;

	for (int i{ 0 }; i < 10; i++)
		for (int j{ 0 }; j < 10; j++)
		{
			Tgame::inGameBoatsPositions[i][j].taken = false;
			Tgame::inGameBoatsPositions[i][j].shooted = false;
		}

	sf::Image clearMap; clearMap.loadFromFile("RESOURCES/IMG/map.png");
	Tgame::myMap.setNewImage(clearMap);
	Tgame::myMap.setScale(1.0f, 1.0f);
	Tgame::enemiesMap.setNewImage(clearMap);
	Tgame::enemiesMap.setScale(1.0f, 1.0f);

	Tgame::enemiesBoatsLeftCounter = 10;
	Tgame::mineLeftBoatsCounter = 10;

	if (Tgame::AmIReady)
	{
		delete Tgame::AmIReady;
		Tgame::AmIReady = nullptr;
	}
	if (Tgame::isEnemyReady)
	{
		delete Tgame::isEnemyReady;
		Tgame::isEnemyReady = nullptr;
	}

	ThandleNetwork::status = STATUSOFCONNECTION::NOTHING;
	ThandleNetwork::receiver.terminate();

	Tgame::prepareMainMenu();
}
