#include"game.h"
#include"Tsounds.h"
#include<iostream>
#include <algorithm>

template< typename T >
struct delete_pointer_element
{
	void operator()(T element) const
	{
		delete element;
		element = nullptr;
	}
};

#define CONFIGTIMEINSECONDS sf::seconds(15) // 180
#define ROUNDTIMEINSECONDS sf::seconds(10)  // 60

enum cursor
{
	arrow = true,
	hand = false
};

bool Tgame::takenFields[10][10]{ false };
boatsPositions Tgame::inGameBoatsPositions[10][10]{ boatsPositions{ false, false } };
int Tgame::avaibleBoats[4]{ 0, 0, 0, 0 };
std::vector<boat*> Tgame::Boats;
std::vector<button*> Tgame::Buttons;
std::vector<sf::Drawable*> Tgame::AdditionalElements;
TavaibleBoatsText Tgame::textes{ fontPath, sf::Vector2f{600.0f, 110.0f} }; // position to change
ThandleNetwork Tgame::handleNetwork{};
playerReadyInformation* Tgame::isEnemyReady{ nullptr };
myReadyInformation* Tgame::AmIReady{ nullptr };
TgetCodeGUI* Tgame::getInvitingCodeGui{ nullptr };
TenterCodeGUI* Tgame::enterInvitingCodeGui{ nullptr };
Tdative* Tgame::dative{ nullptr };
bool Tgame::myRound{ false };
bool Tgame::allowClicking{ true };
int Tgame::enemiesBoatsLeftCounter{ 10 };
int Tgame::mineLeftBoatsCounter{ 10 };
sf::Text Tgame::boatsLeft{};


GAMESTATUS Tgame::status{ GAMESTATUS::NOTHING };

Ttimer Tgame::timer{};
Tmap Tgame::myMap{ sf::Vector2f{ 0.0f, 0.0f }, "RESOURCES/IMG/map.png" };
Tmap Tgame::enemiesMap{ myMap };

Tgame::Tgame() 
	: Drawable(), bg{ "RESOURCES/IMG/bg.png" }
{
	Tsounds::prepareClass();

	mouseCursorIsArrow.first.loadFromSystem(sf::Cursor::Arrow);
	mouseCursorIsArrow.second = cursor::arrow;
	timer.text.setFont(textes.font);

	this->prepareMainMenu(); // only this function should be here, but
	//this->prepareGameConfig(); // for debug
}

Tgame::~Tgame()
{
	clearObjectsToDraw();
	if (dative)
	{
		delete dative;
		dative = nullptr;
	}

	ThandleNetwork::waitForConnection.terminate();
	ThandleNetwork::receiver.terminate();
	ThandleNetwork::deleteHost();
}

void Tgame::prepareMainMenu()
{
	clearObjectsToDraw();

	status = GAMESTATUS::MAINMENU;

	createButton(fnc::MainMenu::joinGame, sf::Vector2f{ 370.0f, 200.0f }, sf::Vector2f{ 150.0f, 50.0f }, 20, "Join game", fontPath);
	createButton(fnc::MainMenu::hostGame, sf::Vector2f{ 370.0f, 270.0f }, sf::Vector2f{ 150.0f, 50.0f }, 20, "Host game", fontPath);
	createButton(fnc::MainMenu::settings, sf::Vector2f{ 370.0f, 340.0f }, sf::Vector2f{ 150.0f, 50.0f }, 20, "Settings", fontPath);
	createButton(fnc::MainMenu::how2Play, sf::Vector2f{ 370.0f, 410.0f }, sf::Vector2f{ 150.0f, 50.0f }, 20, "How to play", fontPath);
}

void Tgame::prepareGameConfig()
{
	status = GAMESTATUS::CONFIG;

	clearObjectsToDraw();

	myMap.setPosition(sf::Vector2f{ 17.0f, 20.0f });
	timer.resetTimer();
	timer.text.setPosition(sf::Vector2f{ 655.0f, 20.0f }); // position to change
	for (int i{ 0 }; i < 4; i++)
		Tgame::avaibleBoats[i] = 4 - i;
	textes.update();

	createReadyInformation(sf::Vector2f{ 640.0f, 440.0f }, sf::Vector2f{ 200.0f, 37.0f });

	std::string folders{ "RESOURCES/IMG/" };
	std::string fileName{ "0maszt.png" };
	for (int i{ 4 }; i >= 1; i--) 
	{
		fileName.at(0) = std::to_string(i).at(0);
		for (int j{ i }; j <= 4; j++) // first loop -> 4 loops, second loop -> 3 loops, third loop -> 2 loops, fourth loop -> 1 loop
		{
			createBoat(folders + fileName, sf::Vector2f{ 660.0f, 75.0f + i*70.0f});
		}
	}	
}

