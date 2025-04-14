#include"map.h"

#define NUMBEROFCOLOUMNS 11.0f
#define NUMBEROFROWS 11.0f

sf::Vector2f Tmap::sizeOfField { Tmap::sizeOfField.x, Tmap::sizeOfField.y };

Tmap::Tmap(sf::Vector2f pos, std::string pathToImage) : sf::RectangleShape{}
{
	this->setPosition(pos);
	this->img.loadFromFile(pathToImage);
	this->txt.loadFromImage(this->img);
	this->setTexture(&this->txt);

	this->setSize( static_cast<sf::Vector2f>( img.getSize() ) );

	Tmap::sizeOfField.x = this->getSize().x / NUMBEROFCOLOUMNS;
	Tmap::sizeOfField.y = this->getSize().y / NUMBEROFROWS;
}

Tmap::Tmap(const Tmap& map)
{
	this->setPosition(map.getPosition());
	this->img = map.img;
	this->txt.loadFromImage(this->img);
	this->setTexture(&this->txt);

	this->setSize(static_cast<sf::Vector2f>(img.getSize()));
}

Tmap::~Tmap()
{
}

sf::Image Tmap::getMapsImage()
{
	return Tmap::img;
}

void Tmap::setNewImage(const sf::Image& image)
{
	this->img = image;
	this->txt.loadFromImage(this->img);
	this->setTexture(&this->txt);
}

sf::FloatRect Tmap::getMapsCollision()
{
	return sf::FloatRect{	this->getPosition().x + sizeOfField.x, 
							this->getPosition().y + sizeOfField.y  ,
							10*sizeOfField.x, 
							10*sizeOfField.y };

}
