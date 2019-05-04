#pragma once

#include "SFML/Network.hpp"
#include "Messenger.h"
#include <string>

constexpr int INVALID_CLIENT_ID = -1;

class Client
{
public:
	Client(Messenger& messenger, sf::TcpSocket& tcpSocket);

	int getID() const;
	sf::TcpSocket& getTCPSocket();

	void listen();

private:
	std::string m_name;
	Messenger& m_messenger;
	sf::TcpSocket& m_tcpSocket;
	int m_ID;
	bool m_connected;
};