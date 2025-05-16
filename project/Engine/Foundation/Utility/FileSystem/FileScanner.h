#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */

//c++
#include <string>
#include <vector>
#include <filesystem>

class FileScanner{

	//指定したディレクトリのファイルを検索
	static std::vector<std::filesystem::path> ScanFiles(const std::string& rootDir, const std::string& extensionFilter = "");
};

