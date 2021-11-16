#pragma once

#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include "client.hpp"
#include "records_cache_config.hpp"
#include "records_cache_block.hpp"


namespace engine
{
	class records_cache final
	{
	public:
		using cache_block_list_type = std::vector<std::shared_ptr<cache_block>>;

		records_cache(records_cache_config config, std::shared_ptr<net::client_iface> clptr);
		~records_cache();

		bool send(const std::string& pack);
		void resume();
		void stop();

	private:
		void service_func();
		void clean_func();
		bool do_send(const std::string& pack);
		bool send_block(std::shared_ptr<cache_block>);

		bool can_resume();
		bool block_can_be_in_memory();
		void finalize_cache_block();

		void read_block_list();
		bool read_last_temp_block();
		void restore_not_processed(cache_block_list_type cache_block_list);

	private:
		records_cache_config m_config;

		std::shared_ptr<net::client_iface> m_clptr;

		std::shared_ptr<cache_block> m_cache_block;
		cache_block_list_type m_cache_block_list;
		fs::path m_tmp_path;
		fs::path m_ready_path;
		fs::path m_del_path;
		fs::path m_fail_path;

		std::thread m_service_thread;
		std::mutex m_service_mutex;
		std::thread m_clean_thread;
		std::mutex m_clean_mutex;
		std::condition_variable m_ready_event;
		std::condition_variable m_clean_event;
		std::atomic_bool m_done{ false };
		bool m_use_cache;
		bool m_use_retention;
	};
}