void Tgame::prepareGame(sf::RenderWindow& window)
{
	Tgame::status = GAMESTATUS::RUNTIME;

	Tgame::cleanAfterConfig();

	timer.text.setPosition(sf::Vector2f{ 595.0f, 20.0f }); // position to change
	Tgame::enemiesMap.setPosition(Tgame::myMap.getPosition());
	Tgame::dative = new Tdative{ window };

	static sf::Font font;
	font.loadFromFile(fontPath);

	Tgame::boatsLeft.setFont(font);
	Tgame::boatsLeft.setCharacterSize(35);
	Tgame::boatsLeft.setFillColor(sf::Color::Black);
	Tgame::boatsLeft.setPosition(sf::Vector2f{ 785.0f, 27.0f });
	Tgame::boatsLeft.setString("Boats\nleft: " + std::to_string(Tgame::enemiesBoatsLeftCounter));

	if (ThandleNetwork::status == STATUSOFCONNECTION::HOST)
	{
		Tgame::myRound = true;
		Tgame::myMap.setPosition(sf::Vector2f{ 585.0f, 110.0f });
		Tgame::myMap.setScale(0.549f, 0.549f);
		Tgame::dative->setPosition(static_cast<sf::Vector2f>( sf::Mouse::getPosition(window) ));
		Tgame::dative->onClick();
	}
	else if (ThandleNetwork::status == STATUSOFCONNECTION::JOINER)
	{
		Tgame::myRound = false;
		Tgame::enemiesMap.setPosition(sf::Vector2f{ 585.0f, 110.0f });
		Tgame::enemiesMap.setScale(0.549f, 0.549f);
	}

	Tgame::createButton(fnc::inGame::surrender, sf::Vector2f{ 635.0f, 440.0f }, sf::Vector2f{ 210.0f, 55.0f }, 30, "SURRENDER", fontPath);
}

void Tgame::cleanAfterConfig()
{
	sf::Image img{ Tgame::myMap.getMapsImage() };
	sf::FloatRect mapsCollisions{ Tgame::myMap.getMapsCollision() };
	sf::Vector2f boatsPos;
	sf::Image boatsImg;

	for (auto boat : Boats)
	{
		boatsPos = boat->getPosition();
		if (boat->orientation == orient::left)
			boatsPos.x -= Tmap::sizeOfField.x * (boat->size - 1);
		if(boat->orientation == orient::up)
			boatsPos.y -= Tmap::sizeOfField.y * (boat->size - 1);

		if (boat->orientation == orient::up)
		{
			boatsImg.loadFromFile("RESOURCES/IMG/" + std::to_string(boat->size) + "vertical.png");
			if (boat->isTurnedToRight)
			{
				boatsImg.flipVertically();
				boatsImg.flipHorizontally();
			}
			else
			{
				boatsImg.flipVertically();
			}
			
		}
		else if (boat->orientation == orient::down)
		{
			boatsImg.loadFromFile("RESOURCES/IMG/" + std::to_string(boat->size) + "vertical.png");
			if(!boat->isTurnedToRight)
				boatsImg.flipHorizontally();
		}
		else
		{
			boatsImg = boat->img;
			if (!boat->isTurnedToRight)
			{
				boatsImg.flipVertically();
				boatsImg.flipHorizontally();
			}
		}

		img.copy(boatsImg, 
			static_cast<unsigned int>(boatsPos.x - mapsCollisions.left + Tmap::sizeOfField.x / 2 ), 
			static_cast<unsigned int>( boatsPos.y - mapsCollisions.top + Tmap::sizeOfField.y / 2 + 1.0f ), 
			sf::IntRect{0,0,0,0}, true);

		for (int i{ 0 }; i < boat->size; i++)
		{
			switch (boat->orientation)
			{
			case orient::down:
			{
				Tgame::inGameBoatsPositions[boat->whichFieldWasTaking.x][boat->whichFieldWasTaking.y + i].taken = true;
				Tgame::inGameBoatsPositions[boat->whichFieldWasTaking.x][boat->whichFieldWasTaking.y + i].startingPos = boat->whichFieldWasTaking;
				Tgame::inGameBoatsPositions[boat->whichFieldWasTaking.x][boat->whichFieldWasTaking.y + i].boatsOrientationIfTaken = static_cast<orient>(boat->orientation);
				Tgame::inGameBoatsPositions[boat->whichFieldWasTaking.x][boat->whichFieldWasTaking.y + i].size = boat->size;
				Tgame::inGameBoatsPositions[boat->whichFieldWasTaking.x][boat->whichFieldWasTaking.y + i].isBoatTurnedToRight = boat->isTurnedToRight;
				break;
			}
			case orient::left:
			{
				Tgame::inGameBoatsPositions[boat->whichFieldWasTaking.x - i][boat->whichFieldWasTaking.y].taken = true;
				Tgame::inGameBoatsPositions[boat->whichFieldWasTaking.x - i][boat->whichFieldWasTaking.y].startingPos = boat->whichFieldWasTaking;
				Tgame::inGameBoatsPositions[boat->whichFieldWasTaking.x - i][boat->whichFieldWasTaking.y].boatsOrientationIfTaken = static_cast<orient>(boat->orientation);
				Tgame::inGameBoatsPositions[boat->whichFieldWasTaking.x - i][boat->whichFieldWasTaking.y].size = boat->size;
				Tgame::inGameBoatsPositions[boat->whichFieldWasTaking.x - i][boat->whichFieldWasTaking.y].isBoatTurnedToRight = boat->isTurnedToRight;
				break;
			}
			case orient::right:
			{
				Tgame::inGameBoatsPositions[boat->whichFieldWasTaking.x + i][boat->whichFieldWasTaking.y].taken = true;
				Tgame::inGameBoatsPositions[boat->whichFieldWasTaking.x + i][boat->whichFieldWasTaking.y].startingPos = boat->whichFieldWasTaking;
				Tgame::inGameBoatsPositions[boat->whichFieldWasTaking.x + i][boat->whichFieldWasTaking.y].boatsOrientationIfTaken = static_cast<orient>(boat->orientation);
				Tgame::inGameBoatsPositions[boat->whichFieldWasTaking.x + i][boat->whichFieldWasTaking.y].size = boat->size;
				Tgame::inGameBoatsPositions[boat->whichFieldWasTaking.x + i][boat->whichFieldWasTaking.y].isBoatTurnedToRight = boat->isTurnedToRight;
				break;
			}
			case orient::up:
			{
				Tgame::inGameBoatsPositions[boat->whichFieldWasTaking.x][boat->whichFieldWasTaking.y - i].taken = true;
				Tgame::inGameBoatsPositions[boat->whichFieldWasTaking.x][boat->whichFieldWasTaking.y - i].startingPos = boat->whichFieldWasTaking;
				Tgame::inGameBoatsPositions[boat->whichFieldWasTaking.x][boat->whichFieldWasTaking.y - i].boatsOrientationIfTaken = static_cast<orient>(boat->orientation);
				Tgame::inGameBoatsPositions[boat->whichFieldWasTaking.x][boat->whichFieldWasTaking.y - i].size = boat->size;
				Tgame::inGameBoatsPositions[boat->whichFieldWasTaking.x][boat->whichFieldWasTaking.y - i].isBoatTurnedToRight = boat->isTurnedToRight;
				break;
			}
			default:
				std::cout << "DEBUG: Invalid boat orientation" << std::endl;
				break;
			}
		}
	}
	Tgame::myMap.setNewImage(img);

	for (int i{ 0 }; i < 10; i++)
		for (int j{ 0 }; j < 10; j++)
			Tgame::takenFields[i][j] = false;

	Tgame::timer.resetTimer();

	Tgame::clearObjectsToDraw();
}

