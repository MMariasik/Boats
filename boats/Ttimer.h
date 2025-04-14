#ifndef TIMER
#define TIMER

#include <SFML/Graphics.hpp>

struct Ttimer : sf::Drawable
{
	Ttimer() : clock{}, text{}
	{
		this->text.setFillColor(sf::Color::Black);
		this->text.setCharacterSize(70);
	}

	sf::Clock clock;
	sf::Text text;

	inline void resetTimer()
	{
		clock.restart();
	}
	inline sf::Time getElapsedTime() const
	{
		return clock.getElapsedTime();
	}
	void updateText(sf::Time time) 
	{
		int secondsElapsed{ static_cast<int>(time.asSeconds()) };
		this->text.setString(calcMinutes(secondsElapsed) + ":" + calcSeconds(secondsElapsed));
	}

private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		target.draw(text);
	}

	std::string calcMinutes(int time)
	{
		if (time >= 600)
		{
			return std::to_string(time / 60);
		}
		else if(time >= 60)
		{
			return ("0" + std::to_string(time / 60));
		}
		else
		{
			return "00";
		}
	}
	std::string calcSeconds(int time)
	{
		int seconds{ time % 60 };
		if (seconds > 9)
		{
			return std::to_string(seconds);
		}
		else if (seconds > 0)
		{
			return "0" + std::to_string(seconds);
		}
		else return "00";
	}
};

#endif // !TIMER