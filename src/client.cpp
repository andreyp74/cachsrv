
#include "Poco/Logger.h"

#include "net.hpp"
#include "client.hpp"

using Poco::Logger;


namespace net
{
	client::client(const std::string& host, Poco::UInt16 port) :
		socket_addr(host, port)
	{
		std::srand((unsigned int)std::time(nullptr));
	}

	client::~client()
	{
	}

	void  client::connect()
	{
		try 
		{
			socket.connect(socket_addr);
			Logger::get("cachesrv").information("Connected to " + socket_addr.host().toString() + ":" + std::to_string(socket_addr.port()));
		}
		catch (Poco::Exception& err) 
		{
			Logger::get("cachesrv").error("Connection failed: " + std::string(err.what()));
			throw;
		}
	}

	std::string client::receive()
	{
		std::string msg = net::receive(this->socket);
		Logger::get("cachesrv").information("Received request: " + msg);
		return msg;
	}

	void client::send(const std::string& msg)
	{
		net::send(this->socket, msg);
		Logger::get("cachesrv").information("Sent: " + msg);
	}
}