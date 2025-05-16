#include "FileScanner.h"

std::vector<std::filesystem::path> FileScanner::ScanFiles(const std::string& rootDir, const std::string& extensionFilter){
	std::vector<std::filesystem::path> result;

	namespace fs = std::filesystem;

	fs::path rootPath(rootDir);
	if (!fs::exists(rootPath) || !fs::is_directory(rootPath)){
		return result;
	}

	for (const auto& entry : fs::recursive_directory_iterator(rootPath)){
		if (entry.is_regular_file()){
			if (extensionFilter.empty() || entry.path().extension() == extensionFilter){
				result.push_back(entry.path());
			}
		}
	}

	return result;
}