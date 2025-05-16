#pragma once
/* ========================================================================
/* include space
/* ===================================================================== */

//c++
#include <string>
#include <externals/nlohmann/json.hpp>

class JsonFileLoader{
public:
	//===================================================================*/
	//				public functions
	//===================================================================*/
	JsonFileLoader(const std::string directoryPath = "Resources/Jsons/");
	JsonFileLoader() = default;
	~JsonFileLoader() = default;

	nlohmann::json Load(const std::string fileName);

private:
	//===================================================================*/
	//				private functions
	//===================================================================*/
	const std::string directoryPath_;
};

