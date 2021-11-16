#include <filesystem>
#include <functional>

#include "Poco/Logger.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/JSONException.h"
#include "Poco/JSON/Handler.h"

#include "records_cache_block.hpp"
#include "fs_helper.hpp"

using namespace Poco::JSON;
using Poco::Logger;

namespace engine
{
	namespace
	{
		std::string unpack_from_array(const std::string& json_str)
		{
			std::string str(json_str);
			auto pos = str.find_first_of('[');
			if (pos != std::string::npos)
			{
				str.erase(pos, 1);
			}
			pos = str.find_last_of(']');
			if (pos != std::string::npos)
			{
				str.erase(pos, 1);
			}
			return str;
		}

		std::string pack_to_array(const std::string& dump)
		{
			return "[" + dump + "]";
		}
	}

	cache_block::cache_block(const fs::path& tmp_path,
							 const fs::path& ready_path,
							 const fs::path& del_path,
							 const fs::path& fail_path,
							 bool in_memory,
		                     bool remove_file,
							 cache_block_state state)
		: m_tmp_path(tmp_path),
		  m_ready_path(ready_path),
		  m_del_path(del_path),
		  m_fail_path(fail_path),
		  m_records_count(0),
		  m_in_memory(in_memory),
		  m_remove_file(remove_file),
		  m_state(state)
	{
		auto curr_path = get_path();
		if (m_in_memory && is_file_exists(curr_path))
		{
			std::string json_dump = read_from_file(curr_path);
			m_records = deserialize(pack_to_array(json_dump));
		}
		else
		{
			m_records = create_records_array_ptr();
		}
		m_records_count = m_records->size();
	}

	cache_block::~cache_block()
	{
		try
		{
			if (is_temp())
			{
				set_ready();
			}
			else if (is_released() && m_remove_file)
			{
				remove();
			}
		}
		catch (std::exception& err)
		{
			Logger::get("cachesrv").error(err.what());
		}
	}

	fs::path cache_block::get_path() const
	{
		if (m_state == cache_block_state::READY)
			return m_ready_path;
		else if (m_state == cache_block_state::FAILED)
			return m_fail_path;
		else if (m_state == cache_block_state::RELEASED)
			return m_del_path;
		return m_tmp_path;
	}

	void cache_block::append(const records_array_type& records)
	{
		if (is_temp() && records.size() != 0)
		{
			std::string json_dump;
			if (m_records_count > 0)
				json_dump += ",";
			json_dump += unpack_from_array(serialize(records));

			append_to_file(m_tmp_path, json_dump);

			if (m_in_memory)
			{
				for (auto& rec: records)
				{
					m_records->add(rec);
				}
			}
			m_records_count += records.size();
		}
	}

	records_array_ptr_type cache_block::read()
	{
		records_array_ptr_type records;
		if (is_ready() && is_file_exists(m_ready_path))
		{
			if (m_records->size() != 0)
			{
				records = m_records;
			}
			else
			{
				std::string json_dump = read_from_file(m_ready_path);
				records = deserialize(pack_to_array(json_dump));
			}
		}
		return records;
	}

	void cache_block::set_ready()
	{
		if (!is_temp())
		{
			Logger::get("cachesrv").information("set_ready: block is not in the 'Temporary' state: " + get_path().string());
			return;
		}
		if (!is_file_exists(m_tmp_path))
		{
			Logger::get("cachesrv").information("set_ready: file doesn't exist: " + m_tmp_path.string());
			return;
		}

		m_ready_path = file_move(m_tmp_path, m_ready_path);
		m_state = cache_block_state::READY;
	}

	void cache_block::set_failed()
	{
		if (!is_ready())
		{
			Logger::get("cachesrv").information("set_failed: block is not in the 'Ready' state: " + get_path().string());
			return;
		}
		if (!is_file_exists(m_ready_path))
		{
			Logger::get("cachesrv").information("set_failed: file doesn't exist: " + m_ready_path.string());
			return;
		}

		try
		{
			m_fail_path = file_move(m_ready_path, m_fail_path);
			m_state = cache_block_state::FAILED;
		}
		catch (std::exception& err)
		{
			Logger::get("cachesrv").error(err.what());
		}
	}

	void cache_block::release()
	{
		if (!is_ready())
		{
			Logger::get("cachesrv").information("release: block is not in the 'Ready' state: " + get_path().string());
			return;
		}
		if (!is_file_exists(m_ready_path))
		{
			Logger::get("cachesrv").information("release: file doesn't exist: " + m_ready_path.string());
			return;
		}

		m_del_path = file_move(m_ready_path, m_del_path);
		m_state = cache_block_state::RELEASED;
	}

	void cache_block::remove()
	{
		if (!is_released())
		{
			Logger::get("cachesrv").information("remove: block is not in the 'Released' state: " + get_path().string());
			return;
		}
		if (!is_file_exists(m_del_path))
		{
			Logger::get("cachesrv").information("remove: file doesn't exist: " + m_del_path.string());
			return;
		}
		fs::remove(m_del_path);
	}

	bool cache_block::is_released() const
	{
		return m_state == cache_block_state::RELEASED;
	}

	bool cache_block::is_ready() const
	{
		return m_state == cache_block_state::READY;
	}

	bool cache_block::is_failed() const
	{
		return m_state == cache_block_state::FAILED;
	}

	bool cache_block::is_temp() const
	{
		return m_state == cache_block_state::TEMP;
	}

	size_t cache_block::size() const
	{
		return m_records_count;
	}

}