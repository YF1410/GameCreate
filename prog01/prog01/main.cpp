#include "WinApp.h"
#include "DirectXCommon.h"
#include "Input.h"

#include <vector>
#include <string>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#pragma comment(lib, "dinput8.lib")

#include <DirectXTex.h>
#define _RPT0(rptno, msg)      _RPTN(rptno, "%s", msg)

using namespace DirectX;
using namespace Microsoft::WRL;

#include <time.h>

#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")
#include <fstream>

struct ConstBufferData
{
	XMFLOAT4 color;    //色(RGBA)
	XMMATRIX mat;      //3D変換行列
};

//3Dオブジェクト型
struct Object3d
{
	//定数バッファ
	ComPtr<ID3D12Resource> constBuff;
	//定数バッファビューのハンドル(CPU)
	D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandleCBV;
	//定数バッファビューのハンドル(GPU)
	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleCBV;
	//アフィン変換情報
	XMFLOAT3 scale = { 1, 1, 1 };     //スケーリング倍率
	XMFLOAT3 rotation = { 0, 0, 0 };  //回転角
	XMFLOAT3 position = { 0, 0, 0 };  //座標
#pragma ワールド変換
	//ワールド変換行列
	XMMATRIX matWorld;
	//親オブジェクトへのポインタ
	Object3d* parent = nullptr;
};

struct PipelineSet
{
	//パイプラインステートオブジェクト
	ComPtr<ID3D12PipelineState> pipelinestate;
	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootsignature;
};

//頂点データ構造体
struct VertexPosUv
{
	XMFLOAT3 pos;     //xyz座標
#pragma テクスチャUV
	XMFLOAT2 uv;      //uv座標
};

//スプライト1枚分のデータ
struct Sprite
{
	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//定数バッファ
	ComPtr<ID3D12Resource> constBuff;
	//z軸回りの回転角
	float rotation = 0.0f;
	//座標
	XMFLOAT3 position = { 0, 0, 0 };
	//ワールド行列
	XMMATRIX matWorld;
	//色(RGBA)
	XMFLOAT4 color = { 1, 1, 1, 1 };
	//テクスチャ番号
	UINT texNumber = 0;
	//大きさ
	XMFLOAT2 size = { 100, 100 };
	//アンカーポイント
	XMFLOAT2 anchorpoint = { 0.5f, 0.5f };
	//左右反転
	bool isFlipX = false;
	//上下反転
	bool isFlipY = false;
	//テクスチャ左上座標
	XMFLOAT2 texLeftTop = { 0, 0 };
	//テクスチャ切り出しサイズ
	XMFLOAT2 texSize = { 1280, 720 };
	//非表示
	bool isInvisible = false;
};

//テクスチャの最大枚数
const int spriteSRVCount = 512;

//スプライトの共通データ
struct SpriteCommon
{
	//パイプラインセット
	PipelineSet pipelineSet;
#pragma 射影変換
	//射影行列(透視投影)
	XMMATRIX matProjection{};
	//テクスチャ用のデスクリプタヒープの生成
	ComPtr<ID3D12DescriptorHeap> descHeap;
	//テスクチャリソース(テスクチャバッファ)の配列
	ComPtr<ID3D12Resource> texBuff[spriteSRVCount];
};

//デバック文字列クラスの定義
class DebugText
{
public:
	//ここに定数の宣言を記述
	static const int maxCharCount = 256;  //1フレームでの最大表示文字数
	static const int fontWidth = 9;  //フォント画像内1文字分の横幅
	static const int fontHeight = 18;  //フォント画像内1文字分の縦幅
	static const int fontLineCount = 14;  //フォント画像内1行分の文字数

	//ここにメンバ関数の宣言を記述
	void Initialize(ID3D12Device* dev, int window_width, int window_height, UINT texNumber,
		SpriteCommon& spriteCommon);
	void Print(const SpriteCommon& spriteCommon, const std::string& text, float x, float y, float scale);
	void DrawAll(ID3D12GraphicsCommandList* cmdList, const SpriteCommon& spriteCommon, ID3D12Device* dev);

private:
	//ここにメンバ変数の宣言を記述
	//スプライトの配列
	Sprite sprites[maxCharCount];
	//スプライト配列の添え字番号
	int spriteIndex = 0;
};

//　チャンクヘッダ
struct ChunkHeader
{
	char id[4];  //チャンク毎のID
	int32_t size;  //チャンクサイズ
};
//RIFFヘッダチャンク
struct RiffHeader
{
	ChunkHeader chunk;  //"RIFF"
	char type[4];  //"WAVE"
};
//FMTチャンク
struct FormatChunk
{
	ChunkHeader chunk;  //"fmt"
	WAVEFORMATEX fmt;  //波形フォーマット
};

class XAudio2VoiceCallback : public IXAudio2VoiceCallback
{
public:
	// ボイス処理パスの開始時
	STDMETHOD_(void, OnVoiceProcessingPassStart) (THIS_ UINT32 BytesRequired) {};
	// ボイス処理パスの終了時
	STDMETHOD_(void, OnVoiceProcessingPassEnd) (THIS) {};
	// バッファストリームの再生が終了した時
	STDMETHOD_(void, OnStreamEnd) (THIS) {};
	// バッファの使用開始時
	STDMETHOD_(void, OnBufferStart) (THIS_ void* pBufferContext) {};
	// バッファの末尾に達した時
	STDMETHOD_(void, OnBufferEnd) (THIS_ void* pBufferContext) {};
	// 再生がループ位置に達した時
	STDMETHOD_(void, OnLoopEnd) (THIS_ void* pBufferContext) {};
	// ボイスの実行エラー時
	STDMETHOD_(void, OnVoiceError) (THIS_ void* pBufferContext, HRESULT Error) {};
};

//音声データ
struct SoundData
{
	//波形フォーマット
	WAVEFORMATEX wfex;
	//バッファの先頭アドレス
	BYTE* pBuffer;
	//バッファのサイズ
	unsigned int bufferSize;
};

void InitializeObject3d(Object3d* object, int index, ID3D12Device* dev,
	ID3D12DescriptorHeap* descHeap);
void UpdateObject3d(Object3d* object, XMMATRIX& matView, XMMATRIX& matProjection);
void DrawObject3d(Object3d* object, ID3D12GraphicsCommandList* cmdList,
	ID3D12DescriptorHeap* descHeap, D3D12_VERTEX_BUFFER_VIEW& vbView,
	D3D12_INDEX_BUFFER_VIEW& ibView, D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV,
	UINT numIndices);
