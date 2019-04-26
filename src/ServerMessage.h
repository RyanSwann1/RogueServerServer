#pragma once

#include <SFML/Graphics.hpp>

enum class PacketType
{
	Connect = 0,
	Disconnect,
	HeartBeat,
	PlayerPosition,
	LatestGameData
};

struct ServerMessage
{
	ServerMessage(int clientID, PacketType packetType, sf::Vector2f position = sf::Vector2f())
		: m_clientID(clientID),
		m_packetType(packetType),
		m_position(position)
	{}

	int m_clientID;
	PacketType m_packetType;
	sf::Vector2f m_position;
};