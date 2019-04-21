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

struct TileSheet
{
	TileSheet(const std::string& name, int tileSize, int columns);

	sf::IntRect getTileLocation(int tileID, int tileSize) const;

	const int m_columns;
	sf::Texture m_texture;
};

class TileLayer
{
public:
	TileLayer(const std::vector<std::vector<int>>& tileLayer, const std::string& tileSheetName);

	const std::vector<std::vector<int>>& getTileLayer() const;
	const std::string& getName() const;
	void draw(const TileSheet& tileSheet, sf::RenderWindow& window, const LevelDetails& levelDetails) const;

private:
	const std::string m_name;
	const std::vector<std::vector<int>> m_tileLayer;
};

class Level
{
public:
	Level(const LevelDetails& levelDetails, std::vector<TileLayer> tileLayers, std::unordered_map<std::string, TileSheet> tileSheets,
		std::vector<sf::FloatRect>&& collisionLayer, std::vector<sf::Vector2f> entityStartingPositions);

	const LevelDetails& getDetails() const;
	const TileLayer& getTileLayer() const;
	const std::vector<sf::FloatRect>& getCollisionLayer() const;
	const TileSheet& getTileSheet(const std::string& name) const;

	void draw(sf::RenderWindow& window) const;


	void updatePlayerPosition(int clientID, sf::Vector2f newPosition);
	void addPlayer(int clientID, sf::Vector2f startingPosition);
	void removePlayer(int clientID);

private:
	const LevelDetails m_details;
	const std::vector<TileLayer> m_tileLayers;
	const std::unordered_map<std::string, TileSheet> m_tileSheets;
	const std::vector<sf::FloatRect> m_collisionLayer;
	std::vector<Player> m_players;
};