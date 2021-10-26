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
#include "stdlib.h"
#include "time.h"

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
	//車の描画位置初期化
	void ScrollCarPos(int activeCarNum);
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
	Sprite* warningMarkLT = nullptr;
	Sprite* warningMarkLM = nullptr;
	Sprite* warningMarkLB = nullptr;
	Sprite* warningMarkRT = nullptr;
	Sprite* warningMarkRM = nullptr;
	Sprite* warningMarkRB = nullptr;
	Model* playerModel = nullptr;
	Model* playerJumpLModel = nullptr;
	Model* playerJumpRModel = nullptr;
	Model* skydomeModel = nullptr;
	Model* groundModel = nullptr;
	Model* largeCarLModel = nullptr;
	Model* largeCarRModel = nullptr;
	Model* miniCarLModel = nullptr;
	Model* miniCarRModel = nullptr;
	Model* truckLModel = nullptr;
	Model* truckRModel = nullptr;
	Object3d* playerObj = nullptr;
	Object3d* playerJumpLObj = nullptr;
	Object3d* playerJumpRObj = nullptr;
	Object3d* skydomeObj = nullptr;
	Object3d* groundObj = nullptr;
	Object3d* largeCarLObj[2] = { nullptr,nullptr };
	Object3d* largeCarRObj[2] = { nullptr,nullptr };
	Object3d* miniCarLObj[2] = { nullptr,nullptr };
	Object3d* miniCarRObj[2] = { nullptr,nullptr };
	Object3d* truckLObj[2] = { nullptr,nullptr };
	Object3d* truckRObj[2] = { nullptr,nullptr };
	float playerScale = 1.0f;
	float groundScale = 15.0f;
	float largeCarScale = 3.0f;
	float miniCarScale = 2.6f;
	float truckScale = 3.0f;
	float carMove = 2.5f;
	int carTypeR = 0;
	int carTypeL = 0;
	int activeCarNum = 0;
	bool isJumpUp = false;
	bool isJumpDown = false;
	float scrollGround = 35.0f;
	float scrollCar = 60.0f;
	int scrollCount = 1;
	float zMove = 1.0f;
	float yMove = 0.5f;
	int count = 0;
	bool isRightLeg = true;
	bool isLeftLeg = false;
	int warningRWaitTime;
	int warningLWaitTime;
	bool isWarningRWait;
	bool isWarningLWait;
	bool isRightWarning;
	bool isLeftWarning;
	bool isRunCarR;
	bool isRunCarL;
	int blinkingR;//点滅用
	int blinkingL;//点滅用
	int blinkRCount;
	int blinkLCount;
};