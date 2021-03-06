#pragma once

#include <string>

#include "Poco/Net/StreamSocket.h"

namespace net
{
    using namespace Poco;
    using namespace Poco::Net;

    inline bool send(StreamSocket& socket, const std::string& request)
    {
        size_t size = request.size();
		int sent = 0;
		while (sent < (int)sizeof(size_t))
		{
			auto ret = socket.sendBytes(&size, (int)sizeof(size_t));
			if (ret < 0)
				return false;
			sent += ret;
		}

		sent = 0;
		while (sent < (int)size)
		{
			auto ret = socket.sendBytes(request.data(), (int)size);
			if (ret < 0)
				return false;
			sent += ret;
		}
		return true;
    }

    inline std::string receive(StreamSocket& socket)
    {
        unsigned char buffer[sizeof(size_t)];
        socket.receiveBytes(buffer, (int)sizeof(buffer));
        size_t size = *(size_t*)&buffer;

        std::string msg;
        msg.resize(size);
        int received_bytes = 0;
        while (received_bytes < size)
        {
            received_bytes += socket.receiveBytes(msg.data() + received_bytes, (int)(size - received_bytes));
        }
        return msg;
    }
}