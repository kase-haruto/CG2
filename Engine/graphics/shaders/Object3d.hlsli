
struct VertexShaderOutput{
	float4 position:SV_POSITION;
	float2 texcoord:TEXCOORD0;
	float3 normal:NORMAL0;
	float3 worldPosition:POSITION0;
};

//ワールド
struct TransformationMatrix{
	float4x4 WVP;
	float4x4 world;
	float4x4 worldInverseTranspose;
};

struct VertexShaderInput{
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal:NORMAL0;
};

//マテリアル
struct Material{
	float4 color;			//色
	int enableLighting;		//ライティングの種類
	float4x4 uvTransform;	//uv座標
	float shiniess;		//光沢度
};

struct PixelShaderOutput{
	float4 color : SV_TARGET0;
};

// フォグのパラメータを定義
cbuffer FogConstants : register(b1){
	float fogStart;   // 霧の開始距離
	float fogEnd;     // 霧の終了距離
	float4 fogColor;  // 霧の色
};

//カメラのパラメータ
struct Camera{
	float3 worldPosition;	//カメラのワールド座標
};

//ライト
struct DirectionalLight{
	float4 color;		//ライトの色
	float3 direction;	//ライトの向き
	float intensity;	//輝度
};

struct PointLight{
	float4 color;		//ライトの色
	float3 position;	//ライトの位置
	float intensity;	//光度
};