PipelineSet Object3dCreateGraphicsPipeline(ID3D12Device* dev);
PipelineSet spriteCreateGraphicsPipeline(ID3D12Device* dev);
Sprite SpriteCreate(ID3D12Device* dev, int window_width, int window_height, UINT texNumber,
	SpriteCommon& spriteCommon, XMFLOAT2 anchorpoint,
	bool isFlipX, bool isFlipY);
void SpriteCommonBeginDraw(const SpriteCommon& spriteCommon, ID3D12GraphicsCommandList* cmdList);
void SpriteDraw(const Sprite& sprite, ID3D12GraphicsCommandList* cmdList, const SpriteCommon& spriteCommon,
	ID3D12Device* dev);
SpriteCommon SpriteCommonCreate(ID3D12Device* dev, int window_width, int window_height);
void SpriteUpdate(Sprite& sprite, const SpriteCommon& spriteCommon);
void SpriteCommonLoadTexture(SpriteCommon& spriteCommon,
	UINT texnumber, const wchar_t* filename, ID3D12Device* dev);
void SpriteTransferVertexBuffer(const Sprite& sprite, const SpriteCommon& spriteCommon, bool texSizeFlag);
SoundData SoundLoadWave(const char* filename);
void SoundUnload(SoundData* soundData);
void SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData);

//# Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	HRESULT result;
	ComPtr<ID3D12Device> dev;
	ComPtr<ID3D12GraphicsCommandList> cmdList;


#pragma region WindowsAPI初期化
	WinApp* win = nullptr;
	win = new WinApp();
	win->CreateGameWindow();
#pragma endregion WindowsAPI初期化

#pragma region DirectX初期化処理
	DirectXCommon* dxCommon = nullptr;
	dxCommon = new DirectXCommon();
	dxCommon->Init(win);

	Input* input = nullptr;
	input = new Input();
	input->Init(win->GetInstance(), win->GetHwnd());

	//スプライト
	SpriteCommon spriteCommon;
	const int SPRITES_NUM = 1;
	Sprite sprites[SPRITES_NUM];

	dxCommon->InitDXGIDevice();
	dxCommon->InitCommand();
	//dxCommon->CreateSwapChain();
	dxCommon->CreateFinalRenderTargets();
	dxCommon->ClearDepthBuffer();
	dxCommon->CreateFence();

#pragma デスクリプタヒープ

	//サウンド
	ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;
	XAudio2VoiceCallback voiceCallback;

	//XAudioエンジンのインスタンスを生成
	result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));
	//マスターボイスを生成
	result = xAudio2->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(result));

	//音声読み込み
	//SoundData soundData1 = SoundLoadWave("Resources/Alarm01.wav");

	//音声再生
	//SoundPlayWave(xAudio2.Get(), soundData1);

#pragma endregion DirectX初期化処理

#pragma region 描画初期化処理

	//XMFLOAT3 vertices[] = {
	//{ -0.5f, -0.5f, 0.0f}, // 左下  //インデックス0
	//{ -0.5f, +0.5f, 0.0f}, // 左上  //インデックス1
	//{ +0.5f, -0.5f, 0.0f}, // 右下  //インデックス2
	//{ +0.5f, +0.5f, 0.0f},
	//{ +0.5f, -0.5f, 0.0f},
	//{ -0.5f, +0.5f, 0.0f},
	//};

	//XMFLOAT3 vertices[] = {
	//{ +2.0f, +2.0f, 0.0f}, // 左下  //インデックス0
	//{ +2.0f, -2.0f, 0.0f}, // 左上  //インデックス1
	//{ +0.7f, +2.0f, 0.0f}, // 右下  //インデックス2
	//{ +0.7f, -2.0f, 0.0f}, // 右上  //インデックス3
	//};
	//
	//unsigned short indices[] =
	//{
	//	0, 1, 2,  //三角形1つ目
	//	1, 2, 3,  //三角形2つ目
	//};
	//
	//XMFLOAT3 vertices2[] = {
	//{ -2.0f, -2.0f, 0.0f}, // 左下  //インデックス0
	//{ -2.0f, +2.0f, 0.0f}, // 左上  //インデックス1
	//{ -0.7f, -2.0f, 0.0f}, // 右下  //インデックス2
	//{ -0.7f, +2.0f, 0.0f}, // 右上  //インデックス3
	//};
	//
	//unsigned short indices2[] =
	//{
	//	0, 1, 2,  //三角形1つ目
	//	1, 2, 3,  //三角形2つ目
	//};

	////正三角形
	////頂点配列の宣言（要素数3 : 三角形だから3）
	//XMFLOAT3 vertices[3];
	//vertices[0] =
	//{
	//	1 * cosf((XM_PI * 2) / 3 * 1 + XM_PI / 2),
	//	1 * sinf((XM_PI * 2) / 3 * 1 + XM_PI / 2),
	//	0,
	//};
	//vertices[1] =
	//{
	//	1 * cosf((XM_PI * 2) / 3 * 2 + XM_PI / 2),
	//	1 * sinf((XM_PI * 2) / 3 * 2 + XM_PI / 2),
	//	0,
	//};
	//vertices[2] =
	//{
	//	1 * cosf((XM_PI * 2) / 3 * 3 + XM_PI / 2),
	//	1 * sinf((XM_PI * 2) / 3 * 3 + XM_PI / 2),
	//	0,
	//};
	////インデックス配列の宣言（要素数6 : 頂点数*2）
	//unsigned short indices[3 * 2];
	//indices[0] = 0;
	//indices[1] = 1;
	//indices[2] = 1;
	//indices[3] = 2;
	//indices[4] = 2;
	//indices[5] = 0;

	////正多角形
	////頂点配列の宣言（要素数n : n角形だからn）
	//const int DIV = 5;
	//XMFLOAT3 vertices[DIV];
	//
	//for (int i = 0; i < DIV; i++)
	//{
	//	vertices[i] =
	//	{
	//		1 * sinf((XM_PI * 2) / DIV * i),
	//		1 * cosf((XM_PI * 2) / DIV * i),
	//		0,
	//	};
	//}
	//
	////インデックス配列の宣言（要素数n*2 : 頂点数*2）
	//unsigned short indices[DIV * 2];
	//for (int i = 0; i < DIV; i++)
	//{
	//	indices[i * 2 + 0] = i;
	//	indices[i * 2 + 1] = i + 1;
	//}
	//indices[9] = 0;

	////正多角形(ポリゴン)
	////頂点配列の宣言（要素数n + 1 : n角形だからn + 中心点 1）
	//const int DIV = 100;
	//XMFLOAT3 vertices[DIV + 1];
	//
	//for (int i = 0; i < DIV; i++)
	//{
	//	vertices[i] =
	//	{
	//		0.5f * sinf((XM_PI * 2) / DIV * i),
	//		0.5f * cosf((XM_PI * 2) / DIV * i),
	//		0,
	//	};
	//}
	//vertices[DIV] =
	//{
	//	0,0,0,
	//};
	//
	////インデックス配列の宣言（要素数n*3 : 頂点数*3）
	//unsigned short indices[DIV * 3];
	//for (int i = 0; i < DIV; i++)
	//{
	//	indices[i * 3 + 0] = i;
	//	indices[i * 3 + 1] = i + 1;
	//	indices[i * 3 + 2] = DIV;
	//}
	//indices[DIV * 3 - 1] = 0;

