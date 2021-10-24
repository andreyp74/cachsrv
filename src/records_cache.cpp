
#include "Poco/Logger.h"

#include "records_cache.hpp"
#include "fs_helper.hpp"
#include "json_helper.hpp"


using Poco::Logger;
using std::chrono::system_clock;

namespace fs = std::filesystem;

namespace engine
{
	records_cache::records_cache(std::shared_ptr<net::client> clptr) : m_clptr(clptr)
	{
		m_config.load();
		m_use_cache = m_config.use_cache();
		if (m_use_cache)
		{
			fs::path cache_path = m_config.cache_path();

			m_tmp_path = cache_path / constants::CACHE_TEMP_PATH;
			m_ready_path = cache_path / constants::CACHE_READY_PATH;
			m_del_path = cache_path / constants::CACHE_DEL_PATH;

			fs::create_directories(m_tmp_path);
			fs::create_directories(m_ready_path);
			fs::create_directories(m_del_path);

			read_block_list();
			if (!read_last_temp_block())
				m_cache_block = std::make_unique<cache_block>(m_tmp_path / filename_with_timepoint(system_clock::now()), 
					m_ready_path, m_del_path, true);

			m_service_thread = std::move(std::thread(&records_cache::thread_func, this));
		}
	}

	records_cache::~records_cache()
	{
		try
		{
			resume();
			m_done = true;
			if (m_service_thread.joinable())
			{
				m_service_thread.join();
			}
		}
		catch (std::exception& err)
		{
			Logger::get("cachesrv").error(err.what());
		}
	}

	void records_cache::resume()
	{
		m_ready_event.notify_all();
	}

	bool records_cache::can_resume()
	{
		return (m_cache_block && m_cache_block->size() > 0)
			|| !m_cache_block_list.empty()
			|| m_done;
	}

	bool records_cache::block_can_be_in_memory()
	{
		return m_cache_block_list.size() < m_config.number_of_cached_blocks_in_memory();
	}

	void records_cache::restore_not_processed(cache_block_list_type block_list)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		for (auto& block : block_list)
		{
			if (!block->is_released() && !block->is_failed())
				m_cache_block_list.push_back(block);
		}
	}

	bool records_cache::send_block(std::shared_ptr<cache_block> cache_block)
	{
		try
		{
			records_array_ptr_type records = cache_block->read();
			if (records)
			{
				auto msg = serialize(*records);
				if (do_send(msg))
				{
					cache_block->release();
					return true;
				}
			}
		}
		catch (const std::exception& err)
		{
			cache_block->set_failed();
			Logger::get("cachesrv").error(err.what());
		}
		return false;
	}

	void records_cache::thread_func()
	{
		while (!m_done)
		{
			try
			{
				std::vector<std::shared_ptr<cache_block>> block_list;

				{
					std::unique_lock<std::mutex> lock(m_mutex);

					while (!m_ready_event.wait_for(lock,
						m_config.cache_store_timeout_seconds(),
						std::bind(&records_cache::can_resume, this)));

					finalize_cache_block();
					block_list.swap(m_cache_block_list);
				}

				for (auto& cache_block : block_list)
				{
					send_block(cache_block);
				}

				restore_not_processed(block_list);
			}
			catch (std::exception& err)
			{
				Logger::get("cachesrv").error(err.what());
			}
		}
	}

	void records_cache::read_block_list()
	{
		auto file_list = get_file_list(m_ready_path);
		for (auto& file_path : file_list)
		{
			m_cache_block_list.emplace_back(std::make_shared<cache_block>(file_path, m_ready_path, m_del_path));
		}
	}

	bool records_cache::read_last_temp_block()
	{
		auto tmpfile_list = get_file_list(m_tmp_path);
		std::map<system_clock::time_point, fs::path> tmpFiles;
		for (auto& file_path : tmpfile_list)
		{
			tmpFiles.insert({ timepoint_from_filename(file_path), file_path });
		}
		if (!tmpFiles.empty())
		{
			m_cache_block = std::make_shared<cache_block>(tmpFiles.rbegin()->second, m_ready_path, m_del_path, true);
			return true;
		}
		return false;
	}

	void records_cache::finalize_cache_block()
	{
		if (!m_cache_block)
		{
			m_cache_block = std::make_shared<cache_block>(m_tmp_path / filename_with_timepoint(system_clock::now()), 
				m_ready_path, m_del_path, block_can_be_in_memory());
		}
		else
		{
			if (m_cache_block->size() > 0)
			{
				m_cache_block->set_ready();
				m_cache_block_list.push_back(m_cache_block);
				m_cache_block = std::make_shared<cache_block>(m_tmp_path / filename_with_timepoint(system_clock::now()), 
					m_ready_path, m_del_path, block_can_be_in_memory());
			}
		}
	}

	bool records_cache::send(const std::string& msg)
	{
		try
		{
			if (m_use_cache)
			{
				std::unique_lock<std::mutex> lock(m_mutex);

				auto records = deserialize(msg);
				if (!records)
				{
					return false;
				}
				m_cache_block->append(*records);
				if (m_cache_block->size() >= m_config.set_block_size())
				{
					lock.unlock();
					m_ready_event.notify_one();
				}
			}
			else
			{
				if (!do_send(msg))
					throw std::runtime_error("Couldn't send message");
			}
			return true;
		}
		catch (const std::exception& err)
		{
			Logger::get("cachesrv").error(err.what());
		}

		return false;
	}

	bool records_cache::do_send(const std::string& msg)
	{
		try
		{
			if (m_clptr)
			{
				m_clptr->send(msg);
				return true;
			}
		}
		catch (const std::exception& err)
		{
			Logger::get("cachesrv").error(err.what());
		}
		return false;
	}
}