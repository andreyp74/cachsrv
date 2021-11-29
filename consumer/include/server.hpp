#pragma once

#include <atomic>
#include <memory>
#include <assert.h>
#include <iostream>
#include <iterator>

#include "Poco/Net/TCPServer.h"
#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Net/TCPServerConnectionFactory.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Exception.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"

#include "net.hpp"
#include "json_helper.hpp"


using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;
using namespace engine;

class server_connection: public TCPServerConnection
	/// This class handles all client connections.
{
public:
	explicit server_connection(const StreamSocket& s) 
		: 
		TCPServerConnection(s)
	{}

	std::string receive()
	{
		std::string msg = net::receive(this->socket());
		Logger::get("cachesrv").information("Received request: " + msg);
		return msg;
	}

	void run()
	{
		Application& app = Application::instance();
		app.logger().information("Connection from " + this->socket().peerAddress().toString());
		while(true)
		{
			try
			{	
				app.logger().information("Waiting for request...");

				auto msg = receive();
				auto records = deserialize(msg);
				if (records.isNull())
				{
					app.logger().information("Couldn't unpack received data");
				}
				else
				{
					js::Array& arr = *records;
					app.logger().information("Size of received block: " + std::to_string(arr.size()));
				}
			}
			catch (Poco::Exception& exc)
			{
				app.logger().log(exc);
				break;
			}
		}
		;
		app.logger().information("Connection closed");
	}

private:

};

class server_connection_factory: public TCPServerConnectionFactory
{
public:
	server_connection_factory()
	{}

	TCPServerConnection* createConnection(const StreamSocket& socket) override
	{
		return new server_connection(socket);
	}

private:
};

class server: public ServerApplication
{
protected:

	int main(const std::vector<std::string>&)
	{	
		// get parameters from configuration file
		unsigned short port = (unsigned short)config().getInt("Server.port", 9192);

		// set-up a server socket
		ServerSocket svs(port);
		//svs.setBlocking(false);

		// set-up a TCPServer instance
		TCPServer srv(new server_connection_factory(), svs);
		// start the TCPServer
		srv.start();

		std::cout << "Server started on port: " << port << std::endl;

		// wait for CTRL-C or kill
		waitForTerminationRequest();

		// Stop the TCPServer
		srv.stop();
		
		return Application::EXIT_OK;
	}
};