//頂点データ構造体
	struct Vertex
	{
		XMFLOAT3 pos;     //xyz座標
		XMFLOAT3 normal;  //法線ベクトル
#pragma テクスチャUV
		XMFLOAT2 uv;      //uv座標
	};

	//頂点データ
	Vertex vertices[] =
	{
		//  x      y      z       u     v
		//前
		{{ -5.0f, -5.0f, -5.0f}, {}, {0.0f, 1.0f}}, // 左下
		{{ -5.0f,  5.0f, -5.0f}, {}, {0.0f, 0.0f}}, // 左上
		{{  5.0f, -5.0f, -5.0f}, {}, {1.0f, 1.0f}}, // 右下
		{{  5.0f,  5.0f, -5.0f}, {}, {1.0f, 0.0f}}, // 右上
		//後
		{{  5.0f, -5.0f,  5.0f}, {}, {0.0f, 1.0f}}, // 左下
		{{  5.0f,  5.0f,  5.0f}, {}, {0.0f, 0.0f}}, // 左上
		{{ -5.0f, -5.0f,  5.0f}, {}, {1.0f, 1.0f}}, // 右下
		{{ -5.0f,  5.0f,  5.0f}, {}, {1.0f, 0.0f}}, // 右上
		//左
		{{ -5.0f, -5.0f, -5.0f}, {}, {0.0f, 1.0f}}, // 左下
		{{ -5.0f, -5.0f,  5.0f}, {}, {0.0f, 0.0f}}, // 左上
		{{ -5.0f,  5.0f, -5.0f}, {}, {1.0f, 1.0f}}, // 右下
		{{ -5.0f,  5.0f,  5.0f}, {}, {1.0f, 0.0f}}, // 右上
		//右
		{{  5.0f,  5.0f, -5.0f}, {}, {0.0f, 1.0f}}, // 左下
		{{  5.0f,  5.0f,  5.0f}, {}, {0.0f, 0.0f}}, // 左上
		{{  5.0f, -5.0f, -5.0f}, {}, {1.0f, 1.0f}}, // 右下
		{{  5.0f, -5.0f,  5.0f}, {}, {1.0f, 0.0f}}, // 右上
		//下
		{{  5.0f, -5.0f, -5.0f}, {}, {0.0f, 1.0f}}, // 左下
		{{  5.0f, -5.0f,  5.0f}, {}, {0.0f, 0.0f}}, // 左上
		{{ -5.0f, -5.0f, -5.0f}, {}, {1.0f, 1.0f}}, // 右下
		{{ -5.0f, -5.0f,  5.0f}, {}, {1.0f, 0.0f}}, // 右上
		//上
		{{ -5.0f,  5.0f, -5.0f}, {}, {0.0f, 1.0f}}, // 左下
		{{ -5.0f,  5.0f,  5.0f}, {}, {0.0f, 0.0f}}, // 左上
		{{  5.0f,  5.0f, -5.0f}, {}, {1.0f, 1.0f}}, // 右下
		{{  5.0f,  5.0f,  5.0f}, {}, {1.0f, 0.0f}}, // 右上
	};

#pragma 頂点インデックス
	//インデックスデータ
	unsigned short indices[] =
	{
		0, 1, 2,  //三角形1つ目
		2, 1, 3,  //三角形2つ目

		4, 5, 6,  //三角形1つ目
		6, 5, 7,  //三角形2つ目

		8, 9, 10,  //三角形1つ目
		10, 9, 11,  //三角形2つ目

		12, 13, 14,  //三角形1つ目
		14, 13, 15,  //三角形2つ目

		16, 17, 18,  //三角形1つ目
		18, 17, 19,  //三角形2つ目

		20, 21, 22,  //三角形1つ目
		22, 21, 23,  //三角形2つ目
	};

	for (int i = 0; i < _countof(indices) / 3; i++)
	{//三角形1つごとに計算していく
		//三角形のインデックスを取り出して、一時的な変数に入れる
		unsigned short indices0 = indices[i * 3 + 0];
		unsigned short indices1 = indices[i * 3 + 1];
		unsigned short indices2 = indices[i * 3 + 2];
		//三角形を構成する頂点座標をベクトルに代入
		XMVECTOR p0 = XMLoadFloat3(&vertices[indices0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[indices1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[indices2].pos);
		//p0→p1ベクトルp0→p2ベクトルを計算（ベクトルの減算）
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		//外積は両方から垂直なベクトル
		XMVECTOR normal = XMVector3Cross(v1, v2);
		//正規化（長さを1にする）
		normal = XMVector3Normalize(normal);
		//求めた法線を頂点データに代入
		XMStoreFloat3(&vertices[indices0].normal, normal);
		XMStoreFloat3(&vertices[indices1].normal, normal);
		XMStoreFloat3(&vertices[indices2].normal, normal);
	}

	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * _countof(vertices));

#pragma シェーダリソース
#pragma 頂点バッファ
	// 頂点バッファの生成
	ComPtr<ID3D12Resource> vertBuff;
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeVB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	// GPU上のバッファに対応した仮想メモリを取得
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);

	// 全頂点に対して
	for (int i = 0; i < _countof(vertices); i++)
	{
		vertMap[i] = vertices[i];   // 座標をコピー
	}

	// マップを解除
	vertBuff->Unmap(0, nullptr);

	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	//vbView.StrideInBytes = sizeof(XMFLOAT3);
	vbView.StrideInBytes = sizeof(Vertex);

	//インデックスデータ全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) *
		_countof(indices));

	//インデックスバッファの生成
	ComPtr<ID3D12Resource> indexBuff;
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeIB),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	//インデックスバッファビューの生成
	D3D12_INDEX_BUFFER_VIEW ibView{};
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	//GPU上のバッファに対応した仮想メモリを取得
	unsigned short* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);

	//全インデックスに対して
	for (int i = 0; i < _countof(indices); i++)
	{
		indexMap[i] = indices[i];  //インデックスをコピー
	}

	//繋がりを解除
	indexBuff->Unmap(0, nullptr);

