#include "LevelParser.h"
#include "Utilities/Base64.h"
#include "Utilities/tinyxml.h"
#include "Level/Level.h"
#include <assert.h>

LevelDetails parseLevelDetails(const TiXmlElement& rootElement);
std::vector<sf::Vector2f> parseEntities(const TiXmlElement & rootElement, int tileSize);

Level LevelParser::parseLevel(const std::string& levelName)
{
	TiXmlDocument mapFile;
	bool mapLoaded = mapFile.LoadFile(levelName);
	assert(mapLoaded);

	const auto& rootElement = mapFile.RootElement();
	LevelDetails levelDetails = parseLevelDetails(*rootElement);
	std::vector<sf::Vector2f> entities = parseEntities(*rootElement, levelDetails.m_tileSize);
	
	//return Level();
}

std::vector<sf::Vector2f> parseEntities(const TiXmlElement & rootElement, int tileSize)
{
	std::vector<sf::Vector2f> entityStartingPositions;
	for (const auto* entityElementRoot = rootElement.FirstChildElement(); entityElementRoot != nullptr; entityElementRoot = entityElementRoot->NextSiblingElement())
	{
		if (entityElementRoot->Value() != std::string("objectgroup") || entityElementRoot->Attribute("name") != std::string("Entity Layer"))
		{
			continue;
		}

		for (const auto* entityElement = entityElementRoot->FirstChildElement(); entityElement != nullptr; entityElement = entityElement->NextSiblingElement())
		{
			sf::Vector2i startingPosition;
			entityElement->Attribute("x", &startingPosition.x);
			entityElement->Attribute("y", &startingPosition.y);
			startingPosition.y -= tileSize; //Tiled Hack

			entityStartingPositions.push_back(sf::Vector2f(startingPosition.x, startingPosition.y));
			
		}
	}

	return entityStartingPositions;
}

LevelDetails parseLevelDetails(const TiXmlElement& rootElement)
{
	sf::Vector2i levelSize;
	int tileSize = 0;
	rootElement.Attribute("width", &levelSize.x);
	rootElement.Attribute("height", &levelSize.y);
	rootElement.Attribute("tilewidth", &tileSize);
	return LevelDetails(levelSize, tileSize);
}