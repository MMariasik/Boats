#include"game.h"
#include<iostream>

STATUSOFCONNECTION ThandleNetwork::status{ STATUSOFCONNECTION::NOTHING };

sf::IpAddress ThandleNetwork::localIP{ sf::IpAddress::getLocalAddress() };

sf::TcpListener* ThandleNetwork::listener{ nullptr };
sf::TcpSocket ThandleNetwork::socket;

sf::Thread ThandleNetwork::waitForConnection{ &ThandleNetwork::waitForPlayer };
sf::Thread ThandleNetwork::receiver{ &ThandleNetwork::receiveData };


ThandleNetwork::ThandleNetwork()
{
}

ThandleNetwork::~ThandleNetwork()
{
	this->waitForConnection.terminate();
	this->receiver.terminate();
	ThandleNetwork::deleteHost();
}

int ThandleNetwork::becomeAHost()
{
	ThandleNetwork::listener = new sf::TcpListener{};

	unsigned short port{ 0 };
	do
	{
		port = randomAPort();

	} while (listener->listen(port, localIP) != sf::Socket::Done);

	waitForConnection.launch();
	
	return port;
}

void ThandleNetwork::deleteHost()
{
	if (ThandleNetwork::listener)
	{
		listener->close();
		delete listener;
		listener = nullptr;
	}
}

bool ThandleNetwork::becomeAJoiner(unsigned short port)
{
	if (socket.connect(localIP, port, sf::seconds(1.0f)) == sf::Socket::Done)
	{
		receiver.launch();

		status = STATUSOFCONNECTION::JOINER;
		return true;
	}
	else return false;
}

unsigned short ThandleNetwork::randomAPort()
{
	static std::random_device dev;
	static std::mt19937 rng(dev());
	static std::uniform_int_distribution<std::mt19937::result_type> dist6(10000, 65535);

	return dist6(rng);
}

void ThandleNetwork::waitForPlayer()
{
	if (ThandleNetwork::listener)
	{
		if (listener->accept(ThandleNetwork::socket) == sf::Socket::Done)
		{
			receiver.launch();

			status = STATUSOFCONNECTION::HOST;
		}
	}
}

