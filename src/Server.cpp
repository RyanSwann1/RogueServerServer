#include "Server.h"
#include "Client.h"
#include <iostream>

constexpr int TOTAL_CLIENTS_ALLOWED = 6;
constexpr int HEARTBEAT_INTERVAL = 10000;

Server::Server(sf::IpAddress ipAddress, unsigned short portNumber)
	: m_ipAddress(ipAddress),
	m_portNumber(portNumber),
	m_totalClientsAllowed(TOTAL_CLIENTS_ALLOWED),
	m_messageQueue(),
	m_clientThreads(),
	m_listenThread(),
	m_listenThreadMutex(),
	m_totalClients(0),
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

void Server::update(const sf::Time& time)
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
	//Say to client to stop responding to any incoming messages;

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
			socket->connect(m_ipAddress, m_portNumber);
			sf::Packet packet;
			packet << static_cast<int>(PacketType::Disconnect);
			socket->send(packet);

			//TODO: This might be dangerous
			//Could delete before the message has been sent out 
			delete socket;
			return;
		}

		//socket->connect(m_ipAddress, m_portNumber);
		m_listenThreadMutex.lock();
		m_socketSelector.add(*socket); 
		m_listenThreadMutex.unlock();
		sf::Packet packet;
		//Send game data
		packet << static_cast<int>(PacketType::Connect) << m_totalClients;
		if (socket->send(packet) != sf::Socket::Done)
		{
			delete socket;
			std::cout << "Client added unsucessfuly.\n";
			return;
		}
		
		m_listenThreadMutex.lock();
		//Client(std::deque<ServerMessage>& serverQueue, sf::SocketSelector& socketSelector, sf::TcpSocket& tcpSocket,
		//	const sf::IpAddress& serverIPAddress, unsigned short serverPortNumber);
		Client* client = new Client(m_messageQueue, m_socketSelector, *socket, m_ipAddress, m_portNumber);

			//*socket, m_totalClients, m_ipAddress, m_portNumber, m_messageQueue, m_socketSelector);
		//m_clientThreads.push_back(std::make_pair(m_totalClients, std::thread(&Client::listen, *client)));
		m_clients.push_back(client);
		++m_totalClients;
		m_listenThreadMutex.unlock();
		std::cout << "New Client Added\n";
	}
	else
	{
		delete socket;
		std::cout << "Client added unsuccessfully.\n";
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

void Server::handleServerHeartbeats()
{

}
