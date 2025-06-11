#pragma once

/* ========================================================================
 *  include space
 * ===================================================================== */
#include <externals/nlohmann/json.hpp>

// メンバにコンフィグを適用するマクロ
#define CONFIG_APPLY_MEMBER(member) this->member = config.member;
// メンバをコンフィグに抽出するマクロ
#define CONFIG_EXTRACT_MEMBER(member) config.member = this->member;

#define NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_DERIVED(Type, Base, ...)              \
    inline void to_json(nlohmann::json& j, const Type& t) {                      \
        j = static_cast<const Base&>(t);                                         \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__)) \
    }                                                                            \
    inline void from_json(const nlohmann::json& j, Type& t) {                    \
        static_cast<Base&>(t) = j.get<Base>();                                   \
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM, __VA_ARGS__))\
    }