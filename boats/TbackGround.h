#include <SFML/Graphics.hpp>

struct TbackGround : sf::Sprite
{
	TbackGround(std::string pathToBackGround) : Sprite()
	{
		bgImage.loadFromFile(pathToBackGround);
		bgTexture.loadFromImage(bgImage);
		this->setTexture(bgTexture);
	}

private:
	sf::Image bgImage;
	sf::Texture bgTexture;
};