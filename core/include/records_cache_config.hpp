#pragma once

#include <chrono>
#include <string>

#include "records_cache_constants.hpp"

namespace engine
{
	class records_cache_config final
	{
	public:
		records_cache_config();
		void load();

		bool use_cache() const
		{
			return m_use_cache;
		}

		void use_cache(bool use_cache)
		{
			m_use_cache = use_cache;
		}

		bool use_retention() const
		{
			return m_use_retention;
		}

		void use_retention(bool use_retention)
		{
			m_use_retention = use_retention;
		}

		std::chrono::seconds cache_store_timeout_seconds() const
		{
			return m_cache_store_timeout;
		}

		void cache_store_timeout_seconds(std::chrono::seconds cache_store_timeout)
		{
			m_cache_store_timeout = cache_store_timeout;
		}

		std::chrono::seconds cache_retention_timeout_seconds() const
		{
			return m_cache_retention_timeout;
		}

		void cache_retention_timeout_seconds(std::chrono::seconds cache_retention_timeout)
		{
			m_cache_retention_timeout = cache_retention_timeout;
		}

		unsigned int set_block_size() const
		{
			return m_set_block_size;
		}

		void set_block_size(unsigned int set_block_size)
		{
			m_set_block_size = set_block_size;
		}

		unsigned int number_of_cached_blocks_in_memory() const
		{
			return m_number_of_cached_blocks_in_memory;
		}

		void number_of_cached_blocks_in_memory(unsigned int number_of_cached_blocks_in_memory)
		{
			m_number_of_cached_blocks_in_memory = number_of_cached_blocks_in_memory;
		}

		std::string cache_path() const
		{
			return m_cache_path;
		}

		void cache_path(const std::string& cache_path)
		{
			m_cache_path = cache_path;
		}

	private:
		std::string m_cache_path;
		std::chrono::seconds m_cache_store_timeout;
		std::chrono::seconds m_cache_retention_timeout;
		unsigned int m_set_block_size;
		unsigned int m_number_of_cached_blocks_in_memory;
		bool m_use_cache;
		bool m_use_retention;
	};
}