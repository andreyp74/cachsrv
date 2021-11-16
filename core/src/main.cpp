#include <atomic>
#include <memory>
#include <assert.h>
#include <iostream>
#include <iterator>
#include <algorithm>

#include "Poco/Net/TCPServer.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Logger.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Exception.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"

#include "server.hpp"
#include "client.hpp"
#include "records_cache_config.hpp"
#include "records_cache.hpp"


using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;
using Poco::Logger;
using namespace std::literals;

namespace net
{
	class training_app : public ServerApplication
	{
	protected:
		struct program_options
		{
			int server_port = 0;
			int client_port = 0;
			std::string client_host;
		};

		program_options parse_command_line(const std::vector<std::string>& args)
		{
			program_options options;
			auto end = args.end();
			for (auto it = args.begin(); it != end; ++it)
			{
				if (*it == "--server_port" && ++it != end)
				{
					options.server_port = std::stoi(*it);
				}
				else if (*it == "--client_host" && ++it != end)
				{
					options.client_host = *it;
				}
				else if (*it == "--client_port" && ++it != end)
				{
					options.client_port = std::stoi(*it);
				}
			}
			return options;
		}

		int main(const std::vector<std::string>& args)
		{
			Logger& logger = Logger::get("cachesrv");

			program_options options = parse_command_line(args);

			engine::records_cache_config config;
			config.cache_path("./cache");
			config.cache_retention_timeout_seconds(0s);
			config.cache_store_timeout_seconds(60s);
			config.number_of_cached_blocks_in_memory(10000);
			config.set_block_size(1000);
			config.use_cache(true);
			config.use_retention(false);

			auto clptr = std::make_shared<client>(options.client_host, (Poco::UInt16)options.client_port);

			auto storage = std::make_shared<engine::records_cache>(config, clptr);

			ServerSocket socket((Poco::UInt16)options.server_port);
			TCPServer server(new server_factory(storage), socket);
			server.start();

			logger.information("Server started on port: " + options.server_port);

			// wait for CTRL-C or kill
			waitForTerminationRequest();

			server.stop();

			return Application::EXIT_OK;
		}
	};
}

int main(int argc, char** argv)
{
	net::training_app app;
	return app.run(argc, argv);
}
