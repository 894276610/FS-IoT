#ifndef _GROUNDNUT_FILE_UTILS_H
#define _GROUNDNUT_FILE_UTILS_H

#include <vector>
#include <filesystem>

namespace groundnut{

inline bool IsFileInSuffix(const std::filesystem::directory_entry& dirEntry, std::initializer_list<std::string> suffixes)
{
	if (!dirEntry.is_regular_file())
		return false;

	std::filesystem::path path = dirEntry.path();
	std::string extension = path.extension().string();

	for (auto& suffix : suffixes)
	{
		if (extension == suffix)
			return true;
	}
	
	return false;
}

inline std::vector<std::filesystem::path> GetFilesInSuffix(std::filesystem::path folder,
	std::initializer_list<std::string> args)
{
	std::vector<std::filesystem::path> files;

	if (!std::filesystem::exists(folder) || !std::filesystem::is_directory(folder))
	{
		return std::vector<std::filesystem::path>();
	}

	for (const std::filesystem::directory_entry& dirEntry : std::filesystem::recursive_directory_iterator(folder))
	{
		if (!IsFileInSuffix(dirEntry, args))
		{
			continue;
		}

		files.push_back(dirEntry.path());
	}

	return files;
}



}
#endif