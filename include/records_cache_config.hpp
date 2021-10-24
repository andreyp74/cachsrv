#pragma once

#include <chrono>
#include <string>

namespace engine
{
	namespace constants
	{
		static constexpr std::chrono::seconds CACHE_STORE_TIMEOUT{ 60 };
		static constexpr unsigned int CACHE_SENT_BLOCK_size{ 1000 };
		static constexpr unsigned int NUMBER_OF_CACHED_BLOCKS_IN_MEMORY{ 32 };
		static constexpr char* CACHE_PATH = "local";
		static constexpr char* CACHE_TEMP_PATH = "tmp";
		static constexpr char* CACHE_READY_PATH = "ready";
		static constexpr char* CACHE_DEL_PATH = "del";
	}

	class records_cache_config final
	{
	public:
		records_cache_config();
		void load();

		bool use_cache() const
		{
			return m_use_cache;
		}

		std::chrono::seconds cache_store_timeout_seconds() const
		{
			return m_cache_store_timeout;
		}

		unsigned int set_block_size() const
		{
			return m_set_block_size;
		}

		unsigned int number_of_cached_blocks_in_memory() const
		{
			return m_number_of_cached_blocks_in_memory;
		}

		std::string cache_path() const
		{
			return m_cache_path;
		}

	private:
		std::string m_cache_path;
		std::chrono::seconds m_cache_store_timeout;
		unsigned int m_set_block_size;
		unsigned int m_number_of_cached_blocks_in_memory;
		bool m_use_cache;
	};
}