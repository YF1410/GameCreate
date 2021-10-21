#pragma once

#include "SafeDelete.h"
#include "DirectXCommon.h"
#include <DirectXMath.h>
#include "Input.h"
#include "Sprite.h"
#include "Object3d.h"
#include "DebugText.h"
#include "Audio.h"
#include "Model.h"

class GameScene {
private: // エイリアス
// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

private: // 静的メンバ変数
	static const int debugTextTexNumber = 0;

public: // メンバ関数
	// コンストクラタ
	GameScene();
	// デストラクタ
	~GameScene();
	// 初期化
	void Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio);
	// 毎フレーム処理
	void Update();
	// 描画
	void Draw();

private: // メンバ変数
	DirectXCommon* dxCommon = nullptr;
	Input* input = nullptr;
	Audio* audio = nullptr;
	DebugText debugText;

	// ゲームシーン用
	Sprite* sprite = nullptr;
	Sprite* back1 = nullptr;
	Sprite* back2 = nullptr;
	Sprite* back3 = nullptr;
	Sprite *warningMarkLT = nullptr;
	Sprite *warningMarkLM = nullptr;
	Sprite *warningMarkLB = nullptr;
	Sprite *warningMarkRT = nullptr;
	Sprite *warningMarkRM = nullptr;
	Sprite *warningMarkRB = nullptr;
	Model* playerModel = nullptr;
	Model* playerJump1Model = nullptr;
	Model* playerJump2Model = nullptr;
	Model* skydomeModel = nullptr;
	Model* groundModel = nullptr;
	Model* largeCarModel = nullptr;
	Model* largeCarRModel = nullptr;
	Model* miniCarModel = nullptr;
	Model* miniCarRModel = nullptr;
	Model* truckModel = nullptr;
	Model* truckRModel = nullptr;
	Object3d* playerObj = nullptr;
	Object3d* playerJump1Obj = nullptr;
	Object3d* playerJump2Obj = nullptr;
	Object3d* skydomeObj = nullptr;
	Object3d* groundObj = nullptr;
	Object3d* largeCarObj[2] = { nullptr,nullptr };
	Object3d* largeCarRObj[2] = { nullptr,nullptr };
	Object3d* miniCarObj[2] = { nullptr,nullptr };
	Object3d* miniCarRObj[2] = { nullptr,nullptr };
	Object3d* truckObj[2] = { nullptr,nullptr };
	Object3d* truckRObj[2] = { nullptr,nullptr };
	float playerScale = 1.0f;
	float groundScale = 15.0f;
	float largeCarScale = 3.0f;
	float miniCarScale = 2.6f;
	float truckScale = 3.0f;
	bool isJumpUp = false;
	bool isJumpDown = false;
	float zMove = 1.0f;
	float yMove = 0.5f;
	int count = 0;
	bool isLeftLeg = true;
	bool isRightLeg = false;
	bool isWarning[6];
	int blinking[6];//点滅用
};