void Tgame::createBoat(std::string path, sf::Vector2f pos)
{
	boat* tempBoat{ new boat{ path, pos } };
	Boats.push_back(tempBoat);
}

button* Tgame::createButton(void (* const fnc)(), sf::Vector2f position, sf::Vector2f buttonSize, unsigned int charSize, std::string textOnButton, std::string pathToFont)
{
	button* tempButton{ new button{ fnc, position, buttonSize, charSize, textOnButton, pathToFont } };
	Buttons.push_back(tempButton);

	return tempButton;
}

void Tgame::createReadyInformation(sf::Vector2f pos, sf::Vector2f size)
{
	isEnemyReady = { new playerReadyInformation{ pos, size } };
}

void Tgame::clearObjectsToDraw()
{
	
	std::for_each(Boats.begin(), Boats.end(), delete_pointer_element<boat*>());
	Boats.clear();

	std::for_each(Buttons.begin(), Buttons.end(), delete_pointer_element<button*>());
	Buttons.clear();

	std::for_each(AdditionalElements.begin(), AdditionalElements.end(), delete_pointer_element<sf::Drawable*>());
	AdditionalElements.clear();

	if (isEnemyReady)
	{
		delete isEnemyReady;
		isEnemyReady = nullptr;
	}
	if (AmIReady)
	{
		delete AmIReady;
		AmIReady = nullptr;
	}
	if (getInvitingCodeGui)
	{
		delete getInvitingCodeGui;
		getInvitingCodeGui = nullptr;
	}
	if (enterInvitingCodeGui)
	{
		delete enterInvitingCodeGui;
		enterInvitingCodeGui = nullptr;
	}
}

void Tgame::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(bg, states);

	if (status == GAMESTATUS::CONFIG || status == GAMESTATUS::RUNTIME)
	{
		this->drawMapsAndTextes(target, states);
		this->drawTimer(target, states);
		
	}
	if(status != GAMESTATUS::NOTHING) this->drawButtons(target, states);
	this->drawBoats(target, states);
	this->drawAdditionals(target, states);
	if (boat::grabbedBoat)
		target.draw(*boat::grabbedBoat, states);

}

void Tgame::drawButtons(sf::RenderTarget& target, sf::RenderStates states) const
{
	for (auto button : Buttons)
	{
		if(button)
			target.draw(*button, states);
	}
}

void Tgame::drawBoats(sf::RenderTarget& target, sf::RenderStates states) const
{
	for (auto boat : Boats)
	{
		if(boat && boat != boat::grabbedBoat)
			target.draw(*boat, states);
	}
}

