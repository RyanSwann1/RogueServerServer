#pragma once

#include "SFML/Network.hpp"
#include <SFML/Graphics.hpp>
#include "ServerMessage.h"
#include <deque>
#include <mutex>

constexpr int INVALID_CLIENT_ID = -1;

class Client
{
public:
	Client(std::deque<ServerMessage>& serverQueue, sf::SocketSelector& socketSelector, sf::TcpSocket& tcpSocket,
		const sf::IpAddress& serverIPAddress, unsigned short serverPortNumber);

	int getID() const;
	sf::TcpSocket& getTCPSocket();

	void listen();

private:
	std::deque<ServerMessage>& m_serverQueue;
	sf::SocketSelector& m_socketSelector;
	bool m_connected;
	sf::TcpSocket& m_tcpSocket;
	std::mutex m_mutex;
	sf::IpAddress m_serverIPAddress;
	unsigned short m_serverPortNumber;
	int m_ID;
};