#pragma 射影変換
	//射影変換行列(透視投影)
	XMMATRIX matProjection = XMMatrixPerspectiveFovLH
	(
		XMConvertToRadians(60.0f),
		(float)WinApp::WIN_WIDTH / WinApp::WIN_HEIGHT,
		0.1f, 1000.0f
	);

#pragma ビュー変換
	//ビュー変換行列
	XMMATRIX matView;
	XMFLOAT3 eye(0, 0, -100);  //視点座標
	XMFLOAT3 target(0, 0, 0);  //注視点座標
	XMFLOAT3 up(0, 1, 0);      //上方向ベクトル
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	float angle = 0.0f;  //カメラの回転

#pragma 定数バッファ
	const int constantBufferNum = 128;  //定数バッファの最大数

	//定数バッファ用のデスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> basicDescHeap;

	//設定構造体
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = constantBufferNum + 10;    //SRV1つとCBV1つ

#pragma 定数レジスタ
	//デスクリプタヒープの生成
	result = dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&basicDescHeap));

	//デスクリプタヒープの先頭ハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV =
		basicDescHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV =
		basicDescHeap->GetGPUDescriptorHandleForHeapStart();
	//ハンドルのアドレスを進める
	cpuDescHandleSRV.ptr += constantBufferNum *
		dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	gpuDescHandleSRV.ptr += constantBufferNum *
		dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//関数化してもいい
	const int OBJECT_NUM = 1;
	Object3d player[OBJECT_NUM];
	//配列内の全オブジェクトに対して
	for (int i = 0; i < _countof(player); i++)
	{
		//初期化
		InitializeObject3d(&player[i], i, dev.Get(), basicDescHeap.Get());

		//一つ前のオブジェクトを親のオブジェクトとする
		//enemy[i].parent = &enemy[i - 1];
		//オブジェクトの9割の大きさ
		player[i].scale = { 0.9f, 0.9f, 0.9f };
		//オブジェクトに対してZ軸まわりに30度回転
		player[i].rotation = { 0.0f, 0.0f, 0.0f };
		//オブジェクトに対してずらす
		player[i].position = { 0.0f, 0.0f, -50.0f };
	}

	//スプライト共通データ生成
	spriteCommon = SpriteCommonCreate(dev.Get(), WinApp::WIN_WIDTH, WinApp::WIN_HEIGHT);

	SpriteCommonLoadTexture(spriteCommon, 0, L"Resources/APEX_01.png", dev.Get());

	for (int i = 0; i < _countof(sprites); i++)
	{
		int texNumber = i;
		sprites[i] = SpriteCreate(dev.Get(), WinApp::WIN_WIDTH, WinApp::WIN_HEIGHT, texNumber, spriteCommon, { 0.5f, 0.5f }, false, false);
		sprites[i].rotation = 0;
		sprites[i].position = { (float)100, (float)100, 0 };
		sprites[i].color = { 1, 0, 0, 1 };
		sprites[i].size.x = 100.0f;
		sprites[i].size.y = 100.0f;
		//頂点バッファに反映
		SpriteTransferVertexBuffer(sprites[i], spriteCommon, false);
	}

	DebugText debugText;
	//デバックテキスト用のテクスチャ番号を指定
	const int debugTextTexNumber = 2;
	//デバックテキスト用のテクスチャ読み込み
	SpriteCommonLoadTexture(spriteCommon, debugTextTexNumber, L"Resources/debugfont.png", dev.Get());
	//デバックテキストの初期化
	debugText.Initialize(dev.Get(), WinApp::WIN_WIDTH, WinApp::WIN_HEIGHT, debugTextTexNumber, spriteCommon);

	//WICテクスチャのロード
	TexMetadata metadata{};
	ScratchImage scratchImg{};

	result = LoadFromWICFile
	(
		L"Resources/i.jpg",
		WIC_FLAGS_NONE,
		&metadata, scratchImg
	);

	const Image* img = scratchImg.GetImage(0, 0, 0);  //生データ摘出

	//リソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);

	//テスクチャ用バッファの生成
	ComPtr<ID3D12Resource> texbuff = nullptr;
	result = dev->CreateCommittedResource
	(  //GPUリソースの生成
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,  //テスクチャ用指定
		nullptr,
		IID_PPV_ARGS(&texbuff)
	);

#pragma バッファへのデータ転送
	//テクスチャバッファにデータ転送
	result = texbuff->WriteToSubresource
	(
		0,
		nullptr,  //全領域へコピー
		img->pixels,  //元データアドレス
		(UINT)img->rowPitch,  //1ラインサイズ
		(UINT)img->slicePitch  //1枚サイズ
	);

#pragma シェーダリソースビュー
	//シェーダリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};  //設定構造体
	//srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;  //RGBA
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;  //2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	//ヒープの2番目にシェーダーリソースビュー作成
	dev->CreateShaderResourceView(texbuff.Get(),  //ビューと関連付けるバッファ
		&srvDesc,  //テクスチャの設定情報
		cpuDescHandleSRV
	);

	//3Dオブジェクト用パイプライン生成
	PipelineSet object3dPipelineSet = Object3dCreateGraphicsPipeline(dev.Get());

#pragma endregion 描画初期化処理

#pragma region 初期化処理
	float r[] = { 0.0f, 1.0f, 0.0f };
	float g[] = { 0.0f, 1.0f, 0.0f };
	float b[] = { 0.5f, 1.0f, 1.0f };
	float a[] = { 1.0f, 1.0f, 1.0f };
	float xMove = 0.0f;
	float yMove = 0.0f;
	float zMove = 0.0f;