void ThandleNetwork::receiveData()
{
	sf::Packet packet;
	while (1)
	{
		if (socket.receive(packet) == sf::Socket::Done)
		{
			short whatWasSent;
			packet >> whatWasSent;
			switch (whatWasSent)
			{
				case TYPEOFSENTDATA::IMREADY:
				{
					Tgame::isEnemyReady->update();
					break;
				}
				case TYPEOFSENTDATA::DATIVEPOS:
				{
					sf::Vector2f pos;
					packet >> pos.x >> pos.y;
					
					if (Tgame::dative)
						Tgame::dative->update(pos);
	   				
					break;
				}
				case TYPEOFSENTDATA::SHOOT:
				{
					Tgame::animateShoot();

					sf::Vector2u shootedFieldPos;
					packet >> shootedFieldPos.x >> shootedFieldPos.y;

					boatsPositions* shootedField{ &Tgame::inGameBoatsPositions[shootedFieldPos.x][shootedFieldPos.y] };
					Tgame::dative->whichFieldWasTaking = shootedFieldPos;

					short feedback;

					sf::Image mapsImg{ Tgame::myMap.getMapsImage() };
					sf::FloatRect mapsCollisions{ Tgame::myMap.getMapsCollision() };

					if (shootedField->taken == false)
					{ // empty field

						shootedField->shooted = true;
						feedback = 0;

						packet.clear();
						packet << TYPEOFSENTDATA::SHOOTFEEDBACK << feedback; // MISS

						while (ThandleNetwork::socket.send(packet) != sf::Socket::Done)
						{
							std::cout << "DEBUG something went wrong while sending shoot feedback" << std::endl;
						}
						sf::sleep(sf::seconds(0.5f));
	
						sf::Image xImg; xImg.loadFromFile("RESOURCES/IMG/x.png");
						mapsImg.copy(xImg,
							static_cast<unsigned int>( mapsCollisions.left + Tmap::sizeOfField.x * Tgame::dative->whichFieldWasTaking.x - Tmap::sizeOfField.x / 3 + 1.0f ),
							static_cast<unsigned int>( mapsCollisions.top + Tmap::sizeOfField.y * Tgame::dative->whichFieldWasTaking.y - Tmap::sizeOfField.y / 3 - 2.0f ),
							sf::IntRect{ 0, 0, 0, 0 }, true);

					}
					else
					{ // taken field

						bool suken{ true };
						shootedField->shooted = true;
						for (int i{ 0 }; i < shootedField->size; i++)
						{
							switch (shootedField->boatsOrientationIfTaken)
							{
							case orient::down: // y + i
							{
								if (Tgame::inGameBoatsPositions[shootedField->startingPos.x][shootedField->startingPos.y + i].shooted == false)
								{
									suken = false;
								}

								break;
							}
							case orient::left: // x - i
							{
								if (Tgame::inGameBoatsPositions[shootedField->startingPos.x - i][shootedField->startingPos.y].shooted == false)
								{
									suken = false;
								}

								break;
							}
							case orient::right: // x + i
							{
								if (Tgame::inGameBoatsPositions[shootedField->startingPos.x + i][shootedField->startingPos.y].shooted == false)
								{
									suken = false;
								}

								break;
							}
							case orient::up: // y - i
							{
								if (Tgame::inGameBoatsPositions[shootedField->startingPos.x][shootedField->startingPos.y - i].shooted == false)
								{
									suken = false;
								}

								break;
							}
							default:
								std::cout << "DEBUG: Invalid boat orientation" << std::endl;
								break;
							}	
						}

						if (suken == false)
						{
							shootedField->shooted = true;
							feedback = 1;

							packet.clear();
							packet << TYPEOFSENTDATA::SHOOTFEEDBACK << feedback; // HIT

							while (ThandleNetwork::socket.send(packet) != sf::Socket::Done)
							{
								std::cout << "DEBUG something went wrong while sending shoot feedback" << std::endl;
							}
							sf::sleep(sf::seconds(0.5f));

							sf::Image oImg; oImg.loadFromFile("RESOURCES/IMG/o.png");
							mapsImg.copy(oImg,
								static_cast<unsigned int>( mapsCollisions.left + Tmap::sizeOfField.x * Tgame::dative->whichFieldWasTaking.x - Tmap::sizeOfField.x / 3 + 1.0f ),
								static_cast<unsigned int>( mapsCollisions.top + Tmap::sizeOfField.y * Tgame::dative->whichFieldWasTaking.y - Tmap::sizeOfField.y / 3 - 2.0f ),
								sf::IntRect{ 0, 0, 0, 0 }, true);
						}
						else
						{

							feedback = 2;
							Tgame::mineLeftBoatsCounter--;

							packet.clear();
							packet << TYPEOFSENTDATA::SHOOTFEEDBACK << feedback << shootedField->startingPos.x << shootedField->startingPos.y << 
								short(shootedField->boatsOrientationIfTaken) << shootedField->size << shootedField->isBoatTurnedToRight; // SUKEN SHIP
							while (ThandleNetwork::socket.send(packet) != sf::Socket::Done)
							{
								std::cout << "DEBUG something went wrong while sending shoot feedback" << std::endl;
							}
							sf::sleep(sf::seconds(0.5f));

							if (shootedField->boatsOrientationIfTaken == orient::left)
								shootedField->startingPos.x -= (shootedField->size - 1);
							if (shootedField->boatsOrientationIfTaken == orient::up)
								shootedField->startingPos.y -= (shootedField->size - 1);


							sf::Image boatsImg;
							if (shootedField->boatsOrientationIfTaken == orient::up)
							{
								boatsImg.loadFromFile("RESOURCES/IMG/" + std::to_string(shootedField->size) + "verticalfire.png");
								if (shootedField->isBoatTurnedToRight)
								{
									boatsImg.flipVertically();
									boatsImg.flipHorizontally();
								}
								else
								{
									boatsImg.flipVertically();
								}
							}
							else if (shootedField->boatsOrientationIfTaken == orient::down)
							{
								boatsImg.loadFromFile("RESOURCES/IMG/" + std::to_string(shootedField->size) + "verticalfire.png");
								if (!shootedField->isBoatTurnedToRight)
									boatsImg.flipHorizontally();
							}
							else if (shootedField->boatsOrientationIfTaken == orient::left)
							{
								boatsImg.loadFromFile("RESOURCES/IMG/" + std::to_string(shootedField->size) + "masztfire.png");
								boatsImg.flipHorizontally();
							}
							else if (shootedField->boatsOrientationIfTaken == orient::right)
							{
								boatsImg.loadFromFile("RESOURCES/IMG/" + std::to_string(shootedField->size) + "masztfire.png");
							}

							mapsImg.copy(boatsImg, 
								static_cast<unsigned int>( shootedField->startingPos.x * Tmap::sizeOfField.x + mapsCollisions.left - Tmap::sizeOfField.x / 3 + 2.0f ),
								static_cast<unsigned int>( shootedField->startingPos.y * Tmap::sizeOfField.y + mapsCollisions.top - Tmap::sizeOfField.y / 3 - 2.0f ),
								sf::IntRect{ 0,0,0,0 }, true);

							sf::Image oImg; oImg.loadFromFile("RESOURCES/IMG/o.png");
							sf::Vector2f mapsLeftTop{ mapsCollisions.left, mapsCollisions.top };

							for (int i{ 0 }; i < shootedField->size; i++)
							{
								switch (shootedField->boatsOrientationIfTaken)
								{

								case orient::right:
								{
									ThandleNetwork::markInGameBoatsPositions(shootedField->startingPos.x + i, shootedField->startingPos.y, oImg, mapsImg, mapsLeftTop);
									break;
								}
								case orient::left:
								{
									ThandleNetwork::markInGameBoatsPositions(shootedField->startingPos.x + i, shootedField->startingPos.y, oImg, mapsImg, mapsLeftTop);
									break;
								}
								case orient::down:
								{
									ThandleNetwork::markInGameBoatsPositions(shootedField->startingPos.x, shootedField->startingPos.y + i, oImg, mapsImg, mapsLeftTop);
									break;
								}
								case orient::up:
								{
									ThandleNetwork::markInGameBoatsPositions(shootedField->startingPos.x, shootedField->startingPos.y + i, oImg, mapsImg, mapsLeftTop);
									break;
								}

								default:
									break;
								}
							}
						}
					}

					Tgame::myMap.setNewImage(mapsImg);

					sf::sleep(sf::seconds(2.0f));
					Tgame::endRound(feedback);

					break;
				}
				case TYPEOFSENTDATA::SHOOTFEEDBACK:
				{
					sf::sleep(sf::seconds(0.5f));

					short feedback;
					packet >> feedback;

					sf::Image mapsImg{ Tgame::enemiesMap.getMapsImage() };
					sf::FloatRect mapsCollisions{ Tgame::enemiesMap.getMapsCollision() };

					if (feedback == 0) // miss
					{
						Tgame::takenFields[Tgame::dative->whichFieldWasTaking.x][Tgame::dative->whichFieldWasTaking.y] = true;

						sf::Image xImg; xImg.loadFromFile("RESOURCES/IMG/x.png");
						mapsImg.copy( xImg, 
							static_cast<unsigned int>( mapsCollisions.left + Tmap::sizeOfField.x * Tgame::dative->whichFieldWasTaking.x - Tmap::sizeOfField.x / 3 + 1.0f ),
							static_cast<unsigned int>( mapsCollisions.top + Tmap::sizeOfField.y * Tgame::dative->whichFieldWasTaking.y - Tmap::sizeOfField.y / 3 - 2.0f ),
									  sf::IntRect{0, 0, 0, 0}, true );
					}
					else if (feedback == 1) // hit
					{
						Tgame::takenFields[Tgame::dative->whichFieldWasTaking.x][Tgame::dative->whichFieldWasTaking.y] = true;

						sf::Image oImg; oImg.loadFromFile("RESOURCES/IMG/o.png");
						mapsImg.copy(oImg,
							static_cast<unsigned int>( mapsCollisions.left + Tmap::sizeOfField.x * Tgame::dative->whichFieldWasTaking.x - Tmap::sizeOfField.x / 3 + 1.0f ),
							static_cast<unsigned int>( mapsCollisions.top + Tmap::sizeOfField.y * Tgame::dative->whichFieldWasTaking.y - Tmap::sizeOfField.y / 3 - 2.0f ),
							sf::IntRect{ 0, 0, 0, 0 }, true);
					}
					else if (feedback == 2) // suken ship
					{
						sf::Vector2i boatsPos;
						short boatsOrientation;
						short boatsSize;
						bool isBoatTurnedToRight;
						sf::Image boatsImg;

						packet >> boatsPos.x >> boatsPos.y >> boatsOrientation >> boatsSize >> isBoatTurnedToRight;
						

						if (boatsOrientation == orient::left)
							boatsPos.x -= (boatsSize - 1);
						if (boatsOrientation == orient::up)
							boatsPos.y -= (boatsSize - 1);


						if (boatsOrientation == orient::up)
						{
							boatsImg.loadFromFile("RESOURCES/IMG/" + std::to_string(boatsSize) + "verticalfire.png");
							if (isBoatTurnedToRight)
							{
								boatsImg.flipVertically();
								boatsImg.flipHorizontally();
							}
							else
							{
								boatsImg.flipVertically();
							}
						}
						else if (boatsOrientation == orient::down)
						{
							boatsImg.loadFromFile("RESOURCES/IMG/" + std::to_string(boatsSize) + "verticalfire.png");
							if (!isBoatTurnedToRight)
								boatsImg.flipHorizontally();
						}
						else if(boatsOrientation == orient::left)
						{
							boatsImg.loadFromFile("RESOURCES/IMG/" + std::to_string(boatsSize) + "masztfire.png");
							boatsImg.flipHorizontally();
						}
						else if (boatsOrientation == orient::right)
						{
							boatsImg.loadFromFile("RESOURCES/IMG/" + std::to_string(boatsSize) + "masztfire.png");
						}

						mapsImg.copy(boatsImg, 
									 static_cast<unsigned int>( boatsPos.x * Tmap::sizeOfField.x + mapsCollisions.left - Tmap::sizeOfField.x / 3 + 2.0f ), 
									 static_cast<unsigned int>( boatsPos.y * Tmap::sizeOfField.y + mapsCollisions.top - Tmap::sizeOfField.y / 3 - 2.0f ), 
									sf::IntRect{ 0,0,0,0 }, true);

						sf::Image oImg; oImg.loadFromFile("RESOURCES/IMG/o.png");
						sf::Vector2f mapsLeftTop{ mapsCollisions.left, mapsCollisions.top };

						for (int i{ 0 }; i < boatsSize; i++)
						{
							switch (boatsOrientation)
							{
							case orient::right:
							{
								ThandleNetwork::markTakenFields(boatsPos.x + i, boatsPos.y, oImg, mapsImg, mapsLeftTop);
								break;
							}
							case orient::left:
							{
								ThandleNetwork::markTakenFields(boatsPos.x + i, boatsPos.y, oImg, mapsImg, mapsLeftTop);
								break;
							}
							case orient::down:
							{
								ThandleNetwork::markTakenFields(boatsPos.x, boatsPos.y + i, oImg, mapsImg, mapsLeftTop);
								break;
							}
							case orient::up:
							{
								ThandleNetwork::markTakenFields(boatsPos.x, boatsPos.y + i, oImg, mapsImg, mapsLeftTop);
								break;
							}
							}
						}
						Tgame::enemiesBoatsLeftCounter--;
						Tgame::boatsLeft.setString("Boats\nleft: " + std::to_string(Tgame::enemiesBoatsLeftCounter));
					}
					Tgame::enemiesMap.setNewImage(mapsImg);

					sf::sleep(sf::seconds( 2.0f ));
					Tgame::endRound(feedback);
					break;
				}
				case TYPEOFSENTDATA::SURRENDER:
				{
					sf::Packet packet;
					packet.clear();
					packet << TYPEOFSENTDATA::SURRENDERFEEDBACK;
					while (ThandleNetwork::socket.send(packet) != sf::Socket::Done)
					{
						std::cout << "something went wrong with surrender info" << std::endl;
					}
					
					Tgame::enemiesBoatsLeftCounter = 0;
					Tgame::endRound(3);

					break;
				}
				case TYPEOFSENTDATA::SURRENDERFEEDBACK:
				{
					Tgame::mineLeftBoatsCounter = 0;
					Tgame::endRound(3);

					break;
				}
			}
		}
		packet.clear();
	}
}

