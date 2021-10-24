#pragma once

#include <string>
#include <vector>

#include "json_helper.hpp"
#include "fs_helper.hpp"

namespace engine
{
	class cache_block final
	{
	public:
		cache_block(const fs::path& tmp_path,
			const fs::path& ready_path,
			const fs::path& del_path,
			bool in_memory = false);
		~cache_block();

		void append(const records_array_type& records);
		records_array_ptr_type read();

		void set_ready();
		void release();
		void set_failed();
		bool is_temp() const;
		bool is_released() const;
		bool is_ready() const;
		bool is_failed() const;
		size_t size() const;

	private:
		enum class cache_block_state
		{
			TEMP = 0,
			READY = 1,
			RELEASED = 2,
			FAILED = 3
		};

		fs::path m_tmp_path;
		fs::path m_ready_path;
		fs::path m_del_path;
		records_array_ptr_type m_records;
		size_t m_records_count;
		bool m_in_memory;
		cache_block_state m_state;
	};
}

