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
	Audio* titleAudio = nullptr;
	Audio *playAudio = nullptr;
	Audio *gameoverAudio = nullptr;
	Audio *carAudio = nullptr;
	Audio *dangerAudio = nullptr;
	Audio *deadAudio = nullptr;
	DebugText debugText;

	// ゲームシーン用
	Sprite* sprite = nullptr;
	Sprite* back1 = nullptr;
	Sprite* back2 = nullptr;
	Sprite* back3 = nullptr;
	Sprite *title = nullptr;
	Sprite *noOnes0 = nullptr;
	Sprite *noOnes1 = nullptr;
	Sprite *noOnes2 = nullptr;
	Sprite *noOnes3 = nullptr;
	Sprite *noOnes4 = nullptr;
	Sprite *noOnes5 = nullptr;
	Sprite *noOnes6 = nullptr;
	Sprite *noOnes7 = nullptr;
	Sprite *noOnes8 = nullptr;
	Sprite *noOnes9 = nullptr;
	Sprite *noTens0 = nullptr;
	Sprite *noTens1 = nullptr;
	Sprite *noTens2 = nullptr;
	Sprite *noTens3 = nullptr;
	Sprite *noTens4 = nullptr;
	Sprite *noTens5 = nullptr;
	Sprite *noTens6 = nullptr;
	Sprite *noTens7 = nullptr;
	Sprite *noTens8 = nullptr;
	Sprite *noTens9 = nullptr;
	Sprite *noHundreds0 = nullptr;
	Sprite *noHundreds1 = nullptr;
	Sprite *noHundreds2 = nullptr;
	Sprite *noHundreds3 = nullptr;
	Sprite *noHundreds4 = nullptr;
	Sprite *noHundreds5 = nullptr;
	Sprite *noHundreds6 = nullptr;
	Sprite *noHundreds7 = nullptr;
	Sprite *noHundreds8 = nullptr;
	Sprite *noHundreds9 = nullptr;
	Sprite *uiBack = nullptr;
	Sprite *gameOver = nullptr;
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
	int nowScene = 0;
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
	int blinking[6];//点滅用
	float nowTime = 0;
	float endTime = 1.0;
	float timeRate = 0;
	int isChange = 0;
	int walkCountOnesP = 0;
	int walkCountTensP = 0;
	int walkCountHundredsP = 0;
	int soundCount = -10;
	int soundCount2 = -10;
};