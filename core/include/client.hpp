#pragma once

#include <thread>
#include <string>
#include <atomic>
#include <chrono>

#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/StreamSocket.h"
//#include "Poco/Net/SocketStream.h"
//#include "Poco/StreamCopier.h"

#include "client_iface.hpp"


namespace net
{
	using namespace Poco;
	using namespace Poco::Net;

	class client final : public client_iface
	{
	public:
		client(const std::string& host, Poco::UInt16 port);
		~client() override;

		std::string receive() override;
		bool send(const std::string& packet) override;

	private:
		client(client&) = delete;
		client& operator=(client&) = delete;

		StreamSocket& get_socket() { return socket; }

		void  connect();

	private:
		// IP endpoint/socket address (consists of host addr and port #)
		SocketAddress socket_addr;

		// Interface to a TCP stream socket
		StreamSocket socket;
	};
}
