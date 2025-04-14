#include "button.h"
#include "Tsounds.h"

button::button(void (* const fnc)(), sf::Vector2f position, sf::Vector2f buttonSize, unsigned int charSize, std::string textOnButton, std::string fontPath)
	: myFunction{ fnc }, originPos{ position }
{
	this->font.loadFromFile(fontPath);

	this->shape.setSize(buttonSize);
	this->shape.setPosition(position);
	this->shape.setFillColor(sf::Color{ 0x4b97bb });

	this->text.setPosition(position.x + 10.0f, position.y + 10.0f);
	this->text.setString(textOnButton);
	this->text.setFont(this->font);
	this->text.setCharacterSize(charSize);

	this->isMouseHoveringMe = false;
	//here more settings to text
}

//button::~button()
//{
//
//}


void button::mouseClicked(bool pressed, sf::Vector2i mousePos)
{
	if (isMouseOnMe(mousePos))
	{
		if (pressed) // mouse button is pressed
		{
			this->leftMouseButtonIsPressed = true;
			this->buttonAnimation(0);
		}
		else // mouse button is released
		{
			if (leftMouseButtonIsPressed)
			{
				this->leftMouseButtonIsPressed = false;
				this->buttonAnimation(1);
				this->onClick();
			}
		}
	}
	else
	{
		leftMouseButtonIsPressed = false;
		this->buttonAnimation(1);
	}
}

void button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(shape, states);
	target.draw(text, states);
}

void button::buttonAnimation(bool deanimate)
{
	if (deanimate)
		shape.setFillColor(sf::Color{ 0x4b97bb });
	else
		shape.setFillColor(sf::Color{ 0x67b4d8 });
}

bool button::isMouseOnMe(sf::Vector2i mousePos)
{
	sf::Vector2f betterPos{ static_cast<float>(mousePos.x), static_cast<float>(mousePos.y) };
	sf::FloatRect SHP{ this->shape.getPosition(), this->shape.getSize() };
	if (SHP.contains(betterPos))
	{
		return true;
	}
	return false;
}

void button::onClick()
{
	Tsounds::play(SOUNDTYPE::BUTTONCLICK);

	if (this->myFunction)
		this->myFunction();
}

bool button::animateHover(sf::Vector2i mousePos)
{
	bool returnOfIsMouseOnMe;
	if (returnOfIsMouseOnMe = this->isMouseOnMe(mousePos))
	{
		if (this->isMouseHoveringMe == false)
		{
			this->shape.setFillColor(sf::Color{ 0x1c3b4a });
			this->isMouseHoveringMe = true;
		}
	}
	else
	{
		if (this->isMouseHoveringMe == true)
		{
			this->shape.setFillColor(sf::Color{ 0x4b97bb });
			this->isMouseHoveringMe = false;
		}
	}
	return returnOfIsMouseOnMe;
}