void Tgame::drawTimer(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(timer, states);
}

void Tgame::drawMapsAndTextes(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (status == GAMESTATUS::RUNTIME)
	{
		target.draw(enemiesMap, states);
		target.draw(boatsLeft, states);
	}
	else // status must be config then
	{
		target.draw(textes);
	}
	target.draw(myMap, states);
}

void Tgame::drawAdditionals(sf::RenderTarget& target, sf::RenderStates states) const
{
	if(status != GAMESTATUS::RUNTIME)
		for (auto additional : AdditionalElements)
		{
			if(additional)
				target.draw(*additional, states);
		}

	if (isEnemyReady)
		target.draw(*isEnemyReady, states);
	if (AmIReady)
		target.draw(*AmIReady, states);
	if (getInvitingCodeGui)
		target.draw(*getInvitingCodeGui, states);
	if (enterInvitingCodeGui)
		target.draw(*enterInvitingCodeGui, states);
	if (dative)
		target.draw(*dative, states);
}

void Tgame::resetTakenFields()
{
	for (int i{ 0 }; i < 10; i++)
	{
		for (int j{ 0 }; j < 10; j++)
		{
			Tgame::takenFields[i][j] = false;
		}
	}


	for (auto boat : Tgame::Boats)
	{
		if (boat == boat::grabbedBoat) continue;
		if (boat->whichFieldImTaking.x == NOTDEFINEDPOSITION || boat->whichFieldImTaking.y == NOTDEFINEDPOSITION) continue;

		sf::Vector2i takenFieldByBoat{ boat->whichFieldImTaking };
		for (int i{ 0 }; i < boat->size; i++)
		{
			switch (boat->orientation)
			{
			case orient::down:
				Tgame::markField(sf::Vector2i{ takenFieldByBoat.x, takenFieldByBoat.y + i });
				break;
			case orient::left:
				Tgame::markField(sf::Vector2i{ takenFieldByBoat.x - i , takenFieldByBoat.y });
				break;
			case orient::right:
				Tgame::markField(sf::Vector2i{ takenFieldByBoat.x + i  , takenFieldByBoat.y });
				break;
			case orient::up:
				Tgame::markField(sf::Vector2i{ takenFieldByBoat.x, takenFieldByBoat.y - i });
				break;

			default:
				std::cout << "DEBUG: Invalid boat orientation" << std::endl;
				break;
			}
		}
	}
}

void Tgame::markField(sf::Vector2i which)
{
	int x{ which.x }, y{ which.y };

	if(x > 0 && y > 0)	Tgame::takenFields[x - 1][y - 1] = true;
	if(y > 0)			Tgame::takenFields[x][y - 1] = true;
	if(x < 9 && y > 0)	Tgame::takenFields[x + 1][y - 1] = true;

	if(x > 0)			Tgame::takenFields[x - 1][y] = true;
						Tgame::takenFields[x][y] = true;
	if(x < 9)			Tgame::takenFields[x + 1][y] = true;

	if(x > 0 && y < 9)	Tgame::takenFields[x - 1][y + 1] = true;
	if(y < 9)			Tgame::takenFields[x][y + 1] = true;
	if(x < 9 && y < 9)	Tgame::takenFields[x + 1][y + 1] = true;

}

void Tgame::decreaseAmountOfAvaibleBoats(short int size)
{
	Tgame::avaibleBoats[size - 1]--;
}

