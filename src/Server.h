#pragma once

#include "Level/Level.h"
#include "SFML/Network.hpp"
#include "Messenger.h"
#include "GameData.h"
#include <deque>
#include <list>
#include <thread>
#include <mutex>

struct Client;
class Server
{
public:
	Server(const sf::IpAddress& ipAddress, unsigned short portNumber);
	~Server();
	Server(const Server&) = delete;
	Server& operator=(const Server&) = delete;
	Server(Server&&) = delete;
	Server&& operator=(Server&&) = delete;

	bool isRunning() const;
	void beginListenThread();

	void update();

private:
	const sf::IpAddress m_ipAddress;
	const unsigned short m_portNumber;
	const int m_totalClientsAllowed;
	Messenger m_messenger;
	std::list<std::pair<int, std::thread>> m_clientThreads;
	std::thread m_listenThread;
	std::mutex m_listenThreadMutex;
	std::list<Client*> m_clients;
	sf::UdpSocket m_udpSocket;
	sf::TcpListener m_tcpListener;
	sf::SocketSelector m_socketSelector;
	bool m_running;


	void disconnectClient(int clientID);
	void broadcastUDPMessage(sf::Packet& packet);
	void broadcastTCPMessage(sf::Packet& packet);
	void addClient();
	void listen();
	void updatePlayerPosition(int clientID, sf::Vector2f newPosition);
	void handleMessageQueue();
	
	int getNumberOfClients() const;
	//bool sendLatestGameDataToClient(sf::TcpSocket& socket);
};