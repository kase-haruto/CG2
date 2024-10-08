
struct VertexShaderOutput{
	float4 position:SV_POSITION;
	float2 texcoord:TEXCOORD0;
	float4 color:COLOR0;
};

//ワールド
struct TransformationMatrix{
	float4x4 WVP;
	float4x4 world;
};

struct ParticleForGPU{
	float4x4 WVP;
	float4x4 world;
	float4 color;
};

struct VertexShaderInput{
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal:NORMAL0;
};

//マテリアル
struct Material{
	float4 color;
	int enableLighting;
	float4x4 uvTransform;
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
//struct CameraConstants{
//	float4x4 view;			//ワールド→ビュー変換行列
//	float4x4 projection;	//ビュー→プロジェクション変換行列
//	float3 cameraPos;		//カメラ座標(ワールド座標)
//};

//ライト
struct DirectionalLight{
	float4 color;		//ライトの色
	float3 direction;	//ライトの向き
	float intensity;	//輝度
};