#include "GameScene.h"
#include <cassert>

GameScene::GameScene() {

}

GameScene::~GameScene() {
	safe_delete(playerModel);
	safe_delete(skydomeModel);
	safe_delete(groundModel);
	safe_delete(largeCarModel);
	safe_delete(playerObj);
	safe_delete(skydomeObj);
	safe_delete(groundObj);
	safe_delete(largeCarObj);
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio) {
	// nullptrチェック
	assert(dxCommon);
	assert(input);
	assert(audio);

	this->dxCommon = dxCommon;
	this->input = input;
	this->audio = audio;

	// デバッグテキスト用テクスチャ読み込み
	if (!Sprite::LoadTexture(debugTextTexNumber, L"Resources/debugfont.png")) {
		assert(0);
	}
	// デバッグテキスト初期化
	debugText.Initialize(debugTextTexNumber);

	// テクスチャ読み込み

	//.objの名前を指定してモデルを読み込む
	playerModel = playerModel->CreateFromObject("player");
	skydomeModel = skydomeModel->CreateFromObject("skydome");
	groundModel = groundModel->CreateFromObject("ground");
	largeCarModel = largeCarModel->CreateFromObject("largeCar");
	// 3Dオブジェクト生成
	playerObj = Object3d::Create();
	skydomeObj = Object3d::Create();
	groundObj = Object3d::Create();
	largeCarObj = Object3d::Create();
	// 3Dオブジェクトにモデルを割り当てる
	playerObj->SetModel(playerModel);
	skydomeObj->SetModel(skydomeModel);
	groundObj->SetModel(groundModel);
	largeCarObj->SetModel(largeCarModel);

	playerObj->SetPosition({ 0.0f, 0.0f, -35.0f });
	float playerScale = 1.0f;
	playerObj->SetScale({ playerScale,playerScale,playerScale });
	float groundScale = 15.0f;
	groundObj->SetScale({ groundScale,groundScale, groundScale });

	float carLargeScale = 3.0f;
	largeCarObj->SetScale({ carLargeScale,carLargeScale,carLargeScale });
	//groundObj->SetPosition({0.0f, -2.5f, 0.0f});

	//サウンド再生
	audio->PlayWave("Resources/Alarm01.wav");
}

