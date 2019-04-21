#pragma once

#include "Level/Level.h"
#include "SFML/Network.hpp"
#include "ServerMessage.h"
#include <vector>
#include <deque>
#include <list>
#include <thread>
#include <mutex>

struct Client;
class Server
{
public:
	Server(sf::IpAddress ipAddress, unsigned short portNumber);
	Server(const Server&) = delete;
	Server& operator=(const Server&) = delete;
	Server(Server&&) = delete;
	Server&& operator=(Server&&) = delete;

	void update();

private:
	const sf::IpAddress m_ipAddress;
	const unsigned short m_portNumber;
	Level m_currentLevel;
	std::deque<ServerMessage> m_messageQueue;
	std::list<std::pair<int, std::thread>> m_clientThreads;
	std::thread m_listenThread;
	std::mutex m_listenThreadMutex;
	int m_totalClients;
	std::list<Client*> m_clients;
	sf::UdpSocket m_udpSocket;
	sf::TcpListener m_tcpListener;
	sf::SocketSelector m_socketSelector;
	bool m_running;

	void disconnectClient(int clientID);
	void broadcastUDPMessage(sf::Packet& packet);
	void addClient();
	void listen();
	void updatePlayerPosition(int clientID, sf::Vector2f newPosition);
};