#pragma endregion 初期化処理

	while (true)  // ゲームループ
	{
		win->ProcessMessage();

#pragma region DirectX毎フレーム処理
		// 更新処理　ここから

		input->Update();

		//キーが押されているときの処理
		if (input->TriggerKey(DIK_0))
		{
			OutputDebugStringA("Hit 0\n");
		}

		// マップを解除
		vertBuff->Unmap(0, nullptr);

		//X座標、Y座標を指定して表示
		debugText.Print(spriteCommon, "Hello,DirectX!!", 200, 100, 1.0f);
		//X座標、Y座標、縮尺を指定して表示
		debugText.Print(spriteCommon, "Nihon Kogakuin", 200, 200, 2.0f);

		XMVECTOR v0 = { 0, 0, -50, 0 };
		XMMATRIX rotM = XMMatrixRotationY(XMConvertToRadians(angle));
		XMVECTOR v = XMVector3TransformNormal(v0, rotM);
		XMVECTOR playerTarget = { player[0].position.x,  player[0].position.y,  player[0].position.z };
		XMVECTOR v3 = playerTarget + v;
		XMFLOAT3 f = { v3.m128_f32[0], v3.m128_f32[1], v3.m128_f32[2] };
		target = { playerTarget.m128_f32[0], playerTarget.m128_f32[1], playerTarget.m128_f32[2] };
		eye = f;

		matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

		for (int i = 0; i < _countof(player); i++)
		{
			UpdateObject3d(&player[i], matView, matProjection);
		}

		//スプライトの更新
		for (int i = 0; i < _countof(sprites); i++)
		{
			SpriteUpdate(sprites[i], spriteCommon);
		}

		// GPU上のバッファに対応した仮想メモリを取得
		Vertex* vertMap = nullptr;
		result = vertBuff->Map(0, nullptr, (void**)&vertMap);

		// 全頂点に対して
		for (int i = 0; i < _countof(vertices); i++)
		{
			vertMap[i] = vertices[i];   // 座標をコピー
		}

#pragma endregion DirectX毎フレーム処理

#pragma region グラフィックスコマンド
		

		dxCommon->PreDraw();


		// ４．描画コマンドここから

		_RPTN(_CRT_WARN, "xMove %d : yMove %d : zMove %d\n", (int)xMove, (int)yMove, (int)zMove);

#pragma ルートシグネチャ
#pragma グラフィックスパイプラインステート
		//パイプラインステートとルートシグネチャの設定
		cmdList->SetPipelineState(object3dPipelineSet.pipelinestate.Get());
		cmdList->SetGraphicsRootSignature(object3dPipelineSet.rootsignature.Get());

#pragma ビューポート変換

#pragma 描画プリミティブ
#pragma トライアングルストリップ
		//cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);      //点のリスト
		//cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);       //線のリスト
		//cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);      //線のストリップ
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);   //三角形のリスト
		//cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);  //三角形のストリップ

		for (int i = 0; i < _countof(player); i++)
		{
			DrawObject3d(&player[i], cmdList.Get(), basicDescHeap.Get(), vbView, ibView, gpuDescHandleSRV, _countof(indices));
		}

		//スプライト共通コマンド
		SpriteCommonBeginDraw(spriteCommon, cmdList.Get());

		//スプライト描画
		for (int i = 0; i < _countof(sprites); i++)
		{
			SpriteDraw(sprites[i], cmdList.Get(), spriteCommon, dev.Get());
		}

		debugText.DrawAll(cmdList.Get(), spriteCommon, dev.Get());

		// ４．描画コマンドここまで
		dxCommon->PostDraw();
		

#pragma endregion グラフィックスコマンド

	}

	// XAudio2解放
	xAudio2.Reset();
	// 音声データ解放
	//SoundUnload(&soundData1);

	// ウィンドウクラスを登録解除
	win->DeleteGameWindow();

	return 0;
}

//3Dオブジェクト初期化
void InitializeObject3d(Object3d* object, int index, ID3D12Device* dev,
	ID3D12DescriptorHeap* descHeap)
{
	HRESULT result;

#pragma 定数バッファ
	//定数バッファの生成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&object->constBuff));

	//デスクリプタ1つ分のサイズ
	UINT descHandleIncrementSize =
		dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//CBVのCPUデスクリプタハンドルを計算
	object->cpuDescHandleCBV = descHeap->GetCPUDescriptorHandleForHeapStart();
	object->cpuDescHandleCBV.ptr += index * descHandleIncrementSize;
	//CBVのGPUデスクリプタハンドルを計算
	object->gpuDescHandleCBV = descHeap->GetGPUDescriptorHandleForHeapStart();
	object->gpuDescHandleCBV.ptr += index * descHandleIncrementSize;

#pragma 定数バッファビュー
	//定数バッファビューの生成
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	cbvDesc.BufferLocation = object->constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)object->constBuff->GetDesc().Width;
	dev->CreateConstantBufferView(&cbvDesc, object->cpuDescHandleCBV);
}

