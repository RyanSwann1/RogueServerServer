#include "Level.h"
#include <utility>
#include <assert.h>
#include <algorithm>

//Level
Level::Level(const LevelDetails& levelDetails, std::vector<sf::FloatRect>&& collisionLayer, std::vector<sf::Vector2f> entityStartingPositions)
	: m_details(levelDetails),
	m_collisionLayer(std::move(collisionLayer))
{
		
}

const LevelDetails & Level::getDetails() const
{
	return m_details;
}

const std::vector<sf::FloatRect>& Level::getCollisionLayer() const
{
	assert(!m_collisionLayer.empty());
	return m_collisionLayer;
}

void Level::updatePlayerPosition(int clientID, sf::Vector2f newPosition)
{
	auto iter = std::find_if(m_players.begin(), m_players.end(), [clientID](const auto& player) { return clientID == player.m_clientID; });
	assert(iter != m_players.cend());
	iter->m_position = newPosition;
}

void Level::addPlayer(int clientID, sf::Vector2f startingPosition)
{
	auto cIter = std::find_if(m_players.cbegin(), m_players.cend(), [clientID](const auto& player) { return clientID == player.m_clientID; });
	assert(cIter != m_players.cend());

	m_players.emplace_back(clientID, startingPosition);
}

void Level::removePlayer(int clientID)
{
	auto cIter = std::find_if(m_players.cbegin(), m_players.cend(), [clientID](const auto& player) { return clientID == player.m_clientID; });
	assert(cIter != m_players.cend());
	m_players.erase(cIter);
}