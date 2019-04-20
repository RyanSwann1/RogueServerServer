#include "LevelParser.h"
#include "Utilities/Base64.h"
#include "Utilities/tinyxml.h"
#include "Level/Level.h"
#include <assert.h>

std::vector<TileLayer> parseTileLayers(const TiXmlElement& rootElement, const LevelDetails& levelDetails);
LevelDetails parseLevelDetails(const TiXmlElement& rootElement);
std::vector<std::vector<int>> decodeTileLayer(const TiXmlElement & tileLayerElement, sf::Vector2i levelSize);
std::unordered_map<std::string, TileSheet> parseTileSheets(const TiXmlElement& rootElement);
std::vector<sf::Vector2f> parseEntities(const TiXmlElement & rootElement, int tileSize);
std::vector<sf::FloatRect> parseCollisionLayer(const TiXmlElement & rootElement, int tileSize);

//Level LevelParser::parseLevel(const std::string& levelName)
//{
//	TiXmlDocument mapFile;
//	bool mapLoaded = mapFile.LoadFile(levelName);
//	assert(mapLoaded);
//
//	const auto& rootElement = mapFile.RootElement();
//	LevelDetails levelDetails = parseLevelDetails(*rootElement);
//	std::vector<TileLayer> tileLayers = parseTileLayers(*rootElement, levelDetails);
//	std::unordered_map<std::string, TileSheet> tileSheets = parseTileSheets(*rootElement);
//	std::vector<sf::FloatRect> collisionLayer = parseCollisionLayer(*rootElement, levelDetails.m_tileSize);
//	std::vector<sf::Vector2f> entities = parseEntities(*rootElement, levelDetails.m_tileSize);
//	
//	//return Level();
//}

std::vector<sf::FloatRect> parseCollisionLayer(const TiXmlElement & rootElement, int tileSize)
{
	std::vector<sf::FloatRect> collidablePositions;
	for (const auto* collisionLayerElement = rootElement.FirstChildElement(); collisionLayerElement != nullptr;
		collisionLayerElement = collisionLayerElement->NextSiblingElement())
	{
		if (collisionLayerElement->Value() != std::string("objectgroup"))
		{
			continue;
		}

		if (collisionLayerElement->Attribute("name") != std::string("Collision Layer"))
		{
			continue;
		}

		for (const auto* collisionElement = collisionLayerElement->FirstChildElement();
			collisionElement != nullptr; collisionElement = collisionElement->NextSiblingElement())
		{
			int xPosition = 0, yPosition = 0;
			collisionElement->Attribute("x", &xPosition);
			collisionElement->Attribute("y", &yPosition);
			//Hack for Tiled.
			yPosition -= tileSize;
			//collidablePositions.emplace_back(xPosition, yPosition, tileSize, tileSize);
		}
	}
	
	return collidablePositions;
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

std::unordered_map<std::string, TileSheet> parseTileSheets(const TiXmlElement& rootElement)
{
	std::unordered_map<std::string, TileSheet> tileSheets;
	for (const auto* tileSheetElement = rootElement.FirstChildElement();
		tileSheetElement != nullptr; tileSheetElement = tileSheetElement->NextSiblingElement())
	{
		if (tileSheetElement->Value() != std::string("tileset"))
		{
			continue;
		}

		std::string name = tileSheetElement->Attribute("name");
		std::string source =  tileSheetElement->FirstChildElement()->Attribute("source");
		sf::Vector2i size;
		int tileSize = 0;
		tileSheetElement->FirstChildElement()->Attribute("width", &size.x);
		tileSheetElement->FirstChildElement()->Attribute("height", &size.y);
		tileSheetElement->Attribute("tilewidth", &tileSize);
		int columns = size.x / tileSize;
		tileSheets.emplace(name, TileSheet( source, tileSize, columns ));
	}

	assert(!tileSheets.empty());
	return tileSheets;
}

std::vector<std::vector<int>> decodeTileLayer(const TiXmlElement & tileLayerElement, sf::Vector2i levelSize)
{
	std::vector<std::vector<int>> tileData;
	tileData.reserve(levelSize.y);

	std::string decodedIDs; //Base64 decoded information
	const TiXmlElement* dataNode = nullptr; //Store our node once we find it
	for (const TiXmlElement* e = tileLayerElement.FirstChildElement(); e != nullptr; e = e->NextSiblingElement())
	{
		if (e->Value() == std::string("data"))
		{
			dataNode = e;
		}
	}
	assert(dataNode);

	Base64 base64;
	const TiXmlText* text = dataNode->FirstChild()->ToText();
	const std::string t = text->Value();
	decodedIDs = base64.base64_decode(t);

	const std::vector<int> layerColumns(levelSize.x);
	for (int i = 0; i < levelSize.y; ++i)
	{
		tileData.push_back(layerColumns);
	}

	for (int rows = 0; rows < levelSize.y; ++rows)
	{
		for (int cols = 0; cols < levelSize.x; ++cols)
		{
			tileData[rows][cols] = *((int*)decodedIDs.data() + rows * levelSize.x + cols) - 1;
		}
	}

	return tileData;
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

std::vector<TileLayer> parseTileLayers(const TiXmlElement & rootElement, const LevelDetails& levelDetails)
{
	std::vector<TileLayer> tileLayers;
	for (const auto* tileLayerElement = rootElement.FirstChildElement(); 
		tileLayerElement != nullptr; tileLayerElement = tileLayerElement->NextSiblingElement())
	{
		if (tileLayerElement->Value() != std::string("layer"))
		{
			continue;
		}

		auto tileMap = decodeTileLayer(*tileLayerElement, levelDetails.m_size);
		std::string name = tileLayerElement->Attribute("name");
		tileLayers.emplace_back(std::move(tileMap), name);
	}

	assert(!tileLayers.empty());
	return tileLayers;
}