#include "Client.h"

//Client
Client::Client(std::deque<ServerMessage>& serverQueue, sf::SocketSelector & socketSelector, 
	sf::TcpSocket & tcpSocket, const sf::IpAddress & serverIPAddress, unsigned short serverPortNumber)
	: m_serverQueue(serverQueue),
	m_socketSelector(socketSelector),
	m_connected(false),
	m_tcpSocket(tcpSocket),
	m_mutex(),
	m_serverIPAddress(serverIPAddress),
	m_serverPortNumber(serverPortNumber),
	m_ID(INVALID_CLIENT_ID)
{}

bool Client::isActive() const
{
	return waitingForHearbeat;
}

bool Client::waitingForHeartbeat() const
{
	return m_waitingForHearbeat;
}

bool Client::waitingForSecondHeartbeat() const
{
	return m_waitingForSecondHeartbeat;
}

int Client::getID() const
{
	return m_ID;
}

sf::TcpSocket & Client::getTCPSocket()
{
	return m_tcpSocket;
}

void Client::waitForHeartbeat()
{
	m_waitingForHearbeat = true;
}

void Client::listen()
{
	sf::Packet packet;
	while (m_connected)
	{
		packet.clear();
		if (m_tcpSocket.receive(packet) != sf::Socket::Done)
		{
			continue;
		}

		int packetType = 0;
		int clientID = 0;
		packet >> packetType >> clientID;
		switch (static_cast<PacketType>(packetType))
		{
		case PacketType::Disconnect:
			m_mutex.lock();
			m_serverQueue.emplace_back(clientID, static_cast<PacketType>(packetType));
			m_mutex.unlock();
			break;
		case PacketType::PlayerPosition:
			m_mutex.lock();
			sf::Vector2f position;
			packet >> packetType >> clientID >> position.x >> position.y;
			m_serverQueue.emplace_back(clientID, static_cast<PacketType>(packetType), position);
			m_mutex.unlock();
			break;
			case PacketTpy
		}
	}
}