void ThandleNetwork::markInGameBoatsPositions(unsigned int fieldX, unsigned int fieldY, sf::Image& oImg, sf::Image& mapsImg, sf::Vector2f& mapsLeftTop)
{
	bool OnLeftEdge{false};
	bool onRightEdge{false};
	bool onTopEdge{ false };
	bool onBottomEdge{ false };

	if (fieldX == 0)
		OnLeftEdge = true;
	if (fieldX == 9)
		onRightEdge = true;
	if (fieldY == 0)
		onTopEdge = true;
	if (fieldY == 9)
		onBottomEdge = true;

	if (!onTopEdge)
	{
		if (!OnLeftEdge) 
		{ 
			Tgame::inGameBoatsPositions[fieldX - 1][fieldY - 1].shooted = true; 

			mapsImg.copy(oImg, 
				static_cast<unsigned int>( (fieldX - 1) * Tmap::sizeOfField.x + mapsLeftTop.x - Tmap::sizeOfField.x / 3 + 1.0f ),
				static_cast<unsigned int>( (fieldY - 1) * Tmap::sizeOfField.y + mapsLeftTop.y - Tmap::sizeOfField.y / 3 - 2.0f ),
				sf::IntRect{ 0,0,0,0 }, true);
		}
		{
			Tgame::inGameBoatsPositions[fieldX][fieldY - 1].shooted = true;

			mapsImg.copy(oImg, 
				static_cast<unsigned int>( fieldX * Tmap::sizeOfField.x + mapsLeftTop.x - Tmap::sizeOfField.x / 3 + 1.0f ),
				static_cast<unsigned int>( (fieldY - 1) * Tmap::sizeOfField.y + mapsLeftTop.y - Tmap::sizeOfField.y / 3 - 2.0f ),
				sf::IntRect{ 0,0,0,0 }, true);
		}
		if (!onRightEdge)
		{
			Tgame::inGameBoatsPositions[fieldX + 1][fieldY - 1].shooted = true;

			mapsImg.copy(oImg, 
				static_cast<unsigned int>((fieldX + 1) * Tmap::sizeOfField.x + mapsLeftTop.x - Tmap::sizeOfField.x / 3 + 1.0f ),
				static_cast<unsigned int>( (fieldY - 1) * Tmap::sizeOfField.y + mapsLeftTop.y - Tmap::sizeOfField.y / 3 - 2.0f ),
				sf::IntRect{ 0,0,0,0 }, true);
		}
	}
	
		if (!OnLeftEdge)
		{
			Tgame::inGameBoatsPositions[fieldX - 1][fieldY].shooted = true;

			mapsImg.copy(oImg, 
				static_cast<unsigned int>( (fieldX - 1) * Tmap::sizeOfField.x + mapsLeftTop.x - Tmap::sizeOfField.x / 3 + 1.0f ),
				static_cast<unsigned int>( fieldY * Tmap::sizeOfField.y + mapsLeftTop.y - Tmap::sizeOfField.y / 3 - 2.0f ),
				sf::IntRect{ 0,0,0,0 }, true);
		}
		{
			Tgame::inGameBoatsPositions[fieldX][fieldY].shooted = true;

			mapsImg.copy(oImg,
				static_cast<unsigned int>( fieldX * Tmap::sizeOfField.x + mapsLeftTop.x - Tmap::sizeOfField.x / 3 + 1.0f ),
				static_cast<unsigned int>( fieldY * Tmap::sizeOfField.y + mapsLeftTop.y - Tmap::sizeOfField.y / 3 - 2.0f ),
				sf::IntRect{ 0,0,0,0 }, true);
		}
		if (!onRightEdge)
		{
			Tgame::inGameBoatsPositions[fieldX + 1][fieldY].shooted = true;

			mapsImg.copy(oImg, 
				static_cast<unsigned int>( (fieldX + 1) * Tmap::sizeOfField.x + mapsLeftTop.x - Tmap::sizeOfField.x / 3 + 1.0f ),
				static_cast<unsigned int>( fieldY * Tmap::sizeOfField.y + mapsLeftTop.y - Tmap::sizeOfField.y / 3 - 2.0f ),
				sf::IntRect{ 0,0,0,0 }, true);
		}

	if (!onBottomEdge)
	{
		if (!OnLeftEdge)
		{
			Tgame::inGameBoatsPositions[fieldX - 1][fieldY + 1].shooted = true;

			mapsImg.copy(oImg, 
				static_cast<unsigned int>( (fieldX - 1) * Tmap::sizeOfField.x + mapsLeftTop.x - Tmap::sizeOfField.x / 3 + 1.0f ),
				static_cast<unsigned int>( (fieldY + 1) * Tmap::sizeOfField.y + mapsLeftTop.y - Tmap::sizeOfField.y / 3 - 2.0f ),
				sf::IntRect{ 0,0,0,0 }, true);
		}
		{
			Tgame::inGameBoatsPositions[fieldX][fieldY + 1].shooted = true;

			mapsImg.copy(oImg, 
				static_cast<unsigned int>( fieldX * Tmap::sizeOfField.x + mapsLeftTop.x - Tmap::sizeOfField.x / 3 + 1.0f ),
				static_cast<unsigned int>( (fieldY + 1) * Tmap::sizeOfField.y + mapsLeftTop.y - Tmap::sizeOfField.y / 3 - 2.0f ),
				sf::IntRect{ 0,0,0,0 }, true);
		}
		if (!onRightEdge)
		{
			Tgame::inGameBoatsPositions[fieldX + 1][fieldY + 1].shooted = true;

			mapsImg.copy(oImg, 
				static_cast<unsigned int>((fieldX + 1) * Tmap::sizeOfField.x + mapsLeftTop.x - Tmap::sizeOfField.x / 3 + 1.0f ),
				static_cast<unsigned int>( (fieldY + 1) * Tmap::sizeOfField.y + mapsLeftTop.y - Tmap::sizeOfField.y / 3 - 2.0f ),
				sf::IntRect{ 0,0,0,0 }, true);
		}
	}
}

