
#include <functional>
#include <time.h>
#include <chrono>
#include <filesystem>
//#include <fstream>
#include <cstdio>

#include "fs_helper.hpp"

using namespace std::chrono;

namespace engine
{
	void append_to_file(const fs::path& file_path, const std::string& str)
	{
		FILE* outfile = fopen(file_path.string().c_str(), "a+b");
		if (!outfile)
		{
			throw std::runtime_error("Couldn't open file " + file_path.string());
		}
		fwrite(str.c_str(), sizeof(char), str.length(), outfile);
		fclose(outfile);
	}

	std::string read_from_file(const fs::path& file_path)
	{
		std::string str;
		char buffer[8 * 1024];

		FILE* infile = fopen(file_path.string().c_str(), "rb");
		if (!infile)
		{
			throw std::runtime_error("Couldn't open file " + file_path.string());
		}

		while (int bytes = fread(buffer,  sizeof(char), sizeof(buffer), infile))
		{
			str += std::string(buffer, bytes);
		}
		fclose(infile);
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

	void remove_all_from_dir(const fs::path& dir_path)
	{
		for (auto const& dir_entry : fs::directory_iterator(dir_path))
		{
			fs::remove_all(dir_entry.path());
		}
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
		std::time_t tt = system_clock::to_time_t(time);
		std::tm* ptm = std::localtime(&tt);
		char mbstr[100];
		std::strftime(mbstr, sizeof(mbstr), "%Y%m%d_%H%M%S", ptm);
		return mbstr;
	}

	system_clock::time_point timepoint_from_filename(const fs::path& file_name)
	{
		//TODO
		return system_clock::now();
	}
}