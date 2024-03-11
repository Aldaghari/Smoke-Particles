#include <iostream>
#include <SFML/Graphics.hpp>
#include <random>
#include <vector>


std::random_device rd;
std::mt19937 gen(rd());

float myRandom(float low, float high)
{
	std::uniform_real_distribution<> dist(low, high);
	return dist(gen);
}

float followCursor(const sf::CircleShape& tringle, const sf::Vector2i& mousePos)
{
	// calcullate the rotration angle between mouse and object (tringle)

	const float PI = 3.14159265358979f;
	float tringleAngle = 0.0f;
	sf::Vector2f tringlePos = tringle.getPosition();

	tringleAngle = atan2(mousePos.y - tringlePos.y, mousePos.x - tringlePos.x);
	tringleAngle = tringleAngle * (180 / PI);
	//std::cout << tringleAngle << "\n";

	return tringleAngle;
}

sf::Vector2f pointingPosOfTringle(const sf::CircleShape& tringle, const sf::Vector2i& mousePos)
{
	// caculated the pointing direction of Tringle
	sf::Vector2f tringlePos = tringle.getPosition();
	sf::Vector2f vector(mousePos.x - tringlePos.x , mousePos.y - tringlePos.y );
	float vectorLenght = sqrt(pow(vector.x, 2) + pow(vector.y, 2));
	sf::Vector2f vectorNorm(vector.x / vectorLenght, vector.y / vectorLenght);
	//std::cout << vectorNorm.x * tringle.getRadius() << ", " << vectorNorm.y * tringle.getRadius() << std::endl;
	return tringlePos + (vectorNorm * tringle.getRadius() * 6.0f);
}

sf::Vector2f directionOfTringle(const sf::CircleShape& tringle, const sf::Vector2i& mousePos)
{
	// caculated the pointing direction of Tringle
	sf::Vector2f tringlePos = tringle.getPosition();
	sf::Vector2f vector(mousePos.x - tringlePos.x, mousePos.y - tringlePos.y);
	float vectorLenght = sqrt(pow(vector.x, 2) + pow(vector.y, 2));
	sf::Vector2f vectorNorm(vector.x / vectorLenght, vector.y / vectorLenght);
	//std::cout << vectorNorm.x * tringle.getRadius() << ", " << vectorNorm.y * tringle.getRadius() << std::endl;
	return vectorNorm;
}


class Rect
{
public:
	sf::RectangleShape shape;
	sf::Vector2f shapeSize;
	sf::Vector2f shapeSpeed;
	sf::Vector2f shapeMovingDirection;
	sf::Vector2f smoothStop;
	sf::Uint8 red = 255, green = 255, blue = 255, alpha = 50;
	bool moving = false;
	bool created = false;

private:
	// Textue
	sf::Texture texture;
	

public:
	Rect()
	{
		shapeSize = sf::Vector2f(60.0f, 60.0f);
		shapeSpeed = sf::Vector2f(5.0f, 5.0f);
		shapeMovingDirection = sf::Vector2f(0.0f, 0.0f);
		smoothStop = sf::Vector2f(0.001f, 0.001f);

		shape.setSize(shapeSize);
		shape.setFillColor(sf::Color(red, green, blue, alpha));
		shape.setOrigin(shapeSize / 2.0f);

		if (!texture.loadFromFile("smoke.png"))
		{
			std::cout << "ERROR loading texture" << std::endl;
		}

		shape.setTexture(&texture);
	}


};


int main()
{
	// settings
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(1920, 995), "Smoke Particle", sf::Style::Default, settings);
	window.setPosition(sf::Vector2i(-9, 0));

	// Tringle properties
	sf::CircleShape tringle(15.0f, 3);
	tringle.setOrigin(tringle.getRadius(), tringle.getRadius());
	tringle.setPosition(90.0f, window.getSize().y / 2);
	tringle.setFillColor(sf::Color::White);
	tringle.scale(1.5f, 3.0f);
	

	// Rect
	Rect rect;
	std::vector<Rect> rects;

	// Clock
	sf::Clock clock;


	while (window.isOpen())
	{
		sf::Event event;

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		// == Update ==
		
		// Rotate tringle around mouse
		sf::Vector2i mousePos = sf::Mouse::getPosition(window);
		float tringleAngle = followCursor(tringle, mousePos);
		tringle.setRotation(tringleAngle + 90.0f);


		// Add rect when Mouse 1 is clicked/held
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			// random y coordinate
			rect.shape.setPosition(pointingPosOfTringle(tringle, mousePos));
			rects.push_back(rect);
		}

		// Update position of Rects (Move) and sets randomSpeed
		for (int i = 0; i < rects.size(); i++)
		{
			// if rect not moving (just was created) move it according to direction of tringle
			// Also set its speed (this will only run oce for each rect)
			float randomSpeed = myRandom(0.0f, 1.0f);
			if (!rects[i].moving)
			{
				rects[i].shapeSpeed += sf::Vector2f(randomSpeed, randomSpeed);
				rects[i].shapeMovingDirection = directionOfTringle(tringle, mousePos);
				rects[i].moving = true;
			}

			// Update the shapeSpeed with random speed and SmoothStop 
			if (rects[i].shapeSpeed.x > 0)
			{
				rects[i].shapeSpeed -= rects[i].smoothStop;

				// make smoothStop stronger (slower)
				rects[i].smoothStop = rects[i].smoothStop + sf::Vector2f(0.0001f, 0.0001f);
			}

			rects[i].shape.move(rects[i].shapeMovingDirection.x * rects[i].shapeSpeed.x,
				rects[i].shapeMovingDirection.y * rects[i].shapeSpeed.y - 1.0f);

		}


		// increase shapeSize and set random initial size and rotation
		for (int i = 0; i < rects.size(); i++)
		{
			// set random size and rotation only once
			float randomSize = myRandom(10.0f, 100.0f);
			float randomRotation = myRandom(0.0f, 180.0f);
			if (!rects[i].created)
			{
				rects[i].shape.setSize(sf::Vector2f(rects[i].shapeSize.x + randomSize, rects[i].shapeSize.y + randomSize));
				rects[i].shape.setRotation(randomRotation);
				rects[i].created = true;
			}
			// increase shape size
			sf::Vector2f shapeSize = rects[i].shape.getSize();
			shapeSize += sf::Vector2f(0.1f, 0.1f);
			rects[i].shape.setSize(shapeSize);
		}
	



		// set colors and opacity every n seconds
		sf::Time time = clock.getElapsedTime();
		if (time.asSeconds() >= 0.1f)
		{
			for (int i = 0; i < rects.size(); i++)
			{
				if (rects[i].alpha > 0)
					rects[i].alpha -= 1;

				rects[i].shape.setFillColor(sf::Color(rects[i].red, rects[i].green, rects[i].blue, rects[i].alpha));
			}
			clock.restart();
		}

		// delete rects
		for (int i = rects.size() - 1; i >= 0; i--)
		{
			if (rects[i].shape.getPosition().y <= -10 || rects[i].alpha <= 0)
			{
				//rects.erase(rects.begin() + i);
			}
		}
		//std::cout << rects.size() << std::endl;

		// == Draw ==
		window.clear(sf::Color::Black);

		window.draw(tringle);

		// draw rects
		for (int i = 0; i < rects.size(); i++)
		{
			window.draw(rects[i].shape);
		}

		window.display();
	}

}