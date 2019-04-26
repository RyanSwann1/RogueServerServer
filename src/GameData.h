#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

struct ClientProperties
{
	ClientProperties(sf::Vector2i position, int ID);

	sf::Vector2i m_position;
	int m_ID;
};

struct GameData
{
	GameData();

	const std::string& getCurrentLevelName() const;
	void reset(const std::string& levelName);

	void updatePlayerPosition(int clientID, sf::Vector2i newPosition);
	void addPlayer(int clientID, sf::Vector2i startingPosition);
	void removePlayer(int clientID);

	std::string m_currentLevelName = "Level1";
	std::vector<ClientProperties> m_clients;
	std::vector<sf::Vector2i> m_startingPositions;
};