#include "Server.h"
#include "Client.h"
#include <iostream>

constexpr int TOTAL_CLIENTS_ALLOWED = 6;
constexpr int HEARTBEAT_INTERVAL = 10000;

Server::Server(const sf::IpAddress& ipAddress, unsigned short portNumber)
	: m_ipAddress(ipAddress),
	m_portNumber(portNumber),
	m_totalClientsAllowed(TOTAL_CLIENTS_ALLOWED),
	m_messageQueue(),
	m_clientThreads(),
	m_listenThread(),
	m_listenThreadMutex(),
	m_clients(),
	m_udpSocket(),
	m_tcpListener(),
	m_socketSelector(),
	m_running(true)
{
	if (m_tcpListener.listen(m_portNumber, m_ipAddress) != sf::Socket::Done)
	{
		std::cout << "TCP Listener unable to listen\n";
	}
	m_socketSelector.add(m_tcpListener);
	m_socketSelector.add(m_udpSocket);
	std::cout << "Server Successfully Started\n";
}

Server::~Server()
{
	m_listenThread.join();
}

bool Server::isRunning() const
{
	return m_running;
}

void Server::beginListenThread()
{
	m_listenThread = std::thread(&Server::listen, this);
}

void Server::update()
{
	handleMessageQueue();

	//m_elaspedTime += time.asMilliseconds();
	//if (m_elaspedTime >= HEARTBEAT_INTERVAL)
	//{
	//	for (auto& client : m_clients)
	//	{
	//		if (!client->waitingForHeartbeat())
	//		{
	//			client->waitForHeartbeat();
	//		}
	//		if (client->waitingForHeartbeat() && !client->waitingForSecondHeartbeat())
	//		{
	//			client->waitForSecondHeartBeat();
	//		}
	//		//Allow for client to attempt connection 
	//		if (client->waitingForHeartbeat())
	//		{
	//			continue;
	//		}
	//		//Client Not responding to server for second heartbeat
	//		//Disconnect client from server
	//		if (client->waitingForSecondHeartbeat())
	//		{
	//			addServerMessage(ServerMessage(client->getID(), PacketType::Disconnect));
	//		}
	//	}
	//	m_elaspedTime = 0;
	//}
}

void Server::disconnectClient(int clientID)
{
	//Say to client to stop responding to any incoming messages;m_
	auto iter = std::find_if(m_clients.begin(), m_clients.end(), [clientID](Client* client) { return client->getID() == clientID; });
	if (iter != m_clients.end())
	{
		m_socketSelector.remove((*iter)->getTCPSocket());
		//Erase Thread
		auto activeClientThread = std::find_if(m_clientThreads.begin(), m_clientThreads.end(), [clientID](const auto& thread) { return thread.first == clientID; });
		if (activeClientThread != m_clientThreads.end())
		{
			//TODO: Not sure about this function. 
			//Might be dangerous
			activeClientThread->second.detach();
			m_clientThreads.erase(activeClientThread);
		}

		//m_currentLevel.removePlayer(clientID);
		m_clients.erase(iter);

		//Inform other clients on disconnection
		sf::Packet packet;
		packet << clientID << static_cast<int>(PacketType::Disconnect);
		broadcastUDPMessage(packet);
	}
}

void Server::broadcastUDPMessage(sf::Packet& packet)
{
	//Inform all clients of client disconnection
	/*sf::Packet packet;
	packet << static_cast<int>(PacketType::Disconnect) << clientID;*/
	if (m_udpSocket.send(packet, m_ipAddress, m_portNumber) == sf::Socket::Done)
	{
		std::cout << "Client disconnected from server.\n";
	}
}

void Server::addClient()
{
	sf::TcpSocket* socket = new sf::TcpSocket();
	if (m_tcpListener.accept(*socket) == sf::Socket::Done)
	{
		//Game is full - disallow connection
		if (getNumberOfClients() == m_totalClientsAllowed)
		{
			sf::Packet packet;
			packet << static_cast<int>(PacketType::Disconnect);
			socket->send(packet);
			delete socket;
			return;
		}

		//TODO: Not sure if need
		//m_listenThreadMutex.lock();
		//m_socketSelector.add(*socket); 
		//m_listenThreadMutex.unlock();

		if (!sendLatestGameDataToClient(*socket))
		{
			delete socket;
			std::cout << "Client Added unsuccesfully\n";
			return;
		}

		m_listenThreadMutex.lock();
		Client* client = new Client(m_messageQueue, m_socketSelector, *socket, m_ipAddress, m_portNumber);
		m_clients.push_back(client);
		m_listenThreadMutex.unlock();
		std::cout << "New Client Added\n";
	}
	else
	{
		delete socket;
		std::cout << "Failed to listen to client.\n";
	}
}

