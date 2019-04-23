#include "GameState.h"
#include <algorithm>

GameState::GameState()
{
}

const std::string & GameState::getCurrentLevelName() const
{
	return m_currentLevelName;
}

void GameState::updatePlayerPosition(int clientID, sf::Vector2i newPosition)
{
	auto iter = std::find_if(m_players.begin(), m_players.end(), [clientID](const auto& player) { return player.m_ID == clientID; });
	if (iter != m_players.end())
	{
		iter->m_position = newPosition;
	}
}

void GameState::addPlayer(int clientID, sf::Vector2i startingPosition)
{
	auto cIter = std::find_if(m_players.cbegin(), m_players.cend(), [clientID](const auto& player) { return player.m_ID == clientID; });
	if (cIter == m_players.cend())
	{
		m_players.emplace_back(startingPosition, clientID);
	}
}

void GameState::removePlayer(int clientID)
{
	auto iter = std::find_if(m_players.begin(), m_players.end(), [clientID](const auto& player) { return player.m_ID == clientID; });
	if (iter != m_players.end())
	{
		m_players.erase(iter);
	}
}
