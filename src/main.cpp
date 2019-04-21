#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include <list>
#include <thread>
#include <deque>
#include <mutex>

//class MyClass {
//private:
//	int data;
//public:
//	MyClass(int data) : data{ data };
//
//	// Function that is going to be run in the thread.
//	// Typically contains some loop.
//	void doSomething(int param1, int param2, int param3);
//
//	int getValue();
//};
//
//int main()
//{
//	MyClass my_obj(5);
//
//	std::thread t{ &MyClass::doSomething, // function
//				  &my_obj,               // object
//				  15,                    // Param 1
//				  -1,                    // Param 2
//				  42 };                   // Param 3
//	// This will run my_obj.doSomething(param1, param2,
//	// param3) in the thread t.
//
	//1
	//2
	//3
	//4
//
//	// Do something else in main thread...
//	// Usually you want to run things in a loop here.
//
//	// Waiting for the thread to terminate.
//	t.join();
//	return 0;
//}

//https://support.holmsecurity.com/hc/en-us/articles/212963869-What-is-the-difference-between-TCP-and-UDP-
//https://stackoverflow.com/questions/3329641/how-do-multiple-clients-connect-simultaneously-to-one-port-say-80-on-a-server
//http://it-elibrary.blogspot.com/2016/01/sfml-game-development-by-example.html
//https://en.sfml-dev.org/forums/index.php?topic=943.0

//Listen all the time on one thread
//input buffer
//receives data
//stores for a bit
//spits out when neccessary

enum class PacketType
{
	Connect = 0,
	Disconnect,
	PlayerPosition
};

std::string IP_ADDRESS = "192.168.0.14";
int PORT_NUMBER = 5030;
sf::IpAddress ip("192.168.0.14");
unsigned short port = 5030;
constexpr int INVALID_CLIENT_ID = -1;

//struct Client
//{
//	Client(sf::TcpSocket& socket, int clientID, sf::IpAddress& ipAddress, unsigned short portNumber, 
//		std::deque<std::pair<PacketType, int>>& serverQueue, sf::SocketSelector& socketSelector)
//		: m_tcpSocket(socket),
//		m_ID(clientID),
//		m_ipAddress(ipAddress),
//		m_portNumber(portNumber),
//		m_connected(true),
//		m_serverQueue(serverQueue)
//	{}
//
//	void listen()
//	{

//	}
//
//	std::mutex m_mutex;
//	sf::TcpSocket& m_tcpSocket;
//	sf::IpAddress m_ipAddress;
//	int m_ID;
//	unsigned short m_portNumber;
//	bool m_connected;
//	std::deque<std::pair<PacketType, int>>& m_serverQueue;
//	sf::SocketSelector m_socketSelector;
//};