void Tgame::update(sf::RenderWindow& window)
{
	bool isMouseOnSomething{ false };
	sf::Vector2i mousePos{sf::Mouse::getPosition(window)};

	for (auto boat : Boats)
	{
		if (boat->isMouseOnMe(static_cast<sf::Vector2f>(mousePos)))
		{
			if (mouseCursorIsArrow.second == cursor::arrow && mouseCursorIsArrow.first.loadFromSystem(sf::Cursor::Hand))
			{
				window.setMouseCursor(mouseCursorIsArrow.first);
				mouseCursorIsArrow.second = cursor::hand;
			}
			isMouseOnSomething = true;
		}
	}
	for (auto button : Buttons)
	{
		if (button->animateHover(mousePos))
		{
			if (mouseCursorIsArrow.second == cursor::arrow && mouseCursorIsArrow.first.loadFromSystem(sf::Cursor::Hand))
			{
				window.setMouseCursor(mouseCursorIsArrow.first);
				mouseCursorIsArrow.second = cursor::hand;
			}
			isMouseOnSomething = true;
		}
	}

	if(getInvitingCodeGui)
		if (getInvitingCodeGui->exitButton.isMouseOnMe(mousePos) || getInvitingCodeGui->copyCodeToClipBoard.animateHover(mousePos))
		{
			if (mouseCursorIsArrow.second == cursor::arrow && mouseCursorIsArrow.first.loadFromSystem(sf::Cursor::Hand))
			{
				window.setMouseCursor(mouseCursorIsArrow.first);
				mouseCursorIsArrow.second = cursor::hand;
			}
			isMouseOnSomething = true;
		}

	if (enterInvitingCodeGui)
		if (enterInvitingCodeGui->exitButton.isMouseOnMe(mousePos) || enterInvitingCodeGui->enterCode.animateHover(mousePos))
		{
			if (mouseCursorIsArrow.second == cursor::arrow && mouseCursorIsArrow.first.loadFromSystem(sf::Cursor::Hand))
			{
				window.setMouseCursor(mouseCursorIsArrow.first);
				mouseCursorIsArrow.second = cursor::hand;
			}
			isMouseOnSomething = true;
		}

	if (isMouseOnSomething == false)
	{
		if (mouseCursorIsArrow.second == cursor::hand && mouseCursorIsArrow.first.loadFromSystem(sf::Cursor::Arrow))
		{
			window.setMouseCursor(mouseCursorIsArrow.first);
			mouseCursorIsArrow.second = cursor::arrow;
		}
	}

	static bool createOneButton{ true };
	sf::Time time{ timer.getElapsedTime() };
	if (Tgame::status == GAMESTATUS::MAINMENU)
	{
		if (ThandleNetwork::status == STATUSOFCONNECTION::HOST || ThandleNetwork::status == STATUSOFCONNECTION::JOINER)
			Tgame::prepareGameConfig();
	}
	else if (Tgame::status == GAMESTATUS::CONFIG)
	{
		if (avaibleBoats[0] == 0 && avaibleBoats[1] == 0 && avaibleBoats[2] == 0 && avaibleBoats[3] == 0 && createOneButton)
		{
			this->createButton(fnc::Config::imReady, sf::Vector2f{ 636.0f, 500.0f }, sf::Vector2f{ 150.0f, 50.0f }, 20, "    READY", fontPath);
			this->AmIReady = new myReadyInformation{ sf::Vector2f{ 796.0f, 500.0f }, sf::Vector2f{ 50.0f, 50.0f } };
			createOneButton = false;
		}

		sf::Time timeElapsed{ CONFIGTIMEINSECONDS - time };

		timer.updateText(timeElapsed);

		this->countDown(timeElapsed, time, window);
	}
	else if (Tgame::status == GAMESTATUS::RUNTIME)
	{
		createOneButton = true;
		static bool prepareOnce{ true };
		if (Tgame::enemiesBoatsLeftCounter > 0 && Tgame::mineLeftBoatsCounter > 0) // OR ENEMY FF'ED
		{
			timer.updateText(ROUNDTIMEINSECONDS - time);
			if (time + sf::seconds(1.0f) > ROUNDTIMEINSECONDS && myRound == true && prepareOnce == true && Tdative::isFollowing == true)
			{
				Tgame::prepareDative();
				prepareOnce = false;
				allowClicking = false;
			}
			if (time > ROUNDTIMEINSECONDS + sf::seconds(0.1f) && myRound == true)
			{
				Tgame::timer.resetTimer();
				prepareOnce = true;
				fnc::inGame::shoot();
			}
		}

		if (Tgame::dative)
		{
			if (Tgame::myRound == true && Tdative::isFollowing == true)
			{
				sf::Packet packet;
				packet.clear();
				packet << TYPEOFSENTDATA::DATIVEPOS << Tgame::dative->getPosition().x << Tgame::dative->getPosition().y;
				Tgame::handleNetwork.socket.send(packet);
			}
			if (Tgame::dative->letMeDissolve == true)
			{
				sf::Uint32 color{ Tgame::dative->getColor().toInteger() };
				static bool secondPass{ false };

				if(color - 1 >= 4294967040 && (secondPass == true || myRound == true))
					Tgame::dative->setColor(sf::Color{ color - 1 });

				secondPass = !secondPass;
			}
		}
	}
	
}

void Tgame::countDown(const sf::Time& timeElapsed, const sf::Time& time, sf::RenderWindow& window)
{
	static bool countOnce{ true };
	static sf::Time startingTime;
	static sf::Text* text{ nullptr };

	if (countOnce && ((
		Tgame::AmIReady && Tgame::isEnemyReady &&
		Tgame::AmIReady->imReady == true && 
		Tgame::isEnemyReady->isReady == true &&
		timeElapsed.asSeconds() > 3.5f) 
		|| (time > CONFIGTIMEINSECONDS)))
	{
		startingTime = time;
		countOnce = false;
		Tgame::allowClicking = false;

		Tgame::randomNotPlacedBoatsPositions();

		static sf::Font font;
		font.loadFromFile(fontPath);

		text = new sf::Text{};
		text->setFont(font);
		text->setCharacterSize(400);
		text->setFillColor(sf::Color::Black);
		text->setPosition(sf::Vector2f{ 340.0f, 50.0f });
		text->setString("3");
		Tgame::AdditionalElements.push_back(text);
	}
	else if (!countOnce)
	{ // here updating/animating counting
		int timeCounted{ 3 - static_cast<int>((time - startingTime).asSeconds()) };

		if (timeCounted < 0)
		{
			Tgame::prepareGame(window);
			Tgame::allowClicking = true;
			countOnce = true;
			return;
		}

		std::string str{ std::to_string(timeCounted) };
		if (text) text->setString(str);
	}
}

