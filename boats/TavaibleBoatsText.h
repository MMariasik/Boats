#ifndef TAVAIBLEBOATSTEXT
#define TAVAIBLEBOATSTEXT


#include <SFML/Graphics.hpp>

class TavaibleBoatsText : public sf::Drawable
{
public:
	TavaibleBoatsText(std::string pathToFont, sf::Vector2f pos);

	sf::Vector2f position;
	sf::Font font;

	sf::Text first;
	sf::Text second;
	sf::Text third;
	sf::Text fourth;

	void update();

private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	
};

#endif // !TAVAIBLEBOATSTEXT