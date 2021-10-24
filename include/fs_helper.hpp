#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <filesystem>

namespace fs = std::filesystem;

namespace engine
{
	std::vector<fs::path> get_file_list(const fs::path& path);

	void append_to_file(const fs::path& file_path, const std::string& str);
	std::string read_from_file(const fs::path& file_path);

	fs::path file_move(const fs::path& file_path, const fs::path& dst_path);

	std::string get_file_name(const std::string& file_path);
	bool is_file_exists(const fs::path& file_path);

	fs::path filename_with_timepoint(const std::chrono::system_clock::time_point& time);
	std::chrono::system_clock::time_point timepoint_from_filename(const fs::path& file_name);
}
