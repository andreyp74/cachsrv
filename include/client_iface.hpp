#pragma once

#include <string>

namespace net
{
	class client_iface
	{
	public:
		virtual std::string receive() = 0;
		virtual bool send(const std::string& packet) = 0;

		virtual ~client_iface() {}
	};
}