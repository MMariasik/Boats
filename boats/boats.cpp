// boats.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
// byMateuszMariasik

#include <iostream>
#include<SFML/Graphics.hpp>
#include<vector>
#include"game.h"

//#define TESTING

#ifndef TESTING
int main()
{
	{
		sf::RenderWindow window{ sf::VideoMode{ 900, 600 }, "Boats___byMM" };
		sf::Event event;

		Tgame myGame{ };

		while (window.isOpen())
		{
			myGame.update(window);
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();
				if (event.type == sf::Event::MouseButtonPressed)
				{
					if (event.key.code == sf::Mouse::Left)
						myGame.onClick(window, sf::Mouse::getPosition(window), true);
				
				}
				if (event.type == sf::Event::MouseButtonReleased)
				{
					if (event.key.code == sf::Mouse::Left)
						myGame.onClick(window, sf::Mouse::getPosition(window), false);
					
				}
				if (event.type == sf::Event::KeyPressed)
					myGame.onKeyPressed(event.key.code);
				
			}

			window.clear();

			window.draw(myGame);

			window.display();

		}
	}
}
#endif // !TESTING




#ifdef TESTING

#include"Tsounds.h"

int main()
{
	sf::RenderWindow window{ sf::VideoMode{ 900, 600 }, "Boats___byMM" };
	sf::Event event;

	Tsounds::prepareClass();

	while (window.isOpen())
	{


		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Q)
					Tsounds::play(SOUNDTYPE::BUTTONCLICK);

				if (event.key.code == sf::Keyboard::W)
					Tsounds::play(SOUNDTYPE::ENEMYORMEREADY);

				if (event.key.code == sf::Keyboard::E)
					Tsounds::play(SOUNDTYPE::ENTERLETTER);

				if (event.key.code == sf::Keyboard::R)
					Tsounds::play(SOUNDTYPE::EXPLOSION);

				if (event.key.code == sf::Keyboard::T)
					Tsounds::play(SOUNDTYPE::HIT);

				if (event.key.code == sf::Keyboard::Y)
					Tsounds::play(SOUNDTYPE::JUMPINTOWATER);

				if (event.key.code == sf::Keyboard::U)
					Tsounds::play(SOUNDTYPE::LOSE);

				if (event.key.code == sf::Keyboard::I)
					Tsounds::play(SOUNDTYPE::PUICKUPBOAT);

				if (event.key.code == sf::Keyboard::O)
					Tsounds::play(SOUNDTYPE::PLACEDATIVE);

				if (event.key.code == sf::Keyboard::P)
					Tsounds::play(SOUNDTYPE::SUKEN);

				if (event.key.code == sf::Keyboard::A)
					Tsounds::play(SOUNDTYPE::WIN);

				if (event.key.code == sf::Keyboard::S)
					Tsounds::play(SOUNDTYPE::WRONGCODE);

				if (event.key.code == sf::Keyboard::D)
					Tsounds::play(SOUNDTYPE::MUSIC);

			}
		}

		window.clear(sf::Color::White);


		window.display();
	}
}
#endif // TESTING

