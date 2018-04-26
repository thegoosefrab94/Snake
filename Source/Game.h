#pragma once
#include <SFML\Graphics.hpp>

class Game
{
public:
	Game();
	bool Init();
	void Run();

private:
	enum Direction
	{
		Up = 0,
		Down,
		Left,
		Right
	} 
	m_CurrentDir;
	struct Point
	{
		int x, y;
	}
	bodyPoints[100], fruitPoint;
	void Logic();
	void Render();
	void ProcessEvents();
	void HandleInput();
	void Reset();

	sf::RenderWindow m_Window;
	sf::Texture m_Texture;
	sf::Font m_Font;
	sf::Sprite m_Snake, m_Fruit, m_Background;
	sf::Text m_CurrScore, m_HighScore, m_Pause;
	float m_Delay;
	bool m_isPaused;
};