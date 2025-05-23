#include "FileScanner.h"

/////////////////////////////////////////////////////////////////////////////////////////
//		ライル探索
/////////////////////////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////////////////////////
//		ファイルが存在しているか確認
/////////////////////////////////////////////////////////////////////////////////////////
bool FileScanner::Exists(const std::string& filePath) {
	return std::filesystem::exists(filePath);
}

/////////////////////////////////////////////////////////////////////////////////////////
//		拡張子チェック
/////////////////////////////////////////////////////////////////////////////////////////
bool FileScanner::HasExtension(const std::filesystem::path& path, const std::string& ext) {
	if (ext.empty()) return true;
	return path.extension() == ext;
}

/////////////////////////////////////////////////////////////////////////////////////////
//		ディレクトリの作成
/////////////////////////////////////////////////////////////////////////////////////////
bool FileScanner::EnsureDirectoryExists(const std::string& dirPath) {
	std::filesystem::path path(dirPath);
	if (std::filesystem::exists(path)) return true;
	return std::filesystem::create_directories(path);
}


/////////////////////////////////////////////////////////////////////////////////////////
//		ファイル名取得
/////////////////////////////////////////////////////////////////////////////////////////
std::string FileScanner::GetFileName(const std::filesystem::path& path) {
	return path.stem().string();
}
