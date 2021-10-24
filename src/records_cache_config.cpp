#include "records_cache_config.hpp"

namespace engine
{
	records_cache_config::records_cache_config() :
		m_use_cache(true),
		m_cache_store_timeout(constants::CACHE_STORE_TIMEOUT),
		m_set_block_size(constants::CACHE_SENT_BLOCK_size),
		m_number_of_cached_blocks_in_memory(constants::NUMBER_OF_CACHED_BLOCKS_IN_MEMORY),
		m_cache_path(constants::CACHE_PATH)
	{
	}

	void records_cache_config::load(/*cfg*/)
	{
	}
}