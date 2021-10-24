
#include <chrono>
#include <sstream>
#include <functional>

#include "Poco/Util/ServerApplication.h"
#include "Poco/Logger.h"

#include "server.hpp"


using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;
using Poco::Logger;

using namespace std::placeholders;

namespace net
{
	server::server(const Poco::Net::StreamSocket& s,
		std::shared_ptr<engine::records_cache> cache)
		: Poco::Net::TCPServerConnection(s),
		  m_cache(cache)
	{
	}

	std::string server::receive()
	{
		std::string msg = net::receive(this->socket());
		Logger::get("cachesrv").information("Received request: " + msg);
		return msg;
	}

	void server::send(const std::string& msg)
	{
		net::send(this->socket(), msg);
		Logger::get("cachesrv").information("Sent: " + msg);
	}

	void server::run()
	{
		auto& logger = Logger::get("cachesrv");
		logger.information("Connection from " + this->socket().peerAddress().toString());

		while (true)
		{
			try
			{
				logger.information("Waiting for request...");
				auto msg = receive();
				m_cache->send(msg);
			}
			catch (Poco::Exception& exc)
			{
				//deny?
				logger.error(exc.displayText());
			}
		}
		logger.information("Connection is closing");
	}
}