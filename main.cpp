#include <iostream>
#include <random>
#include <thread>
#include <chrono>
#include <SFML/Graphics.hpp>

constexpr int cellLength = 40;
constexpr int lineWidth = 1;
constexpr int numColumns = 20;
constexpr int numRows = 20;
constexpr int windowX = numColumns*cellLength;
constexpr int windowY = numRows*cellLength;

struct position
{
	int x, y;
};

class cell
{
	private:
		position p;
		bool isSnake = false;
		bool isFood = false;
		sf::RectangleShape lines[4];
		sf::RectangleShape fillSnake;
		sf::RectangleShape fillFood;

	public:
		cell(int x, int y);
		void configShapes();
		sf::RectangleShape returnLine(int d);
		sf::RectangleShape returnSnakeFill();
		sf::RectangleShape returnFoodFill();
		void setSnake(bool set);
		bool returnSnake();
		void setFood(bool set);
		bool returnFood();
};

cell::cell(int x, int y)
{
	p.x = x;
	p.y = y;
	configShapes();
}

void cell::configShapes()
{
	for (int i = 0; i < 4; i++)
	{
		lines[i].setSize(sf::Vector2f(cellLength, lineWidth));
		lines[i].setFillColor(sf::Color::White);
		if (i == 0 || i == 1)
		{
			lines[i].setPosition(p.x*cellLength, p.y*cellLength);
		}
		if (i == 1 || i == 3)
		{
			lines[i].setRotation(90.f);
		}
	}

	lines[2].setPosition(p.x*cellLength, (p.y+1)*cellLength);
	lines[3].setPosition((p.x+1)*cellLength, p.y*cellLength);
	
	fillSnake.setSize(sf::Vector2f(cellLength, cellLength));
	fillSnake.setFillColor(sf::Color::White);
	fillSnake.setPosition(p.x*cellLength, p.y*cellLength);

	fillFood.setSize(sf::Vector2f(cellLength, cellLength));
	fillFood.setFillColor(sf::Color::Green);
	fillFood.setPosition(p.x*cellLength, p.y*cellLength);
}

sf::RectangleShape cell::returnLine(int d)
{
	return lines[d];
}

sf::RectangleShape cell::returnSnakeFill()
{
	return fillSnake;
}

sf::RectangleShape cell::returnFoodFill()
{
	return fillFood;
}

void cell::setSnake(bool set)
{
	isSnake = set;
}

bool cell::returnSnake()
{
	return isSnake;
}

void cell::setFood(bool set)
{
	isFood = set;
}

bool cell::returnFood()
{
	return isFood;
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(windowX,windowY), "Snake");
	std::vector<std::vector<cell>> cellMap;
	bool isPlaying = true;
	int direction = 0;
	std::vector<position> snake;
	
	position foodPos;
	bool isFoodOnBoard = false;

	std::random_device rd;
	std::default_random_engine generator;
	generator.seed(rd());
	std::uniform_int_distribution<int> distributionX(0, numColumns-1);
	std::uniform_int_distribution<int> distributionY(0, numRows-1);

	for (int i = 0; i < numColumns; i++)
	{
		cellMap.push_back(std::vector<cell>());
		for (int j = 0; j < numRows; j++)
		{
			cell newCell(i, j);
			cellMap[i].push_back(newCell);
		}
	}
	
	position start;
	start.x = 10;
	start.y = 10;
	snake.push_back(start);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Space)
				{
					if (isPlaying)
					{
						std::cout << "Not playing" << std::endl;
						isPlaying = false;
					}
					else
					{
						std::cout << "Playing" << std::endl;
						isPlaying = true;
					}
				}

				if ((event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) && isPlaying)
				{
					direction = 0;
				}

				if ((event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A) && isPlaying)
				{
					direction = 1;
				}
				
				if ((event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) && isPlaying)
				{
					direction = 2;
				}

				if ((event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D) && isPlaying)
				{
					direction = 3;
				}
			}
		}

		window.clear();
		
		for (int i = 0; i < snake.size(); i++)
		{	
			cellMap[snake[i].x][snake[i].y].setSnake(true);
		}

		for (int i = 0; i < numColumns; i++)
		{
			for (int j = 0; j < numRows; j++)
			{
				for (int k = 0; k < 4; k++)
				{
					window.draw(cellMap[i][j].returnLine(k));
				}

				if (cellMap[i][j].returnFood())
				{
					window.draw(cellMap[i][j].returnFoodFill());
				}

				if (cellMap[i][j].returnSnake())
				{
					window.draw(cellMap[i][j].returnSnakeFill());
				}
			}
		}
		if (isPlaying)
		{
			for (int i = 0; i < snake.size(); i++)
			{	
				cellMap[snake[i].x][snake[i].y].setSnake(false);
			}
			
			for (int i = snake.size() - 1; i >= 0; i--)
			{
				snake[i+1] = snake[i];
			}
	
			if (direction == 0)
			{
				snake[0].y--;
			}
			else if (direction == 1)
			{
				snake[0].x--;
			}
			else if (direction == 2)
			{
				snake[0].y++;
			}	
			else
			{
				snake[0].x++;
			}

			if (!isFoodOnBoard)
			{
				foodPos.x = distributionX(generator);
				foodPos.y = distributionY(generator);
				cellMap[foodPos.x][foodPos.y].setFood(true);
				isFoodOnBoard = true;
			}

			if (isFoodOnBoard && foodPos.x == snake[0].x && foodPos.y == snake[0].y)
			{
				cellMap[foodPos.x][foodPos.y].setFood(false);
				isFoodOnBoard = false;
				position spawnPos;
				if (direction == 0)
				{
					spawnPos.x = snake[snake.size()-1].x;
					spawnPos.y = snake[snake.size()-1].y + 1;
				}
				else if (direction == 1)
				{
					spawnPos.x = snake[snake.size()-1].x + 1;
					spawnPos.y = snake[snake.size()-1].y;
				}
				else if (direction == 2)
				{
					spawnPos.x = snake[snake.size()-1].x;
					spawnPos.y = snake[snake.size()-1].y - 1;
				}
				else
				{
					spawnPos.x = snake[snake.size()-1].x - 1;
					spawnPos.y = snake[snake.size()-1].y;
				}

				snake.push_back(spawnPos);

				std::cout << "Score: " << snake.size() << std::endl;
			}

			for (int i = 1; i < snake.size(); i++)
			{
				if (snake[0].x == snake[i].x && snake[0].y == snake[i].y)
				{
					std::cout << "Final Score: " << snake.size() << std::endl;
					isPlaying = false;
					for (int i = 1; i < snake.size(); snake.pop_back())
					{
						std::cout << "Boop." << std::endl;
					}
				}
			}

			if (snake[0].x < 0)
			{
				snake[0].x = numColumns - 1;
			}
			else if (snake[0].x >= numColumns)
			{
				snake[0].x = 0;
			}

			if (snake[0].y < 0)
			{
				snake[0].y = numRows - 1;
			}
			else if (snake[0].y >= numRows)
			{
				snake[0].y = 0;
			}
		}
		window.display();

		std::this_thread::sleep_for(std::chrono::milliseconds(150));
	}
}