void Server::listen()
{
	while (m_running)
	{
		if (m_socketSelector.wait())
		{
			if (m_socketSelector.isReady(m_tcpListener))
			{
				addClient();
			}
		}
	}
}

void Server::updatePlayerPosition(int clientID, sf::Vector2f newPosition)
{
	//m_currentLevel.updatePlayerPosition(clientID, newPosition);
	sf::Packet packet;
	packet << clientID << static_cast<int>(PacketType::PlayerPosition) << newPosition.x << newPosition.y;
	broadcastUDPMessage(packet);
}

int Server::getNumberOfClients() const
{
	return static_cast<int>(m_clients.size());
}

void Server::handleMessageQueue()
{
	if (m_messageQueue.empty())
	{
		return;
	}

	for (const auto& message : m_messageQueue)
	{
		switch (message.m_packetType)
		{
		case PacketType::Disconnect:
			disconnectClient(message.m_clientID);
			break;

		case PacketType::PlayerPosition:
			updatePlayerPosition(message.m_clientID, message.m_position);
			break;
		}
	}

	m_messageQueue.clear();
}

void Server::addServerMessage(const ServerMessage & serverMessage)
{
	m_messageQueue.push_back(serverMessage);
}

//struct GameData
//{
//	GameData();
//
//	const std::string& getCurrentLevelName() const;
//	void reset(const std::string& levelName);
//
//	void updatePlayerPosition(int clientID, sf::Vector2i newPosition);
//	void addPlayer(int clientID, sf::Vector2i startingPosition);
//	void removePlayer(int clientID);
//
//	std::string m_currentLevelName;
//	std::vector<ClientProperties> m_clients;
//	std::vector<sf::Vector2i> m_startingPositions;
//};
//uint16_t size = sizeof(levelName);
//memcpy(message[0], &size, sizeof(uint16_t));
//memcpy(message[sizeof(uint16_t)], levelName.data(), levelname.size());

bool Server::sendLatestGameDataToClient(sf::TcpSocket& socket)
{
	std::string levelName = std::string("Level1");
	sf::Packet packet;
	packet << static_cast<int>(PacketType::LatestLevelName) << levelName;
	if (socket.send(packet) != sf::Socket::Done)
	{
		std::cout << "Couldn't send packet to client\n";
	}

	std::vector<ClientProperties> gameData;
	gameData.emplace_back(sf::Vector2i(5, 5), 0);
	gameData.emplace_back(sf::Vector2i(11, 5), 1);
	gameData.emplace_back(sf::Vector2i(7, 5), 2);
	gameData.emplace_back(sf::Vector2i(8, 5), 3);

	packet.clear();
	for (auto& client : gameData)
	{
		packet << static_cast<int>(PacketType::LatestClientPositions) << client.m_ID << client.m_position.x << client.m_position.y;
		if (socket.send(packet) != sf::Socket::Done)
		{
			std::cout << "Couldn't send packet to client\n";
		}
		packet.clear();
	}

	//char message[1500];
	//uint16_t levelNameSize = sizeof(m_latestGameData.m_currentLevelName);
	////memcpy(&message[0], &levelNameSize, sizeof(uint16_t));
	//memcpy(&message[0], m_latestGameData.m_currentLevelName.data(), m_latestGameData.m_currentLevelName.size());
	//std::vector<int> p;
	//p.push_back(1);
	//p.push_back(2);
	//p.push_back(3);
	////memcpy(&message[sizeof(m_latestGameData.m_currentLevelName)], p.data(), p.size());
	////memcpy(&message[sizeof(levelNameSize)], )
	//memcpy(&message[sizeof(m_latestGameData.m_currentLevelName)], p.data(), p.size());
	//if (socket.send(message, sizeof(message)) != sf::Socket::Done)
	//{
	//	std::cout << "Failed to send data\n";
	//	return false;
	//}

	return true;
}