void Tgame::randomNotPlacedBoatsPositions()
{
	if (boat::grabbedBoat)
		boat::grabbedBoat->cancelPuttingBoat(false);

	static std::random_device dev;
	static std::mt19937 rng(dev());
	static std::uniform_int_distribution<std::mt19937::result_type> dist6(0, 9);

	sf::Vector2i currentField;
	sf::Vector2i startingField;

	for (auto boat : Boats)
	{
		if (boat->wasGrabbed == false)
		{ // not grabbed so random its position
			startingField = currentField = sf::Vector2i{ int(dist6(rng)), int(dist6(rng)) };

		findingFreeSpace:
			if (++currentField.x > 9)
			{
				if (++currentField.y > 9)
					currentField.y = 0;
				currentField.x = 0;
			}
			while (Tgame::takenFields[currentField.x][currentField.y] == true) // is taken
			{
				currentField.x++;
				if (currentField.x > 9)
				{
					currentField.y++;
					currentField.x = 0;
				}
				if (currentField.y > 9)
				{
					currentField.y = 0;
				}

				if (startingField == currentField)
				{
					std::cout << "something went wrong in randomNotPlacedBoatsPositions function" << std::endl;
					return;
				}
			} 
			
			int i{ -1 };
		checkNextDirection:
			i++;
			for (i; i < 4;i++) // in every direction
			{
				if (boat->size == 1) goto setPlace;
				for (int j{ 1 }; j < boat->size; j++) // check all positions
				{
					switch (i) // i is orientation
					{
					case orient::down:
						if (currentField.y + j > 9 || Tgame::takenFields[currentField.x][currentField.y + j] == true)
							goto checkNextDirection;
						if (j + 1 == boat->size)
							goto setPlace;
						break;
					case orient::left:
						if (currentField.x - j < 0 || Tgame::takenFields[currentField.x - j][currentField.y] == true)
							goto checkNextDirection;
						if (j + 1 == boat->size)
							goto setPlace;
						break;
					case orient::right:
						if (currentField.x + j > 9 || Tgame::takenFields[currentField.x + j][currentField.y] == true)
							goto checkNextDirection;
						if (j + 1 == boat->size)
							goto setPlace;
						break;
					case orient::up:
						if (currentField.y - j < 0 || Tgame::takenFields[currentField.x][currentField.y - j] == true)
							goto findingFreeSpace;
						if (j + 1 == boat->size)
							goto setPlace;
						break;

					default:
						std::cout << "DEBUG: Invalid boat orientation" << std::endl;
						break;
					}
				}
			}
			// found free space
		setPlace:

			Tgame::decreaseAmountOfAvaibleBoats(boat->size);
			Tgame::textes.update();
			boat->wasGrabbed = true;

			boat->whichFieldWasTaking = boat->whichFieldImTaking = currentField;
			
			sf::FloatRect map{ Tgame::myMap.getMapsCollision() };
			sf::FloatRect field{ sf::Vector2f{ map.left + currentField.x * Tmap::sizeOfField.x, map.top + currentField.y * Tmap::sizeOfField.y }, Tmap::sizeOfField };
			boat->setPosition(sf::Vector2f{ field.left + Tmap::sizeOfField.x / 1.9f, field.top + Tmap::sizeOfField.y / 2.05f });

			boat::grabbedBoat = boat;
			while (boat->orientation != i)
				boat::turn(true);
			boat::grabbedBoat = nullptr;

			Tgame::resetTakenFields();
		}
	}
}

void Tgame::onClick(sf::RenderWindow& window, sf::Vector2i mousePos, bool mouseIsPressed)
{
	if (Tgame::allowClicking)
	{
		for (auto button : Buttons)
		{
			if (button->originPos == button->shape.getPosition() && button->shape.getPosition().x != 0)
				button->mouseClicked(mouseIsPressed, mousePos);
		}
		if (mouseIsPressed)
		{
			for (auto boat : Boats)
			{
				if (boat->onClick(window, Tgame::myMap))
				{
					break;
				}
			}
		}

		if (getInvitingCodeGui)
		{
			getInvitingCodeGui->copyCodeToClipBoard.mouseClicked(mouseIsPressed, mousePos);

			if (getInvitingCodeGui->exitButton.isMouseOnMe(mousePos))
				getInvitingCodeGui->exitButton.onClick();
		}
		if (enterInvitingCodeGui)
		{
			enterInvitingCodeGui->enterCode.mouseClicked(mouseIsPressed, mousePos);

			if (enterInvitingCodeGui->exitButton.isMouseOnMe(mousePos))
				enterInvitingCodeGui->exitButton.onClick();
		}

		if (mouseIsPressed && dative)
			dative->onClick();
	}
}

