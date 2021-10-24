#pragma once

#include <string>

#include "Poco/Net/TCPServerConnectionFactory.h"
#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Net/StreamSocket.h"

#include "records_cache.hpp"
#include "net.hpp"

namespace net
{
	class server : public Poco::Net::TCPServerConnection
	{
	public:
		server(const Poco::Net::StreamSocket& s,
			   std::shared_ptr<engine::records_cache> cache);
		virtual ~server() {}

		void run() override;

	private:
		std::string receive();
		void send(const std::string& packet);

	private:
		std::shared_ptr<engine::records_cache> m_cache;
	};

	class server_factory : public Poco::Net::TCPServerConnectionFactory
	{
	public:
		server_factory(std::shared_ptr<engine::records_cache> cache) :
			m_cache(cache)
		{}

		Poco::Net::TCPServerConnection* createConnection(const Poco::Net::StreamSocket& socket) override
		{
			return new server(socket, m_cache);
		}

	private:
		std::shared_ptr<engine::records_cache> m_cache;
	};
}
