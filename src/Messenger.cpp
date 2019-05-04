#include "Messenger.h"

ServerMessage::ServerMessage(PacketType packetType, int clientID)
	: m_clientID(clientID),
	m_packetType(packetType),
	m_position(),
	m_name()
{
}

ServerMessage::ServerMessage(PacketType packetType, int clientID, sf::Vector2i position)
	: m_clientID(clientID),
	m_packetType(packetType),
	m_position(),
	m_name()
{
}

ServerMessage::ServerMessage(PacketType packetType, int clientID, std::string&& stringMessage)
	: m_clientID(clientID),
	m_packetType(packetType),
	m_position(),
	m_name(std::move(stringMessage))
{
}

Messenger::Messenger()
	: m_serverMessages(),
	m_mutex()
{}

bool Messenger::isEmpty() const
{
	return m_serverMessages.empty();
}

void Messenger::addMessage(ServerMessage && newMessage)
{
	std::unique_lock<std::mutex> lock{ m_mutex };
	m_serverMessages.push_back(std::move(newMessage));
}

ServerMessage Messenger::getMessage()
{
	std::unique_lock<std::mutex> lock{ m_mutex };
	ServerMessage serverMessage = m_serverMessages.front();
	m_serverMessages.pop_front();
	return serverMessage;
}

void Messenger::update()
{
}
