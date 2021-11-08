#include <gtest/gtest.h>
#include <filesystem>
#include <future>

#include "client_iface.hpp"
#include "records_cache.hpp"
#include "records_cache_constants.hpp"
#include "fs_helper.hpp"
#include "json_helper.hpp"

using namespace engine;
using namespace std::literals;
namespace fs = std::filesystem;

namespace
{
	constexpr auto json_array{ R"json([
	{
		"id": "0",
		"string_value": "zero",
		"integer_value": 0,
		"bool_value": true
	},
	{
		"id": "1",
		"string_value": "first",
		"integer_value": 1,
		"bool_value": false
	},
	{
		"id": "2",
		"string_value": "second",
		"integer_value": 2,
		"bool_value": true
	},
	{
		"id": "3",
		"string_value": "third",
		"integer_value": 3,
		"bool_value": false
	}])json" };
}

class client_mock final: public net::client_iface
{
public:
	client_mock()
	{

	}

	~client_mock()
	{

	}

	std::string receive() override
	{
		return "";
	}

	bool send(const std::string& packet) override
	{
		bool ret;
		if (m_error_mode)
			ret = false;
		else
			ret = true;

		m_send_result->set_value(ret);
		return ret;
	}

	void set_error_mode(bool error_mode)
	{
		m_error_mode = error_mode;
	}

	std::future<bool> get_send_result()
	{
		m_send_result = std::make_shared<std::promise<bool>>();
		return m_send_result->get_future();
	}

private:
	client_mock(client_mock&) = delete;
	client_mock& operator=(client_mock&) = delete;

	std::shared_ptr<std::promise<bool>> m_send_result;
	bool m_error_mode{ false };
};

class records_cache_test : public testing::Test {
protected:
	void SetUp()
	{
		m_cache_path = "./RecordsCacheTests";

		m_tmp_path = m_cache_path / constants::CACHE_TEMP_PATH;
		m_ready_path = m_cache_path / constants::CACHE_READY_PATH;
		m_del_path = m_cache_path / constants::CACHE_DEL_PATH;
		m_fail_path = m_cache_path / constants::CACHE_FAILED_PATH;

		fs::create_directories(m_tmp_path);
		fs::create_directories(m_ready_path);
		fs::create_directories(m_del_path);
		fs::create_directories(m_fail_path);

		m_config.cache_path(m_cache_path.string());
		m_config.cache_retention_timeout_seconds(0s);
		m_config.cache_store_timeout_seconds(10s);
		m_config.number_of_cached_blocks_in_memory(10);
		m_config.set_block_size(10);
		m_config.use_cache(true);
		m_config.use_retention(true);
	}

	void TearDown()
	{
		fs::remove_all(m_cache_path);
	}

	fs::path m_cache_path;
	fs::path m_tmp_path;
	fs::path m_ready_path;
	fs::path m_del_path;
	fs::path m_fail_path;

	records_cache_config m_config;
};

TEST_F(records_cache_test, without_cache)
{
    try
    {
		records_cache_config config;
		config.use_cache(false);
		config.cache_path(m_cache_path.string());
		config.cache_retention_timeout_seconds(0s);

        auto client = std::make_shared<client_mock>();
        records_cache cache(config, client);

		auto records = deserialize(json_array);
		ASSERT_TRUE(records->size() > 0);

        auto future = client->get_send_result();

        ASSERT_TRUE(cache.send(serialize(*records)));

        ASSERT_EQ(future.get(), true);
		cache.stop();
    }
	catch (const std::exception& err)
	{
		std::cerr << err.what() << std::endl;
		ASSERT_FALSE(err.what());
	}
}

TEST_F(records_cache_test, send_empty)
{
    try
    {
		auto client = std::make_shared<client_mock>();
		records_cache cache(m_config, client);

        engine::records_array_type records;
		ASSERT_TRUE(records.size() == 0);
        ASSERT_TRUE(cache.send(serialize(records)));

        cache.stop();
    }
	catch (const std::exception& err)
	{
		std::cerr << err.what() << std::endl;
		ASSERT_FALSE(err.what());
	}
}

TEST_F(records_cache_test, send)
{
    try
    {
		records_cache_config config;
		config.use_cache(true);
		config.set_block_size(4);
		config.cache_path(m_cache_path.string());
		config.cache_retention_timeout_seconds(0s);

		auto client = std::make_shared<client_mock>();
		records_cache cache(config, client);

		auto records = deserialize(json_array);
		ASSERT_TRUE(records->size() == 4ULL);

		auto future = client->get_send_result();

		ASSERT_TRUE(cache.send(serialize(*records)));
		cache.stop();

		ASSERT_EQ(future.get(), true);
    }
	catch (const std::exception& err)
	{
		std::cerr << err.what() << std::endl;
		ASSERT_FALSE(err.what());
	}
}

TEST_F(records_cache_test, send_at_first_with_error)
{
    try
    {
		records_cache_config config;
		config.use_cache(true);
		config.set_block_size(4);
		config.cache_path(m_cache_path.string());
		config.cache_retention_timeout_seconds(0s);

		auto client = std::make_shared<client_mock>();
		records_cache cache(config, client);

		auto records = deserialize(json_array);
		ASSERT_TRUE(records->size() == 4ULL);

		client->set_error_mode(true);

		auto future = client->get_send_result();

		ASSERT_TRUE(cache.send(serialize(*records)));
		cache.resume();

		ASSERT_EQ(future.get(), false);

		client->set_error_mode(false);

		auto future2 = client->get_send_result();

		ASSERT_TRUE(cache.send(serialize(*records)));

		cache.resume();
		ASSERT_EQ(future2.get(), true);

		cache.stop();
    }
	catch (const std::exception& err)
	{
		std::cerr << err.what() << std::endl;
		ASSERT_FALSE(err.what());
	}
}

TEST_F(records_cache_test, send_at_first_with_error_2)
{
    try
    {
		records_cache_config config;
		config.use_cache(true);
		config.set_block_size(4);
		config.cache_path(m_cache_path.string());
		config.cache_retention_timeout_seconds(0s);

        {
			auto client = std::make_shared<client_mock>();
			records_cache cache(config, client);

			auto records = deserialize(json_array);
			ASSERT_TRUE(records->size() == 4ULL);

			client->set_error_mode(true);

			auto future = client->get_send_result();

			ASSERT_TRUE(cache.send(serialize(*records)));

			cache.resume();
			ASSERT_EQ(future.get(), false);
			cache.stop();
        }
        {
			auto client = std::make_shared<client_mock>();
			records_cache cache(config, client);

			client->set_error_mode(false);

			auto future = client->get_send_result();

			cache.resume();
			ASSERT_EQ(future.get(), true);
			cache.stop();
        }
    } 
	catch (const std::exception& err)
	{
		std::cerr << err.what() << std::endl;
		ASSERT_FALSE(err.what());
	}
}


