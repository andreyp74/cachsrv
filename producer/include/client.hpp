#pragma once

#include <string>
#include <iostream>
#include <thread>
#include <atomic>
#include <iterator>

#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketStream.h"

#include "net.hpp"

using namespace Poco;
using namespace Poco::Net;

namespace producer
{

	constexpr auto json_array{ R"json([
	{
		"id": "0",
		"value": 0
	},
	{
		"id": "1",
		"value": 1
	},
	{
		"id": "2",
		"value": 2
	},
	{
		"id": "3",
		"value": 3
	}])json" };


	class client
	{
	public:
		client(const std::string& host, Poco::UInt16 port) :
			socket_addr(host, port),
			host_(host),
			port_(port),
			done(false)
		{
		}

		~client()
		{
			stop();
		}

		void start()
		{
			while (!connect())
				std::this_thread::sleep_for(std::chrono::seconds(5));
			client_thread = std::thread(&client::run, this);
		}

		void stop()
		{
			done = true;
			if (client_thread.joinable())
				client_thread.join();
		}

		std::string gen_json_packet()
		{
			return json_array;
		}

		void run()
		{
			while (!done)
			{
				try
				{
					net::send(socket, gen_json_packet());
				}
				catch (Poco::Exception err)
				{
					std::cerr << "Error occurred: " << err.what() << std::endl;
				}

				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		}

		bool connect()
		{
			try 
			{
				socket.connect(socket_addr);
				std::cout << "Connected to " << host_ << ":" << port_ << std::endl;
				return true;
			}
			catch (Poco::Exception err) 
			{
				std::cerr << "Connection failed: " << err.what() << std::endl;
			}
			return false;
		}

	private:
		client(client&) = delete;
		client& operator=(client&) = delete;

		

	private:
		// IP endpoint/socket address (consists of host addr and port #)
		SocketAddress socket_addr;

		// Interface to a TCP stream socket
		StreamSocket socket;

		std::thread client_thread;

		std::string host_;
		int port_;

		std::atomic<bool> done;
	};
}