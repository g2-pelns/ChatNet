// ChatServer.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <SFML\Network.hpp>

constexpr int SERVER_TCP_PORT(53000);
constexpr int SERVER_UDP_PORT(53001);

using TcpClient		= sf::TcpSocket;
using TcpClientPtr	= std::unique_ptr<TcpClient>;
using TcpClients	= std::vector<TcpClientPtr>;

bool bindServerPort(sf::TcpListener&);
void listen(sf::TcpListener&, sf::SocketSelector&, TcpClients&);
void connect(sf::TcpListener&, sf::SocketSelector&, TcpClients&);
void receiveMsg(sf::SocketSelector&, TcpClients&);
void runServer();

int main()
{
	runServer();
    return 0;
}

bool bindServerPort(sf::TcpListener& _listener)
{
	if (_listener.listen(SERVER_TCP_PORT) != sf::Socket::Done)
	{
		std::cout << "Could not bind server port";
		std::cout << std::endl << "Port: " << SERVER_TCP_PORT;
		std::cout << std::endl;
		return false;
	}
}

void connect(sf::TcpListener& _listener, sf::SocketSelector& _selector, TcpClients& _clients)
{
	auto client_ptr = std::make_unique<sf::TcpSocket>();
	auto& client_ref = *client_ptr;
	if (_listener.accept(client_ref) == sf::Socket::Done)
	{
		_selector.add(client_ref);
		_clients.push_back(std::move(client_ptr));
	}
}

void receiveMsg(sf::SocketSelector& _selector, TcpClients& _clients)
{
	for (auto& sender : _clients)
	{
		auto& sender_ref = *sender;
		if (_selector.isReady(sender_ref))
		{
			sf::Packet packet;
			std::string string;
			packet >> string;

			std::cout << string << std::endl;

			for (auto& client : _clients)
			{
				client->send(packet);
			}
		}
	}
}

void listen(sf::TcpListener& _listener, sf::SocketSelector& _selector, TcpClients& _clients)
{
	while (true)
	{
		if (_selector.wait())
		{
			if (_selector.isReady(_listener))
			{
				connect(_listener, _selector, _clients);
			}
			else
			{
				receiveMsg( _selector, _clients);
			}
		}
	}
}

void runServer()
{
	sf::TcpListener _listener;
	if (!bindServerPort(_listener))
	{
		return;
	}

	sf::SocketSelector _selector;
	_selector.add(_listener);

	TcpClients _clients;
	return listen(_listener, _selector, _clients);
}