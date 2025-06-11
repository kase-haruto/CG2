#pragma once
#include <Engine/Objects/3D/Actor/SceneObject.h>
#include <Engine/Objects/ConfigurableObject/IConfigurable.h>
#include <Engine/Foundation/Json/JsonUtils.h>


template<typename TConfig>
class ConfigurableObject
	: public IConfigurable {
public:
	//* apply/extract ========================================//
	void ApplyConfigFromJson(const nlohmann::json& j) override;
	void ExtractConfigToJson(nlohmann::json& j) const override;

	//* save/load ============================================//
	void LoadConfig(const std::string& path);
	void SaveConfig(const std::string& path) const;

	virtual void ApplyConfig() = 0;   ///< config_ → 実行状態へ反映
	virtual void ExtractConfig() = 0; ///< 実行状態 → config_ へ反映

protected:
	TConfig config_;
};

/////////////////////////////////////////////////////////////////////////////////////////
//      jsonからコンフィグを適用
/////////////////////////////////////////////////////////////////////////////////////////
template<typename TConfig>
inline void ConfigurableObject<TConfig>::ApplyConfigFromJson(const nlohmann::json& j){
	config_ = j.get<TConfig>();
	ApplyConfig();
}

/////////////////////////////////////////////////////////////////////////////////////////
//      コンフィグをjsonに変換
/////////////////////////////////////////////////////////////////////////////////////////
template<typename TConfig>
inline void ConfigurableObject<TConfig>::ExtractConfigToJson(nlohmann::json& j) const{
	const_cast<ConfigurableObject*>(this)->ExtractConfig();  // 状態を config_ に反映
	j = config_;
}

/////////////////////////////////////////////////////////////////////////////////////////
//      コンフィグのロード
/////////////////////////////////////////////////////////////////////////////////////////
template<typename TConfig>
inline void ConfigurableObject<TConfig>::LoadConfig(const std::string& path){
	nlohmann::json j;
	if (JsonUtils::Load(path, j)) ApplyConfigFromJson(j);
}

/////////////////////////////////////////////////////////////////////////////////////////
//      コンフィグのセーブ
/////////////////////////////////////////////////////////////////////////////////////////
template<typename TConfig>
inline void ConfigurableObject<TConfig>::SaveConfig(const std::string& path) const{
	const_cast< ConfigurableObject* >(this)->ExtractConfig(); // mutable化して状態を config_ にコピー
	nlohmann::json j;
	j = config_;
	JsonUtils::Save(path, j);
}