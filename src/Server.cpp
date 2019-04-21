#include "Server.h"
#include "Client.h"
#include <iostream>

Server::Server(sf::IpAddress ipAddress, unsigned short portNumber)
{
	if (m_tcpListener.listen(m_portNumber, m_ipAddress) != sf::Socket::Done)
	{
		std::cout << "TCP Listener unable to listen\n";
	}

	if (m_udpSocket.bind(m_portNumber, m_ipAddress) != sf::Socket::Done)
	{
		std::cout << "Unable to bind to port number\n";
	}

	m_socketSelector.add(m_tcpListener);
	m_socketSelector.add(m_udpSocket);
	std::cout << "Server Successfully Started\n";
}

void Server::update()
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

		m_currentLevel.removePlayer(clientID);
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
		socket->connect(m_ipAddress, m_portNumber);
		m_listenThreadMutex.lock();
		m_socketSelector.add(*socket); 
		m_listenThreadMutex.unlock();
		sf::Packet packet;
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
		m_clientThreads.push_back(std::make_pair(m_totalClients, std::thread(&Client::listen, *client)));
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
		if (m_socketSelector.wait(sf::seconds(2)))
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
	m_currentLevel.updatePlayerPosition(clientID, newPosition);
	sf::Packet packet;
	packet << clientID << static_cast<int>(PacketType::PlayerPosition) << newPosition.x << newPosition.y;
	broadcastUDPMessage(packet);
}