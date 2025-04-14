#ifndef MAP
#define MAP	

#include<SFML/Graphics.hpp>

class Tmap : public sf::RectangleShape
{
public:
	Tmap(sf::Vector2f pos, std::string pathToImage);
	Tmap(const Tmap& map);
	~Tmap();

	sf::Image getMapsImage();
	void setNewImage(const sf::Image& image);
	sf::FloatRect getMapsCollision();
	static sf::Vector2f sizeOfField;

private:
	sf::Image img;
	sf::Texture txt;
};

#endif // !MAP
