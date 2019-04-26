#include "GameData.h"
#include <algorithm>
#include "Utilities/Base64.h"
#include "Utilities/tinyxml.h"
#include <vector>
#include <assert.h>
#include <string>

////Game Data Parser
//struct LevelDetails
//{
//public:
//	LevelDetails(sf::Vector2i size, int tileSize)
//		: m_size(size),
//		m_tileSize(tileSize)
//	{}
//
//	const sf::Vector2i m_size;
//	const int m_tileSize;
//};
//
//LevelDetails parseLevelDetails(const TiXmlElement& rootElement);
//std::vector<sf::Vector2i> parseEntities(const TiXmlElement & rootElement, int tileSize);
//
//std::vector<sf::Vector2i> parseGameData(const std::string & levelName)
//{
//	TiXmlDocument mapFile;
//	bool mapLoaded = mapFile.LoadFile(levelName);
//	//assert(mapLoaded);
//
//	const auto& rootElement = mapFile.RootElement();
//	LevelDetails levelDetails = parseLevelDetails(*rootElement);
//	return parseEntities(*rootElement, levelDetails.m_tileSize);
//}
//
//std::vector<sf::Vector2i> parseEntities(const TiXmlElement & rootElement, int tileSize)
//{
//	std::vector<sf::Vector2i> entityStartingPositions;
//	for (const auto* entityElementRoot = rootElement.FirstChildElement(); entityElementRoot != nullptr; entityElementRoot = entityElementRoot->NextSiblingElement())
//	{
//		if (entityElementRoot->Value() != std::string("objectgroup") || entityElementRoot->Attribute("name") != std::string("Entity Layer"))
//		{
//			continue;
//		}
//
//		for (const auto* entityElement = entityElementRoot->FirstChildElement(); entityElement != nullptr; entityElement = entityElement->NextSiblingElement())
//		{
//			sf::Vector2i startingPosition;
//			entityElement->Attribute("x", &startingPosition.x);
//			entityElement->Attribute("y", &startingPosition.y);
//			startingPosition.y -= tileSize; //Tiled Hack
//
//			entityStartingPositions.push_back(sf::Vector2i(startingPosition.x, startingPosition.y));
//
//		}
//	}
//
//	return entityStartingPositions;
//}
//
//LevelDetails parseLevelDetails(const TiXmlElement& rootElement)
//{
//	sf::Vector2i levelSize;
//	int tileSize = 0;
//	rootElement.Attribute("width", &levelSize.x);
//	rootElement.Attribute("height", &levelSize.y);
//	rootElement.Attribute("tilewidth", &tileSize);
//	return LevelDetails(levelSize, tileSize);
//}

//Client Properties
ClientProperties::ClientProperties(sf::Vector2i position, int ID)
	: m_position(position),
	m_ID(ID)
{}

//Game Data
GameData::GameData()
{

}

const std::string & GameData::getCurrentLevelName() const
{
	return m_currentLevelName;
}

void GameData::reset(const std::string & levelName)
{
	m_clients.clear();
	m_currentLevelName = levelName;
}

void GameData::updatePlayerPosition(int clientID, sf::Vector2i newPosition)
{
	auto iter = std::find_if(m_clients.begin(), m_clients.end(), [clientID](const auto& player) { return player.m_ID == clientID; });
	if (iter != m_clients.end())
	{
		iter->m_position = newPosition;
	}
}

void GameData::addPlayer(int clientID, sf::Vector2i startingPosition)
{
	auto cIter = std::find_if(m_clients.cbegin(), m_clients.cend(), [clientID](const auto& player) { return player.m_ID == clientID; });
	if (cIter == m_clients.cend())
	{
		m_clients.emplace_back(startingPosition, clientID);
	}
}

void GameData::removePlayer(int clientID)
{
	auto iter = std::find_if(m_clients.begin(), m_clients.end(), [clientID](const auto& player) { return player.m_ID == clientID; });
	if (iter != m_clients.end())
	{
		m_clients.erase(iter);
	}
}