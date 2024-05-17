#pragma once
#include"Pipeline.h"
#include"DirectXCommon.h"

class System{
private:
	Pipeline* pipeline_;
	DirectXCommon* dxCommon_ = nullptr;

public:
	System();
	~System();
	void Initialize();

};

