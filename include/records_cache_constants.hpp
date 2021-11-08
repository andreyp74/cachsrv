#pragma once

#include <chrono>

namespace engine
{
	namespace constants
	{
		static constexpr std::chrono::seconds CACHE_STORE_TIMEOUT{ 60 };
		static constexpr std::chrono::seconds CACHE_RETENTION_TIMEOUT{ 60 };
		static constexpr unsigned int CACHE_SENT_BLOCK_size{ 1000 };
		static constexpr unsigned int NUMBER_OF_CACHED_BLOCKS_IN_MEMORY{ 32 };
		static constexpr char* CACHE_PATH = "local";
		static constexpr char* CACHE_TEMP_PATH = "tmp";
		static constexpr char* CACHE_READY_PATH = "ready";
		static constexpr char* CACHE_DEL_PATH = "del";
		static constexpr char* CACHE_FAILED_PATH = "failed";
	}
}