void Tgame::onKeyPressed(sf::Keyboard::Key key)
{
	if (this->status == GAMESTATUS::MAINMENU)
	{
		if (this->enterInvitingCodeGui)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && key == sf::Keyboard::V)
			{
				Tsounds::play(SOUNDTYPE::ENTERLETTER);

				std::string str{ sf::Clipboard::getString() };

				if (str.length() > 100) return; // for safety

				for (auto character : str)
				{
					if (static_cast<int>(character) >= 48 && static_cast<int>(character) <= 57) // numbers only
						if (!this->enterInvitingCodeGui->enterDigit(character))
							break;
				}
			}

			if (key == sf::Keyboard::Enter)
			{
				this->enterInvitingCode();
				Tsounds::play(SOUNDTYPE::ENTERLETTER);
			}

			if (key == sf::Keyboard::Backspace)
			{
				this->enterInvitingCodeGui->deleteCharacter();
				Tsounds::play(SOUNDTYPE::ENTERLETTER);
			}

			if (key > 25 && key < 36) // numbers only
			{
				this->enterInvitingCodeGui->enterDigit(std::to_string(int(key) - 26).at(0));
				Tsounds::play(SOUNDTYPE::ENTERLETTER);
			}
		}
		if (this->getInvitingCodeGui)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && key == sf::Keyboard::C)
			{
				fnc::MainMenu::copyCodeToClipBoard();
				Tsounds::play(SOUNDTYPE::ENTERLETTER);
			}
		}

		if (key == sf::Keyboard::Escape)
			fnc::MainMenu::deleteGetCodeGUI();
	}

	else if (status == GAMESTATUS::CONFIG)
	{
		if (key == sf::Keyboard::Left)
			this->turnBoat(true);
		if (key == sf::Keyboard::Right)
			this->turnBoat(false);
	}
}

void Tgame::turnBoat(bool left)
{
	boat::turn(left);
}

void Tgame::prepareDative()
{
	Tgame::dative->setScale(1.0f, 1.0f);
	Tgame::dative->follower.terminate();
	Tgame::dative->randomField();

	Tdative::isFollowing = false;

	// here random position if not defined yet
	sf::FloatRect field{ sf::Vector2f{ Tgame::enemiesMap.getMapsCollision().left + Tgame::dative->whichFieldWasTaking.x * Tmap::sizeOfField.x,
		Tgame::enemiesMap.getMapsCollision().top + Tgame::dative->whichFieldWasTaking.y * Tmap::sizeOfField.y},
		Tmap::sizeOfField };

	Tgame::dative->setPosition(sf::Vector2f{ field.left + Tmap::sizeOfField.x / 1.9f - 1.0f, field.top + Tmap::sizeOfField.y / 2.05f + 1.0f });

	sf::Packet packet;
	packet.clear();
	packet << TYPEOFSENTDATA::DATIVEPOS << Tgame::dative->getPosition().x << Tgame::dative->getPosition().y;
	Tgame::handleNetwork.socket.send(packet);
}

void Tgame::animateShoot()
{
	Tgame::allowClicking = false;

	Tgame::dative->setTexture(Tgame::dative->txtOfExplosion);
	Tgame::dative->letMeDissolve = true; // unlock animating inside update() function

	Tsounds::play(SOUNDTYPE::EXPLOSION, 70.0f);
}

