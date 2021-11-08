#include <gtest/gtest.h>
#include <filesystem>
#include <iostream>

#include "records_cache_block.hpp"
#include "records_cache_constants.hpp"
#include "fs_helper.hpp"
#include "json_helper.hpp"


using namespace engine;
namespace fs = std::filesystem;

namespace 
{
	constexpr auto json_array{R"json([
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
	}])json"};

	constexpr auto json_array2{ R"json([
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

class records_cache_block_test : public testing::Test {
protected:
	void SetUp()
	{
		m_cache_path = "./RecordsCacheBlockTests";

		m_tmp_path = m_cache_path / constants::CACHE_TEMP_PATH;
		m_ready_path = m_cache_path / constants::CACHE_READY_PATH;
		m_del_path = m_cache_path / constants::CACHE_DEL_PATH;
		m_fail_path = m_cache_path / constants::CACHE_FAILED_PATH;

		fs::create_directories(m_tmp_path);
		fs::create_directories(m_ready_path);
		fs::create_directories(m_del_path);
		fs::create_directories(m_fail_path);
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
};


TEST_F(records_cache_block_test, empty)
{
    try
    {
        cache_block cache_block(m_tmp_path / "EmptyRecordTest", m_ready_path, m_del_path, m_fail_path, false, true);

        ASSERT_FALSE(fs::exists(m_tmp_path / "EmptyRecordTest"));

		records_array_type records;
		cache_block.append(records);

		ASSERT_FALSE(fs::exists(m_tmp_path / L"EmptyRecordTest"));
    }
    catch (const std::exception& err)
    {
		std::cerr << err.what() << std::endl;
		ASSERT_FALSE(err.what());
    }
}

TEST_F(records_cache_block_test, set_ready)
{
	try
	{
		{
			cache_block cache_block(m_tmp_path / "SetReadyTest", m_ready_path, m_del_path, m_fail_path, false, true);

			auto records = deserialize(json_array);
			cache_block.append(*records);

			ASSERT_TRUE(fs::exists(m_tmp_path / "SetReadyTest"));
			ASSERT_FALSE(fs::exists(m_ready_path / "SetReadyTest"));

			//Not allowed to read from the unready file
			records_array_ptr_type read_records = cache_block.read();
			ASSERT_FALSE(read_records);

			ASSERT_TRUE(cache_block.is_temp());
			ASSERT_NO_THROW(cache_block.set_ready());
			ASSERT_TRUE(cache_block.is_ready());

			ASSERT_FALSE(fs::exists(m_tmp_path / "SetReadyTest"));
			ASSERT_TRUE(fs::exists(m_ready_path / "SetReadyTest"));

			//Allowed to read from the ready file
			records_array_ptr_type read_records2 = cache_block.read();
			ASSERT_TRUE(read_records2->size() > 0);
		}
		{
			cache_block cache_block(m_tmp_path / "SetReadyTest2", m_ready_path, m_del_path, m_fail_path, true, true);

			auto records = deserialize(json_array);
			cache_block.append(*records);

			ASSERT_TRUE(fs::exists(m_tmp_path / "SetReadyTest2"));
			ASSERT_FALSE(fs::exists(m_ready_path / "SetReadyTest2"));

			//Not allowed to read from the unready file
			records_array_ptr_type read_records = cache_block.read();
			ASSERT_FALSE(read_records);

			ASSERT_TRUE(cache_block.is_temp());
			ASSERT_NO_THROW(cache_block.set_ready());
			ASSERT_TRUE(cache_block.is_ready());

			ASSERT_FALSE(fs::exists(m_tmp_path / "SetReadyTest2"));
			ASSERT_TRUE(fs::exists(m_ready_path / "SetReadyTest2"));

			//Allowed to read from the ready file
			records_array_ptr_type read_records2 = cache_block.read();
			ASSERT_TRUE(read_records2->size() > 0);
		}
	}
	catch (const std::exception& err)
	{
		std::cerr << err.what() << std::endl;
		ASSERT_FALSE(err.what());
	}
}

TEST_F(records_cache_block_test, set_failed)
{
    try
    {
        {
            cache_block cache_block(m_tmp_path / "SetFailedTest", m_ready_path, m_del_path, m_fail_path, false, true);

			auto records = deserialize(json_array);
			cache_block.append(*records);

            ASSERT_TRUE(fs::exists(m_tmp_path / "SetFailedTest"));
            ASSERT_FALSE(fs::exists(m_fail_path / "SetFailedTest"));

			ASSERT_NO_THROW(cache_block.set_ready());
			ASSERT_NO_THROW(cache_block.set_failed());
            ASSERT_TRUE(cache_block.is_failed());

            ASSERT_FALSE(fs::exists(m_tmp_path / "SetFailedTest"));
            ASSERT_TRUE(fs::exists(m_fail_path / "SetFailedTest"));

            //Not allowed to read from the failed file
			auto read_records = cache_block.read();
			ASSERT_FALSE(read_records);
        }
        {
            cache_block cache_block(m_tmp_path / "SetFailedTest2", m_ready_path, m_del_path, m_fail_path, true, true);

			auto records = deserialize(json_array);
			cache_block.append(*records);

            ASSERT_TRUE(fs::exists(m_tmp_path / "SetFailedTest2"));
            ASSERT_FALSE(fs::exists(m_fail_path / "SetFailedTest2"));

			ASSERT_NO_THROW(cache_block.set_ready());
			ASSERT_NO_THROW(cache_block.set_failed());
            ASSERT_TRUE(cache_block.is_failed());

            ASSERT_FALSE(fs::exists(m_tmp_path / "SetFailedTest2"));
            ASSERT_TRUE(fs::exists(m_fail_path / "SetFailedTest2"));

            //Not allowed to read from the failed file
			auto read_records = cache_block.read();
			ASSERT_FALSE(read_records);
        }
    }
    catch (const std::exception& err)
    {
		std::cerr << err.what() << std::endl;
		ASSERT_FALSE(err.what());
    }
}

TEST_F(records_cache_block_test, release)
{
    try
    {
        {
            bool in_memory = false;
            bool remove_file_immediately = true;
            cache_block cache_block(m_tmp_path / "ReleaseTest", 
                m_ready_path, m_del_path, m_fail_path, in_memory, remove_file_immediately);

            auto records = deserialize(json_array);
			cache_block.append(*records);

            ASSERT_TRUE(fs::exists(m_tmp_path / "ReleaseTest"));
            ASSERT_FALSE(fs::exists(m_ready_path / "ReleaseTest"));
            ASSERT_FALSE(fs::exists(m_del_path / "ReleaseTest"));

            ASSERT_TRUE(cache_block.is_temp());
			ASSERT_NO_THROW(cache_block.set_ready());
			ASSERT_NO_THROW(cache_block.release());
            ASSERT_TRUE(cache_block.is_released());

            ASSERT_FALSE(fs::exists(m_tmp_path / "ReleaseTest"));
            ASSERT_FALSE(fs::exists(m_ready_path / "ReleaseTest"));
            ASSERT_TRUE(fs::exists(m_del_path / "ReleaseTest"));

            //Not allowed to read from the released file
			auto read_records = cache_block.read();
			ASSERT_FALSE(read_records);
        }
        {
            bool in_memory = true;
            bool remove_file_immediately = true;
            cache_block cache_block(m_tmp_path / "ReleaseTest2", 
                m_ready_path, m_del_path, m_fail_path, in_memory, remove_file_immediately);

			auto records = deserialize(json_array);
			cache_block.append(*records);

            ASSERT_TRUE(fs::exists(m_tmp_path / "ReleaseTest2"));
            ASSERT_FALSE(fs::exists(m_ready_path / "ReleaseTest2"));
            ASSERT_FALSE(fs::exists(m_del_path / "ReleaseTest2"));

            ASSERT_TRUE(cache_block.is_temp());
			ASSERT_NO_THROW(cache_block.set_ready());
			ASSERT_NO_THROW(cache_block.release());
            ASSERT_TRUE(cache_block.is_released());

            ASSERT_FALSE(fs::exists(m_tmp_path / "ReleaseTest2"));
            ASSERT_FALSE(fs::exists(m_ready_path / "ReleaseTest2"));
            ASSERT_TRUE(fs::exists(m_del_path / "ReleaseTest2"));

            //Not allowed to read from the released file
			auto read_records = cache_block.read();
			ASSERT_FALSE(read_records);
        }
    }
    catch (const std::exception& err)
    {
		std::cerr << err.what() << std::endl;
		ASSERT_FALSE(err.what());
    }
}

TEST_F(records_cache_block_test, remove)
{
    try
    {
        {
            bool in_memory = false;
            bool remove_file_immediately = true;
            cache_block cache_block(m_tmp_path / "RemoveTest", 
                m_ready_path, m_del_path, m_fail_path, in_memory, remove_file_immediately);

			auto records = deserialize(json_array);
			cache_block.append(*records);

            ASSERT_TRUE(fs::exists(m_tmp_path / "RemoveTest"));
            ASSERT_FALSE(fs::exists(m_ready_path / "RemoveTest"));
            ASSERT_FALSE(fs::exists(m_del_path / "RemoveTest"));

			ASSERT_NO_THROW(cache_block.set_ready());
			ASSERT_NO_THROW(cache_block.release());
			ASSERT_NO_THROW(cache_block.remove());

            ASSERT_FALSE(fs::exists(m_tmp_path / "RemoveTest"));
            ASSERT_FALSE(fs::exists(m_ready_path / "RemoveTest"));
            ASSERT_FALSE(fs::exists(m_del_path / "RemoveTest"));

            //Can not read from the removed file
			auto read_records = cache_block.read();
			ASSERT_FALSE(read_records);
        }
        {
            bool in_memory = true;
            bool remove_file_immediately = true;
            cache_block cache_block(m_tmp_path / "RemoveTest2", 
                m_ready_path, m_del_path, m_fail_path, in_memory, remove_file_immediately);

			auto records = deserialize(json_array);
			cache_block.append(*records);

            ASSERT_TRUE(fs::exists(m_tmp_path / "RemoveTest2"));
            ASSERT_FALSE(fs::exists(m_ready_path / "RemoveTest2"));
            ASSERT_FALSE(fs::exists(m_del_path / "RemoveTest2"));

			ASSERT_NO_THROW(cache_block.set_ready());
			ASSERT_NO_THROW(cache_block.release());
			ASSERT_NO_THROW(cache_block.remove());

            ASSERT_FALSE(fs::exists(m_tmp_path / "RemoveTest2"));
            ASSERT_FALSE(fs::exists(m_ready_path / "RemoveTest2"));
            ASSERT_FALSE(fs::exists(m_del_path / "RemoveTest2"));

            //Can not read from the removed file
            auto read_records = cache_block.read();
			ASSERT_FALSE(read_records);
        }
    }
    catch (const std::exception& err)
    {
		std::cerr << err.what() << std::endl;
		ASSERT_FALSE(err.what());
    }
}

TEST_F(records_cache_block_test, not_allowed_release_and_set_failed_for_unready)
{
    try
    {
        {
            bool in_memory = false;
            bool remove_file_immediately = true;
            cache_block cache_block(m_tmp_path / "NotAllowedReleaseAndSetFailedForUnready", 
                m_ready_path, m_del_path, m_fail_path, in_memory, remove_file_immediately);

			auto records = deserialize(json_array);
			cache_block.append(*records);

            ASSERT_TRUE(fs::exists(m_tmp_path / "NotAllowedReleaseAndSetFailedForUnready"));
            ASSERT_FALSE(fs::exists(m_ready_path / "NotAllowedReleaseAndSetFailedForUnready"));
            ASSERT_FALSE(fs::exists(m_del_path / "NotAllowedReleaseAndSetFailedForUnready"));

			ASSERT_NO_THROW(cache_block.set_failed());
            ASSERT_FALSE(cache_block.is_failed());

            ASSERT_TRUE(fs::exists(m_tmp_path / "NotAllowedReleaseAndSetFailedForUnready"));
            ASSERT_FALSE(fs::exists(m_ready_path / "NotAllowedReleaseAndSetFailedForUnready"));
            ASSERT_FALSE(fs::exists(m_del_path / "NotAllowedReleaseAndSetFailedForUnready"));

			ASSERT_NO_THROW(cache_block.release());
            ASSERT_FALSE(cache_block.is_released());

            ASSERT_TRUE(fs::exists(m_tmp_path / "NotAllowedReleaseAndSetFailedForUnready"));
            ASSERT_FALSE(fs::exists(m_ready_path / "NotAllowedReleaseAndSetFailedForUnready"));
            ASSERT_FALSE(fs::exists(m_del_path / "NotAllowedReleaseAndSetFailedForUnready"));
        }
        {
            bool in_memory = true;
            bool remove_file_immediately = true;
            cache_block cache_block(m_tmp_path / "NotAllowedReleaseAndSetFailedForUnready2", 
                m_ready_path, m_del_path, m_fail_path, in_memory, remove_file_immediately);

			auto records = deserialize(json_array);
			cache_block.append(*records);

            ASSERT_TRUE(fs::exists(m_tmp_path / "NotAllowedReleaseAndSetFailedForUnready2"));
            ASSERT_FALSE(fs::exists(m_ready_path / "NotAllowedReleaseAndSetFailedForUnready2"));
            ASSERT_FALSE(fs::exists(m_del_path / "NotAllowedReleaseAndSetFailedForUnready2"));

			ASSERT_NO_THROW(cache_block.set_failed());
            ASSERT_FALSE(cache_block.is_failed());

            ASSERT_TRUE(fs::exists(m_tmp_path / "NotAllowedReleaseAndSetFailedForUnready2"));
            ASSERT_FALSE(fs::exists(m_ready_path / "NotAllowedReleaseAndSetFailedForUnready2"));
            ASSERT_FALSE(fs::exists(m_del_path / "NotAllowedReleaseAndSetFailedForUnready2"));

			ASSERT_NO_THROW(cache_block.release());
            ASSERT_FALSE(cache_block.is_released());

            ASSERT_TRUE(fs::exists(m_tmp_path / "NotAllowedReleaseAndSetFailedForUnready2"));
            ASSERT_FALSE(fs::exists(m_ready_path / "NotAllowedReleaseAndSetFailedForUnready2"));
            ASSERT_FALSE(fs::exists(m_del_path / "NotAllowedReleaseAndSetFailedForUnready2"));
        }
    }
    catch (const std::exception& err)
    {
		std::cerr << err.what() << std::endl;
		ASSERT_FALSE(err.what());
    }
}

TEST_F(records_cache_block_test, append_read)
{
    try
    {
        {
            bool in_memory = false;
            bool remove_file_immediately = true;
            cache_block cache_block(m_tmp_path / "AppendReadTest", 
                m_ready_path, m_del_path, m_fail_path, in_memory, remove_file_immediately);

			auto records1 = deserialize(json_array);
            ASSERT_EQ(records1->size(), 2ULL);
            cache_block.append(*records1);
            ASSERT_EQ(cache_block.size(), 2ULL);

			auto records2 = deserialize(json_array2);
            ASSERT_EQ(records2->size(), 2ULL);
            cache_block.append(*records2);
            ASSERT_EQ(cache_block.size(), 4ULL);

			ASSERT_NO_THROW(cache_block.set_ready());
            ASSERT_TRUE(cache_block.is_ready());

            auto read_records = cache_block.read();
            ASSERT_EQ(read_records->size(), 4ULL);

			records_array_type total_written;
			for (auto& r1 : *records1)
			{
				total_written.add(r1);
			}
			for (auto& r2 : *records2)
			{
				total_written.add(r2);
			}
            ASSERT_EQ(serialize(*read_records), serialize(total_written));
        }
        {
            bool in_memory = true;
            bool remove_file_immediately = true;
            cache_block cache_block(m_tmp_path / "AppendReadTest2", 
                m_ready_path, m_del_path, m_fail_path, in_memory, remove_file_immediately);

			auto records1 = deserialize(json_array);
			ASSERT_EQ(records1->size(), 2ULL);
            cache_block.append(*records1);
            ASSERT_EQ(cache_block.size(), 2ULL);

			auto records2 = deserialize(json_array2);
			ASSERT_EQ(records2->size(), 2ULL);
			cache_block.append(*records2);
			ASSERT_EQ(cache_block.size(), 4ULL);

			ASSERT_NO_THROW(cache_block.set_ready());
            ASSERT_TRUE(cache_block.is_ready());

            auto read_records = cache_block.read();
            ASSERT_EQ(read_records->size(), 4ULL);

			records_array_type total_written;
			for (auto& r1 : *records1)
			{
				total_written.add(r1);
			}
			for (auto& r2 : *records2)
			{
				total_written.add(r2);
			}
			ASSERT_EQ(serialize(*read_records), serialize(total_written));
        }
    }
    catch (const std::exception& err)
    {
		std::cerr << err.what() << std::endl;
		ASSERT_FALSE(err.what());
    }
}