//class Server
//{
//public:
//	Server(sf::IpAddress ipAddress, unsigned short portNumber)
//		: m_totalClients(0),
//		m_clients(),
//		m_udpSocket(),
//		m_tcpListener(),
//		m_ipAddress(ipAddress),
//		m_socketSelector(),
//		m_portNumber(portNumber),
//		m_running(true)
//	{
//		if (m_tcpListener.listen(m_portNumber, m_ipAddress) != sf::Socket::Done)
//		{
//			std::cout << "TCP Listener unable to listen\n";
//		}
//		m_socketSelector.add(m_tcpListener);
//		m_socketSelector.add(m_udpSocket);
//		std::cout << "Server Successfully Started\n";
//	}
//
//	~Server()
//	{
//		for (auto& i : m_clientThreads)
//		{
//			i.second.join();
//		}
//
//		m_listenThread.join();
//	}
//
//	void listen()
//	{
//		while (m_running)
//		{
//			if (m_socketSelector.wait(sf::seconds(2)))
//			{
//				//Handle new clients
//				if (m_socketSelector.isReady(m_tcpListener))
//				{
//					//if == fail
//					//Failed to add client
//					//Display server message saying couldn't accept connection from tcp socket
//					addClient();
//				}
//			}
//		}
//	}
//
//	void addClient()
//	{
//		sf::TcpSocket* socket = new sf::TcpSocket();
//		if (m_tcpListener.accept(*socket) == sf::Socket::Done)
//		{
//			socket->connect(m_ipAddress, m_portNumber);
//			m_socketSelector.add(*socket);
//		
//			sf::Packet packet;
//			packet << static_cast<int>(PacketType::Connect) << m_totalClients;
//			if (socket->send(packet) != sf::Socket::Done)
//			{
//				delete socket;
//				std::cout << "Client added unsucessfuly.\n";
//				return;
//			}
//			
//			Client* client = new Client(*socket, m_totalClients, m_ipAddress, m_portNumber, m_queue, m_socketSelector);
//			//m_clientThreads.push_back(std::make_pair(m_totalClients, std::thread(&Client::listen, *client)));
//			m_clients.push_back(client);
//			++m_totalClients;
//			std::cout << "New Client Added\n";
//		}
//		else
//		{
//			delete socket;
//			std::cout << "Client added unsuccessfully.\n";
//		}
//	}
//
//	void update()
//	{
//		if (m_queue.empty())
//		{
//			return;
//		}
//
//		for (const auto& i : m_queue)
//		{
//			//Handle Client Disconnects
//			if (i.first == PacketType::Disconnect)
//			{
//				disconnectClient(i.second);
//			}
//		}
//		m_queue.clear();
//	}
//
//	void disconnectClient(int clientID)
//	{
//		auto iter = std::find_if(m_clients.begin(), m_clients.end(), [clientID](Client* client) { return client->m_ID == clientID; });
//		if (iter != m_clients.end())
//		{
//			m_socketSelector.remove((*iter)->m_tcpSocket);
//			//Erase Thread
//			auto activeClientThread = std::find_if(m_clientThreads.begin(), m_clientThreads.end(), [clientID](const auto& thread) { return thread.first == clientID; });
//			if (activeClientThread != m_clientThreads.end())
//			{
//				//TODO: Not sure about this function. 
//				//Might be dangerous
//				activeClientThread->second.detach();
//				m_clientThreads.erase(activeClientThread);
//			}
//
//			m_clients.erase(iter);
//			broadcastUDPMessage(clientID);
//		}
//	}
//
//	void broadcastUDPMessage(int clientID)
//	{
//		//Inform all clients of client disconnection
//		sf::Packet packet;
//		packet << static_cast<int>(PacketType::Disconnect) << clientID;
//		if (m_udpSocket.send(packet, m_ipAddress, m_portNumber) == sf::Socket::Done)
//		{
//			std::cout << "Client disconnected from server.\n";
//		}
//	}
//	void broadcastTCPMessage()
//	{
//		//TODO: 
//		//Broadcast messages that have to be picked up from clients
//	}
//
//	bool isRunning() const
//	{
//		return m_running;
//	}
//
//	std::deque<std::pair<PacketType, int>> m_queue;
//	std::list<std::pair<int, std::thread>> m_clientThreads;
//	std::thread m_listenThread;
//	int m_totalClients;
//	std::list<Client*> m_clients;
//	sf::UdpSocket m_udpSocket;
//	sf::TcpListener m_tcpListener;
//	sf::IpAddress m_ipAddress;
//	sf::SocketSelector m_socketSelector;
//	unsigned short m_portNumber;
//	bool m_running;
//};

//Have list mutex locked
//mutex lock
//mutex unlock

//Mutex when adding somethihng to buffer-Contaienr - Queue
//Only one access can access the queue at one time


//Client have own thread


//Send TCP TO SERVER - THEN SEND UDP TO EVERYONE ELSE
//THREAD LISTENING FORF NEW CLIENTS
//THREAD TCP SOCKETES IN CLIENTS
//THREAD UDP MESSAGES
//std promise
//std Future 

//handleNewClients()
//HandleTCPMessage
//sendUDPMessage();

int main()
{
	Server server("192.168.0.14", 5030);
	server.m_listenThread = std::thread(&Server::listen, &server);
	while (server.isRunning())
	{
		server.update();
	}

	return 0;
}