void ThandleNetwork::markTakenFields(unsigned int fieldX, unsigned int fieldY, sf::Image& oImg, sf::Image& mapsImg, sf::Vector2f& mapsLeftTop)
{
	bool OnLeftEdge{ false };
	bool onRightEdge{ false };
	bool onTopEdge{ false };
	bool onBottomEdge{ false };

	if (fieldX == 0)
		OnLeftEdge = true;
	if (fieldX == 9)
		onRightEdge = true;
	if (fieldY == 0)
		onTopEdge = true;
	if (fieldY == 9)
		onBottomEdge = true;

	if (!onTopEdge)
	{
		if (!OnLeftEdge)
		{
			Tgame::takenFields[fieldX - 1][fieldY - 1] = true;

			mapsImg.copy(oImg, 
				static_cast<unsigned int>( (fieldX - 1) * Tmap::sizeOfField.x + mapsLeftTop.x - Tmap::sizeOfField.x / 3 + 1.0f ),
				static_cast<unsigned int>( (fieldY - 1) * Tmap::sizeOfField.y + mapsLeftTop.y - Tmap::sizeOfField.y / 3 - 2.0f ),
				sf::IntRect{ 0,0,0,0 }, true);
		}
		{
			Tgame::takenFields[fieldX][fieldY - 1] = true;

			mapsImg.copy(oImg, 
				static_cast<unsigned int>( fieldX * Tmap::sizeOfField.x + mapsLeftTop.x - Tmap::sizeOfField.x / 3 + 1.0f ),
				static_cast<unsigned int>( (fieldY - 1) * Tmap::sizeOfField.y + mapsLeftTop.y - Tmap::sizeOfField.y / 3 - 2.0f ),
				sf::IntRect{ 0,0,0,0 }, true);
		}
		if (!onRightEdge)
		{
			Tgame::takenFields[fieldX + 1][fieldY - 1] = true;

			mapsImg.copy(oImg, 
				static_cast<unsigned int>((fieldX + 1) * Tmap::sizeOfField.x + mapsLeftTop.x - Tmap::sizeOfField.x / 3 + 1.0f ),
				static_cast<unsigned int>( (fieldY - 1) * Tmap::sizeOfField.y + mapsLeftTop.y - Tmap::sizeOfField.y / 3 - 2.0f ),
				sf::IntRect{ 0,0,0,0 }, true);
		}
	}

		if (!OnLeftEdge)
		{
			Tgame::takenFields[fieldX - 1][fieldY] = true;

			mapsImg.copy(oImg,
				static_cast<unsigned int>( (fieldX - 1) * Tmap::sizeOfField.x + mapsLeftTop.x - Tmap::sizeOfField.x / 3 + 1.0f ),
				static_cast<unsigned int>( fieldY * Tmap::sizeOfField.y + mapsLeftTop.y - Tmap::sizeOfField.y / 3 - 2.0f ),
				sf::IntRect{ 0,0,0,0 }, true);
		}
		{
			Tgame::takenFields[fieldX][fieldY] = true;

			mapsImg.copy(oImg, 
				static_cast<unsigned int>( fieldX * Tmap::sizeOfField.x + mapsLeftTop.x - Tmap::sizeOfField.x / 3 + 1.0f ),
				static_cast<unsigned int>( fieldY * Tmap::sizeOfField.y + mapsLeftTop.y - Tmap::sizeOfField.y / 3 - 2.0f ),
				sf::IntRect{ 0,0,0,0 }, true);
		}
		if (!onRightEdge)
		{
			Tgame::takenFields[fieldX + 1][fieldY] = true;

			mapsImg.copy(oImg, 
				static_cast<unsigned int>( (fieldX + 1) * Tmap::sizeOfField.x + mapsLeftTop.x - Tmap::sizeOfField.x / 3 + 1.0f ),
				static_cast<unsigned int>( fieldY * Tmap::sizeOfField.y + mapsLeftTop.y - Tmap::sizeOfField.y / 3 - 2.0f ),
				sf::IntRect{ 0,0,0,0 }, true);
		}

	if (!onBottomEdge)
	{
		if (!OnLeftEdge)
		{
			Tgame::takenFields[fieldX - 1][fieldY + 1] = true;

			mapsImg.copy(oImg, 
				static_cast<unsigned int>((fieldX - 1) * Tmap::sizeOfField.x + mapsLeftTop.x - Tmap::sizeOfField.x / 3 + 1.0f ),
				static_cast<unsigned int>( (fieldY + 1) * Tmap::sizeOfField.y + mapsLeftTop.y - Tmap::sizeOfField.y / 3 - 2.0f ),
				sf::IntRect{ 0,0,0,0 }, true);
		}
		{
			Tgame::takenFields[fieldX][fieldY + 1] = true;

			mapsImg.copy(oImg, 
				static_cast<unsigned int>( fieldX * Tmap::sizeOfField.x + mapsLeftTop.x - Tmap::sizeOfField.x / 3 + 1.0f ),
				static_cast<unsigned int>( (fieldY + 1) * Tmap::sizeOfField.y + mapsLeftTop.y - Tmap::sizeOfField.y / 3 - 2.0f ),
				sf::IntRect{ 0,0,0,0 }, true);
		}
		if (!onRightEdge)
		{
			Tgame::takenFields[fieldX + 1][fieldY + 1] = true;

			mapsImg.copy(oImg, 
				static_cast<unsigned int>( (fieldX + 1) * Tmap::sizeOfField.x + mapsLeftTop.x - Tmap::sizeOfField.x / 3 + 1.0f ),
				static_cast<unsigned int>( (fieldY + 1) * Tmap::sizeOfField.y + mapsLeftTop.y - Tmap::sizeOfField.y / 3 - 2.0f ),
				sf::IntRect{ 0,0,0,0 }, true);
		}
	}
}