void GameScene::Update() {
	//キーが押されているときの処理
	if (input->TriggerKey(DIK_0)) 	{
		OutputDebugStringA("Hit 0\n");
	}

	//X座標、Y座標を指定して表示
	debugText.Print("Hello,DirectX!!", 200, 100, 1.0f);
	//X座標、Y座標、縮尺を指定して表示
	debugText.Print("Nihon Kogakuin", 200, 200, 2.0f);

	// オブジェクト移動
	if (input->PushKey(DIK_UP) || input->PushKey(DIK_DOWN) || input->PushKey(DIK_RIGHT) || input->PushKey(DIK_LEFT)) 	{
		// 現在の座標を取得
		XMFLOAT3 playerPos = playerObj->GetPosition();
		XMFLOAT3 skydomePos = skydomeObj->GetPosition();
		XMFLOAT3 cameraEye = Object3d::GetEye();
		XMFLOAT3 cameraTarget = Object3d::GetTarget();

		// 移動後の座標を計算
		if (input->PushKey(DIK_UP)) 		{
			playerPos.z += zMove;
			skydomePos.z += zMove;
			cameraEye.z += zMove;
			cameraTarget.z += zMove;
		}
		else if (input->PushKey(DIK_DOWN)) 		{
			playerPos.z -= zMove;
			skydomePos.z -= zMove;
			cameraEye.z -= zMove;
			cameraTarget.z -= zMove;
		}

		if (input->PushKey(DIK_RIGHT)) 		{
			playerPos.x += zMove;
			skydomePos.x += zMove;
			cameraEye.x += zMove;
			cameraTarget.x += zMove;
		}
		else if (input->PushKey(DIK_LEFT)) 		{
			playerPos.x -= zMove;
			skydomePos.x -= zMove;
			cameraEye.x -= zMove;
			cameraTarget.x -= zMove;
		}

		// 座標の変更を反映
		playerObj->SetPosition(playerPos);
		skydomeObj->SetPosition(skydomePos);
		Object3d::SetEye(cameraEye);
		Object3d::SetTarget(cameraTarget);
	}

	// カメラ移動
	if (input->PushKey(DIK_W) || input->PushKey(DIK_S) || input->PushKey(DIK_D) || input->PushKey(DIK_A)) 	{
		if (input->PushKey(DIK_W)) 		{
			Object3d::CameraMoveVector({ 0.0f,+1.0f,0.0f });
			//scroll += 10.0f;
		}
		else if (input->PushKey(DIK_S)) 		{
			Object3d::CameraMoveVector({ 0.0f,-1.0f,0.0f });
			//scroll -= 10.0f;
		}

		if (input->PushKey(DIK_D)) 		{
			Object3d::CameraMoveEyeVector({ +1.0f,0.0f,0.0f });
		}
		else if (input->PushKey(DIK_A)) 		{
			Object3d::CameraMoveEyeVector({ -1.0f,0.0f,0.0f });
		}
	}

	if (input->TriggerKey(DIK_SPACE) && !isJumpUp && !isJumpDown) {
		isJumpUp = true;
	}

	if (isJumpUp == true) {
		// 現在の座標を取得
		XMFLOAT3 playerPos = playerObj->GetPosition();
		XMFLOAT3 skydomePos = skydomeObj->GetPosition();
		XMFLOAT3 cameraEye = Object3d::GetEye();
		XMFLOAT3 cameraTarget = Object3d::GetTarget();

		// 移動後の座標を計算
		playerPos.z += zMove;
		playerPos.y += yMove;
		skydomePos.z += zMove;
		cameraEye.z += zMove;
		cameraTarget.z += zMove;


		// 座標の変更を反映
		playerObj->SetPosition(playerPos);
		skydomeObj->SetPosition(skydomePos);
		Object3d::SetEye(cameraEye);
		Object3d::SetTarget(cameraTarget);

		if (playerPos.y >= 2.5f) {
			isJumpUp = false;
			isJumpDown = true;
		}
	}

	if (isJumpDown == true) {
		// 現在の座標を取得
		XMFLOAT3 playerPos = playerObj->GetPosition();
		XMFLOAT3 skydomePos = skydomeObj->GetPosition();
		XMFLOAT3 cameraEye = Object3d::GetEye();
		XMFLOAT3 cameraTarget = Object3d::GetTarget();

		// 移動後の座標を計算
		playerPos.z += zMove;
		playerPos.y -= yMove;
		skydomePos.z += zMove;
		cameraEye.z += zMove;
		cameraTarget.z += zMove;

		// 座標の変更を反映
		playerObj->SetPosition(playerPos);
		skydomeObj->SetPosition(skydomePos);
		Object3d::SetEye(cameraEye);
		Object3d::SetTarget(cameraTarget);

		if (playerPos.y <= 0.0f) {
			isJumpDown = false;
			count++;
		}
	}

	if (count >= 3) {
		XMFLOAT3 groundPos = groundObj->GetPosition();
		XMFLOAT3 playerPos = playerObj->GetPosition();
		groundPos.z = playerPos.z + 35.0f;
		groundObj->SetPosition(groundPos);
		count = 0;
	}

	XMFLOAT3 largeCarPos = largeCarObj->GetPosition();
	largeCarPos.x += 1.0f;
	largeCarObj->SetPosition(largeCarPos);

	playerObj->Update();
	skydomeObj->Update();
	groundObj->Update();
	largeCarObj->Update();
}

void GameScene::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();
#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());
	// 背景スプライト描画

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon->ClearDepthBuffer();
#pragma endregion 背景スプライト描画
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Object3d::PreDraw(dxCommon->GetCommandList());
	// 3Dオブクジェクトの描画
	playerObj->Draw();
	skydomeObj->Draw();
	groundObj->Draw();
	largeCarObj->Draw();
	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();
#pragma endregion 3Dオブジェクト描画
#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());
	// デバッグテキストの描画
	debugText.DrawAll(dxCommon->GetCommandList());
	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion 前景スプライト描画
}