//オブジェクト更新処理
void UpdateObject3d(Object3d* object, XMMATRIX& matView, XMMATRIX& matProjection)
{
	//スケーリング行列、回転行列、平行移動行列
	XMMATRIX matScale, matRot, matTrans;
	//スケール、回転、平行移動行列の計算
	matScale = XMMatrixScaling(object->scale.x, object->scale.y, object->scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(object->rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(object->rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(object->rotation.y));
	matTrans = XMMatrixTranslation(object->position.x, object->position.y, object->position.z);  //(50, 0, 0)平行移動

	//ワールド行列
	object->matWorld = XMMatrixIdentity();
	object->matWorld *= matScale;  //ワールド行列にスケーリングを反映
	object->matWorld *= matRot;  //ワールド行列に回転を反映
	object->matWorld *= matTrans;  //ワールド行列に平行移動を反映

	//親オブジェクトがあれば
	if (object->parent != nullptr)
	{
		//親オブジェクトのワールド行列を掛ける
		object->matWorld *= object->parent->matWorld;
	}

#pragma バッファへのデータ転送
	//定数バッファへデータ転送
	ConstBufferData* constMap = nullptr;
	if (SUCCEEDED(object->constBuff->Map(0, nullptr, (void**)&constMap)))
	{
		constMap->color = XMFLOAT4(1, 1, 1, 1);  //RGBA
		constMap->mat = object->matWorld * matView * matProjection;
		object->constBuff->Unmap(0, nullptr);
	}
}

void DrawObject3d(Object3d* object, ID3D12GraphicsCommandList* cmdList,
	ID3D12DescriptorHeap* descHeap, D3D12_VERTEX_BUFFER_VIEW& vbView,
	D3D12_INDEX_BUFFER_VIEW& ibView, D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV,
	UINT numIndices)
{
	//頂点バッファの設定
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	//インデックスバッファの設定
	cmdList->IASetIndexBuffer(&ibView);
	//デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, object->constBuff->GetGPUVirtualAddress());
#pragma リソースバインド
	//シェーダリソースビューをセット
	cmdList->SetGraphicsRootDescriptorTable(1, gpuDescHandleSRV);
	//描画コマンド
	cmdList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
}

//パイプライン生成の関数
PipelineSet Object3dCreateGraphicsPipeline(ID3D12Device* dev)
{
	HRESULT result;

#pragma デスクリプタテーブル
	//デスクリプタテーブルの設定
	CD3DX12_DESCRIPTOR_RANGE descTblRangeSRV;
	descTblRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);  //t0 レジスタ

#pragma ルートパラメータ
	//ルートパラメータの設定
	CD3DX12_ROOT_PARAMETER rootparams[2];
	rootparams[0].InitAsConstantBufferView(0);
	rootparams[1].InitAsDescriptorTable(1, &descTblRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

#pragma 頂点シェーダ
	ComPtr<ID3DBlob> vsBlob = nullptr; // 頂点シェーダオブジェクト
#pragma ピクセルシェーダ
	ComPtr<ID3DBlob> psBlob = nullptr; // ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob = nullptr; // エラーオブジェクト

	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shaders/BasicVS.hlsl",  // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob, &errorBlob);

	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shaders/BasicPS.hlsl",   // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob, &errorBlob);

	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

#pragma 頂点レイアウト
	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{  //xyz座標
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}, // (1行で書いたほうが見やすい)

		{  //法線ベクトル
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}, // (1行で書いたほうが見やすい)

		{  //uv座標
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}, // (1行で書いたほうが見やすい)
	};

#pragma グラフィックスパイプライン
	// グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

#pragma ラスタライザ
	// 標準設定(背面カリング、塗りつぶし、深度クリッピング有効)
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

#pragma 深度テスト
	//デプスステンシルステートの設定
	//標準設定(深度テストを行う、書き込み許可、小さければ合格)
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;  //深度値フォーマット

	gpipeline.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;  // RBGA全てのチャンネルを描画

	//レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = gpipeline.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; //標準設定

	blenddesc.BlendEnable = true;                 //ブレンドを有効にする
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;  //加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;   //ソースの値を100%使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO; //デストの値を  0%使う

	//加算合成
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;  //加算
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;   //ソースの値を100%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE;  //デストの値を100%使う

	//減算合成
	//blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;  //デストからソースを減算
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;   //ソースの値を100%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE;  //デストの値を100%使う

	//色反転
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;  //加算
	//blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;   //1.0f - デストカラーの値
	//blenddesc.DestBlend = D3D12_BLEND_ZERO;  //使わない

#pragma アルファブレンディング
	//半透明合成
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;  //加算
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;   //ソースのアルファ値
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;  //1.0f - ソースのアルファ値

	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1; // 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

#pragma ルートシグネチャ
	//ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	PipelineSet pipelineSet;

	ComPtr<ID3DBlob> rootSigBlob;
	//バージョン自動判定でのシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	//ルートシグネチャの生成
	result = dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&pipelineSet.rootsignature));

	// パイプラインにルートシグネチャをセット
	gpipeline.pRootSignature = pipelineSet.rootsignature.Get();

	result = dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelineSet.pipelinestate));

	//パイプラインとルートシグネチャを返す
	return pipelineSet;
}

//スプライト用パイプライン生成の関数
PipelineSet spriteCreateGraphicsPipeline(ID3D12Device* dev)
{
	HRESULT result;

#pragma デスクリプタテーブル
	//デスクリプタテーブルの設定
	CD3DX12_DESCRIPTOR_RANGE descTblRangeSRV;
	descTblRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);  //t0 レジスタ

#pragma ルートパラメータ
	//ルートパラメータの設定
	CD3DX12_ROOT_PARAMETER rootparams[2];
	rootparams[0].InitAsConstantBufferView(0);
	rootparams[1].InitAsDescriptorTable(1, &descTblRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

#pragma 頂点シェーダ
	ComPtr<ID3DBlob> vsBlob = nullptr; // 頂点シェーダオブジェクト
#pragma ピクセルシェーダ
	ComPtr<ID3DBlob> psBlob = nullptr; // ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob = nullptr; // エラーオブジェクト

	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shaders/SpriteVS.hlsl",  // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob, &errorBlob);

	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shaders/SpritePS.hlsl",   // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob, &errorBlob);

	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

#pragma 頂点レイアウト
	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{  //xyz座標
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}, // (1行で書いたほうが見やすい)

		{  //uv座標
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}, // (1行で書いたほうが見やすい)
	};

#pragma グラフィックスパイプライン
	// グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

#pragma ラスタライザ
	// 標準設定(背面カリング、塗りつぶし、深度クリッピング有効)
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

#pragma 深度テスト
	//デプスステンシルステートの設定
	//標準設定(深度テストを行う、書き込み許可、小さければ合格)
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	gpipeline.DepthStencilState.DepthEnable = false;
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;  //深度値フォーマット

	gpipeline.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;  // RBGA全てのチャンネルを描画

	//レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = gpipeline.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; //標準設定

	blenddesc.BlendEnable = true;                 //ブレンドを有効にする
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;  //加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;   //ソースの値を100%使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO; //デストの値を  0%使う

	//加算合成
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;  //加算
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;   //ソースの値を100%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE;  //デストの値を100%使う

	//減算合成
	//blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;  //デストからソースを減算
	//blenddesc.SrcBlend = D3D12_BLEND_ONE;   //ソースの値を100%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE;  //デストの値を100%使う

	//色反転
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD;  //加算
	//blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;   //1.0f - デストカラーの値
	//blenddesc.DestBlend = D3D12_BLEND_ZERO;  //使わない

#pragma アルファブレンディング
	//半透明合成
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;  //加算
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;   //ソースのアルファ値
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;  //1.0f - ソースのアルファ値

	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1; // 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

#pragma ルートシグネチャ
	//ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	PipelineSet pipelineSet;

	ComPtr<ID3DBlob> rootSigBlob;
	//バージョン自動判定でのシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	//ルートシグネチャの生成
	result = dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&pipelineSet.rootsignature));

	// パイプラインにルートシグネチャをセット
	gpipeline.pRootSignature = pipelineSet.rootsignature.Get();

	result = dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelineSet.pipelinestate));

	//パイプラインとルートシグネチャを返す
	return pipelineSet;
}

