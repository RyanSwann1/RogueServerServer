#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

struct EnemyProperties
{
	EnemyProperties(sf::Vector2i position, int ID)
		: m_position(position),
		m_ID(ID)
	{}

	sf::Vector2i m_position;
	int m_ID;
};

class GameState
{
public:
	GameState();

	const std::string& getCurrentLevelName() const;

	void updatePlayerPosition(int clientID, sf::Vector2i newPosition);
	void addPlayer(int clientID, sf::Vector2i startingPosition);
	void removePlayer(int clientID);

private:
	std::string m_currentLevelName;
	std::vector<EnemyProperties> m_players;
	sf::Vector2i m_playerStartingPosition;
};