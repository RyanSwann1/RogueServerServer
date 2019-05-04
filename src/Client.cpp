#include "Client.h"

Client::Client(Messenger & messenger, sf::TcpSocket & tcpSocket)
	: m_messenger(messenger),
	m_tcpSocket(tcpSocket),
	m_ID(INVALID_CLIENT_ID)
{
	sf::Packet packet;
	int packetType = static_cast<int>(PacketType::EstablishConnection);
	packet << packetType << m_ID;
	m_tcpSocket.send(packet);
}

int Client::getID() const
{
	return m_ID;
}

sf::TcpSocket & Client::getTCPSocket()
{
	return m_tcpSocket;
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
		sf::Vector2i position;
		std::string name;
		packet >> packetType >> clientID;
		switch (static_cast<PacketType>(packetType))
		{
		case PacketType::Disconnect:
			m_connected = false;
			m_messenger.addMessage(ServerMessage(static_cast<PacketType>(packetType), clientID));
			break;
		case PacketType::Connect :
			packet >> packetType >> clientID >> name;
			m_ID = clientID;
			m_name = name;
			m_messenger.addMessage(ServerMessage(static_cast<PacketType>(packetType), clientID));
			break;
		}
	}
}