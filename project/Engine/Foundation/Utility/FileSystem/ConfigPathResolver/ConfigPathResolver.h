#pragma once

#include <string>

class ConfigPathResolver {
public:
	// ベースディレクトリ
	static std::string GetBaseDirectory();

	// オブジェクト名とscene名からパスを生成
	static std::string ResolvePath(const std::string& objectType, const std::string& objectName);
};