//スプライト生成
Sprite SpriteCreate(ID3D12Device* dev, int window_width, int window_height, UINT texNumber,
	SpriteCommon& spriteCommon, XMFLOAT2 anchorpoint,
	bool isFlipX, bool isFlipY)
{
	HRESULT result;

	//新しいスプライトを作る
	Sprite sprite{};

	//テクスチャ番号をコピー
	sprite.texNumber = texNumber;

	//頂点データ
	VertexPosUv vertices[] =
	{
		//  x      y      z       u     v
		{{   0.0f, 100.0f, 0.0f}, {0.0f, 1.0f}}, // 左下
		{{   0.0f,   0.0f, 0.0f}, {0.0f, 0.0f}}, // 左上
		{{ 100.0f, 100.0f, 0.0f}, {1.0f, 1.0f}}, // 右下
		{{ 100.0f,   0.0f, 0.0f}, {1.0f, 0.0f}}, // 右上
	};
#pragma シェーダリソース
#pragma 頂点バッファ
	// 頂点バッファの生成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&sprite.vertBuff));

	// GPU上のバッファに対応した仮想メモリを取得
	VertexPosUv* vertMap = nullptr;
	result = sprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
	memcpy(vertMap, vertices, sizeof(vertices));
	sprite.vertBuff->Unmap(0, nullptr);

	sprite.vbView.BufferLocation = sprite.vertBuff->GetGPUVirtualAddress();
	sprite.vbView.SizeInBytes = sizeof(vertices);
	sprite.vbView.StrideInBytes = sizeof(vertices[0]);

	//指定番号の画像が読み込み済みなら
	if (spriteCommon.texBuff[sprite.texNumber])
	{
		//テクスチャ情報取得
		D3D12_RESOURCE_DESC resDesc = spriteCommon.texBuff[sprite.texNumber]->GetDesc();
		//スプライトの大きさを画像の解像度に合わせる
		sprite.size = { (float)resDesc.Width, (float)resDesc.Height };
	}
	//アンカーポイントをコピー
	sprite.anchorpoint = anchorpoint;
	//反転バッファデータ転送
	sprite.isFlipX = isFlipX;
	sprite.isFlipY = isFlipY;
	//頂点バッファへデータ転送
	SpriteTransferVertexBuffer(sprite, spriteCommon, false);

#pragma 定数バッファ
	//定数バッファの生成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&sprite.constBuff));

#pragma バッファへのデータ転送
	//定数バッファへデータ転送
	ConstBufferData* constMap = nullptr;
	result = sprite.constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->color = XMFLOAT4(1, 1, 1, 1);  //色指定(RGBA)
	//平行投影行列
	constMap->mat = XMMatrixOrthographicOffCenterLH
	(0.0f, window_width, window_height, 0.0f, 0.0f, 1.0f);
	sprite.constBuff->Unmap(0, nullptr);

	return sprite;
}

