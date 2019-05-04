#pragma once

#include <deque>
#include <SFML/Graphics.hpp>
#include <mutex>


//enum class PacketType
//{
//	EstablishConnection = 0,
//	RemoteClientConnection,
//	RemoteClientDisconnection,
//	ClientName,
//	EnteredGame,
//	HeartBeat,
//	PlayerPosition,
//	LatestGameData,
//	LatestLevelName,
//	LatestClientPositions
//};


enum class PacketType
{
	Connect = 0,
	Disconnect,
	HeartBeat,
	PlayerPosition,
	LatestGameData,
	LatestLevelName,
	LatestClientPositions
};

struct ServerMessage
{
	ServerMessage(PacketType packetType, int clientID);
	ServerMessage(PacketType packetType, int clientID, sf::Vector2i position);
	ServerMessage(PacketType packetType, int clientID, std::string&& stringMessage);

	int m_clientID;
	PacketType m_packetType;
	sf::Vector2i m_position;
	std::string m_name;
};

class Messenger
{
public:
	Messenger();
	
	bool isEmpty() const;

	void addMessage(ServerMessage&& newMessage);
	ServerMessage getMessage();

	void update();

private:
	std::deque<ServerMessage> m_serverMessages;
	std::mutex m_mutex;
};