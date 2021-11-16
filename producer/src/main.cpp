
#include <chrono>
#include "client.hpp"

int main()
{
    producer::client client("localhost", 9191);
    client.start();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

    return 0;
}