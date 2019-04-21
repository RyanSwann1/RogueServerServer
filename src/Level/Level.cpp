#include "Level.h"
#include <utility>
#include <assert.h>
#include <algorithm>

//TileSheet
TileSheet::TileSheet(const std::string& name, int tileSize, int columns)
	: m_texture(),
	m_columns(columns)
{
	m_texture.loadFromFile(name);
}

sf::IntRect TileSheet::getTileLocation(int tileID, int tileSize) const
{
	return sf::IntRect((tileID % m_columns) * tileSize,
		(tileID / m_columns) * tileSize, tileSize, tileSize);
}

//TileLayer
TileLayer::TileLayer(const std::vector<std::vector<int>>& tileLayer, const std::string& tileSheetName)
	: m_name(tileSheetName),
	m_tileLayer(tileLayer)
{}

const std::vector<std::vector<int>>& TileLayer::getTileLayer() const
{
	return m_tileLayer;
}

const std::string & TileLayer::getName() const
{
	return m_name;
}

void TileLayer::draw(const TileSheet& tileSheet, sf::RenderWindow& window, const LevelDetails& levelDetails) const
{
	for (int y = 0; y < levelDetails.m_size.y; ++y)
	{
		for (int x = 0; x < levelDetails.m_size.x; ++x)
		{
			const int tileID = m_tileLayer[y][x];
			if (tileID >= 0)
			{
				sf::Sprite tile(tileSheet.m_texture, tileSheet.getTileLocation(tileID, levelDetails.m_tileSize));
				tile.setPosition(sf::Vector2f(x * levelDetails.m_tileSize, y * levelDetails.m_tileSize));
				window.draw(tile);
			}
		}
	}
}

//Level
Level::Level(const LevelDetails& levelDetails, std::vector<TileLayer> tileLayers, std::unordered_map<std::string, TileSheet> tileSheets,
	std::vector<sf::FloatRect>&& collisionLayer, std::vector<sf::Vector2f> entityStartingPositions)
	: m_details(levelDetails),
	m_tileLayers(tileLayers),
	m_tileSheets(tileSheets),
	m_collisionLayer(std::move(collisionLayer))
{}

const LevelDetails & Level::getDetails() const
{
	return m_details;
}

const TileLayer & Level::getTileLayer() const
{
	return m_tileLayers.front();
}

const std::vector<sf::FloatRect>& Level::getCollisionLayer() const
{
	assert(!m_collisionLayer.empty());
	return m_collisionLayer;
}

const TileSheet & Level::getTileSheet(const std::string & name) const
{
	auto iter = m_tileSheets.find(name);
	assert(iter != m_tileSheets.cend());
	return iter->second;
}

void Level::draw(sf::RenderWindow& window) const
{
	for (const auto& tileLayer : m_tileLayers)
	{
		tileLayer.draw(m_tileSheets.begin()->second, window, m_details);
	}
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