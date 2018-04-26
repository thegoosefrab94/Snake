#include "Game.h"
#include <sstream>
#include <time.h>
#include <random>

// The head of the snake is the first element in the body array
#define head bodyPoints[0]

const int SQUARE_SIZE = 18;
const int ROWS = 30;
const int COLUMNS = 30;

const int width = COLUMNS * SQUARE_SIZE;
const int height = ROWS * SQUARE_SIZE + SQUARE_SIZE;

template <typename T>
std::string ToString(const T& value)
{
	std::stringstream stream;
	stream << value;
	return stream.str();
}

std::mt19937 Roll;

Game::Game() :
	m_CurrentDir{ Direction::Down }, m_Window{}, m_Texture{}, m_Font{}, m_Snake{}, m_Fruit{}, m_Background{},
	m_CurrScore{}, m_HighScore{}, m_Pause{},
	m_isPaused{ true }, m_Delay{ 0.1f }
{
	for (Point p : bodyPoints)
	{
		p.x = 0;
		p.y = 0;
	}
	head.x = COLUMNS/2;	// Start in the middle of the field
	head.y = ROWS/2;

	fruitPoint.x = Roll() % COLUMNS;
	fruitPoint.y = Roll() % ROWS;
}

bool Game::Init()
{
	m_Window.create(sf::VideoMode(width, height), "Snake Game!");
	if (!m_Texture.loadFromFile("Assets/White.bmp"))
		return false;
	if (!m_Font.loadFromFile("Assets/Sansation.ttf"))
		return false;

	m_CurrScore.setFont(m_Font);
	m_CurrScore.setCharacterSize(13);
	m_CurrScore.setFillColor(sf::Color::White);
	m_CurrScore.setPosition(width - 4 * SQUARE_SIZE, 0);

	m_HighScore.setFont(m_Font);
	m_HighScore.setCharacterSize(m_CurrScore.getCharacterSize());
	m_HighScore.setFillColor(sf::Color::White);
	m_HighScore.setPosition(0, 0);

	m_Pause.setFont(m_Font);
	m_Pause.setFillColor(sf::Color::White);
	m_Pause.setCharacterSize(m_CurrScore.getCharacterSize());
	m_Pause.setOrigin(m_Pause.getGlobalBounds().width / 2, m_Pause.getGlobalBounds().height / 2);
	m_Pause.setPosition(width / 2, height / 2);
	m_Pause.setString("Paused\nPress 'Space' to continue");	// This does not need to change

	m_Background.setTexture(m_Texture);
	m_Background.setColor(sf::Color::Green);
	m_Background.setScale(0.125, 0.125);

	m_Snake.setTexture(m_Texture);
	m_Snake.setColor(sf::Color::Yellow);
	m_Snake.setScale(m_Background.getScale());	// All other sprites depend on the backgrounds scale

	m_Fruit.setTexture(m_Texture);
	m_Fruit.setColor(sf::Color::Red);
	m_Fruit.setScale(m_Background.getScale());

	return true;
}

void Game::Run()
{
	sf::Clock clock;
	float timer = 0;

	while (m_Window.isOpen())
	{
		if (!m_isPaused)	// Only stop the CLOCK if game is paused. Not Input
		{
			float time = clock.getElapsedTime().asSeconds();
			clock.restart();
			timer += time;
		}
		ProcessEvents();
		HandleInput();
		if (timer > m_Delay)
		{
			timer = 0;
			Logic();
		}
		Render();
	}

}

void Game::ProcessEvents()
{
	sf::Event e;
	while (m_Window.pollEvent(e))
	{
		if (e.type == sf::Event::Closed)
			m_Window.close();
		if (e.type == sf::Event::KeyPressed)
			if (e.key.code == sf::Keyboard::Space) m_isPaused = !m_isPaused;
	}
}

void Game::HandleInput()
{
	if (!m_isPaused)	// Prevents from the direction changing if paused
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			if (m_CurrentDir != Direction::Down) m_CurrentDir = Direction::Up;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			if (m_CurrentDir != Direction::Up) m_CurrentDir = Direction::Down;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			if (m_CurrentDir != Direction::Left) m_CurrentDir = Direction::Right;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			if (m_CurrentDir != Direction::Right) m_CurrentDir = Direction::Left;
		}
	}
}

int snakeLength = 1;
unsigned score = 0;
unsigned highscore = 0;
const unsigned fruitScore = 10;

void Game::Reset()
{
	snakeLength = 1;
	if (score > highscore) highscore = score;
	score = 0;

	head.x = COLUMNS / 2;	// Reset to the middle of the field
	head.y = ROWS / 2;

	fruitPoint.x = Roll() % COLUMNS;
	fruitPoint.y = Roll() % ROWS;

	m_isPaused = true;
}
void Game::Logic()
{
	for (int i = snakeLength; i > 0; i--)	// Body follows the head
	{
		bodyPoints[i].x = bodyPoints[i - 1].x;
		bodyPoints[i].y = bodyPoints[i - 1].y;
	}
	if (m_CurrentDir == Direction::Up) head.y -= 1;
	if (m_CurrentDir == Direction::Down) head.y += 1;
	if (m_CurrentDir == Direction::Left) head.x -= 1;
	if (m_CurrentDir == Direction::Right) head.x += 1;

	// When you reach the edge, move to the other end
	if (head.x < 0) head.x = COLUMNS;
	if (head.x > COLUMNS) head.x = 0;
	if (head.y < 0) head.y = ROWS;
	if (head.y > ROWS) head.y = 0;

	if (head.x == fruitPoint.x && head.y == fruitPoint.y)
	{
		snakeLength++;
		score += fruitScore;
		fruitPoint.x = Roll() % COLUMNS;
		fruitPoint.y = Roll() % ROWS;
		if (m_Delay > 0.06)				// Next to uncontrollable after 0.06
			m_Delay -= 0.01;
	}

	for (int i = 1; i < snakeLength; i++)
		if (head.x == bodyPoints[i].x && head.y == bodyPoints[i].y) // Collison with self
		{
			Reset();
		}
}

void Game::Render()
{
	m_Window.clear(sf::Color::Black);
	for (int i = 0; i < COLUMNS; i++)	// Draw the background sprites first
	{
		for (int j = 0; j < ROWS; j++)
		{
			m_Background.setPosition(i*SQUARE_SIZE, j*SQUARE_SIZE + SQUARE_SIZE);
			m_Window.draw(m_Background);
		}
	}

	for (int i = 0; i < snakeLength; i++)	// Draw snake body
	{
		m_Snake.setPosition(bodyPoints[i].x*SQUARE_SIZE, bodyPoints[i].y*SQUARE_SIZE + SQUARE_SIZE);
		m_Window.draw(m_Snake);
	}

	m_Fruit.setPosition(fruitPoint.x*SQUARE_SIZE, fruitPoint.y*SQUARE_SIZE + SQUARE_SIZE);	// And finally fruit
	m_Window.draw(m_Fruit);

	m_CurrScore.setString("Score: " + ToString(score));
	m_HighScore.setString("Highscore: " + ToString(highscore));
	m_Window.draw(m_HighScore);
	m_Window.draw(m_CurrScore);

	if (m_isPaused)
	{
		m_Window.draw(m_Pause);
	}

	m_Window.display();
}