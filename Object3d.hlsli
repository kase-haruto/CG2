
struct VertexShaderOutput{
	float4 position:SV_POSITION;
	float4 worldPosition:POSITION;//ワールド座標
	float2 texcoord:TEXCOORD;
};

//ワールド
cbuffer TransformationMatrix:register(b0){
	float4x4 world;

};

struct VertexShaderInput{
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
};

//マテリアル
struct Material{
	float4 color;
};
ConstantBuffer<Material>gMaterial : register(b0);

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
cbuffer CameraConstants : register(b2){
	float4x4 view;			//ワールド→ビュー変換行列
	float4x4 projection;	//ビュー→プロジェクション変換行列
	float3 cameraPos;		//カメラ座標(ワールド座標)
};