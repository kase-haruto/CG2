#pragma once

// メンバにコンフィグを適用するマクロ
#define CONFIG_APPLY_MEMBER(member) this->member = config.member;
// メンバをコンフィグに抽出するマクロ
#define CONFIG_EXTRACT_MEMBER(member) config.member = this->member;