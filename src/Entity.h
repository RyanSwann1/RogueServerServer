#pragma once

#include <SFML/Graphics.hpp>

struct Player
{
	Player(int clientID, sf::Vector2f startingPosition)
		: m_clientID(clientID),
		m_position(startingPosition)
	{}

	int m_clientID;
	sf::Vector2f m_position;
};