#pragma グラフィックコマンドリスト
//スプライト共通グラフィックコマンドのセット
void SpriteCommonBeginDraw(const SpriteCommon& spriteCommon, ID3D12GraphicsCommandList* cmdList)
{
#pragma グラフィックスパイプラインステート
	//パイプラインステートの設定
	cmdList->SetPipelineState(spriteCommon.pipelineSet.pipelinestate.Get());
	//ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(spriteCommon.pipelineSet.rootsignature.Get());
#pragma 描画プリミティブ
	//プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//テクスチャ用デスクリプタヒープの設定
	ID3D12DescriptorHeap* ppHeaps[] = { spriteCommon.descHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

//スプライト単体描画
void SpriteDraw(const Sprite& sprite, ID3D12GraphicsCommandList* cmdList, const SpriteCommon& spriteCommon,
	ID3D12Device* dev)
{
	//非表示フラグがtrueなら
	if (sprite.isInvisible)
	{
		//描画せずに抜ける
		return;
	}

	//頂点バッファをセット
	cmdList->IASetVertexBuffers(0, 1, &sprite.vbView);
	//定数バッファをセット
	cmdList->SetGraphicsRootConstantBufferView(0, sprite.constBuff->GetGPUVirtualAddress());

#pragma リソースバインド
	//シェーダリソースビューをセット
	cmdList->SetGraphicsRootDescriptorTable(1,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			spriteCommon.descHeap->GetGPUDescriptorHandleForHeapStart(),
			sprite.texNumber,
			dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	//ポリゴンの描画(4頂点で四角形)
	cmdList->DrawInstanced(4, 1, 0, 0);
}

//スプライト共通データ生成
SpriteCommon SpriteCommonCreate(ID3D12Device* dev, int window_width, int window_height)
{
	HRESULT result = S_FALSE;

	//新たなスプライト共通データを生成
	SpriteCommon spriteCommon{};

	//スプライト用パイプライン生成
	spriteCommon.pipelineSet = spriteCreateGraphicsPipeline(dev);

	//並行投影の射影行列生成
	spriteCommon.matProjection = XMMatrixOrthographicOffCenterLH(
		0.0f, (float)window_width, (float)window_height, 0.0f, 0.0f, 1.0f);

	//デスクリプタヒープの生成
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = spriteSRVCount;
	result = dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&spriteCommon.descHeap));

	//生成したスプライト共通データを返す
	return spriteCommon;
}

//スプライト単体更新
void SpriteUpdate(Sprite& sprite, const SpriteCommon& spriteCommon)
{
	//ワールド行列の更新
	sprite.matWorld = XMMatrixIdentity();
	//z軸回転
	sprite.matWorld *= XMMatrixRotationZ(XMConvertToRadians(sprite.rotation));
	//平行移動
	sprite.matWorld *= XMMatrixTranslation(sprite.position.x, sprite.position.y, sprite.position.z);

#pragma バッファへのデータ転送
	//定数バッファの転送
	ConstBufferData* constMap = nullptr;
	HRESULT result = sprite.constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->mat = sprite.matWorld * spriteCommon.matProjection;
	constMap->color = sprite.color;
	sprite.constBuff->Unmap(0, nullptr);
}

//スプライト共通テクスチャ読み込み
void SpriteCommonLoadTexture(SpriteCommon& spriteCommon,
	UINT texnumber, const wchar_t* filename, ID3D12Device* dev)
{
	//異常な番号の指定を検出
	assert(texnumber <= spriteSRVCount - 1);

	HRESULT result;

	//WICテクスチャのロード
	TexMetadata metadata{};
	ScratchImage scratchImg{};

	result = LoadFromWICFile
	(
		filename,
		WIC_FLAGS_NONE,
		&metadata, scratchImg
	);

	const Image* img = scratchImg.GetImage(0, 0, 0);  //生データ摘出

	//リソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);

	//テスクチャ用バッファの生成
	spriteCommon.texBuff[texnumber] = nullptr;
	result = dev->CreateCommittedResource
	(  //GPUリソースの生成
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,  //テスクチャ用指定
		nullptr,
		IID_PPV_ARGS(&spriteCommon.texBuff[texnumber])
	);

#pragma バッファへのデータ転送
	//テクスチャバッファにデータ転送
	result = spriteCommon.texBuff[texnumber]->WriteToSubresource
	(
		0,
		nullptr,  //全領域へコピー
		img->pixels,  //元データアドレス
		(UINT)img->rowPitch,  //1ラインサイズ
		(UINT)img->slicePitch  //1枚サイズ
	);

#pragma シェーダリソースビュー
	//シェーダリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};  //設定構造体
	//srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;  //RGBA
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;  //2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	//ヒープのtexnumber番目にシェーダーリソースビュー作成
	dev->CreateShaderResourceView(spriteCommon.texBuff[texnumber].Get(),  //ビューと関連付けるバッファ
		&srvDesc,  //テクスチャの設定情報
		CD3DX12_CPU_DESCRIPTOR_HANDLE(spriteCommon.descHeap->GetCPUDescriptorHandleForHeapStart(), texnumber,
			dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);
}

//スプライト単体頂点バッファの転送
void SpriteTransferVertexBuffer(const Sprite& sprite, const SpriteCommon& spriteCommon, bool texSizeFlag)
{
	HRESULT result = S_FALSE;

	//頂点データ
	VertexPosUv vertices[] =
	{
		//  x      y      z       u     v
		{{}, {0.0f, 1.0f}}, // 左下
		{{}, {0.0f, 0.0f}}, // 左上
		{{}, {1.0f, 1.0f}}, // 右下
		{{}, {1.0f, 0.0f}}, // 右上
	};

	enum { LB, LT, RB, RT };

	float left = (0.0f - sprite.anchorpoint.x) * sprite.size.x;
	float right = (1.0f - sprite.anchorpoint.x) * sprite.size.x;
	float top = (0.0f - sprite.anchorpoint.y) * sprite.size.y;
	float bottom = (1.0f - sprite.anchorpoint.y) * sprite.size.y;

	if (sprite.isFlipX)
	{
		//左右入れ替え
		left = -left;
		right = -right;
	}

	if (sprite.isFlipY)
	{
		//上下入れ替え
		top = -top;
		bottom = -bottom;
	}

	vertices[LB].pos = { left,  bottom, 0.0f }; // 左下
	vertices[LT].pos = { left,  top,    0.0f }; // 左上
	vertices[RB].pos = { right, bottom, 0.0f }; // 右下
	vertices[RT].pos = { right, top,    0.0f }; // 右上

	if (spriteCommon.texBuff[sprite.texNumber] && texSizeFlag)
	{
		//テクスチャ情報取得
		D3D12_RESOURCE_DESC resDesc = spriteCommon.texBuff[sprite.texNumber]->GetDesc();

		float tex_left = sprite.texLeftTop.x / resDesc.Width;
		float tex_right = (sprite.texLeftTop.x + sprite.texSize.x) / resDesc.Width;
		float tex_top = sprite.texLeftTop.y / resDesc.Height;
		float tex_bottom = (sprite.texLeftTop.y + sprite.texSize.y) / resDesc.Height;

		vertices[LB].uv = { tex_left,  tex_bottom }; // 左下
		vertices[LT].uv = { tex_left,  tex_top }; // 左上
		vertices[RB].uv = { tex_right, tex_bottom }; // 右下
		vertices[RT].uv = { tex_right, tex_top }; // 右上
	}

#pragma バッファへのデータ転送
	//頂点バッファへデータ転送
	VertexPosUv* vertMap = nullptr;
	result = sprite.vertBuff->Map(0, nullptr, (void**)&vertMap);
	memcpy(vertMap, vertices, sizeof(vertices));
	sprite.vertBuff->Unmap(0, nullptr);
}

void DebugText::Initialize(ID3D12Device* dev, int window_width, int window_height, UINT texNumber,
	SpriteCommon& spriteCommon)
{
	//全てのスプライトデータについて
	for (int i = 0; i < _countof(sprites); i++)
	{
		//スプライトを生成する
		sprites[i] = SpriteCreate(dev, window_width, window_height, texNumber, spriteCommon, { 0, 0 }, false, false);
	}
}

void DebugText::Print(const SpriteCommon& spriteCommon, const std::string& text, float x, float y, float scale)
{
	//全ての文字について
	for (int i = 0; i < text.size(); i++)
	{
		//最大文字数超過
		if (spriteIndex >= maxCharCount)
		{
			break;
		}

		//1文字取り出す(ASCIIコードでしか成り立たない)
		const unsigned char& character = text[i];

		//ASCIIコードの2段分飛ばした番号を計算
		int fontIndex = character - 32;
		if (character >= 0x7f)
		{
			fontIndex = 0;
		}

		int fontIndexY = fontIndex / fontLineCount;
		int fontIndexX = fontIndex % fontLineCount;

		//座標計算
		sprites[spriteIndex].position = { x + fontWidth * scale * i, y, 0 };
		sprites[spriteIndex].texLeftTop = { (float)fontIndexX * fontWidth, (float)fontIndexY * fontHeight };
		sprites[spriteIndex].texSize = { fontWidth, fontHeight };
		sprites[spriteIndex].size = { fontWidth * scale, fontHeight * scale };
		//頂点バッファ転送
		SpriteTransferVertexBuffer(sprites[spriteIndex], spriteCommon, true);
		//更新
		SpriteUpdate(sprites[spriteIndex], spriteCommon);
		//文字を1つ進める
		spriteIndex++;
	}
}

//まとめて描画
void DebugText::DrawAll(ID3D12GraphicsCommandList* cmdList, const SpriteCommon& spriteCommon, ID3D12Device* dev)
{
	//全ての文字のスプライトについて
	for (int i = 0; i < spriteIndex; i++)
	{
		//スプライト描画
		SpriteDraw(sprites[i], cmdList, spriteCommon, dev);
	}

	spriteIndex = 0;
}

//音声データの読み込み
SoundData SoundLoadWave(const char* filename)
{
	//ファイル入力ストリームのインスタンス
	std::ifstream file;
	//.wavファイルをバリナリモードで開く
	file.open(filename, std::ios_base::binary);
	//ファイルオープン失敗を検出する
	assert(file.is_open());

	//RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	//ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0)
	{
		assert(0);
	}
	//タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0)
	{
		assert(0);
	}

	//Formatチャンクの読み込み
	FormatChunk format = {};
	//チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt", 4) != 0)
	{
		assert(0);
	}

	//チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	//Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	//JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0)
	{
		//読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		//再読み込み
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0)
	{
		assert(0);
	}

	//Dataチャンクのデータ部(波形データ)の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	//Waveファイルを閉じる
	file.close();

	//returnする為の音声データ
	SoundData soundData = {};

	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;

	return soundData;
}

//音声データの解放
void SoundUnload(SoundData* soundData)
{
	//バッファのメモリを解放
	delete[] soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

//音声再生
void SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData)
{
	HRESULT result;

	//波形フォーマットを元にSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));

	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	//波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();
}