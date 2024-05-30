#include"ConvertString.h"
#include <windows.h>
#include <string>

void Log(const std::string& message){
	ConvertString(message);
	OutputDebugStringA(message.c_str());
}

std::wstring ConvertString(const std::string& str){
	if (str.empty()){
		return std::wstring();
	}

	int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast< int >(str.size()), NULL, 0);
	if (sizeNeeded <= 0){
		return std::wstring();
	}

	std::wstring result(sizeNeeded, 0);
	int convertedChars = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast< int >(str.size()), &result[0], sizeNeeded);
	if (convertedChars == 0){
		return std::wstring();
	}

	return result;
}

std::string ConvertString(const std::wstring& str){
	if (str.empty()){
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast< int >(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0){
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast< int >(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}

std::wstring ConvertString(const std::strong_ordering& str){
	if (str == std::strong_ordering::equal){
		return L"equal";
	} else if (str == std::strong_ordering::greater){
		return L"greater";
	} else{
		return L"less";
	}
}