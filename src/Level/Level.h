#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "SFML/Graphics.hpp" 
#include "../Entity.h"

struct LevelDetails
{
public:
	LevelDetails(sf::Vector2i size, int tileSize)
		: m_size(size),
		m_tileSize(tileSize)
	{}

	const sf::Vector2i m_size;
	const int m_tileSize;
};

class Level
{
public:
	Level(const LevelDetails& levelDetails, std::vector<sf::FloatRect>&& collisionLayer);

	const LevelDetails& getDetails() const;
	const std::vector<sf::FloatRect>& getCollisionLayer() const;

	void updatePlayerPosition(int clientID, sf::Vector2f newPosition);
	void addPlayer(int clientID, sf::Vector2f startingPosition);
	void removePlayer(int clientID);

private:
	const LevelDetails m_details;
	const std::vector<sf::FloatRect> m_collisionLayer;
	std::vector<Player> m_players;
};