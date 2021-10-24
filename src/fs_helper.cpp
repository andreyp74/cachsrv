
#include <functional>
#include <time.h>
#include <chrono>
#include <filesystem>
#include <fstream>

#include "fs_helper.hpp"

using namespace std::chrono;

namespace engine
{
	void append_to_file(const fs::path& file_path, const std::string& str)
	{
		std::ofstream outfile(file_path, std::ios_base::app | std::ios_base::binary | std::ios_base::out);
		if (!outfile.is_open())
		{
			throw std::runtime_error("Couldn't open file " + file_path.string());
		}

		outfile.write(str.c_str(), str.length());
		outfile.close();
	}

	std::string read_from_file(const fs::path& file_path)
	{
		std::string str;
		char buffer[8 * 1024];

		std::ifstream infile(file_path, std::ios_base::binary | std::ios_base::in);
		if (!infile.is_open())
		{
			throw std::runtime_error("Couldn't open file " + file_path.string());
		}

		while (infile.read((char*)&buffer, sizeof(buffer)))
		{
			str += std::string(buffer);
		}
		infile.close();
		return str;
	}

	std::vector<fs::path> get_file_list(const fs::path& path)
	{
		std::vector<fs::path> file_list;
		for (auto const& dir_entry : fs::directory_iterator(path))
		{
			if (dir_entry.is_regular_file())
			{
				file_list.emplace_back(dir_entry.path());
			}
		}
		return file_list;
	}

	fs::path file_move(const fs::path& file_path, const fs::path& dst_path)
	{
		auto file_name = file_path.filename();
		auto newfile_path = dst_path / file_name;
		fs::rename(file_path, newfile_path);
		return newfile_path;
	}

	bool is_file_exists(const fs::path& file_path)
	{
		return fs::exists(file_path) && !fs::is_directory(file_path);
	}

	std::string get_file_name(const std::string& file_path)
	{
		return fs::path(file_path).filename().string();
	}

	fs::path filename_with_timepoint(const system_clock::time_point& time)
	{
		//TODO
		/*std::time_t tt = system_clock::to_time_t(time);
		std::tm* ptm = std::localtime(&tt);
		char mbstr[100];
		std::strftime(mbstr, sizeof(mbstr), "%Y%m%d_%H%M%S", ptm);*/

		return "123456";
	}

	system_clock::time_point timepoint_from_filename(const fs::path& file_name)
	{
		//TODO
		return system_clock::now();
	}
}