//
//if (socketSelector.isReady(server.m_udpSocket))
//{
//	sf::Packet packet;
//	int packetType = 0;
//	if (server.m_udpSocket.receive(packet, server.m_ipAddress, server.m_portNumber) == sf::Socket::Done)
//	{
//		packet >> packetType;
//		if (static_cast<PacketType>(packetType) == PacketType::Connect)
//		{
//			packet.clear();
//			packet << static_cast<int>(PacketType::Connect) << server.m_totalClients;
//			if (server.m_udpSocket.send(packet, server.m_ipAddress, server.m_portNumber) == sf::Socket::Done)
//			{
//				server.addClient();
//			}
//		}
//		else if (static_cast<PacketType>(packetType) == PacketType::Disconnect)
//		{
//			int clientID = INVALID_CLIENT_ID;
//			packet >> packetType >> clientID;
//			if (clientID == INVALID_CLIENT_ID)
//			{
//				int i = 0;
//			}
//		}
//	}
//}

//#include <SFML/Graphics.hpp>
//#include <SFML/Network.hpp>
//#include <iostream>
//#include <vector>
//#include <memory>
//#include <list>
//
////https://support.holmsecurity.com/hc/en-us/articles/212963869-What-is-the-difference-between-TCP-and-UDP-
////https://stackoverflow.com/questions/3329641/how-do-multiple-clients-connect-simultaneously-to-one-port-say-80-on-a-server
////http://it-elibrary.blogspot.com/2016/01/sfml-game-development-by-example.html
////https://en.sfml-dev.org/forums/index.php?topic=943.0
//
////Listen all the time on one thread
////input buffer
////receives data
////stores for a bit
////spits out when neccessary
//
//enum class PacketType
//{
//	connect = 0
//};
//
//std::string IP_ADDRESS = "192.168.0.14";
//int PORT_NUMBER = 5030;
//sf::IpAddress ip("192.168.0.14");
//unsigned short port = 5030;
//constexpr int INVALID_CLIENT_ID = -1;
//
//struct Client
//{
//	Client()
//		: m_tcpSocket(),
//		m_ID(INVALID_CLIENT_ID)
//	{
//		sf::Packet packet;
//		int type;
//		int clientID = INVALID_CLIENT_ID;
//		if (m_tcpSocket.receive(packet) != sf::Socket::Done)
//		{
//			packet >> type >> clientID;
//			if (static_cast<PacketType>(type) == PacketType::connect)
//			{
//				m_ID = clientID;
//			}
//		}
//	}
//
//	void connect()
//	{
//		if (m_tcpSocket.connect(IP_ADDRESS, PORT_NUMBER) != sf::Socket::Done)
//		{
//			std::cout << "New Client failed to connect to server\n";
//		}
//
//		//m_tcpSocket.send()
//	}
//
//	int m_ID;
//	sf::TcpSocket m_tcpSocket;
//};
//
//class Server
//{
//public:
//	Server()
//	{
//		if (m_listener.listen(PORT_NUMBER) != sf::Socket::Done)
//		{
//			std::cout << "Failed to listen\n";
//		}
//		else
//		{
//			std::cout << "Listening succesful\n";
//		}
//
//	}
//	~Server()
//	{
//		m_listener.close();
//	}
//
//	void addClient()
//	{
//
//	}
//
//	void acceptNewClient(sf::TcpSocket& socket)
//	{
//		if (m_listener.accept(socket) != sf::Socket::Done)
//		{
//			std::cout << "Failed to add new client.\n";
//		}
//		else
//		{
//			std::cout << "Server Added new client\n";
//			
//		}
//	}
//	
//	std::list<Client*> m_clients;
//	sf::TcpListener m_listener;
//	sf::UdpSocket m_udpSocket;
//};
//
////std promise
////std Future 
//
//int main()
//{
//	sf::RenderWindow window(sf::VideoMode(640, 480), "SFML_WINDOW", sf::Style::Default);
//	window.setFramerateLimit(30);
//	Server server;
//	sf::SocketSelector socketSelector;
//
//	while (true)
//	{
//		if (socketSelector.wait(sf::seconds(2)))
//		{
//			if (socketSelector.isReady(server.m_listener))
//			{
//				//thread
//				Client* newClient = new Client();
//				if (server.m_listener.accept(newClient->m_tcpSocket) == sf::Socket::Done)
//				{	
//					socketSelector.add(newClient->m_tcpSocket);
//					newClient->connect();
//					server.m_clients.push_back(newClient);
//
//					std::cout << "New Client Added\n";
//				}
//				else
//				{
//					delete newClient;
//					std::cout << "Cannot attach new client to server\n";
//				}	
//				//join();
//			}
//		}
//	}
//
//	return 0;
//}