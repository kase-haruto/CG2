#pragma once

#include <vector>
#include <string>
#include <chrono>

struct ComboInput{
    std::string attackName;
    float maxInterval; // 次の入力までの最大許容時間（秒）
};

struct Combo{
    std::string name;
    std::vector<ComboInput> sequence;
};