void Tgame::endRound(short int MissHitOrSuken)
{
	Tgame::status = GAMESTATUS::NOTHING;

	static sf::Font font;
	font.loadFromFile(fontPath);

	sf::Text* txt{ new sf::Text{} };
	txt->setFont(font);
	txt->setCharacterSize(110);
	txt->setFillColor(sf::Color::Black);
	txt->setPosition(sf::Vector2f{ 90.0f, 170.0f });

	Tgame::dative->setPosition(NOTDEFINEDPOSITION, NOTDEFINEDPOSITION);


	if (MissHitOrSuken == 0)
	{
		txt->setString("MISSED SHOOT\n  NEXT ROUND"); 
		txt->setPosition(sf::Vector2f{ 30.0f, 180.0f });
	}
	else if (MissHitOrSuken == 1)
	{
		txt->setString("HITTED SHIP\nNEXT ROUND");
		txt->setPosition(sf::Vector2f{ 100.0f, 180.0f });

		Tsounds::play(SOUNDTYPE::HIT);
	}
	else if (MissHitOrSuken == 2)
	{
		txt->setString("SUNKEN SHIP\nNEXT  ROUND");
		txt->setPosition(sf::Vector2f{ 85.0f, 180.0f });

		Tsounds::play(SOUNDTYPE::SUKEN);
	}
	else if (MissHitOrSuken == 3)
	{
		txt->setString("SURRENDER\nGAME OVER");
		txt->setPosition(sf::Vector2f{ 105.0f, 180.0f });
	}

	Tgame::AdditionalElements.push_back(txt);

	sf::sleep(sf::seconds(5.0f));

	if (Tdative::shootButton)
		for (std::vector<button*>::iterator it = Tgame::Buttons.begin(); it != Tgame::Buttons.end(); it++)
			if (*it == Tdative::shootButton)
			{
				Tgame::Buttons.erase(it);
				delete Tdative::shootButton;
				Tdative::shootButton = nullptr;
				break;
			}

	if (Tgame::enemiesBoatsLeftCounter > 0 && Tgame::mineLeftBoatsCounter > 0) // OR ENEMY SURRENDERED
	{

		if (Tgame::myRound == false)
		{
			Tgame::myRound = true;

			Tgame::myMap.setPosition(sf::Vector2f{ 585.0f, 110.0f });
			Tgame::myMap.setScale(0.549f, 0.549f);

			Tgame::enemiesMap.setPosition(sf::Vector2f{ 17.0f, 20.0f });
			Tgame::enemiesMap.setScale(1.0f, 1.0f);

			Tgame::dative->whichFieldWasTaking = { NOTDEFINEDPOSITION, NOTDEFINEDPOSITION };
			Tgame::dative->setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*Tgame::dative->window)));
			Tgame::dative->onClick();
		}
		else
		{
			Tgame::myRound = false;

			Tgame::enemiesMap.setPosition(sf::Vector2f{ 585.0f, 110.0f });
			Tgame::enemiesMap.setScale(0.549f, 0.549f);

			Tgame::myMap.setPosition(sf::Vector2f{ 17.0f, 20.0f });
			Tgame::myMap.setScale(1.0f, 1.0f);
		}

		Tgame::dative->letMeDissolve = false;
		Tgame::dative->setTexture(Tgame::dative->txtOfDative);
		Tgame::dative->setColor(sf::Color::White);

		for (std::vector<sf::Drawable*>::iterator it = Tgame::AdditionalElements.begin(); it != Tgame::AdditionalElements.end(); it++)
			if (*it == txt)
			{
				Tgame::AdditionalElements.erase(it);
				delete txt;
				txt = nullptr;
				break;
			}

		Tgame::timer.resetTimer();
		Tgame::status = GAMESTATUS::RUNTIME;
		Tgame::allowClicking = true;

	}
	else
	{
		if (Tgame::enemiesBoatsLeftCounter > 0) // check if enemy surrendered
		{
			txt->setString("LOSE");
			txt->setCharacterSize(300);
			txt->setPosition(60.0f, 100.0f);

			Tsounds::play(SOUNDTYPE::LOSE);
		}
		else
		{
			txt->setString("WIN"); 
			txt->setCharacterSize(300);
			txt->setPosition(170.0f, 100.0f);

			Tsounds::play(SOUNDTYPE::WIN);
		}

		delete Tgame::dative;
		Tgame::dative = nullptr;

		sf::sleep( sf::seconds(5.0f) );
		for (std::vector<sf::Drawable*>::iterator it = Tgame::AdditionalElements.begin(); it != Tgame::AdditionalElements.end(); it++)
			if (*it == txt)
			{
				Tgame::AdditionalElements.erase(it);
				delete txt;
				txt = nullptr;
				break;
			}
		sf::sleep(sf::seconds(0.5f));

		Tgame::timer.updateText(sf::seconds(0.f));

		std::for_each(Buttons.begin(), Buttons.end(), delete_pointer_element<button*>());
		Buttons.clear();

		Tgame::createButton(fnc::inGame::backToMenu, sf::Vector2f{ 635.0f, 505.0f }, sf::Vector2f{ 210.0f, 55.0f }, 24, "BACK TO MENU", fontPath);
		Tgame::status = GAMESTATUS::RUNTIME;
		Tgame::allowClicking = true;
	}
}

void Tgame::enterInvitingCode()
{
	static short numberOfTrys{ 1 };
	std::string str{ Tgame::enterInvitingCodeGui->code.getString() };
	if (str.length() != 5)
	{
		std::string newDescriptonStr{ "  TOO SHORT CODE\n       TRY AGAIN" };
		for (short i{ 1 }; i < numberOfTrys; i++)
			newDescriptonStr.append(" .");
		numberOfTrys = (numberOfTrys++) % 4;
		Tsounds::play(SOUNDTYPE::WRONGCODE);
		Tgame::enterInvitingCodeGui->descripton.setString(newDescriptonStr);
		return;
	}
	unsigned int port{ std::stoul(str) };
	if (!ThandleNetwork::becomeAJoiner(port))
	{
		std::string newDescriptonStr{ "CONNECTION FAILED\n       TRY AGAIN" };
		for (short i{ 1 }; i < numberOfTrys; i++)
			newDescriptonStr.append(" .");
		numberOfTrys = (numberOfTrys++) % 4;
		Tsounds::play(SOUNDTYPE::WRONGCODE);
		Tgame::enterInvitingCodeGui->descripton.setString(newDescriptonStr);
	}
}

void Tgame::createGetCodeGUI()
{
	if(!getInvitingCodeGui)
		Tgame::getInvitingCodeGui = new TgetCodeGUI{ sf::Vector2f{ 275.0f, 200.0f }, sf::Vector2f{ 350.0f, 150.0f } };
}

void Tgame::createInviteCodeGUI()
{
	if (!enterInvitingCodeGui)
		Tgame::enterInvitingCodeGui = new TenterCodeGUI{ sf::Vector2f{ 275.0f, 200.0f }, sf::Vector2f{ 350.0f, 150.0f } };
}

